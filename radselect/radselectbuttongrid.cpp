/* This file is part of kiten, a KDE Japanese Reference Tool
   opyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

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
*	Design a custom QGridLayout to rearrange buttons dynamically to resize
*	Design multiple radical file handling
*/

#include "radselectbuttongrid.h"

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QTextCodec>

#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QMouseEvent>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>

#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>

#include <kdebug.h>

radselectButtonGrid::radselectButtonGrid(QWidget *parent)
    : QWidget(parent), currentMode(kSelection)
{
    loadRadicalFile();

    buildRadicalButtons(this);
}

bool radselectButtonGrid::loadRadicalFile()
{
	//Find our radical file
	KStandardDirs *dirs = KGlobal::dirs();
	QString radkfile = dirs->findResource("data", "kiten/radkfile");
	if (radkfile.isNull())
	{
		KMessageBox::error(0, i18n("Kanji radical information file not installed, so radical searching cannot be used."));
		return false;
	}

	//Open our radical file
	QFile f(radkfile);
	if (!f.open(QIODevice::ReadOnly))
	{
		KMessageBox::error(0, i18n("Kanji radical information could not be loaded, so radical searching cannot be used."));
		return false;
	}

	//Read our radical file through a eucJP codec (helpfully builtin to Qt)
	QTextStream t(&f);
	Radical *newestRadical = NULL;
	QString radical="";

	t.setCodec(QTextCodec::codecForName("eucJP"));
	while (!t.atEnd())
	{
		QString line = t.readLine();
		if(line.at(0) == '#')	//Skip comment characters
			continue;
		else if(line.at(0) == '$') {	//Start of a new radical
			unsigned int strokes = line.right(2).toUInt();
			radical = QString(line.at(2));
			radicals.insert(Radical(radical,strokes));
		} else if(!radical.isEmpty()) {	// List of kanji, potentially
			QList<QString> kanjiList = line.split("");
			radk[radical] += kanjiList.toSet();
			foreach( QString kanji, kanjiList )
				krad[kanji] += radical;
		}
	}

	f.close();

	return true;
}


void radselectButtonGrid::buildRadicalButtons(QWidget* box)
{
	unsigned int i;

	//Setup the grid
	QGridLayout *grid = new QGridLayout(box);

	//Now make labels
	for(i=0;i<number_of_radical_columns;i++){
		QLabel *header = new QLabel(QString::number(i+1),box);
		header->setAlignment(Qt::AlignHCenter);
		grid->addWidget(header,0,i);
	}

	//Sort through and do each column separately to naturally setup tab order
	//It's inefficient, but the number of radicals should only be a few hundred
	for(i=0;i<number_of_radical_columns;i++) {
		int row_index = 1; //Reset where we insert the buttons

		foreach(Radical it, radicals) {
			//For each radical, figure out which slot it goes in
			unsigned int column_index = it.strokes()-1;
			if(column_index >= number_of_radical_columns)
				column_index = number_of_radical_columns-1;

			if(column_index != i)
				continue; //Skip if this is the wrong column

			radicalButton *button = new radicalButton(it,box);
			//Note that this is slightly naughty... since this does not consult
			//QStyle before setting this. (see QPushButton's sizeHint() for what
			//one should do, but that looks fugly in this case)
			QFontMetrics fm = button->fontMetrics();
			QSize sz = fm.size(Qt::TextShowMnemonic, it);
			button->setMinimumSize(sz);

			grid->addWidget(button, row_index++, column_index);

			//Bind slots/signals for this button
			connect(
				button, SIGNAL(userClicked(const QString&,radicalButton::ButtonStatus)),
				this, SLOT(radicalClicked(const QString&,radicalButton::ButtonStatus))
			);
			connect( this, SIGNAL( clearButtonSelections() ),
					button, SLOT( resetButton () ) );
			//Add this button to our list
			buttons.insert(it,button);
		}
	}
}

void radselectButtonGrid::setFont(const QFont &font) {
	foreach(QPushButton *button, buttons.values()) {
		button->setFont(font);
		QFontMetrics fm = button->fontMetrics();
		QSize sz = fm.size(Qt::TextShowMnemonic, button->text());
		button->setMinimumSize(sz);
	}
}

void radselectButtonGrid::radicalClicked(const QString &newrad,
		radicalButton::ButtonStatus newStatus) {
	if(newStatus == radicalButton::kRelated)
		; //Do something fancy
	else if(newStatus == radicalButton::kNormal ||
			newStatus == radicalButton::kSelected) {
		currentMode = kSelection;
		if(newStatus == radicalButton::kNormal)
			selectedRadicals.remove(newrad);
		else
			selectedRadicals.insert(newrad);

		//Figure out what our kanji possibilites are
		QSet<QString> kanjiList;
		if(kanjiList.count() > 0)
			kanjiList = radk.begin().value();
		foreach(QString radical, selectedRadicals) {
			kanjiList &= radk.value(radical); //Make a set intersection of these kanji
		}
		emit possibleKanji(kanjiList); //And tell the world!

		//Now figure out what our remaining radical possibilities are
		QSet<QString> remainingRadicals;
		foreach(QString kanji, kanjiList)
			remainingRadicals |= krad.value(kanji);
		//Remove the already selected ones
		remainingRadicals -= selectedRadicals;

		//Now go through and set status appropriately
		QHash<QString, radicalButton*>::iterator i = buttons.begin();
		while(i != buttons.end()) {
			if(selectedRadicals.contains(i.key()))
				i.value()->setStatus(radicalButton::kSelected);
			else if(remainingRadicals.contains(i.key()))
				i.value()->setStatus(radicalButton::kNormal);
			else
				i.value()->setStatus(radicalButton::kNotAppropriate);
			++i;
		}
	}

}

void radselectButtonGrid::changeStrokeBase(int newBase) {
}

void radselectButtonGrid::changeStrokeRange(int newRange) {
}

void radselectButtonGrid::clearSelections() {
	emit clearButtonSelections();
}

///////////////////////////////////////////////////////////////////////////////
// radicalButton methods
///////////////////////////////////////////////////////////////////////////////

radicalButton::radicalButton(const QString &text, QWidget *parent)
	: QPushButton(text,parent) {
}

void radicalButton::setStatus(radicalButton::ButtonStatus newStatus) {
	if(status == newStatus) return;
	//Because it's more work to check everything rather than just set it,
	//we'll just set everything every time
	bool underline=false, italic=false, hidden=false, disabled=false;
	switch(newStatus) {
		case kNormal:
			break;
		case kSelected:
			underline=true; italic=true; break;
		case kNotAppropriate:
			disabled=true;	break;
		case kRelated:
			italic=true; break;
		case kHidden:
			hidden=true;
	}
	QFont theFont = font();
	theFont.setUnderline(underline);
	theFont.setItalic(italic);
	setFont(theFont);
	setVisible(!hidden);
	setEnabled(!disabled);
}


void radicalButton::resetButton() {
	setStatus(kNormal);
}

void radicalButton::mousePressEvent(QMouseEvent *e)
{
	QPushButton::mousePressEvent(e);
	if(e->button() == Qt::RightButton) {
		setStatus(kRelated);
		emit userClicked(text(), kRelated);
	}
}
void radicalButton::mouseReleaseEvent(QMouseEvent *e){
	QPushButton::mouseReleaseEvent(e);
	if(e->button() == Qt::LeftButton) {
		setStatus(kSelected);
		emit userClicked(text(), kSelected);
	}
}

#include "radselectbuttongrid.moc"
