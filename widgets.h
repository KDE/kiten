/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>

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

#include <tqguardedptr.h>

#include <kaction.h>
#include <klineedit.h>
#include <kmainwindow.h>
#include <ktextbrowser.h>

class KListView;
class KStatusBar;

#include "dict.h"
#include "rad.h"

class KDE_EXPORT ResultView : public KTextBrowser
{
	Q_OBJECT
	
	public:
	ResultView(bool showLinks, TQWidget *parent = 0, const char *name = 0);

	void addResult(Dict::Entry result, bool common = false);
	void addKanjiResult(Dict::Entry, bool common = false, Radical = Radical());

	void addHeader(const TQString &, unsigned int degree = 3);

	public slots:
	void print(TQString = TQString::null);
	void append(const TQString &);
	void flush();
	void clear();
	void setBasicMode(bool yes) { basicMode = yes; }

	void updateFont();

	private:
	TQString putchars(const TQString &);
	TQString printText;

	bool links;
	bool basicMode;
};

class KDE_EXPORT eEdit : public KMainWindow
{
	Q_OBJECT
	
	public:
	eEdit(const TQString &, TQWidget *parent = 0, const char *name = 0);
	~eEdit();

	private slots:
	void add();
	void save();
	void del();
	void disable();
	void openFile(const TQString &);

	private:
	KListView *List;
	TQString filename;
	KStatusBar *StatusBar;
	KAction *addAct;
	KAction *removeAct;
	KAction *saveAct;
	bool isMod;
};

class KDE_EXPORT EditAction : public KAction
{
	Q_OBJECT
	public:
	EditAction( const TQString& text, int accel, const TQObject *receiver, const char *member, TQObject* parent, const char* name );
	virtual ~EditAction();

	virtual int plug( TQWidget *w, int index = -1 );

	virtual void unplug( TQWidget *w );

	TQString text() { return m_combo->text(); }
	void setText(const TQString &text);

	TQGuardedPtr<KLineEdit> editor();

	public slots:
	void insert(const TQString &);

	// also sets focus to the lineedit widget
	void clear();

	signals:
	void plugged();

private:
    TQGuardedPtr<KLineEdit> m_combo;
    const TQObject *m_receiver;
    const char *m_member;
};

#endif
