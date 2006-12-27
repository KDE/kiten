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
protected:
	unsigned int stroke_count;
	QSet<QString> kanji;
public:
	Radical(QString irad, unsigned int istrokes=0)
		: QString(irad.at(0)), stroke_count(istrokes) { }
	unsigned int strokes() const {return stroke_count;}
	const QSet<QString>	&getKanji() const {return kanji;}
	void addKanji(const QSet<QString> newKanji) {kanji += newKanji;}
};

class Kanji : public Radical {
private: void addKanji(const QSet<QString> newKanji) {}
		const QSet<QString>	&getKanji() const {}
public:
	Kanji(const QString &ikanji, const QSet<QString> &radicals) :
		Radical(ikanji.at(0))  {kanji = radicals;}
	void calculateStrokes() {
		stroke_count = 0;
		foreach( Radical it, kanji )
			stroke_count += it.strokes();
	}
	const QSet<QString> &getRadicals() const {return kanji;}
	void addRadical(const QString &it) {kanji +=it;}
};

class radicalButton : public QPushButton {
	Q_OBJECT
public:
	radicalButton(const QString& a, QWidget* b);
	virtual ~radicalButton() {}
	typedef enum {kNormal,  // Normal button
		kSelected,				// This button has been selected: bold + underline
		kNotAppropriate,		// Due to other selected buttons: disabled
		kRelated,				// Display only this radical and related ones: italics?
		kHidden}					// Not related (to above), so hide()
	ButtonStatus;
signals:
	void userClicked(const QString&, radicalButton::ButtonStatus);
public slots:
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void resetButton();
	void setStatus(radicalButton::ButtonStatus);
private:
	ButtonStatus status;
};

class radselectButtonGrid : public QWidget {
	Q_OBJECT
public:
	radselectButtonGrid(QWidget *parent);
	virtual ~radselectButtonGrid() {}

signals:
	/** Our generic message alert signal */
	void signalChangeStatusbar(const QString&);
	/** We emit this whenever something changed. This gives our list of
	  suggested kanji */
	void possibleKanji(const QSet<QString>&);
	void clearButtonSelections();

public slots:
	/** Setup stroke range and base limiters */
	void changeStrokeBase(int);
	void changeStrokeRange(int);
	/** Triggered by a button press */
	void radicalClicked(const QString&, radicalButton::ButtonStatus);
	/** Reset all buttons to the up and uncolored state */
	void clearSelections();
	void setFont(const QFont&);

private:
	bool loadRadicalFile();		// Reads the data from the radkfile
	void buildRadicalButtons(QWidget*);
	void calculateStrokeRange();
	void updateButtons();

	static const unsigned int number_of_radical_columns = 11;
	static const unsigned int maximumStrokeValue = 50;
	enum {kSelection, kRelational} currentMode;

	QString relationalRadical;
	QSet<QString> selectedRadicals;
	unsigned int strokeBase, strokeRange, strokeMin, strokeMax;

	QSet<Radical> radicals;						//Radical list
	QHash<QString, Kanji > kanji;				//Kanji List
	QHash<QString, radicalButton*> buttons; //Radical -> Button Mapping
};

#endif
