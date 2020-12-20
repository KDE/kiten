/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "wordtype.h"

#include "entry.h"


WordType::WordType( QWidget *parent )
: KComboBox( parent )
{
  //TODO: i18n needed
  addItem( QStringLiteral("<No type specified>") );
  addItem( QStringLiteral("Noun") );
  addItem( QStringLiteral("Verb") );
//   addItems( Entry::WordTypesPretty()->uniqueKeys() );
}


