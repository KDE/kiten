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

#ifndef _RADSELECTBUTTONGRID_H_
#define _RADSELECTBUTTONGRID_H_

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtCore/QSet>

class QMouseEvent;

class Radical : public QString {
private:
	unsigned int stroke_count;
public:
	Radical(QString irad, unsigned int istrokes=0)
		: QString(irad.at(0)), stroke_count(istrokes) { }
	unsigned int strokes() {return stroke_count;}
};

class radicalButton : public QPushButton {
	Q_OBJECT
public:
	radicalButton(const QString& a, QWidget* b);
	virtual ~radicalButton() {}
signals:
	void rightClicked(const QString&);
	void leftReleased(const QString&);
public slots:
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
};

class radselectButtonGrid : public QWidget {
	Q_OBJECT
public:
	radselectButtonGrid(QWidget *parent);
	virtual ~radselectButtonGrid() {}

signals:
	/** Our generic message alert signal */
	void signalChangeStatusbar(const QString& text);
	/** We emit this when the user has selected a radical for the search bar */
	void addRadicalToList(const QString&);
	/** We want a radical removed from the selection list */
	void removeRadicalFromList(const QString&);

public slots:
	/** Reset all buttons to the up and uncolored state */
	void clearSelections();
	void setFont(const QFont&);

private:
	bool loadRadicalFile();		// Reads the data from the radkfile
	void buildRadicalButtons(QWidget*);

	static const unsigned int number_of_radical_columns = 11;

	QSet<Radical> radicals;						//Radical list
	QHash<QString, radicalButton*> buttons; //Radical -> Button Mapping
	QHash<QString, QSet<QString> > krad;  //Kanji -> [Radical]* Mapping
	QHash<QString, QSet<QString> > radk;	//Radical -> Kanji Mapping
};

#endif
