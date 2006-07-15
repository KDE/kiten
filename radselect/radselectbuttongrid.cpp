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


/*  TODO:
*	Double check that I need to subclass the QPushButton
*	Ditch some of this massive pile of includes, investigate KDE classes
*	Add an alternate stroke setup (setable in preferences)
*	Add preferences hooks
		radical files
		FONTS!!!
*	Clean up alot of the layout code (both: radical list & main)

    Future Plans:
*	Design a custom QGridLayout to rearrange buttons dynamically to resize
*	Make radical search list a two/three column field with delete button
*	Design multiple radical file handling
*	Build a proper exception handling framework
*	Icon set for properly displayed radicals
*	Radical Decomposition, and associated interface
*	Rebuild most of this file as a ui file, handle any special code in
		radselect.cpp, and move the smaller classes to their own file.

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

radselectButtonGrid::radselectButtonGrid(QWidget *parent)
    : QWidget(parent)
{
    loaded=false;
    loadRadicalFile();

    buildRadicalList(this);
}

bool radselectButtonGrid::loadRadicalFile()
{
	if (loaded)
		return true;

	KStandardDirs *dirs = KGlobal::dirs();
	QString radkfile = dirs->findResource("data", "kiten/radkfile");
	if (radkfile.isNull())
	{
		KMessageBox::error(0, i18n("Kanji radical information file not installed, so radical searching cannot be used."));
		return false;
	}

	QFile f(radkfile);

	if (!f.open(QIODevice::ReadOnly))
	{
		KMessageBox::error(0, i18n("Kanji radical information could not be loaded, so radical searching cannot be used."));
		return false;
	}

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("eucJP"));
	while (!t.atEnd())
	{
		QString s = t.readLine();
		if(s.at(0) == '$') {
			unsigned int strokes = s.right(2).toUInt();
			QString radical = QString(s.at(2));
			radicals.insert(radical,new Radical(radical,strokes));
		}
	}

	f.close();

	loaded = true;
	return true;
}


void radselectButtonGrid::buildRadicalList(QWidget* box)
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

	//Sort through and do each column seperately to naturally setup tab order
	//It's inefficient, but the number of radicals should only be a few hundred
	for(i=0;i<number_of_radical_columns;i++) {
		int row_index = 1; //Reset where we insert the buttons

		foreach(Radical *it, radicals) {
			//For each radical, figure out which slot it goes in
			unsigned int column_index = it->strokes()-1;
			if(column_index >= number_of_radical_columns)
				column_index = number_of_radical_columns-1;

			if(column_index != i)
				continue; //Skip if this is the wrong column

			radicalButton *button = new radicalButton(*it,box);
			grid->addWidget(button, row_index++, column_index);
		
			//Bind slots/signals for this button
    		connect( button, SIGNAL( leftReleased(const QString&) ), 
					this, SIGNAL( addRadicalToList(const QString&) ) );
		}
	}
}

void radselectButtonGrid::clearSelections() {
	//In the future, this will do something
}

///////////////////////////////////////////////////////////////////////////////
// radicalButton methods
///////////////////////////////////////////////////////////////////////////////

radicalButton::radicalButton(const QString &text, QWidget *parent)
	: QPushButton(text,parent) {
}

void radicalButton::mousePressEvent(QMouseEvent *e)
{
	QPushButton::mousePressEvent(e);
	if(e->button() == Qt::RightButton)
		emit rightClicked(text());
}
void radicalButton::mouseReleaseEvent(QMouseEvent *e){
	QPushButton::mouseReleaseEvent(e);
	if(e->button() == Qt::LeftButton)
		emit leftReleased(text());
}

#include "radselectbuttongrid.moc"
