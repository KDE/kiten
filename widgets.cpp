#include <qwidget.h>
#include <qsplitter.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>
#include <kdialog.h>
#include <klistview.h>
#include <klocale.h>
#include <ktoolbar.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kapp.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <kstatusbar.h>
#include <kstringhandler.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qtextedit.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qregexp.h>
#include <qtabwidget.h>
//#include <qtimer.h>
#include <qlayout.h>

#include <stdlib.h> // RAND_MAX
#include <cassert>

#include "widgets.h"
#include "dict.h"


ResultView::ResultView(QWidget *parent, const char *name)
	: QTextEdit(parent, name)
{
	setReadOnly(true);
}

void ResultView::addResult(Dict::Entry result, bool com)
{
	if (result.dictName() != "__NOTSET")
	{
		insertParagraph(i18n("<h3>Results from %1</h3>").arg(result.dictName()), paragraphs() + 1);
		return;
	}

	QString html;
	if (result.kanaOnly())
		html = QString("<p><font size=\"+2\">%1</font>  ").arg(result.reading());
	else
		html = QString("<p><font size=\"+2\">%1</font>: %2  ").arg(result.kanji()).arg(result.reading());

	QStringList::Iterator it;
	QStringList Meanings = result.meanings();
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

void ResultView::addKanjiResult(Dict::Kanji result)
{
	if (result.dictName() != "__NOTSET")
	{
		insertParagraph(i18n("<h3>Results from %1</h3>").arg(result.dictName()), paragraphs() + 1);
		return;
	}

	QString html;
	html = QString("<p><font size=\"+3\">%1</font>: %2  ").arg(result.kanji());

	unsigned int freq = result.freq();
	if (freq == 0) // does it have a frequency?
		html = html.arg(i18n("Rare"));
	else
		html = html.arg(i18n("#%1").arg(freq));

	html += "<br />";

	QStringList::Iterator it;
	QStringList Readings = result.readings();
	for (it = Readings.begin(); it != Readings.end(); ++it)
	{
		if ((*it) == "T1")
		{
			html.truncate(html.length() - 2); // get rid of last ,
			html += i18n("<br />In names: ");
		}
		else
		{
			if ((*it) == "T2")
			{
				html.truncate(html.length() - 2); // get rid of last ,
				html += i18n("<br />As radical: ");
			}
			else
			{
				html += (*it);
				html += ", ";
			}
		}
	}
	html.truncate(html.length() - 2); // get rid of last ,

	html += "<br />";

	QStringList Meanings = result.meanings();
	for (it = Meanings.begin(); it != Meanings.end(); ++it)
	{
		html += (*it);
		html += "; ";
	}
	html += "<br />";
	html += i18n("Grade Level: %1. Strokes: %2.");

	switch (result.grade())
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
		html = html.arg(result.grade());
	}

	html = html.arg(result.strokes());

	if (result.miscount() != 0)
		html.append(i18n(" Common Miscount: %1.").arg(result.miscount()));

	html += "</p>";

	insertParagraph(html, paragraphs() + 1);
}

void ResultView::addHeader(const QString &header, unsigned int degree)
{
	insertParagraph(QString("<h%1>%2</h%3>").arg(degree).arg(header).arg(degree), paragraphs() + 1);
}

////////////////////////////////////////////////

const int Learn::numberOfAnswers = 5;

Learn::Learn(Dict::Index *parentDict, QWidget *parent, const char *name)
	: KMainWindow(parent, name)
	, curItem(0)
{
	index = parentDict;

	QWidget *dummy = new QWidget(this);
	setCentralWidget(dummy);

	QVBoxLayout *veryTop = new QVBoxLayout(dummy, 0);
	Tabs = new QTabWidget(dummy);
	connect(Tabs, SIGNAL(currentChanged(QWidget *)), SLOT(tabChanged(QWidget *)));
	veryTop->addWidget(Tabs);

	listTop = new QSplitter(Tabs);
	listTop->setOrientation(Qt::Vertical);
	quizTop = new QWidget(Tabs);
	Tabs->addTab(listTop, i18n("&List"));
	Tabs->addTab(quizTop, i18n("&Quiz"));

	List = new KListView(listTop);

	List->addColumn(i18n("Kanji"));
	List->addColumn(i18n("Grade"));
	List->addColumn(i18n("Readings"));
	List->addColumn(i18n("Meanings"));

	List->setAllColumnsShowFocus(true);
	List->setColumnWidthMode(0, QListView::Maximum);
	List->setColumnWidthMode(1, QListView::Maximum);
	List->setColumnWidthMode(2, QListView::Maximum);
	List->setColumnWidthMode(3, QListView::Maximum);
	List->setMultiSelection(true);
	List->setDragEnabled(true);
	List->setSorting(-1);
	List->setSelectionModeExt(KListView::Extended);
	connect(List, SIGNAL(executed(QListViewItem *)), SLOT(showKanji(QListViewItem *)));
	connect(List, SIGNAL(selectionChanged()), this, SLOT(itemSelectionChanged()));

	View = new ResultView(listTop, "View");

	QStringList grades(i18n("Grade 1"));
	grades.append(i18n("Grade 2"));
	grades.append(i18n("Grade 3"));
	grades.append(i18n("Grade 4"));
	grades.append(i18n("Grade 5"));
	grades.append(i18n("Grade 6"));
	grades.append(i18n("Others in Jouyou"));
	grades.append(i18n("Jinmeiyou"));

	KAction *closeAction = KStdAction::close(this, SLOT(close()), actionCollection());
	forwardAct = KStdAction::forward(this, SLOT(next()), actionCollection());
	forwardAct->plug(toolBar());
	backAct = KStdAction::back(this, SLOT(prev()), actionCollection());
	backAct->plug(toolBar());
	cheatAct = new KAction(i18n("&Cheat"), CTRL + Key_C, this, SLOT(cheat()), actionCollection(), "cheat");
	randomAct = new KAction(i18n("&Random"), "goto", CTRL + Key_R, this, SLOT(random()), actionCollection(), "random");
	gradeAct = new KListAction(i18n("Grade"), 0, this, SLOT(updateGrade()), actionCollection(), "grade");
	gradeAct->setItems(grades);
	connect(gradeAct, SIGNAL(activated(const QString&)), SLOT(updateGrade()));
	removeAct = new KAction(i18n("&Delete"), "edit_remove", CTRL + Key_X, this, SLOT(del()), actionCollection(), "del");
	addAct = new KAction(i18n("&Add"), "edit_add", CTRL + Key_A, this, SLOT(add()), actionCollection(), "add");
	addAllAct = new KAction(i18n("Add A&ll"), 0, 0, this, SLOT(addAll()), actionCollection(), "addall");
	saveAct = KStdAction::save(this, SLOT(writeConfiguration()), actionCollection());

	removeAct->setEnabled(false);

	QVBoxLayout *quizLayout = new QVBoxLayout(quizTop, 0, KDialog::spacingHint());

	qKanji = new QLabel(quizTop);
	quizLayout->addWidget(qKanji);

	answers = new QButtonGroup(1, Horizontal, quizTop);
	for(int i = 0; i < numberOfAnswers; ++i)
		answers->insert(new KPushButton(answers), i);
	quizLayout->addWidget(answers);
	connect(answers, SIGNAL(clicked(int)), this, SLOT(answerClicked(int)));

	createGUI("learnui.rc");
	StatusBar = statusBar();
	closeAction->plug(toolBar());

	KConfig *config = kapp->config(); // do this here so writeConfig only does the learnlist stuff
	config->setGroup("Learn");
	setCurrentGrade(config->readNumEntry("grade", 1));

	isMod = false;

	updateGrade();
	readConfiguration();

	connect(this, SIGNAL(listDirty()), this, SLOT(updateQuiz()));
	emit listDirty();

	if (List->childCount() >= 1)
	{
		curItem = List->firstChild();
		prevItem = curItem;
		qnew(); // init first quiz
	}

	resize(600, 400);
	applyMainWindowSettings(KGlobal::config(), "LearnWindow");
}

Learn::~Learn()
{
}

void Learn::closeEvent(QCloseEvent *e)
{
	if (isMod)
	{
		int result = KMessageBox::warningYesNoCancel(this, i18n("There are unsaved changes to learning list. Save them?"), i18n("Unsaved changes"), i18n("Save"), i18n("Discard"), "DiscardAsk", true);
		switch(result)
		{
		case KMessageBox::Yes:
			saveAct->activate();
			// fallthrough
		case KMessageBox::No:
			emit listChanged(); // make it seem clean, changes aren't desired
			break;
		case KMessageBox::Cancel:
			return;
		}
	}

	saveMainWindowSettings(KGlobal::config(), "LearnWindow");
	KMainWindow::closeEvent(e);
}

void Learn::random()
{
	float rand = kapp->random();
	float rand2 = RAND_MAX / rand;
	rand = list.count() / rand2;

	current = list.at(rand);
	update();
}

void Learn::next()
{
	++current;
	if(current == list.end())
		current = list.begin();
	update();
}

void Learn::prev()
{
	if (Tabs->currentPageIndex() == 1)
	{
		if (!prevItem)
			return;
		curItem = prevItem;
	
		qupdate();
		return;
	}

	if(current == list.begin())
		current = list.end();
	--current;
	update();
}

void Learn::update()
{
	View->clear();
	
	Dict::Kanji curKanji = *current;

	if (!curKanji.kanji())
	{
		StatusBar->message(i18n("Grade not loaded")); // oops
		return;
	}
	View->addKanjiResult(curKanji);

	// now show some compounds in which this kanji appears
	
	QString kanji = curKanji.kanji();

	unsigned int num, fullNum;
	Dict::SearchResult compounds = index->search(QRegExp(kanji), kanji, num, fullNum, true);
	View->addHeader(i18n("%1 in common compunds").arg(kanji));
	
	for(QValueListIterator<Dict::Entry> it = compounds.list.begin(); it != compounds.list.end(); ++it)
		View->addResult(*it, true);
}

void Learn::updateGrade()
{
	int grade = getCurrentGrade();

	QString regexp("G%1 ");
	regexp = regexp.arg(grade);

	unsigned int num, fullNum;
	Dict::KanjiSearchResult result = index->searchKanji(QRegExp(regexp), regexp, num, fullNum, false);
	list = result.list;

	StatusBar->message(i18n("%1 entries in grade %2").arg(list.count()).arg(grade));
	setCaption(i18n("Grade %1 - Learn").arg(grade));

	list.remove(list.begin());
	current = list.begin();
	update();

	KConfig *config = kapp->config();
	
	config->setGroup("Learn");
	config->writeEntry("grade", grade);
}

void Learn::readConfiguration()
{
	List->clear();
	KConfig *config = kapp->config();

	config->setGroup("Learn");
	setCurrentGrade(config->readNumEntry("grade", 1));

	QStringList kanji = config->readListEntry("__KANJI");
	QStringList::Iterator it;

	// make new list view item(parent, kanji, grade, readings)
	for (it = kanji.begin(); it != kanji.end(); ++it)
		addItem(new QListViewItem(List, *it, config->readEntry(*it), config->readEntry(QString(*it).append("_readings")), config->readEntry(QString(*it).append("_meanings"))), true);
}

void Learn::writeConfiguration()
{
	if (!isMod) // nothing modified, why save
		return;

	KConfig *config = kapp->config();
	config->setGroup("Learn");

	QStringList kanji;
	QListViewItemIterator it(List);
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

	isMod = false;

	emit listChanged();
}

void Learn::externAdd(Dict::Kanji toAdd)
{
	// Remove peripheral readings: This is a study mode, not a reference mode
	QRegExp inNames = QString::fromLatin1(",\\s*[A-Za-z ]+:.*");
	QString readings = Dict::prettyKanjiReading(toAdd.readings()).replace(inNames, "");
	QString meanings = Dict::prettyMeaning(toAdd.meanings()).replace(inNames, "");

	addItem(new QListViewItem(List, toAdd.kanji(), QString::number(toAdd.grade()), readings, meanings));
}

void Learn::add()
{
	externAdd(*current);
}

void Learn::addAll()
{
	int grade = getCurrentGrade();

	QString regexp("G%1 ");
	regexp = regexp.arg(grade);

	unsigned int num, fullNum;
	Dict::KanjiSearchResult result = index->searchKanji(QRegExp(regexp), regexp, num, fullNum, false);
	for(QValueListIterator<Dict::Kanji> i = result.list.begin(); i != result.list.end(); ++i)
		externAdd(*i);
}

void Learn::addItem(QListViewItem *item, bool init)
{
	List->moveItem(item, 0, List->lastItem());

	if (!curItem)
	{
		curItem = item;
		prevItem = curItem;
		qnew(); // init first quiz
	}

	if(!init)
	{
		List->ensureItemVisible(item);

		isMod = true;
		emit listDirty();
	}
}

void Learn::showKanji(QListViewItem *item)
{
	assert(item);

	QString kanji(item->text(0));

	int grade = item->text(1).toUInt();
	if (getCurrentGrade() != grade)
	{
		setCurrentGrade(grade);
		updateGrade();
	}

	// Why does this fail to find the kanji sometimes?
	for(current = list.begin(); current != list.end() && (*current).kanji() != kanji; ++current);

	update();
	setCaption(i18n("%1 - Learn").arg(item->text(0)));
}

void Learn::del()
{
	// quiz page
	if (Tabs->currentPageIndex() == 1)
	{
		delete curItem;
		curItem = prevItem; // needs to be something
		qnew();
	}
	else // setup page
	{
		QPtrList<QListViewItem> selected = List->selectedItems();
		assert(selected.count());

		for(QPtrListIterator<QListViewItem> i(selected); *i; ++i)
			delete *i;

		isMod = true;
		emit listDirty();
	}
}

void Learn::answerClicked(int i)
{
	if(seikai == i)
	{
		StatusBar->message(i18n("Good!"));
		qnew();
	}
	else
	{
		StatusBar->message(i18n("Wrong"));
	}
}

QString Learn::randomMeaning(QStringList &oldMeanings)
{
	QString meaning;

	do
	{
		float rand = kapp->random();
		if ((rand > (RAND_MAX / 2)) || (List->childCount() < numberOfAnswers))
		{
			rand = kapp->random();
			float rand2 = RAND_MAX / rand;
			rand = list.count() / rand2;

			meaning = Dict::prettyMeaning((*list.at(rand)).meanings());
		}
		else
		{
			rand = kapp->random();
			float rand2 = RAND_MAX / rand;
			rand = List->childCount() / rand2;

			int max = (int) rand;
		
			QListViewItemIterator it(List);
			it += max;

			meaning = it.current()->text(3);
		}
	}
	while(oldMeanings.contains(meaning) || meaning == curItem->text(3));

	oldMeanings.append(meaning);
	meaning = shortenString(meaning);

	return meaning;
}

void Learn::qupdate()
{
	if (!curItem)
		return;
	
	qKanji->setText(QString("<font size=\"+3\">%1</font> %2").arg(curItem->text(0)).arg(curItem->text(2)));

	float rand = kapp->random();
	float rand2 = RAND_MAX / rand;
	seikai = static_cast<int>(numberOfAnswers / rand2);

	QStringList oldMeanings;
	for(int i = 0; i < numberOfAnswers; ++i)
		answers->find(i)->setText(randomMeaning(oldMeanings));

	answers->find(seikai)->setText(curItem->text(3));
}

void Learn::qnew() // new quiz kanji
{
	StatusBar->clear();

	float rand = kapp->random();
	float rand2 = RAND_MAX / rand;
	rand = List->childCount() / rand2;
	int max = (int) rand;

	QListViewItemIterator it(List);
	QListViewItemIterator tmp(List);
	int i;
	for (i = 0; i < max; ++it)
		{i++; ++tmp;}

	if (curItem->text(0) == it.current()->text(0)) // same, don't use
	{
		++it;
		if (!it.current())
		{
			tmp--;
			it = tmp;
		}
	}

	if (!it.current())
	{
		return;
	}

	prevItem = curItem;
	curItem = it.current();

	qKanji->setFocus();
	qupdate();
}

void Learn::cheat()
{
	answers->find(seikai)->setFocus();
	StatusBar->message(i18n("Better luck next time"));
}

QString Learn::shortenString(QString thestring)
{
	return KStringHandler::rsqueeze(thestring, 60);
}

void Learn::tabChanged(QWidget *widget)
{
	bool isQuiz = widget == quizTop;

	forwardAct->setEnabled(!isQuiz);
	gradeAct->setEnabled(!isQuiz);
	saveAct->setEnabled(!isQuiz);
	addAct->setEnabled(!isQuiz);
	addAllAct->setEnabled(!isQuiz);
	randomAct->setEnabled(!isQuiz);

	cheatAct->setEnabled(isQuiz);

	// also handled below for !isQuiz case
	removeAct->setEnabled(isQuiz);

	if (isQuiz)
	{
		setCaption(i18n("%1 - Quiz").arg(curItem->text(0)));
		qKanji->setFocus();
	}
	else
	{
		// handle removeAct;
		setCaption(i18n("%1 - Learn").arg((*current).kanji()));
		itemSelectionChanged();
	}
}

void Learn::updateQuiz()
{
	if (List->childCount() < 3)
		Tabs->setTabEnabled(quizTop, false);
	else
		Tabs->setTabEnabled(quizTop, true);
}

void Learn::itemSelectionChanged()
{
	QPtrList<QListViewItem> selected = List->selectedItems();
	removeAct->setEnabled( selected.count() > 0 );
}

int Learn::getCurrentGrade(void)
{
	int grade = gradeAct->currentItem() + 1;
	if(grade > 6) ++grade;
	return grade;
}

void Learn::setCurrentGrade(int grade)
{
	if(grade > 6) --grade;
	gradeAct->setCurrentItem(grade - 1);
}


/////////////////////////////////////////////////////
// sorta taken from konqy

EditAction::EditAction(const QString& text, int accel, const QObject *receiver, const char *member, QObject* parent, const char* name)
    : KAction(text, accel, parent, name)
{
	m_receiver = receiver;
	m_member = member;
}

EditAction::~EditAction()
{
}

int EditAction::plug( QWidget *w, int index )
{
	//  if ( !w->inherits( "KToolBar" ) );
	//    return -1;

	KToolBar *toolBar = (KToolBar *)w;

	int id = KAction::getToolButtonID();

	KLineEdit *comboBox = new KLineEdit(toolBar, "search edit");
	toolBar->insertWidget( id, 70, comboBox, index );
	connect( comboBox, SIGNAL( returnPressed()), m_receiver, m_member );

	addContainer(toolBar, id);

	connect( toolBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

	toolBar->setItemAutoSized( id, true );

	m_combo = comboBox;

	emit plugged();

	//QWhatsThis::add( comboBox, whatsThis() );

	return containerCount() - 1;
}

void EditAction::unplug( QWidget *w )
{
//  if ( !w->inherits( "KToolBar" ) )
//    return;

	KToolBar *toolBar = (KToolBar *)w;

	int idx = findContainer( w );

	toolBar->removeItem( menuId( idx ) );

	removeContainer( idx );
	m_combo = 0L;
}

void EditAction::clear()
{
	m_combo->clear();
}

void EditAction::insert(QString &text)
{
	m_combo->insert(text);
}

#include "widgets.moc"
