/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kromajiedit.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QAction>
#include <QApplication>
#include <QFile>
#include <QKeyEvent>
#include <QMenu>
#include <QStandardPaths>
#include <QTextCodec>
#include <QTextStream>

KRomajiEdit::KRomajiEdit( QWidget *parent, const char *name )
: KLineEdit( name, parent ) //KDE4 CHANGE
{
  m_kana = "unset";

  QString romkana = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kiten/romkana.cnv"));
  if ( romkana.isNull() )
  {
    KMessageBox::error( nullptr, i18n( "Romaji information file not installed, so Romaji conversion cannot be used." ) );
    return;
  }

  QFile f( romkana );

  if ( ! f.open( QIODevice::ReadOnly ) )
  {
    KMessageBox::error( nullptr, i18n( "Romaji information could not be loaded, so Romaji conversion cannot be used." ) );
  }

  QTextStream t( &f );
  t.setCodec( QTextCodec::codecForName( "eucJP" ) );
  while ( ! t.atEnd() ) //KDE4 CHANGE
  {
    QString s = t.readLine();

    QChar first = s.at( 0 );
    if ( first == '#' ) // comment!
    {
      // nothing
    }
    else if ( first == '$' ) // header
    {
      if ( m_kana == "unset" )
      {
        m_kana = "hiragana";
      }
      else
      {
        m_kana = "katakana";
      }
    }
    else // body
    {
      QStringList things( s.split( QChar( ' ' ) ) );
      QString thekana( things.first() );
      QString romaji( /* KDE4 CHANGE: * */things.at( 1 ) );

      if ( m_kana == "hiragana" )
      {
        m_hiragana[ romaji ] = thekana;
      }
      else if ( m_kana == "katakana" )
      {
        m_katakana[ romaji ] = thekana;
      }
    }
  }
  f.close();

  m_kana = "english";
}

KRomajiEdit::~KRomajiEdit()
{
}

QMenu *KRomajiEdit::createPopupMenu()
{
  QMenu *menu;
  menu = new QMenu();
  //TODO: Get the basic editing options in here from a KLineEdit popup menu (or elsewhere?)
  menu->addSeparator();

  //Put our action group together
  QActionGroup *group = new QActionGroup( menu );

  QAction *temp;
  temp = new QAction( i18nc( "@option:radio selects english translation", "English" ), group );
  temp->setCheckable( true );
  menu->addAction( temp );
  if( m_kana == "english" )
  {
    temp->setChecked( true );
  }
  else
  {
    temp->setChecked( false );
  }

  temp = new QAction( i18nc( "@option:radio selects japanese translation", "Kana" ), group );
  temp->setCheckable( true );
  menu->addAction( temp );
  if( m_kana == "kana" )
  {
    temp->setChecked( true );
  }
  else
  {
    temp->setChecked( false );
  }

  connect( group, &QActionGroup::triggered,
                    this, &KRomajiEdit::setKana );

  emit aboutToShowContextMenu( menu );
  return menu;
}

// TODO allow editing not only at end
void KRomajiEdit::keyPressEvent( QKeyEvent *e )
{
  bool shift = qApp->keyboardModifiers() & Qt::ShiftModifier; //KDE4 CHANGE
  QString ji = e->text();

  if ( shift && e->key() == Qt::Key_Space ) // switch hiragana/english //KDE4 CHANGE
  {
    if ( m_kana == "hiragana" )
    {
      m_kana = "english";
    }
    else if ( m_kana == "english" )
    {
      m_kana = "hiragana";
    }

    return;
  }

  if ( m_kana == "english" || ji.isEmpty() )
  {
    KLineEdit::keyPressEvent( e );
    return;
  }

  if ( shift ) // shift for katakana
  {
    if (m_kana == "hiragana")
    {
      m_kana = "katakana";
    }
  }

  //kdDebug() << "--------------------\n";

  QString curEng;
  QString curKana;
  QString text = this->text();

  int i;
  unsigned int len = text.length();
  //kdDebug() << "length = " << len << endl;
  for ( i = len - 1; i >= 0; i-- )
  {
    QChar at = text.at( i );

    //kdDebug() << "at = " << QString(at) << endl;

    if ( at.row() == 0 && at != '.' )
    {
      //kdDebug() << "prepending " << QString(at) << endl;
      curEng.prepend(at);
    }
    else
    {
      break;
    }
  }
  i++;

  //kdDebug() << "i is " << i << ", length is " << len << endl;
  curKana = text.left( i );

  ji.prepend( curEng );
  ji = ji.toLower();
  //kdDebug() << "ji = " << ji << endl;

  QString replace;

  //kdDebug () << "kana is " << m_kana << endl;
  if ( m_kana == "hiragana" )
  {
    replace = m_hiragana[ ji ];
  }
  else if ( m_kana == "katakana" )
  {
    replace = m_katakana[ ji ];
  }

  //kdDebug() << "replace = " << replace << endl;

  if ( ! ( replace.isEmpty() ) ) // if (replace has something in it) //KDE4 CHANGE
  {
    //kdDebug() << "replace isn't empty\n";

    setText( curKana + replace );

    if ( m_kana == "katakana" )
    {
      m_kana = "hiragana";
    }

    return;
  }
  else
  {
    //kdDebug() << "replace is empty\n";
    QString farRight( ji.right( ji.length() - 1 ) );
    //kdDebug() << "ji = " << ji << endl;
    //kdDebug() << "farRight = " << farRight << endl;

    // test if we need small tsu
    if ( ji.at( 0 ) == farRight.at( 0 ) ) // if two letters are same, and we can add a twoletter length kana
    {
      if ( m_kana == "hiragana" )
      {
        setText( curKana + m_hiragana[ ji.at( 0 ) == 'n' ? "n'" : "t-" ] + farRight.at( 0 ) );
      }
      else
      {
        setText( curKana + m_katakana[ ji.at( 0 ) == 'n' ? "n'" : "t-" ] + farRight.at( 0 ) );
      }

      if ( m_kana == "katakana" )
      {
        m_kana = "hiragana";
      }

      return;
    }

    // test for hanging n
    QString newkana;
    if ( m_kana == "hiragana" )
    {
      newkana = m_hiragana[ farRight ];
      //kdDebug() << "newkana = " << newkana << endl;
      if ( ji.at( 0 ) == 'n' && ! ( newkana.isEmpty() ) ) //KDE4 CHANGE
      {
        //kdDebug() << "doing the n thing\n";

        setText( curKana + m_hiragana[ QStringLiteral("n'") ] + newkana );

        if ( m_kana == "katakana" )
        {
          m_kana = "hiragana";
        }

        return;
      }
    }
    else
    {
      newkana = m_katakana[ farRight ];
      if ( ji.at( 0 ) == 'n' && ! newkana.isEmpty() ) //KDE4 CHANGE
      {
        //kdDebug() << "doing the n thing - katakana\n";

        setText( curKana + m_katakana[ QStringLiteral("n'") ] + newkana );

        if ( m_kana == "katakana" )
        {
          m_kana = "hiragana";
        }

        return;
      }
    }
  }

  if ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) // take care of pending n //KDE4 CHANGE
  {
    if ( m_kana == "hiragana" )
    {
      if ( text[ len - 1 ] == 'n' )
      {
        setText( curKana + m_hiragana[ QStringLiteral("n'") ] );
      }
    }
    else
    {
      if ( text[ len - 1 ] == 'N' )
      {
        setText( curKana + m_katakana[ QStringLiteral("n'") ] );
      }
    }
  }

  KLineEdit::keyPressEvent( e ); // don't think we'll get here :)
}

// This is the slot for the menu
void KRomajiEdit::setKana( QAction *action )
{
  if( action->text() == QLatin1String("Kana") )
  {
    m_kana = "hiragana";
  }
  if( action->text() == QLatin1String("English") )
  {
    m_kana = "english";
  }
}


