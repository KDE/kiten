/* This file is part of Kiten, a KDE Japanese Reference Tool...
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>
			  (C) 2006  Eric Kjeldergaard <kjelderg@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "entryEdict.h"
#include "dictFileEdict.h"
#include <kdebug.h>

#include <klocalizedstring.h>

struct EDICT_formatting {
	EDICT_formatting();
	QString nounType,verbType,adjectiveType,adverbType,ichidanType,godanType,particleType;
	QMultiHash<QString, QString> partOfSpeechCategories;
	QSet<QString> partsOfSpeech,miscMarkings,fieldOfApplication;
};
EDICT_formatting *EntryEDICT::m_format = NULL;

EntryEDICT::EntryEDICT(const QString &dict) : Entry(dict) {
}

EntryEDICT::EntryEDICT(const QString &dict, const QString &entry) : Entry(dict) {
	loadEntry(entry);
}
Entry *EntryEDICT::clone() const { return new EntryEDICT(*this); }

/* DISPLAY FUNCTIONS */

#define QSTRINGLISTCHECK(x) (x==NULL?QStringList():*x)

/** returns a HTML version of an Entry */
/* TODO: Currently ingnoring printType */
QString EntryEDICT::toHTML(printType printDirective) const
{
	QString result="<div class=\"EDICTBrief\">";

	foreach(const QString &field, QSTRINGLISTCHECK(dictFileEdict::displayFieldsList)) {
		if(field == "--NewLine--")			result += "<br>";
		else if(field == "Word/Kanji")	result += HTMLWord()+' ';
		else if(field == "Meaning")		result += HTMLMeanings()+' ';
		else if(field == "Reading")		result += HTMLReadings()+' ';
		else if(field == "C")			result += Common();
		else kDebug() << "Unknown field: " << field;
	}
	result += "</div>";
	return result;
}


/** Makes a link out of each kanji in @p inString */
QString EntryEDICT::kanjiLinkify(const QString &inString) const
{
	QString outString;
	int i;

	for(i = 0; i < inString.length(); i++)
	{
		if(isKanji(inString.at(i)))
		{
			outString += makeLink(QString(inString.at(i)));
		}
		else
		{
			outString += inString.at(i);
		}
	}

	return outString;
}

QString EntryEDICT::HTMLWord() const {
	return "<span class=\"Word\">"+
		( Word.isEmpty()?kanjiLinkify(Meanings.first()):kanjiLinkify(Word) ) +
		"</span>";
}

QString EntryEDICT::Common() const
{
	if (getExtendedInfoItem(QString("common")) == "1")
		return "<span>Common</span>";
	else
		return QString();
}

/* DATA LOADING FUNCTIONS */

/** Take a QString and load it into the Entry as appropriate */
/* The format is basically: KANJI [KANA] /(general information) gloss/gloss/.../
 * Note that they can rudely place more (general information) in gloss's that are
 * not the first one */

bool EntryEDICT::loadEntry(const QString &entryLine)
{
	/* Set tempQString to be the reading and word portion of the entryLine */
	int endOfKanjiAndKanaSection = entryLine.indexOf('/');
	if(endOfKanjiAndKanaSection == -1)
		return false;
	QString tempQString = entryLine.left(endOfKanjiAndKanaSection);
	/* The actual Word is the beginning of the line */
	int endOfKanji = tempQString.indexOf(' ');
	if(endOfKanji == -1)
		return false;
	Word = tempQString.left(endOfKanji);

	/* The Reading is either Word or encased in '[' */
	Readings.clear();
	int startOfReading = tempQString.indexOf('[');
	if(startOfReading != -1)  // This field is optional for EDICT (and kiten)
		Readings.append(
				tempQString.left(tempQString.lastIndexOf(']')).mid(startOfReading+1));

	/* set Meanings to be all of the meanings in the definition */
	QString remainingLine = entryLine.mid(endOfKanjiAndKanaSection);
	remainingLine = remainingLine.left(remainingLine.lastIndexOf('/'));	//Trim to last '/'
	Meanings = remainingLine.split('/', QString::SkipEmptyParts);
	if(Meanings.last() == "(P)") {
		ExtendedInfo[QString("common")] = "1";
		Meanings.removeLast();
	}

	QString firstWord = Meanings.first();
	QStringList stringTypes;

	//Pulls the various types out
	//TODO: Remove them from the original string
	for (int i = firstWord.indexOf("("); i != -1; i = firstWord.indexOf("(", i + 1))
	{
		QString parantheses = firstWord.mid(i + 1, firstWord.indexOf(")", i) - i - 1);
		stringTypes += parantheses.split(",");
	}
	foreach(const QString &str, stringTypes) {
		if(format().partsOfSpeech.contains(str))
			m_typeList += str;
		else if(format().fieldOfApplication.contains(str))
			ExtendedInfo["field"] = str;
		else if(format().miscMarkings.contains(str))
			m_miscMarkings += str;
	}

	return true;
}

/** Regenerate a QString like the one we got in loadEntry() */
QString EntryEDICT::dumpEntry() const
{
	return Word +
		((Readings.count() == 0) ? " " : " [" + Readings.first() + "] ")
		+ '/' + Meanings.join("/") + '/';
}

const EDICT_formatting &EntryEDICT::format() {
	if(EntryEDICT::m_format == NULL)
		EntryEDICT::m_format = new EDICT_formatting;
	return *EntryEDICT::m_format;
}

/* TYPES RELATED FUNCTIONS */

/* The basic idea of this function is to provide a mapping from possible entry types to
	possible things the user could enter. Then our code for the matching entry can simply
	use this mapping to determine if a given entry could be understood to match the user's
	input.

	There are two basic approaches we could take:
		Convert the user's entry into a list of types, see if the Entry type matches any of
			the conversions from this list. (the list comparisons will be MANY enums)
		Convert our Entry types to a list of acceptable string aliases. Then compare the
			user's input to this list. (the list will be a relatively small list of strings)

	My gut instinct is that the first case (comparison of a largish list of ints) will be
		faster, and so that's the one that's implemented here.

	The following are the minimum list of case-insensitive aliases that the user could enter:
	noun
	verb:
		ichidan
		godan
	adjective
	adverb
	particle

	Note that our File Parser will also expand to general cases, if not included already:
		For Example: v5aru -> v5aru,v5 (so that a search for "godan" will find it)
	Also note that the basic edict dictionary does not separate ikeiyoushi out from the
		category "adj", so further breakdown of the "adjective" type would be misleading
*/

EDICT_formatting::EDICT_formatting() {
	nounType = QString(i18nc("This must be a single word","Noun"));
	verbType = QString(i18nc("This must be a single word","Verb"));
	adjectiveType = QString(i18nc("This must be a single word","Adjective"));
	adverbType = QString(i18nc("This must be a single word","Adverb"));
	particleType = QString(i18nc("This must be a single word","Particle"));
	ichidanType = QString(i18nc("This is a technical japanese linguist's term... and probably should not be translated(except possibly in far-eastern languages), this must be a single word","Ichidan"));
	godanType = QString(i18nc("This is a technical japanese linguist's term... and probably should not be translated, this must be a single word","Godan"));

		//Nouns
	partOfSpeechCategories.insert(nounType,  "n");
	partOfSpeechCategories.insert(nounType,  "n-adv");
	partOfSpeechCategories.insert(nounType,  "n-pref");
	partOfSpeechCategories.insert(nounType,  "n-suf");
	partOfSpeechCategories.insert(nounType,  "n-t");
	partOfSpeechCategories.insert(nounType,  "adv_n");
	//Ichidan Verbs
	partOfSpeechCategories.insert(verbType,  "v1");
	partOfSpeechCategories.insert(ichidanType,  "v1");
	//Godan Verbs
	partOfSpeechCategories.insert(verbType,  "v5");
	partOfSpeechCategories.insert(verbType,  "v5aru");
	partOfSpeechCategories.insert(verbType,  "v5b");
	partOfSpeechCategories.insert(verbType,  "v5g");
	partOfSpeechCategories.insert(verbType,  "v5k");
	partOfSpeechCategories.insert(verbType,  "v5k_s");
	partOfSpeechCategories.insert(verbType,  "v5m");
	partOfSpeechCategories.insert(verbType,  "v5n");
	partOfSpeechCategories.insert(verbType,  "v5r");
	partOfSpeechCategories.insert(verbType,  "v5r_i");
	partOfSpeechCategories.insert(verbType,  "v5s");
	partOfSpeechCategories.insert(verbType,  "v5t");
	partOfSpeechCategories.insert(verbType,  "v5u");
	partOfSpeechCategories.insert(verbType,  "v5u_s");
	partOfSpeechCategories.insert(verbType,  "v5uru");
	partOfSpeechCategories.insert(godanType,  "v5");
	partOfSpeechCategories.insert(godanType,  "v5aru");
	partOfSpeechCategories.insert(godanType,  "v5b");
	partOfSpeechCategories.insert(godanType,  "v5g");
	partOfSpeechCategories.insert(godanType,  "v5k");
	partOfSpeechCategories.insert(godanType,  "v5k_s");
	partOfSpeechCategories.insert(godanType,  "v5m");
	partOfSpeechCategories.insert(godanType,  "v5n");
	partOfSpeechCategories.insert(godanType,  "v5r");
	partOfSpeechCategories.insert(godanType,  "v5r_i");
	partOfSpeechCategories.insert(godanType,  "v5s");
	partOfSpeechCategories.insert(godanType,  "v5t");
	partOfSpeechCategories.insert(godanType,  "v5u");
	partOfSpeechCategories.insert(godanType,  "v5u_s");
	partOfSpeechCategories.insert(godanType,  "v5uru");
	//Other Verbs
	partOfSpeechCategories.insert(verbType,  "iv");
	partOfSpeechCategories.insert(verbType,  "vi");
	partOfSpeechCategories.insert(verbType,  "vk");
	partOfSpeechCategories.insert(verbType,  "vs");
	partOfSpeechCategories.insert(verbType,  "vs_i");
	partOfSpeechCategories.insert(verbType,  "vs_s");
	partOfSpeechCategories.insert(verbType,  "vt");
	partOfSpeechCategories.insert(verbType,  "vz");
	//Adjectives
	partOfSpeechCategories.insert(adjectiveType,"adj");
	partOfSpeechCategories.insert(adjectiveType,"adj_na");
	partOfSpeechCategories.insert(adjectiveType,"adj_no");
	partOfSpeechCategories.insert(adjectiveType,"adj_pn");
	partOfSpeechCategories.insert(adjectiveType,"adj_t");
	//Adverbs
	partOfSpeechCategories.insert(adverbType,"adv");
	partOfSpeechCategories.insert(adverbType,"adv_n");
	partOfSpeechCategories.insert(adverbType,"adv_to");
	//Particle
	partOfSpeechCategories.insert(particleType,"prt");

	partsOfSpeech <<"n" <<"n-adv" <<"n-pref" <<"n-suf" <<"n-t" <<"adv_n"
	 <<"v1" <<"v1" <<"v5" <<"v5aru" <<"v5b" <<"v5g" <<"v5k" <<"v5k_s"
	 <<"v5m" <<"v5n" <<"v5r" <<"v5r_i" <<"v5s" <<"v5t" <<"v5u" <<"v5u_s"
	 <<"v5uru" <<"v5" <<"v5aru" <<"v5b" <<"v5g" <<"v5k" <<"v5k_s" <<"v5m"
	 <<"v5n" <<"v5r" <<"v5r_i" <<"v5s" <<"v5t" <<"v5u" <<"v5u_s" <<"v5uru"
	 <<"iv" <<"vi" <<"vk" <<"vs" <<"vs_i" <<"vs_s" <<"vt" <<"vz"
	 <<"adj" <<"adj_na" <<"adj_no" <<"adj_pn" <<"adj_t"
	 <<"adv" <<"adv_n" <<"adv_to"
	 <<"prt";
		//Field of Application terms
	fieldOfApplication<<"Buddh" <<"MA" <<"comp" <<"food" <<"geom"
		<<"ling" <<"math" <<"mil" <<"physics";
		//Miscellaneous Markings (in EDICT terms)
	miscMarkings<<"X" <<"abbr" <<"arch" <<"ateji" <<"chn" <<"col" <<"derog"
		<<"eK" <<"ek" <<"fam" <<"fem" <<"gikun" <<"hon" <<"hum" <<"iK" <<"id"
		<<"io" <<"m-sl" <<"male" <<"male-sl" <<"ng" <<"oK" <<"obs" <<"obsc" <<"ok"
		<<"poet" <<"pol" <<"rare" <<"sens" <<"sl" <<"uK" <<"uk" <<"vulg";
}
