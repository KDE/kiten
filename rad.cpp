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

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klistbox.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstandarddirs.h>

#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qtextcodec.h>
#include <qtooltip.h>

#include "kitenconfig.h"
#include "rad.h"

Rad::Rad() : QObject()
{
	loaded = false;
}

void Rad::load()
{
	if (loaded)
		return;

	KStandardDirs *dirs = KGlobal::dirs();
	QString radkfile = dirs->findResource("data", "kiten/radkfile");
	if (radkfile.isNull())
	{
		KMessageBox::error(0, i18n("Kanji radical information file not installed, so radical searching cannot be used."));
		return;
	}

	QFile f(radkfile);

	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Kanji radical information could not be loaded, so radical searching cannot be used."));
	}

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("eucJP"));
	Radical cur;
	while (!t.eof())
	{
		QString s = t.readLine();

		QChar first = s.at(0);
		if (first == '#') // comment!
		{
			// nothing
		}
		else if (first == '$') // header
		{
			// save previous one
			if( !cur.kanji().isNull() )
				list.append(cur);

			//first entry is trim(last 4 chars).. <rad><space><strokes>
			unsigned int strokes = s.right(2).toUInt();
			QString radical = QString(s.at(2));
			cur = Radical(radical, strokes);
		}
		else // continuation
		{
			cur.addKanji(s);
		}
	}

	// we gotta append the last one!!
	// this nagged jasonkb for a bit wondering why fue wasn't showing up ;)
	list.append(cur);

	f.close();

	loaded = true;
}

QStringList Rad::radByStrokes(unsigned int strokes)
{
	load();

	QStringList ret;
	bool hadOne = false;
	QValueListIterator<Radical> it = list.begin();

	do
	{
		if ((*it).strokes() == strokes)
		{
			ret.append((*it).radical());
			hadOne = true;
		}
		else if(hadOne) // shortcut because it's a sorted list
		{
			return ret;
		}
	}
	while (++it != list.end());

	return ret;
}

QStringList Rad::kanjiByRad(const QString &text)
{
	//kdDebug() << "kanjiByRad, text is " << text << endl;
	load();
	QStringList ret;

	QValueListIterator<Radical> it;
	for (it = list.begin(); it != list.end() && (*it).radical() != text; ++it)
	{
		//kdDebug() << "kanjiByRad, looping, radical is " << (*it).radical() << endl;
	}

	QString kanji = (*it).kanji();
	for (unsigned i = 0; i < kanji.length(); ++i)
	{
		//kdDebug() << "kanjiByRad, i is " << i << endl;
		ret.append(QString(kanji.at(i)));
	}

	return ret;
}

QStringList Rad::kanjiByRad(const QStringList &list)
{
	//kdDebug() << "kanjiByRad (list version)\n";

	QStringList ret;
	QValueList<QStringList> lists;

	for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it)
	{
		//kdDebug() << "loading radical " << *it << endl;
		lists.append(kanjiByRad(*it));
	}

	QStringList first = lists.first();
	lists.pop_front();

	for (QStringList::Iterator kit = first.begin(); kit != first.end(); ++kit)
	{
		//kdDebug() << "kit is " << *kit << endl;
		QValueList<bool> outcomes;
		for (QValueList<QStringList>::Iterator it = lists.begin(); it != lists.end(); ++it)
		{
			//kdDebug() << "looping through lists\n";
			outcomes.append((*it).contains(*kit) > 0);
		}

		const bool containsBool = false;
		if ((outcomes.contains(containsBool) < 1))
		{
			//kdDebug() << "appending " << *kit << endl;
			ret.append(*kit);
		}
		else
		{
			//kdDebug() << "not appending " << *kit << endl;
		}
	}

	return ret;
}

Radical Rad::radByKanji(const QString &text)
{
	load();
	QString ret;

	QValueListIterator<Radical> it;
	for (it = list.end(); it != list.begin() && (*it).kanji().find(text) == -1; --it);

	return (*it);
}

unsigned int Rad::strokesByRad(const QString &text)
{
	load();
	QValueListIterator<Radical> it;
	for(it = list.begin(); it != list.end() && (*it).radical() != text; ++it);

	return (*it).strokes();
}

Rad::~Rad()
{
}

///////////////////////////////////////////////

RadWidget::RadWidget(Rad *_rad, QWidget *parent, const char *name) : QWidget(parent, name)
{
	hotlistNum = 3;

	rad = _rad;
	QHBoxLayout *hlayout = new QHBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	QVBoxLayout *vlayout = new QVBoxLayout(hlayout, KDialog::spacingHint());

	hotlistGroup = new QButtonGroup(1, Horizontal, i18n("Hotlist"), this);
	//hotlistGroup->setRadioButtonExclusive(true);
	vlayout->addWidget(hotlistGroup);

	Config* config = Config::self();

	hotlist = config->hotlist();

	while (hotlist.size() > hotlistNum)
		hotlist.pop_front();

	for (unsigned int i = 0; i < hotlistNum; ++i)
	{
		if (i >= hotlistNum)
			break;

		hotlistGroup->insert(new KPushButton(*hotlist.at(i), hotlistGroup), i);
	}
	connect(hotlistGroup, SIGNAL(clicked(int)), SLOT(hotlistClicked(int)));

	QVBoxLayout *layout = new QVBoxLayout(vlayout, KDialog::spacingHint());

	totalStrokes = new QCheckBox(i18n("Search by total strokes"), this);
	connect(totalStrokes, SIGNAL(clicked()), this, SLOT(totalClicked()));
	layout->addWidget(totalStrokes);

	QHBoxLayout *strokesLayout = new QHBoxLayout(layout, KDialog::spacingHint());
	totalSpin = new QSpinBox(1, 30, 1, this);
	strokesLayout->addWidget(totalSpin);
	strokesLayout->addStretch();
	totalErrLabel = new QLabel(i18n("+/-"), this);
	strokesLayout->addWidget(totalErrLabel);
	totalErrSpin = new QSpinBox(0, 15, 1, this);
	strokesLayout->addWidget(totalErrSpin);

	ok = new KPushButton(i18n("&Look Up"), this);
	ok->setEnabled(false);
	connect(ok, SIGNAL(clicked()), SLOT(apply()));
	layout->addWidget(ok);
	cancel = new KPushButton( KStdGuiItem::cancel(), this );

	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	layout->addWidget(cancel);

	QVBoxLayout *middlevLayout = new QVBoxLayout(hlayout, KDialog::spacingHint());

	strokesSpin = new QSpinBox(1, 17, 1, this);
	QToolTip::add(strokesSpin, i18n("Show radicals having this number of strokes"));
	middlevLayout->addWidget(strokesSpin);

	List = new KListBox(this);
	middlevLayout->addWidget(List);
	connect(List, SIGNAL(executed(QListBoxItem *)), this, SLOT(executed(QListBoxItem *)));
	connect(strokesSpin, SIGNAL(valueChanged(int)), this, SLOT(updateList(int)));

	QVBoxLayout *rightvlayout = new QVBoxLayout(hlayout, KDialog::spacingHint());
	selectedList = new KListBox(this);
	rightvlayout->addWidget(selectedList);
	connect(selectedList, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

	remove = new KPushButton(i18n("&Remove"), this);
	rightvlayout->addWidget(remove);
	connect(remove, SIGNAL(clicked()), this, SLOT(removeSelected()));
	remove->setEnabled(false);

	clear = new KPushButton(KStdGuiItem::clear(), this);
	rightvlayout->addWidget(clear);
	connect(clear, SIGNAL(clicked()), this, SLOT(clearSelected()));
	clear->setEnabled(false);

	setCaption(kapp->makeStdCaption(i18n("Radical Selector")));

	strokesSpin->setValue(config->strokes());
	strokesSpin->setFocus();

	totalSpin->setValue(config->totalStrokes());
	totalErrSpin->setValue(config->totalStrokesErrorMargin());
	totalStrokes->setChecked(config->searchByTotal());

	// make sure the right parts of the total stroke
	// selection system are enabled
	totalClicked();

 	// initially show the list of radicals to choose from
	updateList(strokesSpin->value());
}

RadWidget::~RadWidget()
{
}

void RadWidget::hotlistClicked(int num)
{
	addToSelected(*hotlist.at(num));
}

void RadWidget::executed(QListBoxItem *item)
{
	addToSelected(item->text());
}

void RadWidget::clearSelected()
{
	selectedList->clear();
	selected.clear();
	numChanged();
}

void RadWidget::removeSelected()
{
	int currentItem = selectedList->currentItem();
	if (currentItem != -1)
	{
		selectedList->removeItem(currentItem);
		selected.remove(selected.at(currentItem));

		numChanged();
		selectionChanged();
	}
}

void RadWidget::numChanged()
{
	ok->setEnabled(selectedList->count() > 0);
	clear->setEnabled(selectedList->count() > 0);
}

void RadWidget::addRadical(const QString &radical)
{
	addToSelected(radical);
}

void RadWidget::addToSelected(const QString &text)
{
	if (!text.isNull() && !selected.contains(text))
	{
		selectedList->insertItem(text);
		selected.append(text);

		numChanged();
		selectionChanged();
	}
}

void RadWidget::selectionChanged()
{
	//kdDebug() << "selectionChanged()" << endl;
	remove->setEnabled(selectedList->currentItem() != -1);
}

void RadWidget::updateList(int strokes)
{
	List->clear();
	List->insertStringList(rad->radByStrokes(static_cast<unsigned int>(strokes)));
}

void RadWidget::apply()
{
	
	if (selected.count() < 1)
		return;

	emit set(selected, totalStrokes->isChecked() ? totalSpin->value() : 0, totalErrSpin->value());

	Config* config = Config::self();
	config->setStrokes(strokesSpin->value());
	config->setTotalStrokes(totalSpin->value());
	config->setTotalStrokesErrorMargin(totalErrSpin->value());
	config->setSearchByTotal(totalStrokes->isChecked());

	for (QStringList::Iterator it = selected.begin(); it != selected.end(); ++it)
	{
		if (hotlist.find(*it) == hotlist.end())
		{
			if (hotlist.size() >= hotlistNum)
				hotlist.pop_front(); // stupid stl functions in Qt .. ;)
			hotlist.append(*it);

			config->setHotlist(hotlist);
		}
	}
	config->writeConfig();
	close();
}

void RadWidget::totalClicked()
{
	bool enable = totalStrokes->isChecked();
	totalSpin->setEnabled(enable);
	totalErrSpin->setEnabled(enable);
	totalErrLabel->setEnabled(enable);
}

//////////////////////////////////////////////

Radical::Radical(QString text, unsigned int strokes)
{
	_Radical = text;
	Strokes = strokes;
}

void Radical::addKanji(const QString &text)
{
	Kanji.append(text);
}

#include "rad.moc"
