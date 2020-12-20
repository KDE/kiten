/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2008 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "linearedictfile.h"


#include <QFile>
#include <QDebug>
#include <QTextCodec>

LinearEdictFile::LinearEdictFile()
: m_properlyLoaded( false )
{
}

LinearEdictFile::~LinearEdictFile()
{
}

/**
 * Get everything that looks remotely like a given search string
 */
QVector<QString> LinearEdictFile::findMatches( const QString &searchString ) const
{
  QVector<QString> matches;
  foreach( const QString &it, m_edict )
  {
    if ( it.contains( searchString ) )
    {
      matches.append( it );
    }
  }

  return matches;
}

bool LinearEdictFile::loadFile( const QString& filename )
{
  qDebug() << "Loading edict from " << filename;

  //if already loaded
  if ( ! m_edict.isEmpty() )
  {
    return true;
  }

  QFile file( filename );
  if ( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    return false;
  }

  QTextStream fileStream( &file );
  fileStream.setCodec( QTextCodec::codecForName( "eucJP" ) );

  QString lastLine;
  while ( ! fileStream.atEnd() )
  {
    lastLine = fileStream.readLine();
    if ( lastLine[ 0 ] != '#' ) m_edict << lastLine;
  }

  file.close();
  m_properlyLoaded = true;

  return true;
}

bool LinearEdictFile::valid() const
{
  return m_properlyLoaded;
}
