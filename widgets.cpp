#include <qwidget.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <kstandarddirs.h>
#include <kprocess.h>
#include <qtextstream.h>
#include <kfiledialog.h>
#include <qsplitter.h>
#include <kaction.h>
#include <qheader.h>
#include <kstdaction.h>
#include <qpalette.h>
#include <qrect.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qfont.h>
#include <kprinter.h>
#include <kdebug.h>
#include <qsimplerichtext.h>
#include <kdialog.h>
#include <klistview.h>
#include <klocale.h>
#include <ktoolbar.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kapp.h>
#include <kiconloader.h>
#include <kpushbutton.h>
#include <kstatusbar.h>
#include <kstringhandler.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qtextbrowser.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qcstring.h>
#include <qtextcodec.h>
#include <qregexp.h>
#include <qtabwidget.h>
//#include <qtimer.h>
#include <qlayout.h>

#include <stdlib.h> // RAND_MAX
#include <cassert>

#include "widgets.h"
#include "dict.h"
#include "kromajiedit.h"


ResultView::ResultView(bool _links, QWidget *parent, const char *name)
	: QTextBrowser(parent, name)
{
	setReadOnly(true);
	links = _links;

	// qtextbrowser.cpp connects this... we don't need it
	// it gives console output ;)
	disconnect(this, SIGNAL(linkClicked(const QString &)), this, SLOT(setSource(const QString &)));
}

void ResultView::addResult(Dict::Entry result, bool com)
{
	if (result.dictName() != "__NOTSET")
	{
		addHeader((com? i18n("Common results from %1") : i18n("Results from %1")).arg(result.dictName()), 5);
		return;
	}
	if (result.header() != "__NOTSET")
	{
		addHeader(result.header());
		return;
	}

	QString html;
	if (result.kanaOnly())
		html = QString("<p><font size=\"+2\">%1</font>  ").arg(result.firstReading());
	else
		html = QString("<p><font size=\"+2\">%1</font>: %2  ").arg(putchars(result.kanji())).arg(result.firstReading());

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

	append(html);
}

void ResultView::addKanjiResult(Dict::Entry result, bool com, Radical rad)
{
	if (result.dictName() != "__NOTSET")
	{
		addHeader((com? i18n("Common results from %1") : i18n("Results from %1")).arg(result.dictName()), 5);
		return;
	}
	if (result.header() != "__NOTSET")
	{
		addHeader(result.header());
		return;
	}

	QString html;
	html = QString("<p><font size=\"+3\">%1</font>: %2  ").arg(putchars(result.kanji()));

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
				html.truncate(html.length() - 2); 
				html += i18n("<br />As radical: ");
			}
			else
			{
				html += (*it);
				html += ", ";
			}
		}
	}
	html.truncate(html.length() - 2); 

	html += "<br />";

	QStringList Meanings = result.meanings();
	for (it = Meanings.begin(); it != Meanings.end(); ++it)
	{
		html += (*it);
		html += "; ";
	}
	html.truncate(html.length() - 2); 
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

	if (!!rad.radical())
		html.append(i18n(" Largest radical: %1, with %2 strokes.").arg(rad.radical()).arg(rad.strokes()));

	html += "</p>";

	append(html);
}

void ResultView::addHeader(const QString &header, unsigned int degree)
{
	append(QString("<h%1>%2</h%3>").arg(degree).arg(header).arg(degree));
}

QString ResultView::putchars(const QString &text)
{
	if (!links)
		return text;

	unsigned int len = text.length();
	QString ret;

	QTextCodec *codec = QTextCodec::codecForName("eucJP");

	for (unsigned i = 0; i < len; i++)
	{
		QCString str = codec->fromUnicode(QString(text.at(i)));
		unsigned char first = str[0];

		if (first > 0xa8)
			ret.append(QString("<a href=\"%1\">%1</a>").arg(text.at(i)).arg(text.at(i)));
		else
			ret.append(text.at(i));
	}
	
	return ret;
}

void ResultView::append(const QString &text)
{
	printText.append(text);
}

void ResultView::clear()
{
	printText = "";
}

void ResultView::flush()
{
	setText(printText);
}

void ResultView::print(QString title)
{
	KPrinter printer;
	printer.setFullPage(true);
	if (printer.setup(this))
	{
		QPainter p(&printer);
		QPaintDeviceMetrics metrics(p.device());
		int dpix = metrics.logicalDpiX();
		int dpiy = metrics.logicalDpiY();
		const int margin = 72; // pt
	
		QRect body(dpix, dpiy, metrics.width() - margin * dpix / margin * 2, metrics.height() - margin * dpiy / margin * 2);
	
		QSimpleRichText richText(title.isNull()? printText : i18n("<h1>Search for \"%1\"</h1>").arg(title) + printText, font(), context(), styleSheet(), mimeSourceFactory(), body.height(), Qt::black, false);
		richText.setWidth(&p, body.width());
		QRect view(body);
		int page = 1;

		QColorGroup goodColorGroup = QColorGroup(colorGroup());
		goodColorGroup.setColor(QColorGroup::Link, Qt::black);

		do
		{
			richText.draw(&p, body.left(), body.top(), view, goodColorGroup);
			view.moveBy(0, body.height());
			p.translate(0, -body.height());

			QFont myFont(font());
			myFont.setPointSize(9);
			p.setFont(myFont);
			QString footer(QString("%1 - Kiten").arg(QString::number(page)));
			p.drawText(view.right() - p.fontMetrics().width(footer), view.bottom() + p.fontMetrics().ascent() + 5, footer);


			if (view.top() >= richText.height())
				break;

			printer.newPage();
			page++;

			kapp->processEvents();
		}
		while (true);
    }
}

void ResultView::updateFont()
{
	KConfig *config = kapp->config();
	
	config->setGroup("General");
	QFont defaultFont(config->readFontEntry("font"));
	config->setGroup("Font");
	setFont(config->readFontEntry("font", &defaultFont));
}

/////////////////////////////////////////////////////
// nice EDICT dictionary editor

eEdit::eEdit(const QString &_filename, QWidget *parent, const char *name)
	: KMainWindow(parent, name)
	, filename(_filename)
{
	List = new KListView(this);
	setCentralWidget(List);

	List->addColumn(i18n("Kanji"));
	List->addColumn(i18n("Reading"));
	List->addColumn(i18n("Meanings"));
	List->addColumn(i18n("Common"));
	List->setItemsRenameable(true);
	List->setRenameable(0);
	List->setRenameable(1);
	List->setRenameable(2);
	List->setRenameable(3);

	List->setAllColumnsShowFocus(true);
	List->setColumnWidthMode(0, QListView::Maximum);
	List->setColumnWidthMode(1, QListView::Maximum);
	List->setColumnWidthMode(2, QListView::Maximum);
	List->setColumnWidthMode(3, QListView::Maximum);
	List->setMultiSelection(true);
	List->setDragEnabled(true);

	saveAct = KStdAction::save(this, SLOT(save()), actionCollection());
	removeAct = new KAction(i18n("&Delete"), "edit_remove", CTRL + Key_X, this, SLOT(del()), actionCollection(), "del");
	addAct = new KAction(i18n("&Add"), "edit_add", CTRL + Key_A, this, SLOT(add()), actionCollection(), "add");
	KAction *closeAction = KStdAction::close(this, SLOT(close()), actionCollection());

	createGUI("eeditui.rc");
	closeAction->plug(toolBar());
	StatusBar = statusBar();

	openFile(filename);

	isMod = false;
}

eEdit::~eEdit()
{
}

void eEdit::add()
{
	if (List)
		new KListViewItem(List);
}

void eEdit::openFile(const QString &file)
{
	QFile f(file);
	if (!f.open(IO_ReadOnly))
		return;

	QTextStream t(&f);
	QString s;

	while (!t.eof())
	{ 
		s = t.readLine();
		if (s.left(1) == "#" || s.isEmpty())
			continue;
		Dict::Entry entry = Dict::parse(s);
		QString meanings = Dict::prettyMeaning(entry.meanings());
		bool common = meanings.find(QString("(P)")) >= 0;
		meanings.replace(QRegExp("; "), "/");
		meanings.replace(QRegExp("/\\(P\\)"), "");
		new QListViewItem(List, entry.kanji(), Dict::prettyKanjiReading(entry.readings()), meanings, common? i18n("yes") : i18n("no"));
	}
}

void eEdit::save()
{
	QFile f(filename);
	if (!f.open(IO_WriteOnly))
		return;
	QTextStream t(&f);

	t << "# Generated by Kiten's EDICT editor" << endl << "# http://katzbrown.com/kiten" << endl << endl;
	
	QListViewItemIterator it(List);
	for (; it.current(); ++it)
	{
		QString kanji = it.current()->text(0);
		QString text = kanji;
		QString reading = it.current()->text(1);
		QString meanings = it.current()->text(2);
		if (meanings.right(1) != "/")
			meanings.append("/");
		if (meanings.left(1) != "/")
			meanings.prepend("/");
		QString commonString = it.current()->text(3).lower();

		bool common = (commonString == "true" || commonString == "yes" || commonString == "1" || commonString == "common");

		text.append(" ");
		if (!kanji.isEmpty())
			text.append(QString("[%1] ").arg(reading));
		text.append(meanings);

		if (common)
			text.append("(P)/");

		t << text << endl;
	}

	f.flush();

	// find the index generator executable
	KProcess proc;
	proc << KStandardDirs::findExe("kitengen") << filename << KGlobal::dirs()->saveLocation("appdata", "xjdx/", true) + QFileInfo(filename).baseName() + ".xjdx";
	// TODO: put up a status dialog and event loop instead of blocking
	proc.start(KProcess::Block, KProcess::NoCommunication);
	
	StatusBar->message(i18n("Saved"));
	isMod = false;
}

void eEdit::del()
{
	QPtrList<QListViewItem> selected = List->selectedItems();
	assert(selected.count());

	for(QPtrListIterator<QListViewItem> i(selected); *i; ++i)
		delete *i;

	isMod = true;
}

////////////////////////////////////////////////

const int Learn::numberOfAnswers = 5;

Learn::Learn(Dict::Index *parentDict, QWidget *parent, const char *name)
	: KMainWindow(parent, name)
	, curItem(0)
{
	index = parentDict;
	noRead = false;

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
	connect(View, SIGNAL(linkClicked(const QString &)), parentWidget(), SLOT(ressearch(const QString &)));

	QStringList grades(i18n("Grade 1"));
	grades.append(i18n("Grade 2"));
	grades.append(i18n("Grade 3"));
	grades.append(i18n("Grade 4"));
	grades.append(i18n("Grade 5"));
	grades.append(i18n("Grade 6"));
	grades.append(i18n("Others in Jouyou"));
	grades.append(i18n("Jinmeiyou"));

	KAction *closeAction = KStdAction::close(this, SLOT(close()), actionCollection());
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
	openAct = KStdAction::open(this, SLOT(open()), actionCollection());
	openDefaultAct = new KAction(i18n("Open &Default"), 0, this, SLOT(openDefault()), actionCollection(), "open_default");
	saveAct = KStdAction::save(this, SLOT(writeConfiguration()), actionCollection());
	saveAsAct = KStdAction::saveAs(this, SLOT(saveAs()), actionCollection());
	(void) KStdAction::preferences(parentWidget(), SLOT(slotLearnConfigure()), actionCollection());

	removeAct->setEnabled(false);

	QVBoxLayout *quizLayout = new QVBoxLayout(quizTop, 0, KDialog::spacingHint());

	qKanji = new QLabel(quizTop);
	quizLayout->addStretch();
	quizLayout->addWidget(qKanji);
	quizLayout->addStretch();

	answers = new QButtonGroup(1, Horizontal, quizTop);
	for(int i = 0; i < numberOfAnswers; ++i)
		answers->insert(new KPushButton(answers), i);
	quizLayout->addWidget(answers);
	quizLayout->addStretch();
	connect(answers, SIGNAL(clicked(int)), this, SLOT(answerClicked(int)));

	createGUI("learnui.rc");
	StatusBar = statusBar();
	closeAction->plug(toolBar());

	config = kapp->config();
	filename = QString::null;
	readConfiguration();

	config->setGroup("Learn");
	setCurrentGrade(config->readNumEntry("grade", 1));

	isMod = false;

	updateGrade();
	updateQuizConfiguration(); // first

	connect(this, SIGNAL(listDirty()), this, SLOT(updateQuiz()));
	emit listDirty();

	/*
	if (List->childCount() >= 2)
	{
		curItem = List->firstChild();
		prevItem = curItem;
		backAct->setEnabled(false);
		qnew(); // init first quiz
	}
	*/

	resize(600, 400);
	applyMainWindowSettings(KGlobal::config(), "LearnWindow");

	StatusBar->message(i18n("Put on your thinking cap!"));

	nogood = false;
}

Learn::~Learn()
{
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

void Learn::closeEvent(QCloseEvent *e)
{
	if (!warnClose())
		return;

	emit listChanged();

	saveMainWindowSettings(KGlobal::config(), "LearnWindow");
	KMainWindow::closeEvent(e);
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
	
		StatusBar->clear();
		qupdate();
		nogood = true;
		backAct->setEnabled(false);
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
	Dict::Entry curKanji = *current;

	if (!curKanji.kanji())
	{
		StatusBar->message(i18n("Grade not loaded")); // oops
		return;
	}
	View->addKanjiResult(curKanji);

	// now show some compounds in which this kanji appears
	
	QString kanji = curKanji.kanji();

	Dict::SearchResult compounds = index->search(QRegExp(kanji), kanji, true);
	View->addHeader(i18n("%1 in compounds").arg(kanji));
	
	for(QValueListIterator<Dict::Entry> it = compounds.list.begin(); it != compounds.list.end(); ++it)
	{
		kapp->processEvents();
		View->addResult(*it, true);
	}

	View->flush();
}

void Learn::updateGrade()
{
	//kdDebug() << "update grade\n";
	int grade = getCurrentGrade();

	QString regexp("G%1 ");
	regexp = regexp.arg(grade);

	Dict::SearchResult result = index->searchKanji(QRegExp(regexp), regexp, false);
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
	if (noRead)
	{
		noRead = false;
		return;
	}

	List->clear();

	config->setGroup("Learn");
	QStringList kanji = config->readListEntry("__KANJI");
	QStringList::Iterator it;

	// make new list view item(parent, kanji, grade, readings)
	for (it = kanji.begin(); it != kanji.end(); ++it)
		addItem(new QListViewItem(List, *it, config->readEntry(*it), config->readEntry((*it) + "_2"), config->readEntry((*it) + "_3"), config->readEntry((*it) + "_4")), true);
}

void Learn::open()
{
	if (!warnClose())
		return;

	QString prevname = filename;
	filename = KFileDialog::getOpenFileName(QString::null, "*.kiten");
	if (filename.isEmpty())
	{
		filename = prevname;
		return;
	}

	// for a while, i was deleting kapp->config()... and wondering
	// why saveMainWindowSettings() was crashing ;)
	if (kapp->config() != config)
		delete config;

	config = new KConfig(filename, false, false);
	readConfiguration();

	emit listDirty();
}

void Learn::openDefault()
{
	if (!warnClose())
		return;

	filename = QString::null;
	config = kapp->config();
	readConfiguration();
}

void Learn::saveAs()
{
	QString prevname = filename;
	filename = KFileDialog::getSaveFileName(QString::null, "*.kiten");
	if (filename.isEmpty())
	{
		filename = prevname;
		return;
	}
	if (kapp->config() != config)
		delete config;
	config = new KConfig(filename, false, false);
	writeConfiguration();
}

void Learn::writeConfiguration()
{
	config->setGroup("Learn");

	QStringList kanji;
	QListViewItemIterator it(List);
	QString curKanji;

	for (; it.current(); ++it)
	{
		curKanji = it.current()->text(0);
		kanji.append(curKanji);
		config->writeEntry(curKanji, it.current()->text(1));
		config->writeEntry(QString(curKanji).append("_2"), it.current()->text(2));
		config->writeEntry(QString(curKanji).append("_3"), it.current()->text(3).toUInt());
		config->writeEntry(QString(curKanji).append("_4"), it.current()->text(4));
	}

	config->writeEntry("__KANJI", kanji);

	isMod = false;

	// don't emit if it isn't the default
	if (filename == QString::null)
	{
		noRead = true;
		emit listChanged();
	}
	else
	{
		StatusBar->message(i18n("%1 written").arg(filename));
	}

	config->sync();
}

void Learn::externAdd(Dict::Entry toAdd)
{
	// Remove peripheral readings: This is a study mode, not a reference mode
	QRegExp inNames = QString::fromLatin1(",\\s*[A-Za-z ]+:.*");
	QString readings = Dict::prettyKanjiReading(toAdd.readings()).replace(inNames, "");
	QString meanings = shortenString(Dict::prettyMeaning(toAdd.meanings()).replace(inNames, ""));
	QString kanji = toAdd.kanji();
	QListViewItemIterator it(List);
	bool found = false;

	for (; it.current(); ++it)
	{
		if (it.current()->text(0) == kanji)
			found = true;
	}

	if (found)
	{
		StatusBar->message(i18n("%1 already on your list").arg(kanji));
		return;
	}
	else
		StatusBar->message(i18n("%1 added to your list").arg(kanji));
	

	unsigned int grade = toAdd.grade();
	addItem(new QListViewItem(List, kanji, meanings, readings, QString::number(grade), QString::number(0)));
}

void Learn::add()
{
	externAdd(*current);
	isMod = true;
}

void Learn::addAll()
{
	int grade = getCurrentGrade();

	QString regexp("G%1 ");
	regexp = regexp.arg(grade);

	Dict::SearchResult result = index->searchKanji(QRegExp(regexp), regexp, false);
	for(QValueListIterator<Dict::Entry> i = result.list.begin(); i != result.list.end(); ++i)
	{
		if ((*i).dictName() == "__NOTSET" && (*i).header() == "__NOTSET")
			   // don't add headers, Neil
			externAdd(*i);
	}
}

void Learn::addItem(QListViewItem *item, bool init)
{
	if (!curItem)
	{
		if (List->childCount() > 1)
		{
			curItem = item;
			prevItem = curItem;
			qnew(); // init first quiz
		}
	}

	if (!init)
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
	int grade = item->text(3).toUInt();

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

		bool makenewq = false; // must make new quiz if we
		                       // delete the current item
		for(QPtrListIterator<QListViewItem> i(selected); *i; ++i)
		{
			if (curItem == i)
				makenewq = true;
			delete *i;
		}

		if (makenewq)
		{
			curItem = List->firstChild();
			backAct->setEnabled(false);
			prevItem = curItem;
			qnew();
		}

		isMod = true;
		emit listDirty();
	}
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
	KConfig *config = kapp->config();
	config->setGroup("Learn");
	bool donew = false;

	if (seikai == i)
	{
		StatusBar->message(i18n("Good!"));

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
		StatusBar->message(i18n("Wrong"));
		// take one off score
		newscore = curItem->text(4).toInt() - 1;

		if (!nogood)
			nogood = true;
		else
			return;
	}

	config->writeEntry(curItem->text(0) + "_4", newscore);

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
			rand = kapp->random();
			float rand2 = RAND_MAX / rand;
			rand = list.count() / rand2;
			rand -= 1;

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
			rand = kapp->random();
			float rand2 = RAND_MAX / rand;
			rand = List->childCount() / rand2;

			int max = (int) rand;
		
			QListViewItemIterator it(List);
			it += max;

			meaning = it.current()->text(guessOn);
		}
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

	if (quizOn == 0)
		qKanji->setText(QString("<font size=\"+3\">%1</font>").arg(curItem->text(0)));
	else
		qKanji->setText(curItem->text(quizOn));

	float rand = kapp->random();
	float rand2 = RAND_MAX / rand;
	seikai = static_cast<int>(numberOfAnswers / rand2);

	QStringList oldMeanings;
	for(int i = 0; i < numberOfAnswers; ++i)
		answers->find(i)->setText(randomMeaning(oldMeanings));

	answers->find(seikai)->setText(curItem->text(guessOn));
}

void Learn::qnew() // new quiz kanji
{
	nogood = false;

	StatusBar->clear();
	StatusBar->message(QString("%1 %2 %3").arg(curItem->text(0)).arg(curItem->text(1)).arg(curItem->text(2)));

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
	StatusBar->message(i18n("Better luck next time"));
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
	openDefaultAct->setEnabled(!isQuiz);
	saveAsAct->setEnabled(!isQuiz);

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

	StatusBar->clear();
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
	if(grade > 6) ++grade;
	return grade;
}

void Learn::setCurrentGrade(int grade)
{
	if(grade > 6) --grade;
	gradeAct->setCurrentItem(grade - 1);
}

void Learn::updateQuizConfiguration()
{
	KConfig *config = kapp->config();
	config->setGroup("Learn");

	quizOn = config->readNumEntry("Quiz On", 0);
	guessOn = config->readNumEntry("Guess On", 1);

	answers->setTitle(List->columnText(guessOn));

	View->updateFont();
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

	KRomajiEdit *comboBox = new KRomajiEdit(toolBar, "search edit");
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

void EditAction::insert(QString text)
{
	m_combo->insert(text);
}

void EditAction::setText(const QString &text)
{
	m_combo->setText(text);
}

QGuardedPtr<KLineEdit> EditAction::editor()
{
	return m_combo;
}

#include "widgets.moc"
