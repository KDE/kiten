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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef KITEN_WIDGETS_H
#define KITEN_WIDGETS_H

#include <klineedit.h>
#include <kmainwindow.h>
#include <ktextbrowser.h>

class KAction;
class KListView;
class KStatusBar;

#include "rad.h"

class ResultView : public KTextBrowser
{
	Q_OBJECT
	
	public:
	ResultView(bool showLinks, QWidget *parent = 0, const char *name = 0);

	void addResult(Dict::Entry result, bool common = false);
	void addKanjiResult(Dict::Entry, bool common = false, Radical = Radical());

	void addHeader(const QString &, unsigned int degree = 3);

	public slots:
	void print(QString = QString::null);
	void append(const QString &);
	void flush();
	void clear();
	void setBasicMode(bool yes) { basicMode = yes; }

	void updateFont();

	private:
	QString putchars(const QString &);
	QString printText;

	bool links;
	bool basicMode;
};

class eEdit : public KMainWindow
{
	Q_OBJECT
	
	public:
	eEdit(const QString &, QWidget *parent = 0, const char *name = 0);
	~eEdit();

	private slots:
	void add();
	void save();
	void del();
	void disable();
	void openFile(const QString &);

	private:
	KListView *List;
	QString filename;
	KStatusBar *StatusBar;
	KAction *addAct;
	KAction *removeAct;
	KAction *saveAct;
	bool isMod;
};

class EditAction : public KAction
{
	Q_OBJECT
	public:
	EditAction( const QString& text, int accel, const QObject *receiver, const char *member, QObject* parent, const char* name );
	~EditAction();

	virtual int plug( QWidget *w, int index = -1 );

	virtual void unplug( QWidget *w );

	QString text() { return m_combo->text(); }
	void setText(const QString &text);

	QGuardedPtr<KLineEdit> editor();

	public slots:
	void insert(QString);

	// also sets focus to the lineedit widget
	void clear();

	signals:
	void plugged();

private:
    QGuardedPtr<KLineEdit> m_combo;
    const QObject *m_receiver;
    const char *m_member;
};

#endif
