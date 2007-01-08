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
#include <QtCore/QSet>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtGui/QWidget>

#include "radicalbutton.h"
#include "radicalfile.h"

class radselectButtonGrid : public QWidget {
	Q_OBJECT
public:
	radselectButtonGrid(QWidget *parent, radicalFile *iradicalInfo);
	virtual ~radselectButtonGrid() {}

signals:
	/** Our generic message alert signal */
	void signalChangeStatusbar(const QString&);
	/** We emit this whenever something changed. This gives our list of
	  suggested kanji */
	void possibleKanji(const QList<Kanji>&);
	void clearButtonSelections();

public slots:
	/** Triggered by a button press */
	void radicalClicked(const QString&, radicalButton::ButtonStatus);
	/** Reset all buttons to the up and uncolored state */
	void clearSelections();
	void setFont(const QFont&);

private:
	void buildRadicalButtons(QWidget*);
	void updateButtons();

	static const unsigned int number_of_radical_columns = 11;
	static const unsigned int maximumStrokeValue = 50;

	enum {kSelection, kRelational} m_currentMode;

	QString m_relationalRadical;
	QSet<QString> m_selectedRadicals;
	radicalFile *m_radicalInfo;

	QHash<QString, radicalButton*> m_buttons; //Radical -> Button Mapping
};

#endif
