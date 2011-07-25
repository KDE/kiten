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

#include "radicalbutton.h"

#include <KApplication>
#include <KStyle>

#include <QMouseEvent>
#include <QPushButton>
#include <QString>
#include <QStyle>
#include <QStyleOptionButton>
#include <QWidget>

RadicalButton::RadicalButton( const QString &text, QWidget *parent )
: QPushButton( text, parent )
{
  setCheckable( true );
}

RadicalButton::~RadicalButton()
{
}

bool RadicalButton::event( QEvent *event )
{
  //This button does not handle wheel events, and unlike
  //the superclass, we don't care if we're enabled or disabled
  //(the superclass eats wheel events when disabled)
  if( event->type() == QEvent::Wheel )
  {
    return false;
  }
  return QPushButton::event( event );
}

QSize RadicalButton::minimumSizeHint() const
{
  int width = fontMetrics().size( Qt::TextShowMnemonic, text() ).width();
  int height = QPushButton::sizeHint().height();
//TODO: RadicalButton size calculation right, one of these days
//	QSize sz = fontMetrics().size(Qt::TextShowMnemonic, *it);
//	setMinimumSize(sz);

//	QStyleOptionButton opt;
//	opt.initFrom(this);
//	QRect rect = kapp->style()->subElementRect(QStyle::SE_PushButtonContents,&opt,this);
  return QSize( width, height );
}

void RadicalButton::mousePressEvent( QMouseEvent *e )
{
  QPushButton::mousePressEvent( e );
  if( e->button() == Qt::RightButton )
  {
    setStatus( Related );
    emit userClicked( text(), Related );
  }
}

void RadicalButton::mouseReleaseEvent( QMouseEvent *e )
{
  QPushButton::mouseReleaseEvent( e );
  if( e->button() == Qt::LeftButton )
  {
    switch( m_status )
    {
      case Selected:
        setStatus( Normal );
        emit userClicked( text(), Normal );
        break;
      default:
        setStatus( Selected );
        emit userClicked( text(), Selected );
    }
  }
}

void RadicalButton::resetButton()
{
  setStatus( Normal );
}

void RadicalButton::setStatus( RadicalButton::ButtonStatus newStatus )
{
  if( m_status == newStatus )
  {
    return;
  }

  //Because it's more work to check everything rather than just set it,
  //we'll just set everything every time
  bool checked   = false,
       underline = false,
       italic    = false,
       hidden    = false,
       disabled  = false;
  switch( newStatus )
  {
    case Normal:
      break;
    case Selected:
      checked = true;
      break;
    case NotAppropriate:
      disabled = true;
      break;
    case Related:
      italic = true;
      break;
    case Hidden:
      hidden = true;
  }

  QFont theFont = font();
  theFont.setUnderline( underline );
  theFont.setItalic( italic );
  setFont( theFont );
  setVisible( ! hidden );
  setEnabled( ! disabled );
  setChecked( checked );
  m_status = newStatus;
}

QSize RadicalButton::sizeHint() const
{
  return minimumSizeHint();
}

#include "radicalbutton.moc"
