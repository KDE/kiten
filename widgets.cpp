#include <qwidget.h>
#include <qptrlist.h>
#include <qsplitter.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>
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
#include <qlabel.h>
#include <qtextedit.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qregexp.h>
#include <qtabwidget.h>
//#include <qtimer.h>
#include <qlayout.h>

#include <stdlib.h> // RAND_MAX

#include "widgets.h"
#include "dict.h"

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

Learn::Learn(Dict *parentDict, QWidget *parent, const char *name) : KMainWindow(parent, name)
{
	dict = parentDict;

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
	connect(List, SIGNAL(executed(QListViewItem *)), SLOT(updateCaption(QListViewItem *)));
	connect(List, SIGNAL(executed(QListViewItem *)), SLOT(showKanji(QListViewItem *)));

	View = new ResultView(listTop, "View");

	QStringList grades(i18n("Grade 1"));
	grades.append(i18n("Grade 2"));
	grades.append(i18n("Grade 3"));
	grades.append(i18n("Grade 4"));
	grades.append(i18n("Grade 5"));
	grades.append(i18n("Grade 6"));
	grades.append("--");
	grades.append(i18n("Others in Jouyou"));
	grades.append(i18n("Jinmeiyou"));

	KAction *closeAction = KStdAction::close(this, SLOT(close()), actionCollection());
	forwardAct = KStdAction::next(this, SLOT(next()), actionCollection());
	cheatAct = new KAction(i18n("&Cheat"), CTRL + Key_C, this, SLOT(cheat()), actionCollection(), "cheat");
	(void) new KAction(i18n("&Random"), "goto", CTRL + Key_R, this, SLOT(random()), actionCollection(), "random");
	gradeAct = new KListAction(i18n("Grade"), 0, this, SLOT(updateGrade()), actionCollection(), "grade");
	gradeAct->setItems(grades);
	connect(gradeAct, SIGNAL(activated(const QString&)), SLOT(updateGrade()));
	(void) KStdAction::prior(this, SLOT(prev()), actionCollection());
	(void) new KAction(i18n("&Delete"), "editdelete", CTRL + Key_X, this, SLOT(del()), actionCollection(), "del");
	newAct = KStdAction::openNew(this, SLOT(add()), actionCollection());
	saveAct = KStdAction::save(this, SLOT(writeConfiguration()), actionCollection());

	QVBoxLayout *quizLayout = new QVBoxLayout(quizTop, 6);

	qKanji = new QLabel("", quizTop);
	quizLayout->addWidget(qKanji);

	Q1 = new KPushButton("", quizTop);
	quizLayout->addWidget(Q1);
	connect(Q1, SIGNAL(clicked()), SLOT(q1()));
	Q2 = new KPushButton("", quizTop);
	quizLayout->addWidget(Q2);
	connect(Q2, SIGNAL(clicked()), SLOT(q2()));
	Q3 = new KPushButton("", quizTop);
	quizLayout->addWidget(Q3);
	connect(Q3, SIGNAL(clicked()), SLOT(q3()));
	Q4 = new KPushButton("", quizTop);
	quizLayout->addWidget(Q4);
	connect(Q4, SIGNAL(clicked()), SLOT(q4()));
	Q5 = new KPushButton("", quizTop);
	quizLayout->addWidget(Q5);
	connect(Q5, SIGNAL(clicked()), SLOT(q5()));

	quizLayout->addSpacing(10);

	createGUI("learnui.rc");
	StatusBar = statusBar();
	closeAction->plug(toolBar());

	KConfig *config = kapp->config(); // do this here so writeConfig only does the learnlist stuff
	config->setGroup("Learn");
	gradeAct->setCurrentItem(config->readNumEntry("grade", 1));

	isMod = false;

	updateGrade();
	readConfiguration();

	connect(this, SIGNAL(listDirty()), this, SLOT(updateQuiz()));
	emit listDirty();

	curItem = List->firstChild();
	prevItem = curItem;
	qnew(); // init first quiz

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
	if (Tabs->currentPageIndex() == 1)
	{
		if (!prevItem)
			return;
		curItem = prevItem;
	
		qupdate();
		return;
	}

	if (!list.prev())
		list.last();
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
	View->addHeader(i18n("%1 in common compunds").arg(kanji));
		
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
	int grade = gradeAct->currentItem() + 1;

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
	setCaption(i18n("Grade %1 - Learn").arg(grade));

	list.removeFirst();
	list.first();
	update();

	KConfig *config = kapp->config();
	
	config->setGroup("Learn");
	config->writeEntry("grade", gradeAct->currentItem());
}

void Learn::readConfiguration()
{
	List->clear();
	KConfig *config = kapp->config();

	config->setGroup("Learn");
	gradeAct->setCurrentItem(config->readNumEntry("grade", 0));

	QStringList kanji = config->readListEntry("__KANJI");
	QStringList::Iterator it;

	for (it = kanji.begin(); it != kanji.end(); ++it)
	{
		(void) new QListViewItem(List, *it, config->readEntry(*it), config->readEntry(QString(*it).append("_readings")), config->readEntry(QString(*it).append("_meanings"))); // make new list view item(parent, kanji, grade, readings)
	}

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

void Learn::add()
{
	QString readings = Dict::prettyKanjiReading(list.current()->readings());
	QString meanings = Dict::prettyMeaning(list.current()->meanings());

	(void) new QListViewItem(List, list.current()->kanji(), QString::number(list.current()->grade()), readings, meanings);

	isMod = true;
	emit listDirty();
}

void Learn::showKanji(QListViewItem *item)
{
	if (!item)
		return;

	QString kanji(item->text(0));

	unsigned int grade = item->text(1).toUInt() - 1;
	gradeAct->setCurrentItem(grade);
	updateGrade();
	
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

void Learn::updateCaption(QListViewItem *item)
{
	setCaption(kapp->makeStdCaption(item->text(0)));
}

void Learn::del()
{
	if (Tabs->currentPageIndex() == 1)
	{
		delete curItem;
		curItem = prevItem; // needs to be something
		qnew();
		return;
	}

	QListViewItem *kanji = List->selectedItem();
	if (!kanji)
		return;

	delete kanji;

	isMod = true;
	emit listDirty();
}

void Learn::q1()
{
	if (seikai == 1)
	{
		StatusBar->message(i18n("Good!"));
		qnew();
	}
	else
	{
		StatusBar->message(i18n("Wrong"));
	}
}

void Learn::q2()
{
	if (seikai == 2)
	{
		StatusBar->message(i18n("Good!"));
		qnew();
	}
	else
	{
		StatusBar->message(i18n("Wrong"));
	}
}

void Learn::q3()
{
	if (seikai == 3)
	{
		StatusBar->message(i18n("Good!"));
		qnew();
	}
	else
	{
		StatusBar->message(i18n("Wrong"));
	}
}

void Learn::q4()
{
	if (seikai == 4)
	{
		StatusBar->message(i18n("Good!"));
		qnew();
	}
	else
	{
		StatusBar->message(i18n("Wrong"));
	}
}

void Learn::q5()
{
	if (seikai == 5)
	{
		StatusBar->message(i18n("Good!"));
		qnew();
	}
	else
	{
		StatusBar->message(i18n("Wrong"));
	}
}

QString Learn::randomMeaning()
{
	QString meaning;
	float rand = kapp->random();

	QStringList::Iterator itr;

TryAgain:

	if ((rand > (RAND_MAX / 2)) || (List->childCount() < 5))
	{
		rand = kapp->random();
		float rand2 = RAND_MAX / rand;
		rand = list.count() / rand2;
	
		QPtrListIterator<Kanji> it(list);

		int i;
		for (i = 1; i < rand; ++it)
		{i++;}

		meaning = dict->prettyMeaning(it.current()->meanings());
	}
	else
	{
		rand = kapp->random();
		float rand2 = RAND_MAX / rand;
		rand = List->childCount() / rand2;
	
		int max = (int) rand;
	
		QListViewItemIterator it(List);
		int i;
		for (i = 1; i < max; ++it)
			{i++;}
	
		meaning = it.current()->text(3);
	}

	for (itr = oldMeanings.begin(); itr != oldMeanings.end(); ++itr)
	{
		//kdDebug() << "old meaning: " << *itr << " new meaning: " << meaning << endl;
		if ((*itr) == meaning)
		{
			//kdDebug() << "TRYAGAIN\n";
			goto TryAgain;
		}
	}

	if (meaning == curItem->text(3))
		goto TryAgain;

	oldMeanings.append(meaning);
	meaning = shortenString(meaning);

	return meaning;
}

void Learn::qupdate()
{
	if (!curItem)
	{
		return;
	}
	
	qKanji->setText(QString("<font size=\"+3\">%1</font> %2").arg(curItem->text(0)).arg(curItem->text(2)));

	float rand = kapp->random();
	float rand2 = RAND_MAX / rand;
	seikai = static_cast<int>(5 / rand2);
	seikai++;

	//kdDebug() << "seikai = " << seikai << endl;

	Q1->setText(randomMeaning());
	Q2->setText(randomMeaning());
	Q3->setText(randomMeaning());
	Q4->setText(randomMeaning());
	Q5->setText(randomMeaning());

	//kdDebug() << "done with random meanings\n";

	switch (seikai)
	{
		case 1:
			Q1->setText(curItem->text(3));
			break;
		case 2:
			Q2->setText(curItem->text(3));
			break;
		case 3:
			Q3->setText(curItem->text(3));
			break;
		case 4:
			Q4->setText(curItem->text(3));
			break;
		case 5:
			Q5->setText(curItem->text(3));
			break;
	}
}

void Learn::qnew() // new quiz kanji
{
	oldMeanings.clear();
	StatusBar->clear();

	float rand = kapp->random();
	float rand2 = RAND_MAX / rand;
	rand = List->childCount() / rand2;
	int max = (int) rand;

	//kdDebug() << "max = " << max << endl;

	QListViewItemIterator it(List);
	QListViewItemIterator tmp(List);
	int i;
	for (i = 0; i < max; ++it)
		{i++; ++tmp;}

	//kdDebug() << "still here 1\n";
	if (curItem->text(0) == it.current()->text(0)) // same, don't use
	{
		//kdDebug() << "uh oh they are same\n";
		++it;
		if (!it.current())
		{
			tmp--;
			it = tmp;
		}
	}
	//kdDebug() << "still here 2\n";

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
	switch (seikai)
	{
		case 1:
		Q1->setFocus();
		break;
		case 2:
		Q2->setFocus();
		break;
		case 3:
		Q3->setFocus();
		break;
		case 4:
		Q4->setFocus();
		break;
		case 5:
		Q5->setFocus();
		break;
	}
	StatusBar->message(i18n("Better luck next time"));
}

QString Learn::shortenString(QString thestring)
{
	return KStringHandler::rsqueeze(thestring, 60);
}

void Learn::tabChanged(QWidget *widget)
{
	if (widget == quizTop)
	{
		forwardAct->setEnabled(false);
		gradeAct->setEnabled(false);
		saveAct->setEnabled(false);
		newAct->setEnabled(false);

		cheatAct->setEnabled(true);
	}
	else
	{
		forwardAct->setEnabled(true);
		gradeAct->setEnabled(true);
		saveAct->setEnabled(true);
		newAct->setEnabled(true);

		cheatAct->setEnabled(false);
	}
}

void Learn::updateQuiz()
{
	if (List->childCount() < 3)
		Tabs->setTabEnabled(quizTop, false);
	else
		Tabs->setTabEnabled(quizTop, true);
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
  //kdDebug() << "KonqComboAction::plug id=" << id << endl;

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
  //kdDebug() << "KonqComboAction::unplug idx=" << idx << " menuId=" << menuId(idx) << endl;

  toolBar->removeItem( menuId( idx ) );

  removeContainer( idx );
  m_combo = 0L;
}

void EditAction::clear()
{
	m_combo->clear();
}

#include "widgets.moc"
