#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kapp.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qspinbox.h>
#include <qstring.h>

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
	QString s;
	
	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Kanji radical information could not be loaded, so radical searching cannot be used."));
	}

	QTextStream t(&f); // use a text stream
	while (!t.eof())
	{
		s = t.readLine();

		loadLine(s);
	}

	f.close();

	list.setAutoDelete(true); // I should do this, no?
}

void Rad::loadLine(QString &s)
{
	//kdDebug() << s << endl;

	QChar first = s.at(0);
	if (first == '#') // comment!
		return;
	if (first == '$') // header
	{
		//first entry is trim(last 4 chars).. <rad><space><strokes>

		unsigned int strokes = s.right(2).stripWhiteSpace().toUInt();
		QString radical = QString(s.at(2));
		curRadical= new Radical(radical, strokes);
		list.append(curRadical);
		return;
	}

	curRadical->addKanji(s);
}

QStringList Rad::radByStrokes(unsigned int strokes)
{
	QStringList ret;
	bool hadOne = false;
	QPtrListIterator<Radical> it(list);
	Radical *cur;

	while ((cur = it.current()) != 0)
	{
		++it;

		if (cur->strokes() == strokes)
		{
			ret.append(cur->radical());
			hadOne = true;
			continue;
		}

		// if we've hadOne, and now we don't, there won't be anymore
		if (hadOne)
			break;
	}

	return ret;
}

QStringList Rad::kanjiByRad(QString &text)
{
	QStringList ret;
	QPtrListIterator<Radical> it(list);
	Radical *cur;

	while ((cur = it.current()) != 0)
	{
		++it;
		if (cur->radical() == text)
			break;
	}

	QString kanji = cur->kanji();

	unsigned int i;
	for (i = 0; i < kanji.length(); ++i)
	{
		ret.append(QString(kanji.at(i)));
	}

	return ret;
}

Rad::~Rad()
{
}

///////////////////////////////////////////////

RadWidget::RadWidget(Rad *_rad, QWidget *parent, const char *name) : QWidget(parent, name)
{
	rad = _rad;
	QHBoxLayout *hlayout = new QHBoxLayout(this, 6);
	QVBoxLayout *layout = new QVBoxLayout(hlayout, 6);
	strokesSpin = new QSpinBox(1, 17, 1, this);
	layout->addWidget(strokesSpin);
	ok = new QPushButton(i18n("&OK"), this);
	connect(ok, SIGNAL(clicked()), this, SLOT(apply()));
	layout->addWidget(ok);
	cancel = new QPushButton(i18n("&Cancel"), this);
	connect(cancel, SIGNAL(clicked()), this, SLOT(docancel()));
	layout->addWidget(cancel);

	List = new QListBox(this);
	hlayout->addWidget(List);
	connect(strokesSpin, SIGNAL(valueChanged(int)), SLOT(updateList(int)));
	updateList(1);

	setCaption(kapp->makeStdCaption(i18n("Radical Selector")));
}

RadWidget::~RadWidget()
{
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

	emit set(text);

	close();
}

void RadWidget::docancel()
{
	close();
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
