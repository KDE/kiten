/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
 		(C) 2006 Joseph Kerian <jkerian@gmail.com>
		(C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#ifndef KITEN_WIDGETS_H
#define KITEN_WIDGETS_H

#include <qpointer.h>

#include <kaction.h>
#include <klineedit.h>
#include <kmainwindow.h>
#include <ktextbrowser.h>
#include <khtml_part.h>
#include <khtmlview.h>

class KListView;
class KStatusBar;
class KActionMenu;
class KActionCollection;

#include "entry.h"

class ResultView : public KHTMLPart
{
	Q_OBJECT

	public:
		ResultView(QWidget *parent = 0, const char *name = 0);

		void addResult(Entry *result, bool common = false);
		void addKanjiResult(Entry*, bool common = false);

	public slots:
		void print(QString = QString::null);
		void append(const QString &);
		void flush();
		void clear();
		void setContents(const QString &);
		void setBasicMode(bool yes) { basicMode = yes; }
		void openPopupMenu( const QString&, const QPoint&);

		void updateFont();

	signals:
		void urlClicked( const QString& );
		void entrySpecifiedForExport(int index);

	protected:
		virtual void urlSelected (const QString &url, int button, int state, const QString &	_target, KParts::URLArgs args = KParts::URLArgs());
		DOM::Node findEntryNode(DOM::Node node);
		DOM::Node findChildWithClassValue(DOM::Node node, const QString& classValue);
		QString deLinkify(DOM::Node);

	private:
		QString printText;
		bool basicMode;
		QAction *addToExportListAction;
		KActionCollection *popupActions;
		KActionMenu *popupMenu;
};

#endif