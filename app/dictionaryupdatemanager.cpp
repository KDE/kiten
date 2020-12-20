/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "dictionaryupdatemanager.h"

#include "kiten.h"
#include "kitenconfig.h"
#include "kitenmacros.h"

#include <QAction>
#include <KActionCollection>
#include <QDebug>
#include <KCompressionDevice>
#include <KIO/Scheduler>
#include <KIO/StoredTransferJob>
#include <KLocalizedString>
#include <KMessageBox>
#include <QDir>
#include <QUrl>

#include <QTemporaryFile>
#include <QTextCodec>
#include <QTextStream>
#include <QStandardPaths>

// URL to the information file.
#define INFO_URL     "http://ftp.monash.edu.au/pub/nihongo/edicthdr.txt"
// URL to the EDICT dictionary.
#define EDICT_URL    "http://ftp.monash.edu.au/pub/nihongo/edict.gz"
// URL to the KANJIDIC dictionary.
#define KANJIDIC_URL "http://ftp.monash.edu.au/pub/nihongo/kanjidic.gz"

DictionaryUpdateManager::DictionaryUpdateManager( Kiten *parent )
: QObject( parent )
, _parent( parent )
, _config( parent->getConfig() )
, _succeeded( QStringList() )
, _failed( QStringList() )
, _counter( 0 )
{
  _actionUpdate = _parent->actionCollection()->add<QAction>( QStringLiteral("update_dictionaries") );
  _actionUpdate->setText( i18n( "Check for dictionary &updates" ) );
  _parent->actionCollection()->setDefaultShortcut(_actionUpdate, Qt::CTRL+Qt::Key_U );

  connect(_actionUpdate, &QAction::triggered, this, &DictionaryUpdateManager::checkForUpdates);
}

void DictionaryUpdateManager::checkForUpdates()
{
  qDebug() << "Checking for EDICT & KANJIDIC updates.";
  // Download the information file we need to check
  // whether or not an update to our dictionaries is necessary.
  KIO::StoredTransferJob *job = KIO::storedGet( QUrl( INFO_URL ) );
  connect(job, &KIO::StoredTransferJob::result, this, &DictionaryUpdateManager::checkInfoFile);
}

void DictionaryUpdateManager::checkIfUpdateFinished()
{
  // Emit the updateFinished signal once we finished
  // to update our installed dictionaries.
  if( _counter == _config->dictionary_list().size() )
  {
    // Make sure to reset this variable to 0.
    _counter = 0;
    emit updateFinished();
  }
}

void DictionaryUpdateManager::checkInfoFile( KJob *job )
{
  KIO::StoredTransferJob *storedJob = static_cast<KIO::StoredTransferJob*>( job );
  QByteArray data( storedJob->data() );
  // Check if we have valid data to work with.
  if( data.isNull() || data.isEmpty() )
  {
    KMessageBox::sorry( nullptr, i18n( "Update canceled.\nCould not read file." ) );
    job->deleteLater();
    return;
  }

  // We don't need to store this information file in our Hard Disk Drive,
  // a temporary file is enough.
  QTemporaryFile tempFile;
  if( ! tempFile.open() )
  {
    KMessageBox::sorry( nullptr, i18n( "Update canceled.\nCould not open file." ) );
    qDebug() << "Could not open tempFile.";
    tempFile.deleteLater();
    job->deleteLater();
    return;
  }
  tempFile.write( data );
  tempFile.close();

  // Get the latest creation date for the EDICT dictionary on the server.
  QDate webFileDate = getFileDate( tempFile );
  if( ! webFileDate.isValid() )
  {
    // The program should not get to this point.
    // Maybe the format/content of (one or both) the files changed?
    // or maybe one or both of the files could not be opened in the
    // getFileDate function that would return an invalid empty date.
    KMessageBox::sorry( nullptr, i18n( "Update canceled.\nThe update information file has an invalid date." ) );
    tempFile.deleteLater();
    job->deleteLater();
    return;
  }

  // At this point we have a valid creation date from the server.
  // Now we can check our dictionaries.

  // Connect to this signal to know when the update process is finished.
  connect(this, &DictionaryUpdateManager::updateFinished, this, &DictionaryUpdateManager::showUpdateResults);

  // This variable help us to know if we need to download any file.
  bool updates = false;
  // Iterate on each of our installed dictionaries.
  foreach( const QString &dict, _config->dictionary_list() )
  {
    QString filePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral( "kiten/" ) + dict.toLower() );
    QFile file( filePath );
    qDebug() << "Local dictionary path:" << file.fileName();

    // Get the creation date for this dictionary.
    QDate localFileDate = getFileDate( file );

    if( ! localFileDate.isValid() )
    {
      // Add it to our 'failed to udate' list.
      _failed.append( dict.toUpper() );
      qDebug() << "Failed (invalid date):" << dict.toUpper();
      continue;
    }
    else if( localFileDate == webFileDate )
    {
      // Add it to our 'up to date' list.
      _succeeded.append( dict.toUpper() );
      qDebug() << "Success (up to date):" << dict.toUpper();
      continue;
    }
    else
    {
      // Indicate we need to download something.
      updates = true;

      if( dict.toLower() == EDICT )
      {
        downloadDictionary( EDICT_URL );
      }
      else
      {
        downloadDictionary( KANJIDIC_URL );
      }
    }
  }

  // Let Kiten know we finished and don't need to download anything else.
  if( ! updates )
  {
    emit updateFinished();
  }

  tempFile.deleteLater();
  job->deleteLater();
}

void DictionaryUpdateManager::downloadDictionary( const QString &url )
{
  qDebug() << "Download started!";
  // Download dictionary.
  KIO::StoredTransferJob *dictionaryJob = KIO::storedGet( QUrl( url ) );
  connect(dictionaryJob, &KIO::StoredTransferJob::result, this, &DictionaryUpdateManager::installDictionary);
}

QDate DictionaryUpdateManager::getFileDate( QFile &file )
{
  if( ! file.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    qDebug() << "Could not open " << file.fileName();
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
  // That file has only 2 lines, one is the '?' symbols with the name of the
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

  qDebug() << "Date found:" << dateSection << "(" << file.fileName() << ")";

  return QDate( year, month, day );
}

void DictionaryUpdateManager::installDictionary( KJob *job )
{
  // Increase the number of dictionaries we try to install.
  // This way we can know when we finished with our installed dictionaries.
  _counter++;

  KIO::StoredTransferJob *storedJob = static_cast<KIO::StoredTransferJob*>( job );
  QByteArray data( storedJob->data() );
  QString url( storedJob->url().toDisplayString() );

  // What we actually downloaded was a GZIP file that we need to extract.
  // As there is no need to keep this file, we make it a temporary file.
  QTemporaryFile compressedFile;
  if( ! compressedFile.open() )
  {
    qDebug() << "Could not create the downloaded .gz file.";
    _failed.append( url.contains( EDICT ) ? EDICT : KANJIDIC );
    job->deleteLater();
    checkIfUpdateFinished();
    return;
  }
  // Create the GZIP file from the downloaded data.
  compressedFile.write( data );
  compressedFile.close();

  qDebug() << "Dictionary download finished!";
  qDebug() << "Extracting dictionary...";

  // Extract the GZIP file.
  //QIODevice *device = KFilterDev::deviceForFile( compressedFile.fileName(), "application/x-gzip" );
  KCompressionDevice *device = new KCompressionDevice(compressedFile.fileName(), KCompressionDevice::GZip);
  if( ! device->open( QIODevice::ReadOnly ) )
  {
    qDebug() << "Could not extract the dictionary file.";
    _failed.append( url.contains( EDICT ) ? EDICT : KANJIDIC );
    delete device;
    job->deleteLater();
    checkIfUpdateFinished();
    return;
  }

  // Check the first line's first character of the extracted file.
  // EDICT starts with a '　', KANJIDIC starts with a '#'.
  QString fileName = device->readLine().startsWith( '#' ) ? QStringLiteral( "kanjidic" ) : QStringLiteral( "edict" );
  // Reset the position where we are going to start reading the content.
  device->reset();
  // Thanks to the above lines we can get the path to the correct file to be updated.
  QString dictPath = QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation) + QStringLiteral( "/kiten/" );
  QDir dir(dictPath);
  dir.mkpath(dictPath);

  QFile dictionary( dictPath  + fileName);
  if( ! dictionary.open( QIODevice::WriteOnly ) )
  {
    qDebug() << "Could not create the new dictionary file.";
    _failed.append( fileName.toUpper() );
    device->close();
    delete device;
    job->deleteLater();
    checkIfUpdateFinished();
    return;
  }

  // Write the new dictionary file to disk.
  dictionary.write( device->readAll() );
  dictionary.close();
  device->close();

  delete device;
  job->deleteLater();

  // Check if we finished updating.
  checkIfUpdateFinished();
  qDebug() << "Successfully installed at:" << dictionary.fileName();
}

void DictionaryUpdateManager::showUpdateResults()
{
  // Avoid multiple calls to this slot.
  disconnect( this, &DictionaryUpdateManager::updateFinished,
              this,   &DictionaryUpdateManager::showUpdateResults );
  
  if( ! _succeeded.isEmpty() && _failed.isEmpty() )
  {
    KMessageBox::information( nullptr, i18n( "You already have the latest updates." ) );
  }
  else if( _succeeded.isEmpty() && _failed.isEmpty() )
  {
    KMessageBox::information( nullptr, i18n( "Successfully updated your dictionaries." ) );
  }
  else if( ! _succeeded.isEmpty() && ! _failed.isEmpty() )
  {
    KMessageBox::information( nullptr, i18n( "Successfully updated:\n%1\n\nFailed to update:\n%2"
                                      , _succeeded.join( "\n" )
                                      , _failed.join( "\n" ) ) );
  }
  else if( _succeeded.isEmpty() && ! _failed.isEmpty() )
  {
    KMessageBox::sorry( nullptr, i18n( "Failed to update:\n%1"
                                , _failed.join( "\n" ) ) );
  }

  // Avoid repetitions in our lists.
  _succeeded.clear();
  _failed.clear();
}


