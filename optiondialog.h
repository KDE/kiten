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

#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <kdialogbase.h>
#include <qwidget.h>

class KListView;
class QString;
class KKeyChooser;
class KFontChooser;
class QLabel;
class QPushButton;
class QComboBox;
class QCheckBox;
class KGlobalAccel;
class DictList;

class ConfigureDialog : public KDialogBase
{
	Q_OBJECT

	public:
	ConfigureDialog(KGlobalAccel *accel, QWidget *parent = 0, char *name = 0, bool modal = 0);
	~ConfigureDialog();

	protected slots:
	virtual void slotOk();
	virtual void slotApply();

	public slots:
	void openURL(const QString &url);

	private slots:
	void readConfig();
	void writeConfig();

	private:
	DictList *DictDictList;
	DictList *KanjiDictList;
	QCheckBox *wholeWordCB;
	QCheckBox *caseSensitiveCB;
	QCheckBox *startLearnCB;
	QComboBox *quizOn;
	QComboBox *guessOn;
	KFontChooser *font;

	KGlobalAccel *Accel;
	KKeyChooser *Chooser;
	
	signals:
	void valueChanged();
};

class DictList : public QWidget
{
	Q_OBJECT
	public:
	DictList(const QString &configkey, QWidget *parent = 0, char *name = 0);
	void readConfig();
	void writeConfig();

	private slots:
	void add();
	void del();

	private:
	QPushButton *AddButton;
	QPushButton *DelButton;
	QCheckBox *useGlobal;

	KListView *List;

	QString _configKey;
};

#endif
