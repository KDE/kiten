/* This file is part of Kiten, a KDE Japanese Reference Tool...
    Copyright (C) 2001 by Jason Katz-Brown 
              (C) 2006 by Joseph Kerian  <jkerian@gmail.com> 
              (C) 2006 by Eric Kjeldergaard <kjelderg@gmail.com> 

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

#include <kconfig.h>
#include <kconfigskeleton.h>
#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kprocess.h>

#include <QtCore/QByteArray>
#include <QtCore/QVector>
#include <QtCore/QString>

#include <qfileinfo.h>
#include <qfile.h>
#include <qtextcodec.h>
#include <QTextStream>

#include <sys/mman.h>

#include "dictFilePreferenceDialog.h"
#include "dictquery.h"  //dictQuery classs
#include "dictFileEDICT.h" //dictFile (superclass) class
#include "entry.h"      //Entry and EntryList classes

/** Per instructions in the super-class, this constructor basically sets the 
  dictionaryType member variable to identify this as an edict-type database handler */
dictFileEdict::dictFileEdict() 
	: dictFile(),
	dictPtr((unsigned char*) MAP_FAILED),
	indexPtr((uint32_t*) MAP_FAILED),
	valid(false)
{
	dictionaryType = "edict";
}

QStringList *dictFileEdict::displayFieldsList = NULL;
QStringList *dictFileEdict::displayFieldsFull = NULL;

/** The destructor... ditch our memory maps and close our files here 
  (if they were open) */
dictFileEdict::~dictFileEdict() {
	if(valid) {
		munmap((char*)dictPtr,dictFile.size());
		munmap((char*)indexPtr,indexFile.size());
		dictFile.close();
		indexFile.close();
	}
}

/** Scan a potential file for the correct format, remembering to skip comment
  characters. This is not a foolproof scan, but it should be checked before adding
  a new dictionary. 
  Valid EDICT format is considered:
  <kanji or kana>+ [<kana>] /latin characters & symbols/seperated with slashes/
  Comment lines start with... something... not remembering now.
 */
bool dictFileEdict::validDictionaryFile(const QString filename) {
	QFile file(filename);
	int totalLineCounter = 0;
	bool returnFlag = true;          

	if(!file.exists())	//The easy test... does it exist?
		return false;
	if(!file.open(QIODevice::ReadOnly)) //And can we read it?
		return false;
	
	//Now we can actually check the file
	QTextStream fileStream(&file);
	fileStream.setCodec(QTextCodec::codecForName("eucJP"));
	QString commentMarker("？？？？"); //Note: Don't touch this! vim seems to have
														//An odd text codec error here too :(
	QRegExp formattedLine("^\\S+\\s+(\\[\\S+\\]\\s+)?/.*/$");
	while( !fileStream.atEnd() ) {
		QString line = fileStream.readLine();
		totalLineCounter++;

		if(line.left(4)==commentMarker)
			continue;
		if(line.contains(formattedLine)) //If it matches our regex
			continue;
		
		returnFlag = false;
		break;
	}
	
	file.close();
	return returnFlag;
}

/** Reject queries that specify anything we don't understand */
//TODO: Actually write this method (validQuery)
bool dictFileEdict::validQuery(const dictQuery &query) {
	return true;
}
	
/** Load up the dictionary... at this point we assume that this is a reload from
  previously... to this class, it means that we assume there is an index file
  already built. If not, (or the index is outdated), we call loadNewDictionary()*/
//TODO: Eliminate the memory mapping from this setup. It should not be needed with
//	modern hardware... get rid of the C calls in favor of QT file handling
// And import the index generator into C++/QT.
bool dictFileEdict::loadDictionary(const QString fileName, const QString dictName) {
	if(valid) return false; //Already loaded

	//Check format here?

	dictFile.setFileName(fileName);
	dictPtr = (const unsigned char *)MAP_FAILED;
	indexFile.setFileName(KGlobal::dirs()->saveLocation("data","kiten/xjdx/",true)+
										QFileInfo(fileName).baseName() + ".xjdx");
	indexPtr = (const uint32_t *)MAP_FAILED;
	
	if(!dictFile.exists())	//If there is no dictionary file... bail
		return false;
	if(!indexFile.exists())	//If there is no index... build one
		return loadNewDictionary(fileName,dictName);
	
	//An index file exists... is it the right version/size?
	//Index files store the bytesize of the dictionary + file version + 1 in the first
	//data entry point.
	int dictionaryLength = dictFile.size();
	dictionaryLength++;

	int32_t indexVersionTest;
	if(!indexFile.open(QIODevice::ReadOnly))
		return false;
	
	//If we can read the first four bytes
	if(4 == indexFile.read((char*)(&indexVersionTest),4)) {
		//If those four bytes match the version number + size + 1
		if(indexVersionTest == dictionaryLength + indexFileVersion ) {
			//Memory Map the index file
			indexPtr = (const uint32_t*)mmap(0, indexFile.size(), PROT_READ, MAP_SHARED,
					indexFile.handle(), 0);
			//If we successfully memory map the file
			if(indexPtr != (uint32_t*)MAP_FAILED) {
	
				//Now we open up the dictionary file
				if(dictFile.open(QIODevice::ReadOnly)) {
					//And memory map the dictionary file
					dictPtr = (const unsigned char *)mmap(0, dictFile.size(), PROT_READ,
							MAP_SHARED, dictFile.handle(), 0);
					//If we succeed in all things, return true
					if(dictPtr != (unsigned char*) MAP_FAILED) {
						//Mark ourselves as a valid memmapped dict!
						valid = true; 
						dictionaryName=dictName;
						dictionaryFile=fileName;
						return true;
					} else {
						dictFile.close();
					}
				}
				
				//If we reach here... the index mmap was setup, but we failed later
				munmap((char*)indexPtr,indexFile.size()); //Tear down the index mmap
				indexFile.close();
				return false;	//Problems with the dictionary won't be solved by
									//rebuilding the index file... so give up here.
			}
		}
	}
	//If we reach here... we opened the index file but one of the checks failed
	indexFile.close();
	kDebug() << "Opening dictionary failure" <<endl;
	return loadNewDictionary(fileName,dictName);
}


/** Load up a dictionary for the first time. Mainly this needs to build the
  index, then open the mmaps. Potentially, this could take a very long time. */
bool dictFileEdict::loadNewDictionary(const QString fileName, const QString dictName) {
	if(valid) return false; //Already loaded

	if(!validDictionaryFile(fileName)) return false; //Invalid dict file
	
	dictFile.setFileName(fileName);
	dictPtr = (const unsigned char *)MAP_FAILED;
	indexFile.setFileName(KGlobal::dirs()->saveLocation("data","kiten/xjdx/",true)+
										QFileInfo(fileName).baseName() + ".xjdx");
	indexPtr = (const uint32_t *)MAP_FAILED;

	if(!dictFile.exists())	//If there is no dictionary file... bail
		return false;

	//Unlike before... we assume the index is either invalid or not there
	KProcess proc;
	proc << KStandardDirs::findExe("kitengen") << fileName << indexFile.fileName();
	//TODO: Status dialog or something
	proc.start(KProcess::Block,KProcess::NoCommunication);
	
	
	int dictionaryLength = dictFile.size();
	dictionaryLength++;

	int32_t indexVersionTest;
	if(!indexFile.open(QIODevice::ReadOnly))
		return false;
	
	//If we can read the first four bytes
	if(4 == indexFile.read((char*)(&indexVersionTest),4)) {
		//If those four bytes match the version number + size + 1
		if(indexVersionTest == dictionaryLength + indexFileVersion ) {
			//Memory Map the index file
			indexPtr = (const uint32_t*)mmap(0, indexFile.size(), PROT_READ, MAP_SHARED,
					indexFile.handle(), 0);
			//If we successfully memory map the file
			if(indexPtr != (uint32_t*)MAP_FAILED) {
	
				//Now we open up the dictionary file
				if(dictFile.open(QIODevice::ReadOnly)) {
					//And memory map the dictionary file
					dictPtr = (const unsigned char *)mmap(0, dictFile.size(), PROT_READ,
							MAP_SHARED, dictFile.handle(), 0);
					//If we succeed, return true
					if(dictPtr != (unsigned char*) MAP_FAILED) {
						//Mark ourselves as a valid memmapped dict!
						valid = true; 
						dictionaryName=dictName;
						dictionaryFile=fileName;
						return true;
					} else {
						dictFile.close();
					}
				}
				
				//If we reach here... the index mmap was setup, but we failed later
				munmap((char*)indexPtr,indexFile.size()); //Tear down the index mmap
			}
		}
	}
	//If we reach here... we opened the index file but one of the checks failed
	indexFile.close();
	return false;
}


/**This is a safety class to allow us to easily access the memory mapped arrays
	It essentially allows us to typecast and access the entirety of the mmap as
	an array. It provides some safety over QMemArray directly because it ensures
 	that it will call the resetRawData method. */
template <class T> class memMapArray {
	public:
		memMapArray(T*,int);
		virtual ~memMapArray();
		int size();
	private:
		T *data;
		int dataSize;
		int sizeOf;
};
template<class T> memMapArray<T>::memMapArray(T *d, int s) :
													 data(d),dataSize(s) {
	sizeOf = sizeof(T);
//	setRawData(data,dataSize/sizeof(T));
}

template<class T> int memMapArray<T>::size() {
	return dataSize/sizeOf;
}

template<class T> memMapArray<T>::~memMapArray() {
//	resetRawData(data,dataSize/sizeof(T));
}

/** Do a search, respond with a list of entries.
 The general strategy will be to take the first word of the query, and do a
 binary search on the dictionary for that item. Take all results and filter
 them using the rest of the query with the validate method.
 */
EntryList *dictFileEdict::doSearch(const dictQuery &query) {
	kDebug()<< "Search from : " << getName() << endl;
	if(query.isEmpty() || !valid)	//No query or dict, no results.
		return new EntryList();
	
	//Convert the first word of the query to euc for the binary search
	//TODO: Fix this to search for meaning, pronounciation, then entry
	//TODO:If searching for entry... we need to modify the search mechanism
	//TODO:Right now it fails if we search for more than one Kanji in a row
	QTextCodec &codec = *QTextCodec::codecForName("eucJP");
	QByteArray searchString = codec.fromUnicode
		(query.toString().split(dictQuery::mainDelimiter).first());

	//Map the memory maps to more convenient data types.
	memMapArray <const unsigned char> dict(dictPtr,dictFile.size());
	memMapArray <const uint32_t> index(indexPtr,indexFile.size());
	
	int lo = 0;
	int hi = index.size() - 1; //This is not the same as indexFile.size() 
																//(that would be bytes)
	unsigned cur;
	int comp = 0;
	QByteArray currentWord;

	//Conduct a binary search of the memory maps to find our word
	do {
		cur = (hi + lo) / 2;
		comp = equalOrSubstring(searchString,lookupDictLine(cur));
		if(comp < 0)
			lo = cur+1;
		if(comp > 0)
			hi = cur-1;
	}while(hi >= lo && comp != 0 && !(hi==0 && lo==0));
	
	EntryList *results= new EntryList();
	
	if(comp != 0)	//If there were no matches... return an empty list
		return results;

	//Rewind in the index file to make sure we have the first match
	while(cur - 1 && 0 == equalOrSubstring(searchString,lookupDictLine(cur-1)))
		--cur;
	
	//Enumerate each matching entry
	QVector<uint32_t> possibleHits;

	//This is a bit tricky... we do the search over the index, and keep going
	//Over all combinations of currentWord and searchString (either can be properly
	//contained in the other, etc).	

	currentWord = lookupDictLine(cur);
	do {

		int comparison = findMatches(searchString,currentWord);
	
		currentWord = lookupDictLine(++cur);
		
		if(query.getMatchType() == dictQuery::matchExact && 0 != comparison)
				continue;
		if( (query.getMatchType() == dictQuery::matchBeginning 
				|| query.getMatchType() == dictQuery::matchAnywhere) && comparison < 0)
				continue;
		//We can't really implement the others with this index format
		//At least not in a reasonable time
		
		//The index does not actually point at the start of the line... rewind 
		int i=0;                                           //   to the newline
		while(lookupDictChar(indexPtr[cur-1]+i-2) != 0x0A) 
			--i;
		possibleHits.push_back(indexPtr[cur-1]+i-1);
	
	}while(cur < index.size() && 0 == equalOrSubstring(searchString,currentWord));
		
	if(possibleHits.size() <= 0)
		return results;
	
	qSort(possibleHits);
	unsigned last = dict.size() + 2;
	Entry *result;
	foreach(uint32_t it, possibleHits) {
		//Don't print the same line	
		if(last != it) {
			last = it;
			//Grab a Line, translate it from euc, make an entry
			result = makeEntry(codec.toUnicode(lookupFullLine(it)));
			//Evaluate more carefully
			if(result->matchesQuery(query))
			//Add to list if acceptable
				results->append(result);
		}
	}
		
	return results;
}

/** Similer to it's larger cousin below, this scans the dictionary at
  a particular location and extracts the unsigned char at a particular
  location within the file/memorymap, offset from the start of the
  dictionary */
inline unsigned char dictFileEdict::lookupDictChar(unsigned i) {
	if(i > dictFile.size()) return 0x0A;	//If out of bounds, return endl
	return dictPtr[i];
}

/** This is just like lookupDictChar, but grabs till EOL */
QByteArray dictFileEdict::lookupFullLine(unsigned i) {
	if(i > dictFile.size())
		return QByteArray(0x0A,1);	//If out of bounds, return endl
	uint32_t start = i;
	uint32_t pos = start;
	const unsigned max = dictFile.size();
	while(pos <= max && dictPtr[pos] != 0 && dictPtr[pos] != 0x0A)
		++pos;
	QByteArray retval((const char*)(dictPtr+start),1+pos-start);
	//and away we go
	return retval;
}
	
/** This quick utility method looks in index at location i and pulls,
  the corresponding line from the dictionary  returning it as an euc
  formatted QCString. i=1 is the first entry that the index points to. */
QByteArray dictFileEdict::lookupDictLine(unsigned i) {
	if(i > dictFile.size()) return QByteArray("");
	
	uint32_t start = indexPtr[i] - 1;
	uint32_t pos = start;
	const unsigned size = dictFile.size();
	//Grab the whole word
	//As long as we don't get EOF, null or newline... keep going forward
	while(pos<=size && dictPtr[pos] != 0 && dictPtr[pos] != 0x0A) {
		++pos;
	}
	//Copy the word to a QCString
	QByteArray retval((const char*)(dictPtr+start),1+pos-start);
	//and away we go
	return retval;
}

/** A small set of EUC formatted string comparison functions
  that will handle katakana and hiragana as the same, and compare both
  kanji and latin chars in the same function. Handy for a few sections
  of the doSearch() routine.
  This version returns 0 if the first string is equal to or the same
  as the beginning of the second string
  */
int dictFileEdict::equalOrSubstring(const char *str1, const char *str2) {
	for(unsigned i=0; ; ++i) {
		unsigned char c1 = static_cast<unsigned char>(str1[i]);
		unsigned char c2 = static_cast<unsigned char>(str2[i]);

		if( (c1 == '\0') )
			return 0;
		
		if((i%2) == 0) { //on the highbyte (of kana)
			if(c2 == 0xA5) //Make katakana and hiragana equivelent
				c2 = 0xA4;
			if(c1 == 0xA5)
				c1 = 0xA4;
		}
		
		if(('A' <= c1) && (c1 <= 'Z')) c1 |= 0x20; // 'fix' uppercase
		if(('A' <= c2) && (c2 <= 'Z')) c2 |= 0x20;

		if(c1 != c2)
			return (int)c2 - (int)c1;
	}
	return 0; //silly compiler requirements
}

/** A small set of EUC formatted string comparison functions
  that will handle katakana and hiragana as the same, and compare both
  kanji and latin chars in the same function. Handy for a few sections
  of the doSearch() routine.
  findMatches() is another string comparer, but unlike the other one
  compares strictly (with an exepmption for punctuation).
  */
int dictFileEdict::findMatches(const char *str1, const char *str2) {
	
#define EUC_LATIN_CHARACTER(x) (('a'<=x && x<='z')||(x==0xA4)||(x==0x80))

	for(unsigned i=0; ; ++i) {
		unsigned char c1 = static_cast<unsigned char>(str1[i]);
		unsigned char c2 = static_cast<unsigned char>(str2[i]);

		if((i%2) == 0) { //on the highbyte (of kana)
			if(c2 == 0xA5) //Make katakana and hiragana equivelent
				c2 = 0xA4;
			if(c1 == 0xA5)
				c1 = 0xA4;
		}
		
		if(('A' <= c1) && (c1 <= 'Z')) c1 |= 0x20; // 'fix' uppercase
		if(('A' <= c2) && (c2 <= 'Z')) c2 |= 0x20;
		
		if( c1 == '\0' ) {
			if( !EUC_LATIN_CHARACTER(c2) )
				return 0;
			return c2;
		}

		if(c1 != c2)
			return (int)c2 - (int)c1;
	}
	return 0; //silly compiler requirements
}




QStringList dictFileEdict::listDictDisplayOptions(QStringList x) const {
	return x; //Only the basic ones
}

DictionaryPreferenceDialog *dictFileEdict::preferencesWidget(KConfigSkeleton *config, QWidget *parent, const char *name) {
	DictionaryPreferenceDialog *dialog = new dictFileFieldSelector(config, getType(),parent,name);
	//For EDICT, don't change the default display options
	//TODO: Add a few fields
	return dialog;
}

void
dictFileEdict::loadSettings(KConfigSkeleton *config) {
	//We assume that our preference dialog got the chance to make it's own settings...
	QStringList defaultList("Word/Kanji"),fullOrder,listOrder;
	defaultList << "Reading" << "Meaning";

	KConfigSkeletonItem *item = config->findItem(getType()+"__displayFieldsFullView");
	if(item != NULL)
		fullOrder = item->property().toStringList();
	if(!fullOrder.isEmpty()) {
		if(displayFieldsFull != NULL)
			delete displayFieldsFull;
		dictFileEdict::displayFieldsFull = new QStringList(fullOrder);
		kDebug() << "Setup List: "<< dictFileEdict::displayFieldsFull->join(",")<<endl;
	}
	
	item = config->findItem(getType()+"__displayFieldsListView");
	if(item != NULL)
		listOrder = item->property().toStringList();
	if(!listOrder.isEmpty()) {
		if(displayFieldsList != NULL)
			delete displayFieldsList;
		dictFileEdict::displayFieldsList = new QStringList(listOrder);
		kDebug() << "Setup List: "<< dictFileEdict::displayFieldsList->join(",")<<endl;
	}
}

QStringList
dictFileEdict::getDisplayList(QString type) {
	QStringList *list = type=="Full"?dictFileEdict::displayFieldsFull:
								dictFileEdict::displayFieldsList;
	if(list == NULL) 
		return QStringList();
	else
		return *list;
}

