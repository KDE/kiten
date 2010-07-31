/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
	       (C) 2005 Paul Temple <paul.temple@gmx.net>

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

#ifndef RAD_H
#define RAD_H

#include <tqstringlist.h>

class Config;
class QCheckBox;
class QLabel;
class QListBoxItem;
class QSpinBox;
class KPushButton;
class KListBox;
class QButtonGroup;

class KDE_EXPORT Radical
{
	public:
	Radical(TQString = TQString::null, unsigned int = 0);

	TQString radical() { return _Radical; }
	unsigned int strokes() { return Strokes; }
	TQString kanji() { return Kanji; }

	void addKanji(const TQString &);

	private:
	TQString _Radical;
	unsigned int Strokes;
	TQString Kanji;
};

class KDE_EXPORT Rad : public QObject
{
	Q_OBJECT
	
	public:
	Rad();
	~Rad();

	TQStringList radByStrokes(unsigned int);
	TQStringList kanjiByRad(const TQString &);
	TQStringList kanjiByRad(const TQStringList &);
	Radical radByKanji(const TQString &);
	unsigned int strokesByRad(const TQString &);

	private:
	void load();

	TQValueList<Radical> list;
	bool loaded;
};

class KDE_EXPORT RadWidget : public QWidget
{
	Q_OBJECT

	public:
	RadWidget(Rad *, TQWidget *parent = 0, const char *name = 0);
	~RadWidget();

	signals:
	// if totalStrokes == 0, then don't search by total strokes
	void set(const TQStringList &radical, unsigned int totalStrokes, unsigned int totalStrokesErr);

	public slots:
	void addRadical(const TQString &);

	private slots:
	void updateList(int);
	void apply();
	void totalClicked(void);
	void selectionChanged();
	void hotlistClicked(int);
	void addToSelected(const TQString &);
	void executed(TQListBoxItem *);
	void removeSelected();
	void clearSelected();
	
	private:
	TQSpinBox *strokesSpin;
	TQSpinBox *totalSpin;
	TQSpinBox *totalErrSpin;
	TQLabel *totalErrLabel;
	KPushButton *ok;
	KPushButton *cancel;
	KPushButton *remove;
	KPushButton *clear;
	TQButtonGroup *hotlistGroup;
	TQCheckBox *totalStrokes;
	KListBox *List;
	KListBox *selectedList;
	TQStringList selected;

	Rad *rad;

	unsigned int hotlistNum;
	TQStringList hotlist;

	void numChanged();
};

#endif
