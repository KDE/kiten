#include <kmainwindow.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qpushbutton.h>
#include <kconfig.h>
#include <kedittoolbar.h>
#include <qtextcodec.h>
#include <kaccel.h>
#include <qwidget.h>
#include <kmessagebox.h>
#include <kapp.h>
#include <kstatusbar.h>
#include <kstandarddirs.h>
#include <kglobalaccel.h>
#include <kdebug.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qlayout.h>
#include <kaction.h>
#include <kstatusbar.h>
#include <kstdaction.h>

#include "kiten.h"
#include "optiondialog.h"
#include "widgets.h"
#include "dict.h"
#include "rad.h"
#include "deinf.h"

TopLevel::TopLevel(QWidget *parent, const char *name) : KMainWindow(parent, name)
{
	_ResultView = new ResultView(true, this, "_ResultView");
	setCentralWidget(_ResultView);

	(void) KStdAction::quit(this, SLOT(close()), actionCollection());
	(void) KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	(void) new KAction(i18n("&Learn"), "pencil", CTRL+Key_L, this, SLOT(createLearn()), actionCollection(), "file_learn");
	(void) new KAction(i18n("Ra&dical Search"), "gear", CTRL+Key_R, this, SLOT(radicalSearch()), actionCollection(), "search_radical");
	Edit = new EditAction(i18n("Search Edit"), 0, this, SLOT(search()), actionCollection(), "search_edit");
	(void) new KAction(i18n("Clear"), BarIcon("locationbar_erase", 16), 0, Edit, SLOT(clear()), actionCollection(), "clear_search");
	(void) new KAction(i18n("&Search"), "key_enter", 0, this, SLOT(search()), actionCollection(), "search");
	(void) new KAction(i18n("&Search with beginning of word"), 0, this, SLOT(searchBeginning()), actionCollection(), "search_beginning");
	(void) new KAction(i18n("&Search with end of word"), 0, this, SLOT(searchEnd()), actionCollection(), "search_end");
	(void) new KAction(i18n("&Strokes"), "paintbrush", CTRL+Key_S, this, SLOT(strokeSearch()), actionCollection(), "search_stroke");
	(void) new KAction(i18n("&Grade"), "leftjust", CTRL+Key_G, this, SLOT(gradeSearch()), actionCollection(), "search_grade");
	kanjiCB = new KToggleAction(i18n("&Kanjidic"), "kanjidic", CTRL+Key_K, this, SLOT(kanjiDictChange()), actionCollection(), "kanji_toggle");
	deinfCB = new KToggleAction(i18n("&Deinflect verbs"), 0, this, SLOT(kanjiDictChange()), actionCollection(), "deinf_toggle");
	comCB = new KToggleAction(i18n("&Filter Rare"), "filter", CTRL+Key_F, this, SLOT(toggleCom()), actionCollection(), "common");
	irAction =  new KAction(i18n("Search &in Results"), "find", CTRL+Key_I, this, SLOT(resultSearch()), actionCollection(), "search_in_results");
	(void) KStdAction::configureToolbars(this, SLOT(configureToolBars()), actionCollection());
	addAction = new KAction(i18n("Add Kanji to learning list"), 0, this, SLOT(addToList()), actionCollection(), "add");
	addAction->setEnabled(false);

	createGUI();

	statusBar();
	optionDialog = 0;

	slotUpdateConfiguration();
	if (autoCreateLearn)
		createLearn();

	/* TODO new Accel
	Accel = new KGlobalAccel();
	Accel->insertItem(i18n("Lookup Kanji (Kanjidic)"), "LookupKanji", "CTRL+SHIFT+K");
	Accel->insertItem(i18n("Lookup English/Japanese word"), "LookupWord", "CTRL+SHIFT+A");
	Accel->connectItem("LookupKanji", this, SLOT(kanjiSearchAccel()));
	Accel->connectItem("LookupWord", this, SLOT(searchAccel()));
	Accel->readSettings();
	*/
	isListMod = false;

	resize(600, 400);
	applyMainWindowSettings(KGlobal::config(), "TopLevelWindow");

	name = QString::null;

	connect(_ResultView, SIGNAL(linkClicked(const QString &)), SLOT(ressearch(const QString &)));
}

void TopLevel::closeEvent(QCloseEvent *)
{
	if (isListMod)
	{
		int result = KMessageBox::warningYesNoCancel(this, i18n("There are unsaved changes to learning list. Save them?"), i18n("Unsaved changes"), i18n("Save"), i18n("Discard"), "DiscardAsk", true);
		switch(result)
		{
		case KMessageBox::Yes:
			emit saveLists();
			// fallthrough
		case KMessageBox::No:
			break;
		case KMessageBox::Cancel:
			return;
		}
	}

	KConfig *config = kapp->config();
	config->setGroup("app");
	config->writeEntry("com", comCB->isChecked());
	config->writeEntry("kanji", kanjiCB->isChecked());
	config->writeEntry("deinf", deinfCB->isChecked());

	saveMainWindowSettings(KGlobal::config(), "TopLevelWindow");

	kapp->quit();
}

void TopLevel::addToList()
{
	// TODO! make this write to the kconfig file, and emit listDirty
	emit add(toAddKanji);
}

void TopLevel::doSearch(QString text, QRegExp regexp, bool inResults)
{
	if (text.isEmpty())
	{
		statusBar()->message(i18n("Empty search items"));
		return;
	}

	addAction->setEnabled(false);

	_ResultView->clear();

	statusBar()->message(i18n("Searching..."));
	unsigned int fullNum;
	unsigned int num;

	if (!kanjiCB->isChecked())
	{
		Dict::SearchResult results = _Index.search(regexp, text, num, fullNum, comCB->isChecked());

		// do again... bad because sometimes reading is kanji
		if ((readingSearch || beginningReadingSearch) && (num < 1))
		{
			num = 0;
			fullNum = 0;

			if (beginningReadingSearch)
				regexp = kanjiSearchItems(true);
			else if (readingSearch)
				regexp = kanjiSearchItems();

			results = _Index.search(regexp, text, num, fullNum, comCB->isChecked());
		}
		
		for(QValueListIterator<Dict::Entry> it = results.list.begin(); it != results.list.end(); ++it)
			_ResultView->addResult(*it, comCB->isChecked());
	}
	else
	{
		Dict::KanjiSearchResult results = _Index.searchKanji(regexp, text, num, fullNum, comCB->isChecked());

		if (num == 1) // if its only one entry, give compounds too!
		{
			toAddKanji = firstKanji(results);
			_ResultView->addKanjiResult(toAddKanji);

			addAction->setEnabled(true);

			// now show some compounds in which this kanji appears
			QString kanji = toAddKanji.kanji();
		
			unsigned int _num, _fullNum;
			Dict::SearchResult compounds = _Index.search(QRegExp(kanji), kanji, _num, _fullNum, true);
		
			_ResultView->addHeader(i18n("%1 in common compunds").arg(kanji));

			for(QValueListIterator<Dict::Entry> it = compounds.list.begin(); it != compounds.list.end(); ++it)
			{
				//kdDebug() << "adding " << (*it).kanji() << endl;
				_ResultView->addResult(*it, true);
			}
		}
		else
		{
			for(QValueListIterator<Dict::Kanji> it = results.list.begin(); it != results.list.end(); ++it)
				_ResultView->addKanjiResult(*it);
		}
	}

	readingSearch = false;
	setResults(num, fullNum);
}

void TopLevel::searchBeginning()
{
	QString text = Edit->text();
	QRegExp regexp;

	QTextCodec *codec = QTextCodec::codecForName("eucJP");
	QCString csch_str = codec->fromUnicode(text);
	unsigned char first = csch_str[0];

	if (first <= 128)
		regexp = QRegExp(QString("\\W").append(text));
	else if (first < 0xa5)
	{
		if (kanjiCB->isChecked())
		{
			regexp = QRegExp(QString("\\W").append(text));
		}
		else
		{
			beginningReadingSearch = true;
			regexp = QRegExp(QString("\\[").append(text));
		}
	}
	else if (first > 0xa8)
		regexp = QRegExp(QString("^").append(text));

	doSearch(text, regexp);
}

void TopLevel::searchEnd()
{
	doSearch(Edit->text(), QRegExp(QString("\\W").prepend(Edit->text())));
}

void TopLevel::resultSearch()
{
	search(true);
}

void TopLevel::search()
{
	search(false);
}

void TopLevel::ressearch(const QString &text)
{
	Edit->clear();
	Edit->insert(text);
	kanjiCB->setChecked(true);
	search(false);
}

void TopLevel::search(bool inResults)
{
	QString text = Edit->text();
	QRegExp regexp;

	QTextCodec *codec = QTextCodec::codecForName("eucJP");
	QCString csch_str = codec->fromUnicode(text);
	unsigned char first = csch_str[0];
	unsigned char last = csch_str[csch_str.length()];

	// gjiten seems to do this stuff to tell between the three...
	// TODO: factor out this gjiten heuristic
	if (first <= 128)
	{
		regexp = searchItems();
	}
	else if (first < 0xa5)
	{
		regexp = readingSearchItems(kanjiCB->isChecked());
		readingSearch = true;
	}
	else if (first > 0xa8)
	{
		if (last < 0xa5 && deinfCB->isChecked() && name == QString::null) // deinflect
		{
			dicform = _DeinfIndex.deinflect(text, name);
			if (name == QString::null)
				goto Normal;

			statusBar()->message(name);

			Edit->clear();
			Edit->insert(dicform);
			return;
		}

		Normal:
		regexp = kanjiSearchItems();
	}

	dicform = QString::null;

	doSearch(text, regexp, inResults);
}

void TopLevel::strokeSearch()
{
	unsigned int strokes = Edit->text().toUInt();
	if (strokes <= 0 || strokes > 60)
	{
		statusBar()->message(i18n("Invalid stroke count"));
		return;
	}
	QString text = QString("S%1 ").arg(strokes);
	QRegExp regexp = QRegExp(text);

	// must be in kanjidic mode
	kanjiCB->setChecked(true);

	doSearch(text, regexp);
}

void TopLevel::gradeSearch()
{
	QString editText = Edit->text();
	unsigned int grade;

	if (editText == "Jouyou" || editText == "jouyou")
		grade = 8;
	else if (editText == "Jinmeiyou" || editText == "jinmeiyou")
		grade = 9;
	else
		grade = editText.toUInt();

	if (grade <= 0 || grade > 9)
	{
		statusBar()->message(i18n("Invalid grade"));
		return;
	}
	QString text = QString("G%1 ").arg(grade);
	QRegExp regexp = QRegExp(text);

	kanjiCB->setChecked(true);

	doSearch(text, regexp);
}

QString TopLevel::clipBoardText() // gets text from clipboard for globalaccels
{
	kapp->clipboard()->setSelectionMode(true);
	QString text = kapp->clipboard()->text();
	kapp->clipboard()->setSelectionMode(false);

	return text;
}

void TopLevel::searchAccel()
{
	kanjiCB->setChecked(false);

	clipBoardText();
	search();
}

void TopLevel::kanjiSearchAccel()
{
	kanjiCB->setChecked(true);

	clipBoardText();
	search();
}

void TopLevel::setResults(unsigned int results, unsigned int fullNum)
{
	QString str = i18n("%1 results").arg(results);
	
	if (results < fullNum)
		str += i18n(" out of %1").arg(fullNum);

	name = QString::null;

	statusBar()->message(str);
	setCaption(str);

	_ResultView->updateContents();
}

void TopLevel::slotUpdateConfiguration()
{
	KConfig *config = kapp->config();
	KStandardDirs *dirs = KGlobal::dirs();
	QString globaledict = dirs->findResource("appdata", "edict");
	QString globalkanjidic = dirs->findResource("appdata", "kanjidic");

	config->setGroup("app");
	bool com = config->readBoolEntry("com", false);
	comCB->setChecked(com);
	kanjiCB->setChecked(config->readBoolEntry("kanji", false));
	deinfCB->setChecked(config->readBoolEntry("deinf", true));
	
	config->setGroup("edict");

	QStringList DictNameList = config->readListEntry("__NAMES");
	QStringList DictList;

	QStringList::Iterator it;

	for (it = DictNameList.begin(); it != DictNameList.end(); ++it)
		DictList.append(config->readEntry(*it));
	
	if (globaledict != QString::null)
	{
		DictList.prepend(globaledict);
		DictNameList.prepend("Edict");
	}

	_Index.setDictList(DictList, DictNameList);

	config->setGroup("kanjidic");

	DictList.clear();
	DictNameList = config->readListEntry("__NAMES");

	for (it = DictNameList.begin(); it != DictNameList.end(); ++it)
		DictList.append(config->readEntry(*it));

	if (globalkanjidic != QString::null)
	{
		DictList.prepend(globalkanjidic);
		DictNameList.prepend("Kanjidic");
	}

	_Index.setKanjiDictList(DictList, DictNameList);

	config->setGroup("Learn");
	autoCreateLearn = config->readBoolEntry("startLearn", false);

	config->setGroup("Searching Options");
	wholeWord = config->readBoolEntry("wholeWord", true);
	caseSensitive = config->readBoolEntry("caseSensitive", false);
}

void TopLevel::slotConfigure()
{
	if (optionDialog == 0)
	{
		optionDialog = new ConfigureDialog(Accel, 0);
		if (optionDialog == 0) 
			return;
		connect(optionDialog, SIGNAL(hidden()),this,SLOT(slotConfigureHide()));
		connect(optionDialog, SIGNAL(valueChanged()), this, SLOT(slotUpdateConfiguration()));
	}

	optionDialog->show();
}

void TopLevel::slotConfigureHide()
{
	QTimer::singleShot(0, this, SLOT(slotConfigureDestroy()));
}

void TopLevel::slotConfigureDestroy()
{
	if (optionDialog != 0 && optionDialog->isVisible() == 0)
	{
		delete optionDialog;
		optionDialog = 0;
	}
}

void TopLevel::createLearn()
{
	Learn *_Learn = new Learn(&_Index, 0);
	
	// make all learns have current list
	connect(_Learn, SIGNAL(listChanged()), SLOT(globalListChanged()));
	connect(_Learn, SIGNAL(listDirty()), SLOT(globalListDirty()));
	connect(this, SIGNAL(updateLists()), _Learn, SLOT(readConfiguration()));
	connect(this, SIGNAL(saveLists()), _Learn, SLOT(writeConfiguration()));
	connect(this, SIGNAL(add(Dict::Kanji)), _Learn, SLOT(externAdd(Dict::Kanji)));

	_Learn->show();
}

void TopLevel::globalListChanged()
{
	isListMod = false;
	emit updateLists();
}

void TopLevel::kanjiDictChange()
{
	// Do we even *need* something here?
}

void TopLevel::globalListDirty()
{
	isListMod = true;
}

QRegExp TopLevel::readingSearchItems(bool kanji)
{
	QString text = Edit->text();
	if (text.isEmpty()) // abandon search
	{
		return QRegExp(); //empty
	}

	//CompletionObj->addItem(text);
	QString regexp;
	if (kanji)
		regexp = " %1 ";
	else
		regexp = "\\[%1\\]";

	regexp = regexp.arg(text);
	
	return QRegExp(regexp, caseSensitive);
}

QRegExp TopLevel::kanjiSearchItems(bool beginning)
{
	QString text = Edit->text();

	if (text.isEmpty())
	{
		return QRegExp(); //empty
	}

	QString regexp;
	if (beginning)
		regexp = "^%1";
	else
		regexp = "^%1\\W";

	regexp = regexp.arg(text);

	return QRegExp(regexp, caseSensitive);
}

QRegExp TopLevel::searchItems()
{
	QString regexp;
	QString text = Edit->text();
	if (text.isEmpty())
	{
		return QRegExp(); //empty
	}

	unsigned int contains = text.contains(QRegExp("[A-Za-z0-9_:]"));
	if (contains == text.length())
		regexp = "\\W%1\\W";
	else
		regexp = "%1";

	regexp = regexp.arg(text);
	
	return QRegExp(regexp, caseSensitive);
}

void TopLevel::toggleCom()
{
}

void TopLevel::configureToolBars()
{
	saveMainWindowSettings(KGlobal::config(), "TopLevelWindow");
	KEditToolbar dlg(actionCollection(), "kitenui.rc");
	connect(&dlg, SIGNAL(newToolbarConfig()), SLOT(newToolBarConfig()));
	if (dlg.exec())
	{
		createGUI();
	}
}

void TopLevel::newToolBarConfig()
{
	applyMainWindowSettings(KGlobal::config(), "TopLevelWindow");
}

void TopLevel::radicalSearch()
{
	RadWidget *rw = new RadWidget(&_Rad, 0, "rw");
	connect(rw, SIGNAL(set(QString &, unsigned int)), this, SLOT(radSearch(QString &, unsigned int)));
	rw->show();
}

void TopLevel::radSearch(QString &text, unsigned int strokes)
{
	_ResultView->clear();

	QStringList list = _Rad.kanjiByRad(text);

	unsigned int fullNum, num;
	unsigned int realnum = 0;
	QStringList::iterator it;

	if(strokes)
		_ResultView->addHeader(i18n("Kanji with radical %1 and %2 strokes").arg(text).arg(strokes));
	else
		_ResultView->addHeader(i18n("Kanji with radical %1").arg(text));

	Dict::Kanji kanji;

	for (it = list.begin(); it != list.end(); ++it)
	{
		Dict::KanjiSearchResult results = _Index.searchKanji(QRegExp(QString(*it).prepend("^")), (*it), num, fullNum, comCB->isChecked());

		if(results.list.count() < 1)
			continue;

		kanji = firstKanji(results);
		if (kanji.strokes() == strokes || !strokes)
		{
			_ResultView->addKanjiResult(kanji);
			realnum++;
		}
	}

	setResults(realnum, realnum);
}

Dict::Kanji TopLevel::firstKanji(Dict::KanjiSearchResult result)
{
	for(QValueListIterator<Dict::Kanji> it = result.list.begin(); it != result.list.end(); ++it)
	{
		if ((*it).dictName() == "__NOTSET")
			return (*it);
	}

	return Dict::Kanji("Nothing");
}

#include "kiten.moc"
