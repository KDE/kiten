#include <qwidget.h>
#include <qptrlist.h>
#include <kcompletion.h>
#include <kdebug.h>
#include <klistview.h>
#include <klocale.h>
#include <kconfig.h>
#include <kapp.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kstatusbar.h>
#include <qtoolbutton.h>
#include <qtextedit.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qregexp.h>
#include <qtabwidget.h>
#include <qlayout.h>

#include <stdlib.h> // RAND_MAX

#include "widgets.h"
#include "dict.h"

// Noatun wuz heer
namespace
{
QToolButton *newButton(const QIconSet &iconSet, const QString &textLabel, QObject *receiver, const char * slot, QWidget *parent, const char *name = 0)
{
	QToolButton *button = new QToolButton(parent, name);
	button->setIconSet(iconSet);
	button->setTextLabel(textLabel, true);
	QObject::connect(button, SIGNAL(clicked()), receiver, slot);
	button->setFixedSize(QSize(22, 22));
	return button;
}
}

SearchForm::SearchForm(QWidget *parent, const char *name)
	: QWidget(parent, name)
{
	QHBoxLayout *layout = new QHBoxLayout(this, 6);

	LineEdit = new KLineEdit(this);
	CompletionObj = LineEdit->completionObject();
	ClearButton = newButton(BarIconSet("editclear", KIcon::SizeSmall), "Clear", LineEdit, SLOT(clear()), this);
	layout->addWidget(ClearButton);
	layout->addWidget(LineEdit);


	SearchDictButton = new KPushButton(i18n("&Anywhere"), this);
	layout->addWidget(SearchDictButton);
	SearchReadingButton = new KPushButton(i18n("&Reading"), this);
	layout->addWidget(SearchReadingButton);
	SearchKanjiButton = new KPushButton(i18n("Kan&ji"), this);
	layout->addWidget(SearchKanjiButton);

	// both do same thing
	connect(LineEdit, SIGNAL(returnPressed()), SLOT(returnPressed()));
	connect(SearchDictButton, SIGNAL(pressed()), SLOT(doSearch()));
	connect(SearchReadingButton, SIGNAL(pressed()), SLOT(doReadingSearch()));
	connect(SearchKanjiButton, SIGNAL(pressed()), SLOT(doKanjiSearch()));

	slotUpdateConfiguration();
}

void SearchForm::returnPressed()
{
	doSearch();
}

void SearchForm::slotUpdateConfiguration()
{
	KConfig *config = kapp->config();
	config->setGroup("Searching Options");
	wholeWord = config->readBoolEntry("wholeWord", true);
	caseSensitive = config->readBoolEntry("caseSensitive", false);
}

SearchForm::~SearchForm()
{
}

QRegExp SearchForm::readingSearchItems(bool kanji)
{
	QString text = LineEdit->text();
	if (text.isEmpty()) // abandon search
	{
		return QRegExp(); //empty
	}

	CompletionObj->addItem(text);
	QString regexp;
	if (kanji)
		regexp = " %1 ";
	else
		regexp = "\\[%1\\]";

	regexp = regexp.arg(text);
	
	return QRegExp(regexp, caseSensitive);
}

QRegExp SearchForm::kanjiSearchItems()
{
	QString text = LineEdit->text();
	if (text.isEmpty())
	{
		return QRegExp(); //empty
	}

	CompletionObj->addItem(text);

	QString regexp = "^%1\\W";
	regexp = regexp.arg(text);
	
	return QRegExp(regexp, caseSensitive);
}

QRegExp SearchForm::searchItems()
{
	QString regexp;
	QString text = LineEdit->text();
	if (text.isEmpty())
	{
		return QRegExp(); //empty
	}

	CompletionObj->addItem(text);

	unsigned int contains = text.contains(QRegExp("[A-Za-z0-9_:]"));
	if (contains == text.length())
		regexp = "\\W%1\\W";
	else
		regexp = "%1";

	regexp = regexp.arg(text);
	
	//kdDebug() << "SearchForm::searchItems returning " << regexp << endl;
	return QRegExp(regexp, caseSensitive);
}

void SearchForm::doSearch()
{
	emit search();
}

void SearchForm::doReadingSearch()
{
	emit readingSearch();
}

void SearchForm::doKanjiSearch()
{
	emit kanjiSearch();
}

QString SearchForm::lineText()
{
	return LineEdit->text();
}

void SearchForm::setLineText(const QString& text)
{
	LineEdit->setText(text);
}

void SearchForm::setFocusNow()
{
	LineEdit->setFocus();
}

////////////////////////////////////////////////////////

ResultView::ResultView(QWidget *parent, const char *name)
	: QTextEdit(parent, name)
{
	setReadOnly(true);
}

void ResultView::addResult(Entry *result, bool com)
{
	if (result->dictName() != "__NOTSET")
	{
		insertParagraph(i18n("<h3>Results from %1</h3>").arg(result->dictName()), paragraphs() + 1);
		return;
	}

	QString html;
	if (result->kanaOnly())
		html = QString("<p><font size=\"+2\">%1</font>  ").arg(result->reading());
	else
		html = QString("<p><font size=\"+2\">%1</font>: %2  ").arg(result->kanji()).arg(result->reading());

	QStringList::Iterator it;
	QStringList Meanings = result->meanings();
	for (it = Meanings.begin(); it != Meanings.end(); ++it)
	{
		if ((*it).find("(P)") >= 0)
		{
			if (com)
				continue;
			else
			{
				html += "<strong>Common</strong>  ";
			}
		}
		else
		{
			html += (*it);
			html += "; ";
		}
	}

	html += "</p>";

	insertParagraph(html, paragraphs() + 1);
}

void ResultView::addKanjiResult(Kanji *result)
{
	if (result->dictName() != "__NOTSET")
	{
		insertParagraph(i18n("<h3>Results from %1</h3>").arg(result->dictName()), paragraphs() + 1);
		return;
	}

	QString html;
	html = QString("<p><font size=\"+3\">%1</font>: %2  ").arg(result->kanji());

	unsigned int freq = result->freq();
	if (freq == 0) // does it have a frequency?
		html = html.arg(i18n("Rare"));
	else
		html = html.arg(i18n("#%1").arg(freq));

	html += "<br />";

	QStringList::Iterator it;
	QStringList Readings = result->readings();
	for (it = Readings.begin(); it != Readings.end(); ++it)
	{
		if ((*it) == "T1")
			html += i18n("<br />In names: ");
		else
		{
			if ((*it) == "T2")
				html += i18n("<br />As radical: ");
			else
			{
				html += (*it);
				html += ", ";
			}
		}
	}
	html.truncate(html.length() - 2); // get rid of last ,

	html += "<br />";

	QStringList Meanings = result->meanings();
	for (it = Meanings.begin(); it != Meanings.end(); ++it)
	{
		html += (*it);
		html += "; ";
	}
	html += "<br />";
	html += i18n("Grade Level: %1. Strokes: %2.");

	switch (result->grade())
	{
		case 0:
		html = html.arg(i18n("None"));
		break;
		case 8:
		html = html.arg(i18n("In Jouyou"));
		break;
		case 9:
		html = html.arg(i18n("In Jinmeiyou"));
		break;
		default:
		html = html.arg(result->grade());
	}

	html = html.arg(result->strokes());

	if (result->miscount() != 0)
		html.append(i18n(" Common Miscount: %1.").arg(result->miscount()));

	html += "</p>";

	insertParagraph(html, paragraphs() + 1);
}

void ResultView::addHeader(const QString &header)
{
	insertParagraph(QString("<h3>%1</h3>").arg(header), paragraphs() + 1);
}

////////////////////////////////////////////////

Learn::Learn(Dict *parentDict, QWidget *parent, const char *name) : QWidget(parent, name)
{
	dict = parentDict;

	QVBoxLayout *layout = new QVBoxLayout(this, 6);

	QHBoxLayout *topLayout = new QHBoxLayout(layout, 6);
	UpdateGrade = new KPushButton(i18n("&Update Grade"), this);
	topLayout->addWidget(UpdateGrade);
	connect(UpdateGrade, SIGNAL(pressed()), SLOT(updateGrade()));
	GradeSpin = new QSpinBox(1, 9, 1, this);
	topLayout->addWidget(GradeSpin);
	connect(GradeSpin, SIGNAL(valueChanged(int)), SLOT(gradeChange(int)));

	QHBoxLayout *botLayout = new QHBoxLayout(layout, 6);
	Rand = new KPushButton(i18n("&Random"), this);
	connect(Rand, SIGNAL(pressed()), SLOT(random()));
	botLayout->addWidget(Rand);
	Prev = new KPushButton(i18n("&Previous"), this);
	connect(Prev, SIGNAL(pressed()), SLOT(prev()));
	botLayout->addWidget(Prev);
	Next = new KPushButton(i18n("&Next"), this);
	connect(Next, SIGNAL(pressed()), SLOT(next()));
	botLayout->addWidget(Next);

	View = new ResultView(this, "View");
	layout->addWidget(View);

	QHBoxLayout *deepLayout = new QHBoxLayout(layout, 6);
	Add = new KPushButton(i18n("&Add to list"), this);
	deepLayout->addWidget(Add);
	connect(Add, SIGNAL(pressed()), SLOT(slotAdd()));
	Show = new KPushButton(i18n("&Show list"), this);
	deepLayout->addWidget(Show);
	connect(Show, SIGNAL(pressed()), SLOT(showList()));

	StatusBar = new KStatusBar(this);
	layout->addWidget(StatusBar);

	KConfig *config = kapp->config(); // do this here so writeConfig only does the learnlist stuff
	config->setGroup("Learn");
	GradeSpin->setValue(config->readNumEntry("grade", 1));

	updateGrade();
}

Learn::~Learn()
{
}

void Learn::random()
{
	float rand = kapp->random();
	float rand2 = RAND_MAX / rand;
	rand = list.count() / rand2;

	list.at(rand);
	update();
}

void Learn::next()
{
	if (!list.next())
		list.first();
	update();
}

void Learn::prev()
{
	if (!list.prev())
	{
		kdDebug() << "first, going to last\n";
		list.last();
	}
	update();
}

void Learn::update(Kanji *curKanji)
{
	View->clear();
	
	if (!curKanji) // if curKanji isn't set
		curKanji = list.current();

	if (!curKanji)
	{
		StatusBar->message(i18n("Grade not loaded")); // oops
		return;
	}
	View->addKanjiResult(curKanji);

	// now show some compounds in which this kanji appears
	
	QString kanji = curKanji->kanji();

	bool oldir = dict->isir();
	bool oldcom = dict->iscom();
	dict->toggleCom(true);
	dict->toggleIR(false);
	
	unsigned int num, fullNum;
	QPtrList<Entry> compounds = dict->search(QRegExp(kanji), kanji, num, fullNum);

	View->addHeader(i18n("%1 in compunds").arg(kanji));
		
	QPtrListIterator<Entry> it(compounds);
	Entry *curEntry;
	while ((curEntry = it.current()) != 0)
	{
		++it;
		View->addResult(curEntry, true);
	}

	dict->toggleCom(oldcom);
	dict->toggleIR(oldir);
}

void Learn::updateGrade()
{
	int grade = GradeSpin->value();
	
	if (grade == 7)
	{
		StatusBar->message(i18n("Grade 7 is nothing, aborting!"));
		return;
	}

	QString regexp("G%1 ");
	regexp = regexp.arg(grade);

	bool oldir = dict->isir();
	bool oldcom = dict->iscom();
	dict->toggleCom(false);
	dict->toggleIR(false);

	unsigned int num, fullNum;
	list = dict->kanjiSearch(QRegExp(regexp), regexp, num, fullNum);

	dict->toggleCom(oldcom);
	dict->toggleIR(oldir);

	StatusBar->message(i18n("%1 entries in grade %2").arg(list.count()).arg(grade));
	setCaption(i18n("Grade %1 - Kiten Learn").arg(grade));

	list.removeFirst();
	list.first();
	update();

	KConfig *config = kapp->config();
	
	config->setGroup("Learn");
	config->writeEntry("grade", GradeSpin->value());
}

void Learn::readConfiguration()
{
	if (!_LearnList)
		return;
	KConfig *config = kapp->config();

	config->setGroup("Learn");
	GradeSpin->setValue(config->readNumEntry("grade", 1));

	if (!_LearnList)
	{
		showList();
	}

	QStringList kanji = config->readListEntry("__KANJI");
	QStringList::Iterator it;

	for (it = kanji.begin(); it != kanji.end(); ++it)
	{
		(void) new QListViewItem(_LearnList->list(), *it, config->readEntry(*it), config->readEntry(QString(*it).append("_readings")), config->readEntry(QString(*it).append("_meanings"))); // make new list view item(parent, kanji, grade, readings)
	}
}

void Learn::writeConfiguration()
{
	if (!_LearnList)
		return;

	KConfig *config = kapp->config();
	config->setGroup("Learn");

	QStringList kanji;
	QListViewItemIterator it(_LearnList->list());
	QString curKanji;

	for (; it.current(); ++it)
	{
		curKanji = it.current()->text(0);
		kanji.append(curKanji);
		config->writeEntry(curKanji, it.current()->text(1).toUInt());
		config->writeEntry(QString(curKanji).append("_readings"), it.current()->text(2));
		config->writeEntry(QString(curKanji).append("_meanings"), it.current()->text(3));
	}

	config->writeEntry("__KANJI", kanji);
}

void Learn::slotAdd()
{
	addNew = true;

	if (!_LearnList) // popup the learn list if it isn't already
		showList();
}

void Learn::listLoaded()
{
	if (addNew)
		add();
}

void Learn::add()
{
	if (!_LearnList) // popup the learn list if it isn't already
		showList();

	QStringList::Iterator it;

	QStringList readingslist = list.current()->readings();
	QString readings;
	for (it = readingslist.begin(); it != readingslist.end(); ++it)
		readings.append(*it).append(", ");
	readings.truncate(readings.length() - 2);

	QStringList meaningslist = list.current()->readings();
	QString meanings;
	for (it = meaningslist.begin(); it != meaningslist.end(); ++it)
		meanings.append(*it).append(", ");
	readings.truncate(readings.length() - 2);

	(void) new QListViewItem(_LearnList->list(), list.current()->kanji(), QString::number(list.current()->grade()), readings, meanings);
}

void Learn::gradeChange(int grade)
{
	if (grade == 7)
		StatusBar->message(i18n("Grade 7 is nothing"));
	else if (grade == 8)
	{
		GradeSpin->setValue(8);
		StatusBar->message(i18n("Grade 8 is Jouyou Kanji not in a grade"));
	}
	else if (grade == 9)
		StatusBar->message(i18n("Grade 9 is Jinmeyou"));
	else
		StatusBar->clear();
}

void Learn::showKanji(QListViewItem *item)
{
	if (!item)
		return;

	QString kanji(item->text(0));
	
	QPtrListIterator<Kanji> it(list);
	Kanji *curKanji;

	while ((curKanji = it.current()) != 0)
	{
		++it;
		if (curKanji->kanji() == kanji) // match
			break;
	}

	update(curKanji);
}

void Learn::showList()
{
	_LearnList = new LearnList(0, "_LearnList");
	_LearnList->show();

	connect(_LearnList->list(), SIGNAL(executed(QListViewItem *)), SLOT(showKanji(QListViewItem *)));
	connect(_LearnList->save(), SIGNAL(pressed()), SLOT(writeConfiguration()));
	connect(_LearnList, SIGNAL(initDone()), SLOT(listLoaded()));

	readConfiguration();
}

//////////////////////////////////////////////////////////////////

LearnList::LearnList(QWidget *parent, const char *name) : QWidget(parent, name)
{
	Tabs = new QTabWidget(this);

	QWidget *listTop = new QWidget(Tabs);
	QWidget *quizTop = new QWidget(Tabs);

	QVBoxLayout *layout = new QVBoxLayout(listTop, 6);

	QHBoxLayout *topLayout = new QHBoxLayout(layout, 6);
	Del = new KPushButton(i18n("&Delete"), listTop);
	topLayout->addWidget(Del);
	connect(Del, SIGNAL(pressed()), SLOT(del()));
	Save = new KPushButton(i18n("&Save"), listTop);
	topLayout->addWidget(Save);

	List = new KListView(listTop);
	layout->addWidget(List);

	List->addColumn(i18n("Kanji"));
	List->addColumn(i18n("Grade"));
	List->addColumn(i18n("Meanings"));
	connect(List, SIGNAL(executed(QListViewItem *)), SLOT(updateCaption(QListViewItem *)));

	QVBoxLayout *quizLayout = new QVBoxLayout(quizTop, 6);

	Tabs->addTab(listTop, i18n("&List"));
	Tabs->addTab(quizTop, i18n("&Quiz"));

	setCaption("Your List - Kiten Learn");
	emit initDone();
}

void LearnList::updateCaption(QListViewItem *item)
{
	setCaption(kapp->makeStdCaption(item->text(0)));
}

LearnList::~LearnList()
{
}

void LearnList::del()
{
	QListViewItem *kanji = List->selectedItem();
	if (!kanji)
		return;

	delete kanji;
}

KListView *LearnList::list()
{
	return List;
}

KPushButton *LearnList::save()
{
	return Save;
}
