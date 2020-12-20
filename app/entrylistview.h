/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ENTRYLISTVIEW_H
#define ENTRYLISTVIEW_H

#include <QTableView>

class EntryListView : public QTableView
{
  Q_OBJECT

  public:
    explicit EntryListView( QWidget *parent = nullptr );

  public slots:
    void setEmptyModel();

  protected:
    void resizeEvent( QResizeEvent *event ) override;
};

#endif
