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
			outString += makeLink(inString.at(i));
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
	if (common)
		return "<span>Common</span>";
	else
		return QString();
}

/* DATA LOADING FUNCTIONS */

/* TODO: extendedInfo as described on Breen's site
	http://www.csse.monash.edu.au/~jwb/edict_doc.html */
/** Take a QString and load it into the Entry as appropriate */
bool EntryEDICT::loadEntry(const QString &entryLine)
{
	/* Check the requirements of validity first */
	/* EDICT requires at least two '/' marks and a ' ' */
	if(entryLine.count("/") < 2 || entryLine.at(0)==' ') //KDE4 CHANGE
	{
//		kDebug() << "EDICT Parser received bad data! : "<<entryLine;
		return false;
	}


	/* Set tempQString to be the reading and word portion of the entryLine */
	QString tempQString = entryLine.left(entryLine.indexOf('/'));
	/* The actual Word is the beginning of the line */
	Word = tempQString.left(tempQString.indexOf(' '));

	/* The Reading is either Word or encased in '[' */
	Readings.clear();
	int startOfReading = tempQString.indexOf('[');
	if(startOfReading != -1)  // This field is optional for kiten
		Readings.append(
				tempQString.left(tempQString.lastIndexOf(']')).mid(startOfReading+1));

	/* set Meanings to be all of the meanings in the definition */
	Meanings = entryLine.left(entryLine.lastIndexOf('/')).mid(tempQString.length()).split('/', QString::SkipEmptyParts);
	common = (Meanings.last() == "(P)");
	if (common) Meanings.removeLast();

	QString firstWord = Meanings.first();
	QStringList stringTypes;

	//TODO: optimize so later parentheses are ignored
	for (int i = firstWord.indexOf("("); i != -1; i = firstWord.indexOf("(", i + 1))
	{
		QString parantheses = firstWord.mid(i + 1, firstWord.indexOf(")", i) - i - 1);
		stringTypes += parantheses.split(",");
	}

	foreach( const QString &str, stringTypes)
	{
		if (WordTypes().contains(str))
		{
			types += WordTypes().value(str);
		}
	}

//	kdDebug()<< "Parsed: '"<<Word<<"' ("<<Readings.front()<<") \""<<
//		Meanings.join("|")<<"\" from :"<<entryLine<<endl;
	return true;
}

/** Regenerate a QString like the one we got in loadEntry() */
QString EntryEDICT::dumpEntry() const
{
	return Word +
		((Readings.count() == 0) ? " " : " [" + Readings.first() + "] ")
		+ '/' + Meanings.join("/") + '/';
}

/* TYPES RELATED FUNCTIONS */

QHash<QString, EntryEDICT::WordType> *EntryEDICT::wordTypes = NULL;

const QHash<QString, EntryEDICT::WordType>& EntryEDICT::WordTypes()
{
	if (wordTypes == NULL)
	{
		wordTypes = new QHash<QString, EntryEDICT::WordType>;
		(*wordTypes)["adj"] = adj;  /*	adjective (keiyoushi) */
		(*wordTypes)["adj-na"] = adj_na;  /*	adjectival nouns or quasi-adjectives (keiyodoshi) */
		(*wordTypes)["adj-no"] = adj_no;  /*	nouns which may take the genitive case particle `no' */
		(*wordTypes)["adj-pn"] = adj_pn;  /*	pre-noun adjectival (rentaishi) */
		(*wordTypes)["adj-t"] = adj_t;  /*	`taru' adjective */
		(*wordTypes)["adv"] = adv;  /*	adverb (fukushi) */
		(*wordTypes)["adv-n"] = adv_n;  /*	adverbial noun */
		(*wordTypes)["adv-to"] = adv_to;  /*	adverb taking the `to' particle */
		(*wordTypes)["aux"] = aux;  /*	auxiliary */
		(*wordTypes)["aux-v"] = aux_v;  /*	auxiliary verb */
		(*wordTypes)["aux-adj"] = aux_adj;  /*	auxiliary adjective */
		(*wordTypes)["conj"] = conj;  /*	conjunction */
		(*wordTypes)["exp"] = exp; /*	Expressions (phrases, clauses;  etc.) */
		(*wordTypes)["id"] = id;  /*	idiomatic expression */
		(*wordTypes)["inte"] = inte;  /*	interjection (kandoushi)
										(*wordTypes)["int"] = int is a keyword;  thus 'inte' instead */
		(*wordTypes)["iv"] = iv;  /*	irregular verb */
		(*wordTypes)["n"] = n;  /*	noun (common) (futsuumeishi) */
		(*wordTypes)["n-adv"] = n_adv;  /*	adverbial noun (fukushitekimeishi) */
		(*wordTypes)["n-pref"] = n_pref; /*	noun;  used as a prefix */
		(*wordTypes)["n-suf"] = n_suf; /*	noun;  used as a suffix */
		(*wordTypes)["n-t"] = n_t;  /*	noun (temporal) (jisoumeishi) */
		(*wordTypes)["neg"] = neg; /*	negative (in a negative sentence;  or with negative verb) */
		(*wordTypes)["neg-v"] = neg_v;  /*	negative verb (when used with) */
		(*wordTypes)["num"] = num;  /*	numeric */
		(*wordTypes)["pref"] = pref;  /*	prefix */
		(*wordTypes)["prt"] = prt;  /*	particle */
		(*wordTypes)["suf"] = suf;  /*	suffix */
		(*wordTypes)["v1"] = v1;  /*	Ichidan verb */
		(*wordTypes)["v5"] = v5;  /*	Godan verb (not completely classified) */
		(*wordTypes)["v5aru"] = v5aru;  /*	Godan verb - -aru special class */
		(*wordTypes)["v5b"] = v5b;  /*	Godan verb with `bu' ending */
		(*wordTypes)["v5g"] = v5g;  /*	Godan verb with `gu' ending */
		(*wordTypes)["v5k"] = v5k;  /*	Godan verb with `ku' ending */
		(*wordTypes)["v5k-s"] = v5k_s;  /*	Godan verb - iku/yuku special class */
		(*wordTypes)["v5m"] = v5m;  /*	Godan verb with `mu' ending */
		(*wordTypes)["v5n"] = v5n;  /*	Godan verb with `nu' ending */
		(*wordTypes)["v5r"] = v5r;  /*	Godan verb with `ru' ending */
		(*wordTypes)["v5r-i"] = v5r_i;  /*	Godan verb with `ru' ending (irregular verb) */
		(*wordTypes)["v5s"] = v5s;  /*	Godan verb with `su' ending */
		(*wordTypes)["v5t"] = v5t;  /*	Godan verb with `tsu' ending */
		(*wordTypes)["v5u"] = v5u;  /*	Godan verb with `u' ending */
		(*wordTypes)["v5u-s"] = v5u_s;  /*	Godan verb with `u' ending (special class) */
		(*wordTypes)["v5uru"] = v5uru;  /*	Godan verb - uru old class verb (old form of Eru) */
		(*wordTypes)["vi"] = vi;  /*	intransitive verb */
		(*wordTypes)["vk"] = vk;  /*	kuru verb - special class */
		(*wordTypes)["vs"] = vs;  /*	noun or participle which takes the aux. verb suru */
		(*wordTypes)["vs-i"] = vs_i;  /*	suru verb - irregular */
		(*wordTypes)["vs-s"] = vs_s;  /*	suru verb - special class */
		(*wordTypes)["vt"] = vt;  /*	transitive verb */
		(*wordTypes)["vz"] = vz;  /*	zuru verb - (alternative form of -jiru verbs) */
	}
	return *wordTypes;
}


QMultiHash<QString, EntryEDICT::WordType> *EntryEDICT::wordTypesPretty = NULL;
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

const QMultiHash<QString, EntryEDICT::WordType>& EntryEDICT::WordTypesPretty()
{
	if (wordTypesPretty == NULL)
	{
		QString nounType(i18nc("This must be a single word","noun"));
		QString verbType(i18nc("This must be a single word","verb"));
		QString ichidan(i18nc("This is a technical japanese linguist's term... and probably should not be translated, this must be a single word","ichidan"));
		QString godan(i18nc("This is a technical japanese linguist's term... and probably should not be translated, this must be a single word","godan"));
		QString adjective(i18nc("this must be a single word","adjective"));
		QString adverb(i18nc("This must be a single word","adverb"));
		QString particle(i18nc("This must be a single word","particle"));

		wordTypesPretty = new QMultiHash<QString, EntryEDICT::WordType>;

		//Nouns
		wordTypesPretty->insert(nounType,  n);
		wordTypesPretty->insert(nounType,  n_adv);
		wordTypesPretty->insert(nounType,  n_pref);
		wordTypesPretty->insert(nounType,  n_suf);
		wordTypesPretty->insert(nounType,  n_t);
		wordTypesPretty->insert(nounType,  adv_n);
		//Ichidan Verbs
		wordTypesPretty->insert(verbType,  v1);
		wordTypesPretty->insert(ichidan,  v1);
		//Godan Verbs
		wordTypesPretty->insert(verbType,  v5);
		wordTypesPretty->insert(verbType,  v5aru);
		wordTypesPretty->insert(verbType,  v5b);
		wordTypesPretty->insert(verbType,  v5g);
		wordTypesPretty->insert(verbType,  v5k);
		wordTypesPretty->insert(verbType,  v5k_s);
		wordTypesPretty->insert(verbType,  v5m);
		wordTypesPretty->insert(verbType,  v5n);
		wordTypesPretty->insert(verbType,  v5r);
		wordTypesPretty->insert(verbType,  v5r_i);
		wordTypesPretty->insert(verbType,  v5s);
		wordTypesPretty->insert(verbType,  v5t);
		wordTypesPretty->insert(verbType,  v5u);
		wordTypesPretty->insert(verbType,  v5u_s);
		wordTypesPretty->insert(verbType,  v5uru);
		wordTypesPretty->insert(godan,  v5);
		wordTypesPretty->insert(godan,  v5aru);
		wordTypesPretty->insert(godan,  v5b);
		wordTypesPretty->insert(godan,  v5g);
		wordTypesPretty->insert(godan,  v5k);
		wordTypesPretty->insert(godan,  v5k_s);
		wordTypesPretty->insert(godan,  v5m);
		wordTypesPretty->insert(godan,  v5n);
		wordTypesPretty->insert(godan,  v5r);
		wordTypesPretty->insert(godan,  v5r_i);
		wordTypesPretty->insert(godan,  v5s);
		wordTypesPretty->insert(godan,  v5t);
		wordTypesPretty->insert(godan,  v5u);
		wordTypesPretty->insert(godan,  v5u_s);
		wordTypesPretty->insert(godan,  v5uru);
		//Other Verbs
		wordTypesPretty->insert(verbType,  iv);
		wordTypesPretty->insert(verbType,  vi);
		wordTypesPretty->insert(verbType,  vk);
		wordTypesPretty->insert(verbType,  vs);
		wordTypesPretty->insert(verbType,  vs_i);
		wordTypesPretty->insert(verbType,  vs_s);
		wordTypesPretty->insert(verbType,  vt);
		wordTypesPretty->insert(verbType,  vz);
		//Adjectives
		wordTypesPretty->insert(adjective,adj);
		wordTypesPretty->insert(adjective,adj_na);
		wordTypesPretty->insert(adjective,adj_no);
		wordTypesPretty->insert(adjective,adj_pn);
		wordTypesPretty->insert(adjective,adj_t);
		//Adverbs
		wordTypesPretty->insert(adverb,adv);
		wordTypesPretty->insert(adverb,adv_n);
		wordTypesPretty->insert(adverb,adv_to);
		//Particle
		wordTypesPretty->insert(particle,prt);
	}
	return *wordTypesPretty;
}

