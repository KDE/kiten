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


#include "radicalbutton.h"
#include <QtGui/QPushButton>

#include <QtCore/QString>

#include <QtGui/QWidget>
#include <QtGui/QMouseEvent>

radicalButton::radicalButton(const QString &text, QWidget *parent)
	: QPushButton(text,parent) {
		setCheckable(true);
}

void radicalButton::setStatus(radicalButton::ButtonStatus newStatus) {
	if(status == newStatus) return;
	//Because it's more work to check everything rather than just set it,
	//we'll just set everything every time
	bool checked=false, underline=false, italic=false, hidden=false, disabled=false;
	switch(newStatus) {
		case kNormal:
			break;
		case kSelected:
			checked=true; break;
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
	setChecked(checked);
	status = newStatus;
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
		switch(status) {
			case kSelected:
				setStatus(kNormal);
				emit userClicked(text(), kNormal);
				break;
			default:
				setStatus(kSelected);
				emit userClicked(text(), kSelected);
		}
	}
}

#include "radicalbutton.moc"