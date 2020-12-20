/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

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
