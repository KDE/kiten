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

#ifndef RADICALBUTTON_H
#define RADICALBUTTON_H

#include <QPushButton>
#include <QString>

class QMouseEvent;

class RadicalButton : public QPushButton
{
  Q_OBJECT

  public:
             RadicalButton( const QString &text, QWidget *parent );
    virtual ~RadicalButton();
    /**
     * Overriding QPushButton's event for mousewheel events on a disabled button
     */
    bool     event( QEvent *event ) Q_DECL_OVERRIDE;
    QSize    minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize    sizeHint() const Q_DECL_OVERRIDE;

    typedef enum
    {
      Normal,         /**< Normal button */
      Selected,       /**< This button has been selected: bold + underline */
      NotAppropriate, /**< Due to other selected buttons: disabled */
      Related,        /**< Display only this radical and related ones: italics? */
      Hidden          /**< Not related (to above), so hide() */
    } ButtonStatus;

  signals:
    void userClicked( const QString &text, RadicalButton::ButtonStatus status );

  public slots:
    void mousePressEvent( QMouseEvent *e ) Q_DECL_OVERRIDE;
    void mouseReleaseEvent( QMouseEvent *e ) Q_DECL_OVERRIDE;
    void resetButton();
    void setStatus( RadicalButton::ButtonStatus newStatus );

  private:
    ButtonStatus m_status;
};

#endif
