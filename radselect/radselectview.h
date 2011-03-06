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

#ifndef RADSELECTVIEW_H
#define RADSELECTVIEW_H

//generated from the uic from radical_selector.ui (defines Ui::radical_selector)
#include "ui_radical_selector.h"

#include <QtGui/QWidget>
#include <QtCore/QList>
#include <QtCore/QSet>

class QString;
class QStringList;
class QListWidgetItem;
class radselectButtonGrid;

#include "radicalfile.h"	//For Kanji/radicalFile definitions

class radselectView : public QWidget, public Ui::radical_selector
{
	Q_OBJECT
public:
	radselectView(QWidget *parent);
	virtual ~radselectView();

	//Load pre-determined search parameters
	void loadRadicals(const QString &iRadicals, int strokeMin, int strokeMax);
	void loadKanji(QString &kanji);

signals:
	void signalChangeStatusbar(const QString& text);
	void searchModified(); //listen for this if you want to detect each minor change
	void kanjiSelected(const QStringList&); //This is when they've actually pressed a kanji

private slots:
	void loadSettings();
	void strokeLimitChanged(int);	//Handles either of the stroke limiters moving
	void changedSearch();		//connected to any changes in searches, emits searchModified
	void clearSearch();		//Clear everything
	void kanjiClicked(QListWidgetItem *);	//Result is clicked
	void kanjiDoubleClicked(QListWidgetItem *); //Result is double-clicked
	void listPossibleKanji(const QList<Kanji>&); //Sets the list of visible Kanji
	void toClipboard(); // copy text from lineedit to clipboard

private:
	radicalFile	*radicalInfo;
	radselectButtonGrid *m_buttongrid;
	QList<Kanji> m_possibleKanji;
};


#endif
