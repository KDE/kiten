/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 *
 * USA                                                                       *
 *****************************************************************************/

#include "dictionaryupdatemanager.h"

#include "kiten.h"

#include "kitenconfig.h"

#include <KAction>
#include <KActionCollection>
#include <KDebug>
#include <KFilterDev>
#include <KIO/Scheduler>
#include <KIO/StoredTransferJob>
#include <KLocale>
#include <KMessageBox>
#include <KStandardDirs>
#include <KUrl>

#include <QTemporaryFile>
#include <QTextCodec>
#include <QTextStream>

#define INFO_URL     "http://ftp.monash.edu.au/pub/nihongo/edicthdr.txt"
#define EDICT_URL    "http://ftp.monash.edu.au/pub/nihongo/edict.gz"
#define KANJIDIC_URL "http://ftp.monash.edu.au/pub/nihongo/kanjidic.gz"

DictionaryUpdateManager::DictionaryUpdateManager( Kiten *parent )
: QObject( parent )
, _parent( parent )
, _config( parent->getConfig() )
, _succeeded( QStringList() )
, _failed( QStringList() )
, _counter( 0 )
{
  _actionUpdate = _parent->actionCollection()->add<KAction>( "update_dictionaries" );
  _actionUpdate->setText( i18n( "Check for dictionary &updates" ) );
  _actionUpdate->setShortcut( Qt::CTRL+Qt::Key_U );

  connect( _actionUpdate, SIGNAL( triggered() ),
                    this,   SLOT( checkForUpdates() ) );
}

void DictionaryUpdateManager::checkForUpdates()
{
  kDebug() << "Checking for EDICT & KANJIDIC updates." << endl;
  KIO::StoredTransferJob *job = KIO::storedGet( KUrl( INFO_URL ) );
  connect(  job, SIGNAL( result( KJob* ) ),
           this,   SLOT( checkInfoFile( KJob* ) ) );
}

void DictionaryUpdateManager::checkIfUpdateFinished()
{
  if( _counter == _config->dictionary_list().size() )
  {
    _counter = 0;
    emit updateFinished();
  }
}

void DictionaryUpdateManager::checkInfoFile( KJob *job )
{
  KIO::StoredTransferJob *storedJob = static_cast<KIO::StoredTransferJob*>( job );
  QByteArray data( storedJob->data() );
  if( data.isNull() || data.isEmpty() )
  {
    KMessageBox::sorry( 0, i18n( "Update canceled.\nCould not read file." ) );
    job->deleteLater();
    return;
  }

  QTemporaryFile tempFile;
  if( ! tempFile.open() )
  {
    KMessageBox::sorry( 0, i18n( "Update canceled.\nCould not open file." ) );
    kDebug() << "Could not open tempFile." << endl;
    tempFile.deleteLater();
    job->deleteLater();
    return;
  }
  tempFile.write( data );
  tempFile.close();

  QDate webFileDate = getFileDate( tempFile );
  if( ! webFileDate.isValid() )
  {
    // The program should not get to this point.
    // Maybe the format/content of (one or both) the files changed?
    // or maybe one or both of the files could not be opened in the
    // getFileDate function that would return an invalid empty date.
    KMessageBox::sorry( 0, i18n( "Update canceled.\nThe update information file has an invalid date." ) );
    tempFile.deleteLater();
    job->deleteLater();
    return;
  }

  connect( this, SIGNAL( updateFinished() ),
           this,   SLOT( showUpdateResults() ) );

  bool updates = false;
  foreach( const QString &dict, _config->dictionary_list() )
  {
    QString filePath = KGlobal::dirs()->findResource( "data", QString( "kiten/" ) + dict.toLower() );
    QFile file( filePath );
    kDebug() << "Local dictionary path:" << file.fileName() << endl;

    QDate localFileDate = getFileDate( file );

    if( ! localFileDate.isValid() )
    {
      _failed.append( dict.toUpper() );
      kDebug() << "Failed (invalid date):" << dict.toUpper() << endl;
      continue;
    }
    else if( localFileDate == webFileDate )
    {
      _succeeded.append( dict.toUpper() );
      kDebug() << "Success (up to date):" << dict.toUpper() << endl;
      continue;
    }
    else
    {
      updates = true;

      if( dict.toLower() == QString( "edict" ) )
      {
        downloadDictionary( EDICT_URL );
      }
      else
      {
        downloadDictionary( KANJIDIC_URL );
      }
    }
  }

  if( ! updates )
  {
    emit updateFinished();
  }

  tempFile.deleteLater();
  job->deleteLater();
}

void DictionaryUpdateManager::downloadDictionary( const QString &url )
{
  kDebug() << "Download started!" << endl;
  KIO::StoredTransferJob *dictionaryJob = KIO::storedGet( KUrl( url ) );

  connect( dictionaryJob, SIGNAL( result( KJob* ) ),
                    this,   SLOT( installDictionary( KJob* ) ) );
}

QDate DictionaryUpdateManager::getFileDate( QFile &file )
{
  if( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    kDebug() << "Could not open " << file.fileName() << endl;
    return QDate();
  }

  QTextStream fileStream( &file );
  fileStream.setCodec( QTextCodec::codecForName( "eucJP" ) );

  // The first line of the file is in the following form:
  // 　？？？ /
  // EDICT, EDICT_SUB(P), EDICT2 Japanese-English Electronic Dictionary Files/
  // Copyright Electronic Dictionary Research & Development Group - year/
  // Created: year-month-day/
  // ###### entries

  // NOTE: the above last line only appears in the web status file for EDICT.
  // That file has only 2 lines, one is the '?' simbols with the name of the
  // dictionary, copyright and creation date, the other one is the number
  // of entries in the EDICT dictionary.
  // (see INFO_URL macro).

  // We take the 4th section which is the last one
  // separated by the '/' character and the
  // 10 rightmost characters for that section.
  QString dateSection = fileStream.readLine()
                                  .section( '/', -1, -1, QString::SectionSkipEmpty )
                                  .right( 10 );

  // dateSection has the following value: year-month-day
  // Finally we take the numbers separated by the '-' character.
  int year  = dateSection.section( '-', 0, 0 ).toInt();
  int month = dateSection.section( '-', 1, 1 ).toInt();
  int day   = dateSection.section( '-', 2, 2 ).toInt();

  file.close();

  kDebug() << "Date found:" << dateSection << "(" << file.fileName() << ")" << endl;

  return QDate( year, month, day );
}

void DictionaryUpdateManager::installDictionary( KJob *job )
{
  _counter++;

  KIO::StoredTransferJob *storedJob = static_cast<KIO::StoredTransferJob*>( job );
  QByteArray data( storedJob->data() );
  QString url( storedJob->url().prettyUrl() );

  QTemporaryFile compressedFile;
  if( ! compressedFile.open() )
  {
    kDebug() << "Could not create the downloaded .gz file." << endl;
    _failed.append( url.contains( "edict" ) ? QString( "EDICT" ) : QString( "KANJIDIC" ) );
    job->deleteLater();
    checkIfUpdateFinished();
    return;
  }

  compressedFile.write( data );
  compressedFile.close();

  kDebug() << "Dictionary download finished!" << endl;
  kDebug() << "Extracting dictionary..." << endl;

  QIODevice *device = KFilterDev::deviceForFile( compressedFile.fileName(), "application/x-gzip" );
  if( ! device->open( QIODevice::ReadOnly ) )
  {
    kDebug() << "Could not extract the dictionary file." << endl;
    _failed.append( url.contains( "edict" ) ? QString( "EDICT" ) : QString( "KANJIDIC" ) );
    delete device;
    job->deleteLater();
    checkIfUpdateFinished();
    return;
  }

  QString fileName = device->readLine().startsWith( '#' ) ? QString( "kanjidic" ) : QString( "edict" );
  device->reset();
  QString dictPath = KGlobal::dirs()->locateLocal( "data"
                                                  , QString( "kiten/" ) + fileName
                                                  , true );
  QFile dictionary( dictPath );
  if( ! dictionary.open( QIODevice::WriteOnly ) )
  {
    kDebug() << "Could not create the new dictionary file." << endl;
    _failed.append( fileName.toUpper() );
    device->close();
    delete device;
    job->deleteLater();
    checkIfUpdateFinished();
    return;
  }

  dictionary.write( device->readAll() );
  dictionary.close();
  device->close();

  delete device;
  job->deleteLater();

  checkIfUpdateFinished();
  kDebug() << "Successfully installed at:" << dictionary.fileName() << endl;
}

void DictionaryUpdateManager::showUpdateResults()
{
  disconnect( this, SIGNAL( updateFinished() ),
              this,   SLOT( showUpdateResults() ) );
  
  if( ! _succeeded.isEmpty() && _failed.isEmpty() )
  {
    KMessageBox::information( 0, i18n( "You already have the latest updates." ) );
  }
  else if( _succeeded.isEmpty() && _failed.isEmpty() )
  {
    KMessageBox::information( 0, i18n( "Successfully updated your dictionaries." ) );
  }
  else if( ! _succeeded.isEmpty() && ! _failed.isEmpty() )
  {
    KMessageBox::information( 0, i18n( "Successfully updated:\n%1\n\nFailed to update:\n%2"
                                      , _succeeded.join( "\n" )
                                      , _failed.join( "\n" ) ) );
  }
  else if( _succeeded.isEmpty() && ! _failed.isEmpty() )
  {
    KMessageBox::sorry( 0, i18n( "Failed to update:\n%1"
                                , _failed.join( "\n" ) ) );
  }

  _succeeded.clear();
  _failed.clear();
}

#include "dictionaryupdatemanager.moc"
