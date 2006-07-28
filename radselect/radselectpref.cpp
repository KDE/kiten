/* This file is part of kiten, a KDE Japanese Reference Tool
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include "radselectpref.h"

#include <klocale.h>

#include <qlayout.h>
#include <qlabel.h>

radselectPreferences::radselectPreferences()
	: KPageDialog()
{
	setFaceType(Tree);
	setCaption( i18n("radselect Preferences") );
	setButtons(Help|Default|Ok|Apply|Cancel);
	setDefaultButton(Ok);
    // this is the base class for your preferences dialog.  it is now
    // a Treelist dialog.. but there are a number of other
    // possibilities (including Tab, Swallow, and just Plain)
    m_pageOne = new radselectPrefPageOne(this);
	 addPage(m_pageOne, i18n("First Page"));

    m_pageTwo = new radselectPrefPageTwo(this);
	 addPage(m_pageTwo, i18n("Second Page"));
	 
}

radselectPrefPageOne::radselectPrefPageOne(QWidget *parent)
    : QFrame(parent)
{
    new QLabel(i18n("Add something here"), this);
}

radselectPrefPageTwo::radselectPrefPageTwo(QWidget *parent)
    : QFrame(parent)
{
    new QLabel(i18n("Add something here"), this);
}

#include "radselectpref.moc"
