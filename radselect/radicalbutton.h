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

#ifndef _RADICALBUTTON_H_
#define _RADICALBUTTON_H_

#include <QtCore/QString>
#include <QtGui/QPushButton>

class QMouseEvent;

class radicalButton : public QPushButton {
	Q_OBJECT
public:
	radicalButton(const QString& a, QWidget* b);
	virtual ~radicalButton() {}
	bool event(QEvent *);	//Overriding QPushButton's event for mousewheel
									//events on a disabled button

	typedef enum {kNormal,  // Normal button
		kSelected,				// This button has been selected: bold + underline
		kNotAppropriate,		// Due to other selected buttons: disabled
		kRelated,				// Display only this radical and related ones: italics?
		kHidden}					// Not related (to above), so hide()
	ButtonStatus;
	QSize minimumSizeHint() const;
	QSize sizeHint() const;
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

#endif
