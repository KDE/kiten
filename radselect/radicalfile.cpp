/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "DictKanjidic/dictfilekanjidic.h"
#include "kitenmacros.h"
#include "radicalfile.h"

#include <QFile>
#include <QRegExp>
#include <QString>
#include <QTextCodec>
#include <QTextStream>

RadicalFile::RadicalFile( QString &radkfile, const QString &kanjidic )
{
  loadRadicalFile( radkfile );
  if( ! kanjidic.isEmpty() )
  {
    loadKanjidic( kanjidic );
  }
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
  Radical *newestRadical = nullptr;
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
      if( newestRadical != nullptr )
      {
        m_radicals.insert( newestRadical->toString(), *newestRadical );
      }
      delete newestRadical;
      QStringList lineElements = line.split( QRegExp( QStringLiteral( "\\s+" ) ) );
      newestRadical = new Radical(  lineElements.at( 1 )
                                  , lineElements.at( 2 ).toUInt()
                                  , m_radicals.size() );
    }
    else if( newestRadical != nullptr )
    {
      // List of m_kanji, potentially
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
      const QList<QString> m_kanjiList = line.trimmed().split( QLatin1String(""), QString::SkipEmptyParts );
#else
      const QList<QString> m_kanjiList = line.trimmed().split( QLatin1String(""), Qt::SkipEmptyParts );
#endif
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
      newestRadical->addKanji( m_kanjiList.toSet() );
#else
      const QSet<QString> kanjiSet = QSet<QString>(m_kanjiList.begin(), m_kanjiList.end());
      newestRadical->addKanji( kanjiSet );
#endif
      foreach( const QString &kanji, m_kanjiList )
      {
        krad[ kanji ] += newestRadical->toString();
      }
    }
  }
  if( newestRadical != nullptr )
  {
    m_radicals[ newestRadical->toString() ] = *newestRadical;
    delete newestRadical;
  }

  //Move contents of our krad QHash into our hash of m_kanji
  QHash<QString,QSet<QString> >::iterator it;
  for( it = krad.begin(); it != krad.end(); ++it )
  {
    m_kanji.insert(   it.key()
                    , Kanji( it.key()
                    , it.value() ) );
  }
  f.close();
  return true;
}

// Mostly copied from KanjiBrowser::loadKanji()
bool RadicalFile::loadKanjidic( const QString &kanjidic )
{
  DictFileKanjidic dictFileKanjidic;
  dictFileKanjidic.loadSettings();
  dictFileKanjidic.loadDictionary( kanjidic, KANJIDIC );

  QRegExp strokeMatch( "^S\\d+" );
  foreach( const QString &line, dictFileKanjidic.dumpDictionary() )
  {
    const QString kanji = line[ 0 ];

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    QStringList strokesSection = line.split( " ", QString::SkipEmptyParts )
#else
    QStringList strokesSection = line.split( " ", Qt::SkipEmptyParts )
#endif
                                     .filter( strokeMatch );

    unsigned int strokes = strokesSection.first().remove( 0, 1 ).toInt();

    if( m_kanji.contains( kanji ) ) {
      m_kanji[ kanji ].setStrokes( strokes );
    }
  }

  return true;
}

QMultiMap<int,Radical>* RadicalFile::mapRadicalsByStrokes( int max_strokes ) const
{
  QMultiMap<int, Radical> *result = new QMultiMap<int, Radical>();
  foreach( const Radical &rad, m_radicals )
  {
    int strokes = rad.strokes();
    if( ( max_strokes > 0 ) && ( strokes > max_strokes ) )
    {
      strokes = max_strokes;
    }
    result->insert( strokes, rad );
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
