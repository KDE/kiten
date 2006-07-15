/***************************************************************************
 *   Copyright (C) 2001 by Jason Katz-Brown                                *
 *             (C) 2006 by Joseph Kerian  <jkerian@gmail.com>              *
 *             (C) 2006 by Eric Kjeldergaard <kjelderg@gmail.com>          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include <kglobal.h>
#include <qfile.h>
#include <q3dict.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "dictquery.h"  //dictQuery classs
#include "dictFileKanjidic.h" //dictFile (superclass) class
#include "dictFilePreferenceDialog.h"
#include "entry.h"      //Entry and EntryList classes
#include "entryKanjidic.h"


dictFileKanjidic::dictFileKanjidic() : dictFileEdict(){
	dictionaryType = "kanjidic";
	searchableAttributes.insert("bushu","B");
	searchableAttributes.insert("classical","C");
	searchableAttributes.insert("henshall","E");
	searchableAttributes.insert("frequency","F");
	searchableAttributes.insert("grade","G");
	searchableAttributes.insert("halpern","H");
	searchableAttributes.insert("spahn","I");
	searchableAttributes.insert("hadamitzky","I");
	searchableAttributes.insert("gakken","K");
	searchableAttributes.insert("heisig","L");
	searchableAttributes.insert("morohashi","M");
	searchableAttributes.insert("nelson","N");
	searchableAttributes.insert("oneill","O");
	searchableAttributes.insert("skip","P");
	searchableAttributes.insert("4cc","Q");
	searchableAttributes.insert("stroke","S");
	searchableAttributes.insert("strokes","S");
	searchableAttributes.insert("unicode","U");
	searchableAttributes.insert("haig","V");
	searchableAttributes.insert("korean","W");
	searchableAttributes.insert("pinyin","Y");
	searchableAttributes.insert("other","D");
}

/** Scan a potential file for the correct format, remembering to skip comment
  characters. This is not a foolproof scan, but it should be checked before adding
  a new dictionary. 
 */
bool dictFileKanjidic::validDictionaryFile(const QString filename) {
	QFile file(filename);
	if(!file.exists())
		return false;
	if(!file.open(QIODevice::ReadOnly))
		return false;
	//TODO: Some actual format checking of the kanjidic file
	
	file.close();
	return true;
}

/** Reject queries that specify anything we don't understand */
bool dictFileKanjidic::validQuery(const dictQuery &query) {
	//Multi kanji searches don't apply to this file
	if(query.getWord().length() > 1)
		return false;

	//Now check if we have any properties specified that we don't understand
	QStringList properties = query.getPropertyKeysList();
	QStringList propertiesWeHandle = 
		searchableAttributes.values()+searchableAttributes.keys();
	for(QStringList::Iterator it=properties.begin(); it != properties.end(); ++it)
		if(!propertiesWeHandle.contains(*it))
			return false;
	
	return true;
}
	

/** Make a list of all the extra fields in our db.. Entry uses this to decide
  what goes in the interpretations it gives. */
QStringList dictFileKanjidic::listDictDisplayOptions(QStringList list) const {
	QMap<QString,QString> fullList = dictFileKanjidic::displayOptions();
	QMap<QString,QString>::Iterator it;
	for ( it = fullList.begin(); it != fullList.end(); ++it ) {
		list.append(it.key());
	}
	return list;
}

const QMap<QString,QString> dictFileKanjidic::displayOptions() const {
	//Enumerate the fields in our dict.... there are a rather lot of them here
	//It will be useful for a few things to have the full list generated on it's own
	QMap<QString,QString> list;
	//TODO: Figure out how to internationalize these easily
	list.insert("Bushu Number(B)","B");
	list.insert("Classical Radical Number(C)","C");
	list.insert("Henshall's Index Number(E)","E");
	list.insert("Frequency Ranking(F)","F");
	list.insert("Grade Level(G)","G");
	list.insert("Halpern's New J-E Char Dictionary(H)","H");
	list.insert("Spahn & Hadamitzky Reference(I)","I");
	list.insert("Gakken Kanji Dictionary Index(K)","L");
	list.insert("Heisig's Index(L)","L");
	list.insert("Morohashi's Daikanwajiten(M)","M");
	list.insert("Nelsons Modern Reader's J-E Index(N)","N");
	list.insert("O'Neill's 'Japanese Names' Index(O)","O");
	list.insert("SKIP Code(P)","P");
	list.insert("Four Corner codes(Q)","Q");
	list.insert("Stroke Count(S)","S");
	list.insert("Unicode Value(U)","U");
	list.insert("Haig's New Nelson J-E Dict(V)","V");
	list.insert("Korean Reading(W)","W");
	list.insert("kanjidic field: X","X");
	list.insert("Pinyin Reading(Y)","Y");
	list.insert("Common SKIP Misclassifications(Z)","Z");
	list.insert("Misc Dictionary Codes (D)","D");
	return list;
}

DictionaryPreferenceDialog *dictFileKanjidic::preferencesWidget(KConfigSkeleton *config,QWidget *parent, const char *name) {
	dictFileFieldSelector *dialog = new dictFileFieldSelector(config, getType(), parent, name);

	QStringList nullList;
	dialog->addAvailable(listDictDisplayOptions(nullList));
	return dialog;
}
	
