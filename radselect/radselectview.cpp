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

/*
    Future Plans:
*	Build a proper exception handling framework
*/

#include "radselectview.h"
#include "radselectbuttongrid.h"
#include "radicalfile.h"
#include "radselectconfig.h"

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

#include <QtGui/QWidget>
#include <QtGui/QListWidget>
#include <QtGui/QListWidgetItem>
#include <QtGui/QPushButton>
#include <QtGui/QClipboard>
#include <QtGui/QApplication>

#include "kstandarddirs.h"
#include "kmessagebox.h"

radselectView::radselectView(QWidget *parent) : QWidget(parent)
{
	setupUi(this);	//Setup the ui from the .ui file

	//Load the radical information
	KStandardDirs *dirs = KGlobal::dirs();
	QString radkfilename = dirs->findResource("data", "kiten/radkfile");
	if (radkfilename.isNull())
		KMessageBox::error(0,i18n("Kanji radical information does not seem to be installed (file kiten/radkfile), this file is required for this app to function"));
	else
		radicalInfo = new radicalFile(radkfilename);

	//Configure the scrolling area
	m_buttongrid = new radselectButtonGrid(this, radicalInfo);
	radical_box->setWidget(m_buttongrid);
	radical_box->setWidgetResizable(true);

	//Configure the stroke selectors
	strokes_low->setSpecialValueText(i18n("Min"));
	strokes_high->setSpecialValueText(i18n("Max"));

   //== Now we connect all our signals ==
	//Connect our radical grid to our adding method
	connect( m_buttongrid, SIGNAL( possibleKanji(const QList<Kanji>&) ),
			this, SLOT( listPossibleKanji(const QList<Kanji>&) ) );
	//Connect the results selection to our logic
	connect(selected_radicals, SIGNAL( itemClicked(QListWidgetItem*) ),
			this, SLOT( kanjiClicked(QListWidgetItem*) ) );
	connect(selected_radicals, SIGNAL( itemDoubleClicked(QListWidgetItem*) ),
			this, SLOT( kanjiDoubleClicked(QListWidgetItem*) ) );
	//Connect our stroke limit actions
	connect(strokes_low, SIGNAL( valueChanged(int) ),
			this, SLOT( strokeLimitChanged(int) ) );
	connect(strokes_high, SIGNAL( valueChanged(int) ),
			this, SLOT( strokeLimitChanged(int) ) );
	//Connect statusbar updates
	connect( m_buttongrid, SIGNAL( signalChangeStatusbar(const QString&) ),
			this, SIGNAL( signalChangeStatusbar(const QString&)));

	//Connect our clear button
   connect( clear_button, SIGNAL( clicked() ), this, SLOT(clearSearch()));

	loadSettings();
}

radselectView::~radselectView()
{
}

void
radselectView::loadSettings() {
//TODO: Add preferences for what to do on single/double click
//Suggested options: Lookup in Kiten, Add to Search Bar, Copy to Clipboard
	m_buttongrid->setFont(radselectConfigSkeleton::self()->font());
}

void
radselectView::kanjiClicked(QListWidgetItem *item) {
	QString finalText;
	if(item->text() == i18n("(All)")) {
		foreach(QListWidgetItem *listItem,
				selected_radicals->findItems("*",Qt::MatchWildcard))
			if(listItem->text() != i18n("(All)"))
				finalText += listItem->text();
	} else
		finalText = item->text();
	QApplication::clipboard()->setText(finalText, QClipboard::Selection);
}

void
radselectView::kanjiDoubleClicked(QListWidgetItem *item) {
}

void
radselectView::listPossibleKanji(const QList<Kanji>& list)
{
	int low = strokes_low->value();
	int high = strokes_high->value();

	//Modification of the stroke boxes
	//We want to move the max value to something reasonable...
	//for example, 5 above the current max value so that rollover
	//works nicely. We want to reset to all if the list is empty.
	//And we also don't limit if the current value is higher than
	//max value in the list
	int newMax=20;
	if(list.count() < 1 || list.last().strokes() < low ||
			list.last().strokes()+5 < high)
		newMax = 99;
	else
		newMax = list.last().strokes()+5;

	strokes_low->setMaximum(newMax);
	strokes_high->setMaximum(newMax);
	if(high == 0)
		high = 99;

	selected_radicals->clear();
	foreach(const Kanji &it, list)
		if(low <= it.strokes() && it.strokes() <= high)
			new QListWidgetItem((QString)it,selected_radicals);
	if(selected_radicals->count() > 1)
		new QListWidgetItem(i18n("(All)"),selected_radicals);

	m_possibleKanji = list;

	emit searchModified();
}

void
radselectView::loadRadicals(const QString &iRadicals, int strokeMin, int strokeMax)
{
	//TODO: loadRadicals method
	emit searchModified();
}

void
radselectView::loadKanji(QString &iKanji) {
	//TODO: loadKanji method
}

void
radselectView::clearSearch() {
	m_buttongrid->clearSelections();
	selected_radicals->clear();
	strokes_low->setValue(0);
	strokes_high->setValue(0);
}

void
radselectView::changedSearch() {
	emit searchModified();
}

void
radselectView::strokeLimitChanged(int newvalue) {
	int low = strokes_low->value();
	int high = strokes_high->value();
	if(low > high)
		if(low == newvalue)
			strokes_high->setValue(newvalue);
		else
			strokes_low->setValue(newvalue);

	//This will force reevaluation of the list if it's needed
	QList<Kanji> newList = m_possibleKanji;
	listPossibleKanji(newList);
}

#include "radselectview.moc"
