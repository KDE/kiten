/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
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

#include "radicalfile.h"

#include <QFile>
#include <QString>
#include <QTextCodec>
#include <QTextStream>

RadicalFile::RadicalFile( QString &radkfile )
{
  loadRadicalFile( radkfile );
}

QSet<Kanji> RadicalFile::kanjiContainingRadicals( QSet<QString> &radicallist ) const
{
  QSet<QString> kanjiStringSet;
  QSet<Kanji> result;
  if( m_radicals.count() < 1 || radicallist.count() < 1 )
  {
    return result;
  }

  //Start out with our first set
  kanjiStringSet = m_radicals[ *radicallist.begin() ].getKanji();
  //Make a set intersection of these m_kanji
  foreach( const QString &rad, radicallist )
  {
    kanjiStringSet &= m_radicals[ rad ].getKanji();
  }

  //Convert our set of QString to a set of Kanji
  foreach( const QString &kanji, kanjiStringSet )
  {
    result += m_kanji[ kanji ];
  }

  return result;
}

bool RadicalFile::loadRadicalFile( QString &radkfile )
{
  QFile f( radkfile );
  if ( ! f.open( QIODevice::ReadOnly ) )
  {
    return false;
  }

  //Read our radical file through a eucJP codec (helpfully builtin to Qt)
  QTextStream t( &f );
  Radical *newestRadical = NULL;
  QHash< QString, QSet<QString> > krad;

  t.setCodec( QTextCodec::codecForName( "eucJP" ) );
  while ( ! t.atEnd() )
  {
    QString line = t.readLine();
    if( line.length() == 0 || line.at( 0 ) == '#' )
    {
      //Skip comment characters
      continue;
    }
    else if( line.at( 0 ) == '$' )
    {
      //Start of a new radical
      if( newestRadical != NULL )
      {
        m_radicals.insert( *newestRadical, *newestRadical );
      }
      newestRadical = new Radical(  QString( line.at( 2 ) )
                                  , line.right( 2 ).toUInt() );
    }
    else if( newestRadical != NULL )
    {
      // List of m_kanji, potentially
      QList<QString> m_kanjiList = line.trimmed().split( "", QString::SkipEmptyParts );
      newestRadical->addKanji( m_kanjiList.toSet() );
      foreach( const QString &kanji, m_kanjiList )
      {
        krad[ kanji ] += *newestRadical;
      }
    }
  }
  if( newestRadical != NULL )
  {
    m_radicals[ *newestRadical ] = *newestRadical;
  }

  //Move contents of our krad QHash into our hash of m_kanji
  QHash<QString,QSet<QString> >::iterator it;
  for( it = krad.begin(); it != krad.end(); ++it )
  {
    m_kanji.insert(   it.key()
                    , Kanji( it.key()
                    , it.value() ) )->calculateStrokes( m_radicals.values() );
  }
  f.close();
  return true;
}

QMultiMap<int,Radical>* RadicalFile::mapRadicalsByStrokes() const
{
  QMultiMap<int, Radical> *result = new QMultiMap<int, Radical>();
  foreach( const Radical &rad, m_radicals )
  {
    result->insert( m_radicals[ rad ].strokes(), m_radicals[ rad ] );
  }
  return result;
}

QSet<QString> RadicalFile::radicalsInKanji( QSet<Kanji> &kanjilist ) const
{
  QSet<QString> possibleRadicals;
  foreach( const QString &kanji, kanjilist )
  {
    possibleRadicals |= m_kanji[ kanji ].getRadicals();
  }

  return possibleRadicals;
}
