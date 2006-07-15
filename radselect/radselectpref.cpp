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
//Added by qt3to4:

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
/*    QFrame *frame;
    frame = addPage(i18n("First Page"), i18n("Page One Options"));
    m_pageOne = new radselectPrefPageOne(frame);

    frame = addPage(i18n("Second Page"), i18n("Page Two Options"));
    m_pageTwo = new radselectPrefPageTwo(frame);
	 */
}
/*
radselectPrefPageOne::radselectPrefPageOne(QWidget *parent)
    : Q3Frame(parent)
{
    Q3HBoxLayout *layout = new Q3HBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}

radselectPrefPageTwo::radselectPrefPageTwo(QWidget *parent)
    : Q3Frame(parent)
{
    Q3HBoxLayout *layout = new Q3HBoxLayout(this);
    layout->setAutoAdd(true);

    new QLabel(i18n("Add something here"), this);
}
*/
#include "radselectpref.moc"
