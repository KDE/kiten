#include <kglobal.h>
#include <kdebug.h>
#include <qbuttongroup.h>
#include <kdialog.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
//#include <qradiobutton.h>
#include <kconfig.h>
#include <klocale.h>
#include <kapp.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qtextcodec.h>

#include "rad.h"

Rad::Rad() : QObject()
{
	KStandardDirs *dirs = KGlobal::dirs();
	QString radkfile = dirs->findResource("appdata", "radkfile");
	if (radkfile == QString::null)
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
			if(cur.kanji() != QString::null)
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
}

QStringList Rad::radByStrokes(unsigned int strokes)
{
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

QStringList Rad::kanjiByRad(QString &text)
{
	QStringList ret;

	QValueListIterator<Radical> it;
	for(it = list.begin(); it != list.end() && (*it).radical() != text; ++it);

	QString kanji = (*it).kanji();
	for (unsigned i = 0; i < kanji.length(); ++i)
		ret.append(QString(kanji.at(i)));

	return ret;
}

Radical Rad::radByKanji(QString text)
{
	QString ret;

	QValueListIterator<Radical> it;
	for(it = list.end(); it != list.begin() && (*it).kanji().find(text) == -1; --it);

	return (*it);
}

unsigned int Rad::strokesByRad(QString text)
{
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
	hotlistNum = 5;

	rad = _rad;
	QHBoxLayout *hlayout = new QHBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	QVBoxLayout *vlayout = new QVBoxLayout(hlayout, KDialog::spacingHint());

	hotlistGroup = new QButtonGroup(1, Horizontal, i18n("Hotlist"), this);
	//hotlistGroup->setRadioButtonExclusive(true);
	vlayout->addWidget(hotlistGroup);

	KConfig *config = kapp->config();
	config->setGroup("Radical Searching");

	hotlist = config->readListEntry("Hotlist");
	unsigned int size = hotlist.size();
	for (unsigned int i = 0; i < hotlistNum; ++i)
	{
		if (i >= size)
			break;
		//hotlistGroup->insert(new QRadioButton(*hotlist.at(i), hotlistGroup), i);
		hotlistGroup->insert(new QPushButton(*hotlist.at(i), hotlistGroup), i);
	}
	connect(hotlistGroup, SIGNAL(clicked(int)), SLOT(hotlistClicked(int)));

	QVBoxLayout *layout = new QVBoxLayout(vlayout, KDialog::spacingHint());
	layout->addWidget(new QLabel(i18n("<strong>Radical</strong> strokes:"), this));
	strokesSpin = new QSpinBox(1, 17, 1, this);
	layout->addWidget(strokesSpin);
	layout->addStretch();
	
	totalStrokes = new QCheckBox(i18n("Search by total strokes"), this);
	connect(totalStrokes, SIGNAL(clicked()), this, SLOT(totalClicked()));
	layout->addWidget(totalStrokes);
	layout->addWidget(new QLabel(i18n("<strong>Total</strong> strokes:"), this));
	totalSpin = new QSpinBox(1, 30, 1, this);
	layout->addWidget(totalSpin);
	
	ok = new QPushButton(i18n("&Look Up"), this);
	ok->setEnabled(false);
	connect(ok, SIGNAL(clicked()), SLOT(apply()));
	layout->addWidget(ok);
	cancel = new QPushButton(i18n("&Cancel"), this);
	connect(cancel, SIGNAL(clicked()), SLOT(close()));
	layout->addWidget(cancel);

	List = new QListBox(this);
	connect(List, SIGNAL(highlighted(int)), SLOT(highlighted(int)));
	hlayout->addWidget(List);
	connect(strokesSpin, SIGNAL(valueChanged(int)), SLOT(updateList(int)));

	setCaption(kapp->makeStdCaption(i18n("Radical Selector")));

	strokesSpin->setValue(config->readNumEntry("Strokes", 1));
	totalSpin->setValue(config->readNumEntry("Total Strokes", 1));
	totalStrokes->setChecked(config->readBoolEntry("Search By Total", false));

	totalClicked();
}

RadWidget::~RadWidget()
{
}

void RadWidget::hotlistClicked(int num)
{
	// condense code :)
	strokesSpin->setValue(rad->strokesByRad(*hotlist.at(num)));
	List->setSelected(List->findItem(* hotlist.at(num)), true);
}

void RadWidget::highlighted(int)
{
	ok->setEnabled(true);
}

void RadWidget::updateList(int strokes)
{
	List->clear();

	List->insertStringList(rad->radByStrokes(static_cast<unsigned int>(strokes)));
}

void RadWidget::apply()
{
	QString text = List->currentText();
	if (text == QString::null)
		return;

	emit set(text, totalStrokes->isChecked() ? totalSpin->value() : 0);

	KConfig *config = kapp->config();
	config->setGroup("Radical Searching");
	config->writeEntry("Strokes", strokesSpin->value());
	config->writeEntry("Total Strokes", totalSpin->value());
	config->writeEntry("Search By Total", totalStrokes->isChecked());

	if (hotlist.find(text) == hotlist.end())
	{
		if (hotlist.size() >= hotlistNum)
		{
			hotlist.pop_front(); // stupid stl functions in Qt .. ;)
		}
		hotlist.append(text);

		config->writeEntry("Hotlist", hotlist);
	}
	config->sync();

	close();
}

void RadWidget::totalClicked(void)
{
	totalSpin->setEnabled(totalStrokes->isChecked());
}

//////////////////////////////////////////////

Radical::Radical(QString text, unsigned int strokes)
{
	_Radical = text;
	Strokes = strokes;
}

void Radical::addKanji(QString &text)
{
	Kanji.append(text);
}

#include "rad.moc"
