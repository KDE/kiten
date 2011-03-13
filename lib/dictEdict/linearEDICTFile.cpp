/*
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001 by Jason Katz-Brown <jason@katzbrown.com>
 Copyright (C) 2008 by Joseph Kerian <jkerian@gmail.com>

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

#include "linearEDICTFile.h"

#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <kapplication.h>

#include <QtCore/QTextCodec>
#include <QtCore/QFile>


linearEDICTFile::linearEDICTFile()
: properlyLoaded( false )
{
}

linearEDICTFile::~linearEDICTFile()
{
}

bool linearEDICTFile::loadFile( const QString& filename )
{
  kDebug() << "Loading edict from " << filename << "\n";

  //if already loaded
  if ( ! edict.isEmpty() )
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
    if ( lastLine[ 0 ] != '#' ) edict << lastLine;
  }

  file.close();
  properlyLoaded = true;

  return true;
}

bool linearEDICTFile::valid() const
{
  return properlyLoaded;
}

/**
 * Get everything that looks remotely like a given search string
 */
QVector<QString> linearEDICTFile::findMatches( const QString &searchString ) const
{
  QVector<QString> matches;
  foreach( const QString &it, edict )
  {
    if ( it.contains( searchString ) )
    {
      matches.append( it );
    }
  }

  return matches;
}
