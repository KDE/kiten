/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef WORDTYPE_H
#define WORDTYPE_H

#include <KComboBox>

#include <QWidget>

class /* NO_EXPORT */ WordType : public KComboBox
{
  Q_OBJECT

  public:
    explicit WordType( QWidget *parent );
};

#endif
