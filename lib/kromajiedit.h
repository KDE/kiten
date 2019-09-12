/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
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

#ifndef KITEN_KROMAJIEDIT_H
#define KITEN_KROMAJIEDIT_H

#include <KLineEdit>

#include <QMap>

class QByteArray;
class QKeyEvent;
class QMenu;
class QWidget;

class /* NO_EXPORT */ KRomajiEdit : public KLineEdit
{
  Q_OBJECT

  public:
    KRomajiEdit( QWidget *parent, const char *name );
    ~KRomajiEdit();

  public Q_SLOTS:
    void setKana( QAction* );

  protected:
    QMenu *createPopupMenu();
    void   keyPressEvent( QKeyEvent *e ) override;

  private:
    QMap<QString, QString> m_hiragana;
    QMap<QString, QString> m_katakana;

    QByteArray m_kana;
};

#endif
