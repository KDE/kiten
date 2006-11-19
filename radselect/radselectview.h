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

#ifndef _RADSELECTVIEW_H_
#define _RADSELECTVIEW_H_

//generated from the uic from radical_selector.ui (defines Ui::radical_selector)
#include "ui_radical_selector.h" 

#include <QtGui/QWidget>
#include <QtCore/QSet>

class QString;
class QStringList;
class QListWidgetItem;
class radselectButtonGrid;

class radselectView : public QWidget, public Ui::radical_selector
{
	Q_OBJECT
public:
	radselectView(QWidget *parent);
	virtual ~radselectView();
	void load(QString iRadicals, QString iStrokes);
						//Load pre-determined search parameters

signals:
	void signalChangeStatusbar(const QString& text);
	void searchModified(); //listen for this if you want to detect each minor change
	void searchTrigger(const QStringList& radicals_result, 
			const QString& stroke_result);
			//this will be emitted when the user pushes "Search"

private slots:
	void loadSettings();
	void startSearch();		//Someone has clicked the search button! (uses getSearchInfo)
	QString getSearchInfo(QString& radicals_result,QString& stroke_result);
			// Grab all the info this widget provides (returns a formatted search string)
	void listPossibleKanji(const QSet<QString>&); //Resets the list
	void queueDeleteRadical(QListWidgetItem*); //Prepare to remove an item from search list
	void deleteRadical();	//Called via QTimer by the previous, uses private 'victim'
	void clearSearch();		//Clear everything
	void changedSearch();		//connected to any changes in searches, emits searchModified

private: //Private variables
	QListWidgetItem *victim; //List item to be deleted from the radical search list
	radselectButtonGrid *buttons;

};


#endif
