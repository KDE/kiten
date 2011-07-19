/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#include "dictfilekanjidic.h"

#include "dictquery.h"
#include "entrykanjidic.h"
#include "entrylist.h"

#include <KConfigSkeleton>
#include <KDebug>
#include <KGlobal>

#include <QFile>
#include <QTextCodec>

QStringList *DictFileKanjidic::displayFields = NULL;

DictFileKanjidic::DictFileKanjidic()
: DictFile( "kanjidic" )
{
  m_dictionaryType = "kanjidic"; //Override the default type
  m_searchableAttributes.clear();
  m_searchableAttributes.insert( "bushu",      "B" );
  m_searchableAttributes.insert( "classical",  "C" );
  m_searchableAttributes.insert( "henshall",   "E" );
  m_searchableAttributes.insert( "frequency",  "F" );
  m_searchableAttributes.insert( "grade",      "G" );
  m_searchableAttributes.insert( "halpern",    "H" );
  m_searchableAttributes.insert( "spahn",      "I" );
  m_searchableAttributes.insert( "hadamitzky", "I" );
  m_searchableAttributes.insert( "gakken",     "K" );
  m_searchableAttributes.insert( "heisig",     "L" );
  m_searchableAttributes.insert( "morohashi",  "M" );
  m_searchableAttributes.insert( "nelson",     "N" );
  m_searchableAttributes.insert( "oneill",     "O" );
  m_searchableAttributes.insert( "skip",       "P" );
  m_searchableAttributes.insert( "4cc",        "Q" );
  m_searchableAttributes.insert( "stroke",     "S" );
  m_searchableAttributes.insert( "strokes",    "S" );
  m_searchableAttributes.insert( "unicode",    "U" );
  m_searchableAttributes.insert( "haig",       "V" );
  m_searchableAttributes.insert( "korean",     "W" );
  m_searchableAttributes.insert( "pinyin",     "Y" );
  m_searchableAttributes.insert( "other",      "D" );
}

DictFileKanjidic::~DictFileKanjidic()
{
}

QMap<QString,QString> DictFileKanjidic::displayOptions() const
{
  // Enumerate the fields in our dict.... there are a rather lot of them here
  // It will be useful for a few things to have the full list generated on it's own
  QMap<QString,QString> list;
  // TODO: Figure out how to internationalize these easily
  list.insert( "Bushu Number(B)",                      "B");
  list.insert( "Classical Radical Number(C)",          "C");
  list.insert( "Henshall's Index Number(E)",           "E");
  list.insert( "Frequency Ranking(F)",                 "F");
  list.insert( "Grade Level(G)",                       "G");
  list.insert( "Halpern's New J-E Char Dictionary(H)", "H");
  list.insert( "Spahn & Hadamitzky Reference(I)",      "I");
  list.insert( "Gakken Kanji Dictionary Index(K)",     "K");
  list.insert( "Heisig's Index(L)",                    "L");
  list.insert( "Morohashi's Daikanwajiten(M)",         "M");
  list.insert( "Nelsons Modern Reader's J-E Index(N)", "N");
  list.insert( "O'Neill's 'Japanese Names' Index(O)",  "O");
  list.insert( "SKIP Code(P)",                         "P");
  list.insert( "Four Corner codes(Q)",                 "Q");
  list.insert( "Stroke Count(S)",                      "S");
  list.insert( "Unicode Value(U)",                     "U");
  list.insert( "Haig's New Nelson J-E Dict(V)",        "V");
  list.insert( "Korean Reading(W)",                    "W");
  list.insert( "kanjidic field: X",                    "X");
  list.insert( "Pinyin Reading(Y)",                    "Y");
  list.insert( "Common SKIP Misclassifications(Z)",    "Z");
  list.insert( "Misc Dictionary Codes (D)",            "D");
  return list;
}

EntryList* DictFileKanjidic::doSearch( const DictQuery &query )
{
  if( query.isEmpty() || ! m_validKanjidic )
  {
    return new EntryList();
  }

  kDebug() << "Search from:" << getName() << endl;
  QString searchQuery = query.getWord();
  if( searchQuery.length() == 0 )
  {
    searchQuery = query.getPronunciation();
    if( searchQuery.length() == 0 )
    {
      searchQuery = query.getMeaning().split( ' ' ).first().toLower();
      if( searchQuery.length() == 0 )
      {
        QList<QString> keys = query.listPropertyKeys();
        if( keys.size() == 0 )
        {
          return new EntryList();
        }
        searchQuery = keys[ 0 ];
        searchQuery = searchQuery + query.getProperty( searchQuery );
      }
    }
  }

  EntryList *results = new EntryList();
  foreach( const QString &line, m_kanjidic )
  {
    if( line.contains( searchQuery ) )
    {
      results->append( makeEntry( line ) );
    }
  }

  return results;
}

QStringList DictFileKanjidic::dumpDictionary()
{
  if( ! m_validKanjidic )
  {
    return QStringList();
  }

  return m_kanjidic;
}

inline Entry* DictFileKanjidic::makeEntry( QString entry )
{
  return new EntryKanjidic( getName(), entry );
}

QStringList DictFileKanjidic::listDictDisplayOptions( QStringList list ) const
{
  list += displayOptions().keys();
  return list;
}

bool DictFileKanjidic::loadDictionary( const QString &file, const QString &name )
{
  if( ! m_kanjidic.isEmpty() )
  {
    return true;
  }

  QFile dictionary( file );
  if( ! dictionary.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    return false;
  }

  kDebug() << "Loading kanjidic from:" << file << endl;

  QTextStream fileStream( &dictionary );
  fileStream.setCodec( QTextCodec::codecForName( "eucJP" ) );

  QString currentLine;
  while( ! fileStream.atEnd() )
  {
    currentLine = fileStream.readLine();
    if( currentLine[ 0 ] != '#' )
    {
      m_kanjidic << currentLine;
    }
  }

  dictionary.close();

  if( ! validDictionaryFile( file ) )
  {
    return false;
  }

  m_dictionaryName = name;
  m_dictionaryFile = file;

  return true;
}

QStringList* DictFileKanjidic::loadListType(  KConfigSkeletonItem *item
                                            , QStringList *list
                                            , const QMap<QString,QString> &long2short )
{
  QStringList listFromItem;

  if( item != NULL )
  {
    listFromItem = item->property().toStringList();
  }

  if( ! listFromItem.isEmpty() )
  {
    delete list;

    list = new QStringList();
    foreach( const QString &it, listFromItem )
    {
      if( long2short.contains( it ) )
      {
        list->append( long2short[ it ] );
      }
    }
  }

  return list;
}

void DictFileKanjidic::loadSettings()
{
  QMap<QString,QString> list = displayOptions();
  list[ "Word/Kanji" ]  = "Word/Kanji";
  list[ "Reading" ]     = "Reading";
  list[ "Meaning" ]     = "Meaning";
  list[ "--Newline--" ] = "--Newline--";

  this->displayFields = new QStringList( list.values() );
}

void DictFileKanjidic::loadSettings( KConfigSkeleton *config )
{
  QMap<QString,QString> list = displayOptions();
  list[ "Word/Kanji" ]  = "Word/Kanji";
  list[ "Reading" ]     = "Reading";
  list[ "Meaning" ]     = "Meaning";
  list[ "--Newline--" ] = "--Newline--";

  KConfigSkeletonItem *item = config->findItem( getType() + "__displayFields" );
  this->displayFields = loadListType( item, this->displayFields, list );
}

/**
 * Scan a potential file for the correct format, remembering to skip comment
 * characters. This is not a foolproof scan, but it should be checked before adding
 * a new dictionary.
 */
bool DictFileKanjidic::validDictionaryFile( const QString &filename )
{
  QFile file( filename );
  if( ! file.exists() || ! file.open( QIODevice::ReadOnly ) )
  {
    return false;
  }

  QTextStream fileStream( &file );
  fileStream.setCodec( QTextCodec::codecForName( "eucJP" ) );

  QRegExp format( "^\\S\\s+(\\S+\\s+)+(\\{(\\S+\\s?)+\\})+" );
  m_validKanjidic = true;
  while( ! fileStream.atEnd() )
  {
    QString currentLine = fileStream.readLine();

    if( currentLine[ 0 ] == '#' )
    {
      continue;
    }
    else if( currentLine.contains( format ) )
    {
      continue;
    }

    m_validKanjidic = false;
    break;
  }

  file.close();
  return m_validKanjidic;
}

/**
 * Reject queries that specify anything we don't understand
 */
bool DictFileKanjidic::validQuery( const DictQuery &query )
{
  //Multi kanji searches don't apply to this file
  if( query.getWord().length() > 1 )
  {
    return false;
  }

  //Now check if we have any properties specified that we don't understand
  QStringList propertiesWeHandle = m_searchableAttributes.values() + m_searchableAttributes.keys();
  propertiesWeHandle += "common"; // We map this to be (has a G value)

  const QStringList properties = query.listPropertyKeys();
  for( QStringList::const_iterator it = properties.constBegin(); it != properties.constEnd(); ++it )
  {
    if( ! propertiesWeHandle.contains( *it ) )
    {
      return false;
    }
  }

  return true;
}
