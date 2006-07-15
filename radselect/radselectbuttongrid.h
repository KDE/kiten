/***************************************************************************
 *   Copyright (C) 2005 by Joseph Kerian   *
 *   jkerian@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef _RADSELECTBUTTONGRID_H_
#define _RADSELECTBUTTONGRID_H_

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>

class QMouseEvent;

class Radical : public QString {  //I'd use QChar... but it's basically unusable
private:
	unsigned int stroke_count;
public: Radical(QString irad,unsigned int istrokes):QString(irad.at(0))
		{stroke_count=istrokes;} //Only record first character
	virtual ~Radical() {}
	unsigned int strokes() {return stroke_count;}
	void addComponant(const Radical&) {} //Not implemented yet :(
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

class radselectButtonGrid : public QWidget
{
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

private: //private functions we need
	bool loadRadicalFile();  		// Reads the data from the radkfile
	void buildRadicalList(QWidget*);

private: //Private variables
	bool loaded;
	static const unsigned int number_of_radical_columns = 11;
				//Higher stroke counts are bunched together
	QHash<QString,Radical*> radicals; 

};

#endif
