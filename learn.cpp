#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdialog.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <kstringhandler.h>
#include <ktoolbar.h>
#include <qbuttongroup.h>
#include <qheader.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qsplitter.h>
#include <qtabwidget.h>
#include <qtimer.h>
#include <qtextcodec.h>

#include <stdlib.h> // RAND_MAX
#include <cassert>

#include "dict.h"
#include "kloader.h"
#include "ksaver.h"
#include "learn.h"

const int Learn::numberOfAnswers = 5;

Learn::Learn(Dict::Index *parentDict, QWidget *parent, const char *name)
	: KMainWindow(parent, name)
	, curItem(0)
{
	initialized = false;
	index = parentDict;

	QWidget *dummy = new QWidget(this);
	setCentralWidget(dummy);

	QVBoxLayout *veryTop = new QVBoxLayout(dummy, 0, KDialog::spacingHint());
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
	List->addColumn(i18n("Meanings"));
	List->addColumn(i18n("Readings"));
	List->addColumn(i18n("Grade"));
	List->addColumn(i18n("Your Score"));

	List->setAllColumnsShowFocus(true);
	List->setColumnWidthMode(0, QListView::Maximum);
	List->setColumnWidthMode(1, QListView::Maximum);
	List->setColumnWidthMode(2, QListView::Maximum);
	List->setColumnWidthMode(3, QListView::Maximum);
	List->setMultiSelection(true);
	List->setDragEnabled(true);
	List->setSorting(4);
	List->setSelectionModeExt(KListView::Extended);

	List->header()->setClickEnabled(false);

	connect(List, SIGNAL(executed(QListViewItem *)), SLOT(showKanji(QListViewItem *)));
	connect(List, SIGNAL(selectionChanged()), this, SLOT(itemSelectionChanged()));

	View = new ResultView(true, listTop, "View");
	connect(View, SIGNAL(linkClicked(const QString &)), this, SIGNAL(linkClicked(const QString &)));

	QStringList grades(i18n("Grade 1"));
	grades.append(i18n("Grade 2"));
	grades.append(i18n("Grade 3"));
	grades.append(i18n("Grade 4"));
	grades.append(i18n("Grade 5"));
	grades.append(i18n("Grade 6"));
	grades.append(i18n("Others in Jouyou"));
	grades.append(i18n("Jinmeiyou"));

	/*KAction *closeAction = */(void) KStdAction::close(this, SLOT(close()), actionCollection());
	printAct = KStdAction::print(this, SLOT(print()), actionCollection());
	forwardAct = KStdAction::forward(this, SLOT(next()), actionCollection());
	forwardAct->plug(toolBar());
	backAct = KStdAction::back(this, SLOT(prev()), actionCollection());
	backAct->plug(toolBar());
	cheatAct = new KAction(i18n("&Cheat"), CTRL + Key_C, this, SLOT(cheat()), actionCollection(), "cheat");
	randomAct = new KAction(i18n("&Random"), "goto", CTRL + Key_R, this, SLOT(random()), actionCollection(), "random");
	gradeAct = new KListAction(i18n("Grade"), 0, 0, 0, actionCollection(), "grade");
	gradeAct->setItems(grades);
	connect(gradeAct, SIGNAL(activated(const QString&)), SLOT(updateGrade()));
	removeAct = new KAction(i18n("&Delete"), "edit_remove", CTRL + Key_X, this, SLOT(del()), actionCollection(), "del");
	addAct = new KAction(i18n("&Add"), "edit_add", CTRL + Key_A, this, SLOT(add()), actionCollection(), "add");
	addAllAct = new KAction(i18n("Add A&ll"), 0, this, SLOT(addAll()), actionCollection(), "addall");
	newAct = KStdAction::openNew(this, SLOT(openNew()), actionCollection());
	openAct = KStdAction::open(this, SLOT(open()), actionCollection());
	saveAct = KStdAction::save(this, SLOT(save()), actionCollection());
	saveAsAct = KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());
	(void) KStdAction::preferences(this, SIGNAL(configureLearn()), actionCollection());

	removeAct->setEnabled(false);

	QVBoxLayout *quizLayout = new QVBoxLayout(quizTop, KDialog::marginHint(), KDialog::spacingHint());

	quizLayout->addStretch();
	QHBoxLayout *hlayout = new QHBoxLayout(quizLayout);
	qKanji = new QPushButton(quizTop);
	connect(qKanji, SIGNAL(clicked()), this, SLOT(qKanjiClicked()));
	hlayout->addStretch();
	hlayout->addWidget(qKanji);
	hlayout->addStretch();
	quizLayout->addStretch();

	answers = new QButtonGroup(1, Horizontal, quizTop);
	for (int i = 0; i < numberOfAnswers; ++i)
		answers->insert(new KPushButton(answers), i);
	quizLayout->addWidget(answers);
	quizLayout->addStretch();
	connect(answers, SIGNAL(clicked(int)), this, SLOT(answerClicked(int)));

	createGUI("learnui.rc");
	//closeAction->plug(toolBar());

	resize(600, 400);
	applyMainWindowSettings(kapp->config(), "LearnWindow");

	statusBar()->message(i18n("Put on your thinking cap!"));

	nogood = false;

	// this is so learn doesn't take so long to show itself
	QTimer::singleShot(200, this, SLOT(finishCtor()));
}

void Learn::finishCtor()
{
	KConfig &config = *kapp->config();
	config.setGroup("Learn");

	setCurrentGrade(config.readNumEntry("grade", 1));

	/*
	 * this must be done now, because
	 * to start a quiz, we need a working randomMeaning()
	 * and that needs a loaded grade!
	 */
	updateGrade();
	updateQuizConfiguration(); // first

	config.setGroup("Learn");
	QString entry = config.readEntry("lastFile", QString(""));
	//kdDebug() << "lastFile: " << entry << endl;
	if (!entry.isEmpty())
	{
		filename = entry;
		read(filename);
	}
	else
	{
		openNew();
	}

	initialized = true;
}

Learn::~Learn()
{
	emit destroyed(this);
}

bool Learn::warnClose()
{
	if (isMod)
	{
		int result = KMessageBox::warningYesNoCancel(this, i18n("There are unsaved changes to learning list. Save them?"), i18n("Unsaved changes"), i18n("Save"), i18n("Discard"), "DiscardAsk", true);
		switch (result)
		{
		case KMessageBox::Yes:
			saveAct->activate();
			// fallthrough
		case KMessageBox::No:
			return true;
		case KMessageBox::Cancel:
			return false;
		}
	}

	return true;
}

bool Learn::closeWindow()
{
	if (!warnClose())
	{
		return false;
	}
	else
	{
		close();
		return true;
	}
}

void Learn::closeEvent(QCloseEvent *e)
{
	if (!warnClose())
		return;

	saveScores(); // also sync()s;
	//kapp->config()->sync();

	saveMainWindowSettings(KGlobal::config(), "LearnWindow");
	e->accept();
}

void Learn::random()
{
	int rand = static_cast<int>(static_cast<float>(list.count()) / (static_cast<float>(RAND_MAX) / kapp->random()));

	current = list.at(rand - 1);
	update();
}

void Learn::next()
{
	++current;
	if (current == list.end())
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
	
		statusBar()->clear();
		qupdate();
		nogood = true;
		backAct->setEnabled(false);
		return;
	}

	if (current == list.begin())
		current = list.end();
	--current;
	update();
}

void Learn::update()
{
	View->clear();
	Dict::Entry curKanji = *current;

	if (!curKanji.kanji())
	{
		statusBar()->message(i18n("Grade not loaded")); // oops
		return;
	}
	View->addKanjiResult(curKanji);

	// now show some compounds in which this kanji appears
	
	QString kanji = curKanji.kanji();

	Dict::SearchResult compounds = index->search(QRegExp(kanji), kanji, true);
	View->addHeader(i18n("%1 in compounds").arg(kanji));
	
	for (QValueListIterator<Dict::Entry> it = compounds.list.begin(); it != compounds.list.end(); ++it)
	{
		kapp->processEvents();
		View->addResult(*it, true);
	}

	View->flush();
}

void Learn::updateGrade()
{
	int grade = getCurrentGrade();

	QString regexp("G%1 ");
	regexp = regexp.arg(grade);

	Dict::SearchResult result = index->searchKanji(QRegExp(regexp), regexp, false);
	list = result.list;

	statusBar()->message(i18n("%1 entries in grade %2").arg(list.count()).arg(grade));

	list.remove(list.begin());
	current = list.begin();
	update();

	KConfig &config = *KGlobal::config();
	config.setGroup("Learn");
	config.writeEntry("grade", grade);
}

void Learn::read(const KURL &url)
{
	List->clear();

	KLoader loader(url);
	if (!loader.open())
	{
		KMessageBox::error(this, loader.error(), i18n("Error"));
		return;
	}

	QTextCodec &codec = *QTextCodec::codecForName("eucJP");
	QTextStream &stream = loader.textStream();
	stream.setCodec(&codec);

	while (!stream.atEnd())
	{
		QChar kanji;
		stream >> kanji;
		// ignore whitespace
		if (!kanji.isSpace())
		{
			QRegExp regexp = QString("^%1\\W").arg(kanji);
			Dict::SearchResult res = index->searchKanji(regexp, kanji, false);
			Dict::Entry first = Dict::firstEntry(res);
			if (first.extendedKanjiInfo())
				add(first, true);
		}
	}

	setClean();
}

void Learn::open()
{
	if (!warnClose())
		return;

	KURL prevname = filename;
	filename = KFileDialog::getOpenURL(QString::null, "*.kiten");
	if (filename.isEmpty())
	{
		filename = prevname;
		return;
	}

	read(filename);

	//kdDebug() << "saving lastFile\n";
	KConfig &config = *kapp->config();
	config.setGroup("Learn");
	config.writeEntry("lastFile", filename.url());
	config.sync();

	// redo quiz, because we deleted the current quiz item
	curItem = List->firstChild();
	backAct->setEnabled(false);
	prevItem = curItem;
	qnew();

	numChanged();
}

void Learn::openNew()
{
	if (!warnClose())
		return;

	filename = "";

	List->clear();
	setClean();

	numChanged();
}

void Learn::saveAs()
{
	KURL prevname = filename;
	filename = KFileDialog::getSaveURL(QString::null, "*.kiten");
	if (filename.isEmpty())
	{
		filename = prevname;
		return;
	}
	save();
}

void Learn::save()
{
	if (filename.isEmpty())
		saveAs();
	if (filename.isEmpty())
		return;

	write(filename);

	KConfig &config = *kapp->config();
	//kdDebug() << "saving lastFile\n";
	config.setGroup("Learn");
	config.writeEntry("lastFile", filename.url());
	config.sync();
}

void Learn::write(const KURL &url)
{
	KSaver saver(url);

	if (!saver.open())
	{
		KMessageBox::error(this, saver.error(), i18n("Error"));
		return;
	}

	QTextCodec &codec = *QTextCodec::codecForName("eucJP");
	QTextStream &stream = saver.textStream();
	stream.setCodec(&codec);

	for (QListViewItemIterator it(List); it.current(); ++it)
		stream << it.current()->text(0).at(0);

	if (!saver.close())
	{
		KMessageBox::error(this, saver.error(), i18n("Error"));
		return;
	}

	saveScores();

	setClean();

	statusBar()->message(i18n("%1 written").arg(url.prettyURL()));
}

void Learn::saveScores()
{
	KConfig &config = *kapp->config();
	config.setGroup("Learn Scores");
	for (QListViewItemIterator it(List); it.current(); ++it)
		config.writeEntry(it.current()->text(0), it.current()->text(4).toInt());
	config.sync();
}

void Learn::add(Dict::Entry toAdd, bool noEmit)
{
	// Remove peripheral readings: This is a study mode, not a reference mode
	QRegExp inNames = QString::fromLatin1(",\\s*[A-Za-z ]+:.*");
	QString readings = Dict::prettyKanjiReading(toAdd.readings()).replace(inNames, "");
	QString meanings = shortenString(Dict::prettyMeaning(toAdd.meanings()).replace(inNames, ""));
	QString kanji = toAdd.kanji();

	// here's a dirty rotten cheat (well, not really)
	// noEmit always means it's not added by the user, so this check isn't needed
	if (!noEmit)
	{
		for (QListViewItemIterator it(List); it.current(); ++it)
		{
			if (it.current()->text(0) == kanji)
			{
				statusBar()->message(i18n("%1 already on your list").arg(kanji));
				return;
			}
		}
	}

	statusBar()->message(i18n("%1 added to your list").arg(kanji));

	KConfig &config = *kapp->config();
	int score = 0;
	config.setGroup("Learn Scores");
	score = config.readNumEntry(kanji, score);

	unsigned int grade = toAdd.grade();
	addItem(new QListViewItem(List, kanji, meanings, readings, QString::number(grade), QString::number(score)), noEmit);

	numChanged();
}

void Learn::add()
{
	add(*current);
	setDirty();
}

void Learn::addAll()
{
	int grade = getCurrentGrade();

	QString regexp("G%1 ");
	regexp = regexp.arg(grade);

	Dict::SearchResult result = index->searchKanji(QRegExp(regexp), regexp, false);
	for (QValueListIterator<Dict::Entry> i = result.list.begin(); i != result.list.end(); ++i)
	{
		// don't add headers
		if ((*i).dictName() == "__NOTSET" && (*i).header() == "__NOTSET")
			add(*i);
	}
}

void Learn::addItem(QListViewItem *item, bool noEmit)
{
	// 2 is the magic jump
		if (List->childCount() == 2)
		{
			curItem = item;
			prevItem = curItem;
			qnew(); // init first quiz
			//kdDebug() << "initting first quiz in addItem\n";
		}

	if (!noEmit)
	{
		List->ensureItemVisible(item);

		setDirty();
	}
}

void Learn::showKanji(QListViewItem *item)
{
	assert(item);

	QString kanji(item->text(0));
	int grade = item->text(3).toUInt();

	if (getCurrentGrade() != grade)
	{
		setCurrentGrade(grade);
		updateGrade();
	}

	// Why does this fail to find the kanji sometimes?
	for (current = list.begin(); current != list.end() && (*current).kanji() != kanji; ++current);

	update();
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

		bool makenewq = false; // must make new quiz if we
		                       // delete the current item
		for (QPtrListIterator<QListViewItem> i(selected); *i; ++i)
		{
			if (curItem == i)
				makenewq = true;
			delete *i;
		}

		if (List->childCount() < 1)
		{
			// this isn't a good state, quizzing is unstable
		}
		else
		{
			if (makenewq)
			{
				curItem = List->firstChild();
				backAct->setEnabled(false);
				prevItem = curItem;
				qnew();
			}
		}

		setDirty();
	}

	numChanged();
}

// too easy...
void Learn::print()
{
	View->clear();
	View->addHeader(QString("<h1>%1</h1>").arg(i18n("Learning List")), 1);

	QListViewItemIterator it(List);
	for (; it.current(); ++it)
	{
		QString kanji = it.current()->text(0);
		Dict::SearchResult result = index->searchKanji(QRegExp(kanji), kanji, false);
		for (QValueListIterator<Dict::Entry> i = result.list.begin(); i != result.list.end(); ++i)
		{
			if ((*i).dictName() == "__NOTSET" && (*i).header() == "__NOTSET")
			{
				View->addKanjiResult(*i);
				break;
			}
		}
	}

	View->print();
}

void Learn::answerClicked(int i)
{
	int newscore = 0;
	KConfig &config = *kapp->config();
	config.setGroup("Learn");
	bool donew = false;

	if (seikai == i)
	{
		statusBar()->message(i18n("Good!"));

		if (!nogood) // add two to their score
			newscore = curItem->text(4).toInt() + 2;
		else
		{
			qnew();
			return;
		}

		donew = true;
	}
	else
	{
		statusBar()->message(i18n("Wrong"));
		// take one off score
		newscore = curItem->text(4).toInt() - 1;

		if (!nogood)
			nogood = true;
		else
			return;
	}

	//config.writeEntry(curItem->text(0) + "_4", newscore);

	QListViewItem *newItem = new QListViewItem(List, curItem->text(0), curItem->text(1), curItem->text(2), curItem->text(3), QString::number(newscore));

	// readd, so it sorts
	delete curItem;
	curItem = newItem;

	if (donew)
		qnew();
}

QString Learn::randomMeaning(QStringList &oldMeanings)
{
	QString meaning;

	do
	{
		float rand = kapp->random();
		if ((rand > (RAND_MAX / 2)) || (List->childCount() < numberOfAnswers))
		{
			// get a meaning from dict
			//kdDebug() << "from our dict\n";
			rand = kapp->random();
			float rand2 = RAND_MAX / rand;
			rand = ((float)list.count() - 1) / rand2;
			//rand -= 1;
			//kdDebug() << "rand: " << rand << endl;
			//kdDebug() << "list.count(): " << list.count() << endl;

			switch (guessOn)
			{
				case 1:
				meaning = shortenString(Dict::prettyMeaning((*list.at(rand)).meanings()));
				break;
				case 2:
				meaning = Dict::prettyKanjiReading((*list.at(rand)).readings());
				break;
				case 0:
				meaning = (*list.at(rand)).kanji();
			}
		}
		else
		{
			// get a meaning from our list
			//kdDebug() << "from our list\n";
			rand = kapp->random();
			float rand2 = RAND_MAX / rand;
			rand = List->childCount() / rand2;

			int max = (int) rand;
		
			QListViewItemIterator it(List);
			it += max;

			meaning = it.current()->text(guessOn);
		}

		//kdDebug() << "meaning: " << meaning << endl;
		for (QStringList::Iterator it = oldMeanings.begin(); it != oldMeanings.end(); ++it)
		{
			//kdDebug() << "oldMeaning: " << *it << endl;
		}
		//kdDebug() << "curMeaning: " << curItem->text(guessOn) << endl;
	}
	while (oldMeanings.contains(meaning) || meaning == curItem->text(guessOn));

	oldMeanings.append(meaning);
	meaning = shortenString(meaning);

	return meaning;
}

void Learn::qupdate()
{
	if (!curItem)
		return;

	qKanji->setText(curItem->text(quizOn));
	QFont newFont = font();
	if (quizOn == 0)
		newFont.setPixelSize(24);
	qKanji->setFont(newFont);

	float rand = kapp->random();
	float rand2 = RAND_MAX / rand;
	seikai = static_cast<int>(numberOfAnswers / rand2);

	QStringList oldMeanings;
	for (int i = 0; i < numberOfAnswers; ++i)
		answers->find(i)->setText(randomMeaning(oldMeanings));

	answers->find(seikai)->setText(curItem->text(guessOn));
}

void Learn::qnew() // new quiz kanji
{
	//kdDebug() << "qnew\n";
	nogood = false;

	statusBar()->clear();
	statusBar()->message(QString("%1 %2 %3").arg(curItem->text(0)).arg(curItem->text(1)).arg(curItem->text(2)));

	backAct->setEnabled(true);

	unsigned int count = List->childCount();

	if (count < 2)
		return;

	float max = static_cast<float>(count) / (static_cast<float>(RAND_MAX) / kapp->random());
	if (kapp->random() < (static_cast<float>(RAND_MAX) / 3.25))
		max /= (static_cast<float>(RAND_MAX) / (kapp->random() + 1));

	max = static_cast<int>(max);

	if (max > count)
		max = count;

	QListViewItemIterator it(List);
	QListViewItemIterator tmp(List);
	int i;
	for (i = 2; i <= max; ++it)
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
	statusBar()->message(i18n("Better luck next time"));
	nogood = true;
}

QString Learn::shortenString(QString thestring)
{
	return KStringHandler::rsqueeze(thestring, 60).stripWhiteSpace();
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
	openAct->setEnabled(!isQuiz);
	newAct->setEnabled(!isQuiz);
	saveAsAct->setEnabled(!isQuiz);

	cheatAct->setEnabled(isQuiz);

	// also handled below for !isQuiz case
	removeAct->setEnabled(isQuiz);

	if (isQuiz)
	{
		qKanji->setFocus();

	}
	else
	{
		// handle removeAct;
		itemSelectionChanged();
	}

	statusBar()->clear();
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
	removeAct->setEnabled(List->selectedItems().count() > 0);
}

int Learn::getCurrentGrade(void)
{
	int grade = gradeAct->currentItem() + 1;
	if (grade > 6) ++grade;
	return grade;
}

void Learn::setCurrentGrade(int grade)
{
	if (grade > 6) --grade;
	gradeAct->setCurrentItem(grade - 1);
}

void Learn::updateQuizConfiguration()
{
	KConfig &config = *kapp->config();
	config.setGroup("Learn");

	quizOn = config.readNumEntry("Quiz On", 0);
	guessOn = config.readNumEntry("Guess On", 1);

	answers->setTitle(List->columnText(guessOn));

	View->updateFont();

	if (List->childCount() >= 2 && initialized)
		qnew();
}

void Learn::setDirty()
{
	isMod = true;
	setCaption(filename.prettyURL(), true);
}

void Learn::setClean()
{
	isMod = false;
	if (!filename.prettyURL().isEmpty())
		setCaption(filename.prettyURL(), false);
}

void Learn::qKanjiClicked()
{
	showKanji(curItem);
}

void Learn::numChanged()
{
	Tabs->setTabEnabled(quizTop, List->childCount() >= 2);
	//quizTop->setEnabled(List->childCount() >= 2);
}

#include "learn.moc"
