#include <kmainwindow.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <qcheckbox.h>
#include <qclipboard.h>
#include <qpushbutton.h>
#include <kconfig.h>
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

TopLevel::TopLevel(QWidget *parent, const char *name) : KMainWindow(parent, name)
{
	_ResultView = new ResultView(this, "_ResultView");
	_Dict = new Dict();
	setCentralWidget(_ResultView);

	/*
	QVBoxLayout *layout = new QVBoxLayout(dummy, 6);

	QHBoxLayout *botLayout = new QHBoxLayout(layout, 6);
	QHBoxLayout *topLayout = new QHBoxLayout(layout, 6);

	comCB = new QCheckBox(i18n("&Only common entries"), dummy, "comCB");
	topLayout->addWidget(comCB);

	irCB = new QCheckBox(i18n("Search &in results"), dummy, "irCB");
	topLayout->addWidget(irCB);

	kanjiCB = new QCheckBox(i18n("&Kanjidic?"), dummy, "kanjiCB");
	connect(kanjiCB, SIGNAL(toggled(bool)), SLOT(kanjiDictChange(bool)));
	botLayout->addWidget(kanjiCB);

	strokeButton = new QPushButton(i18n("&Stroke Search"), dummy, "strokeButton");
	botLayout->addWidget(strokeButton);
	connect(strokeButton, SIGNAL(clicked()), SLOT(strokeSearch()));
	gradeButton = new QPushButton(i18n("&Grade Search"), dummy, "gradeButton");
	botLayout->addWidget(gradeButton);
	connect(gradeButton, SIGNAL(clicked()), SLOT(gradeSearch()));

	layout->addWidget(_SearchForm);
	layout->addWidget(_ResultView);

	connect(_SearchForm, SIGNAL(search()), SLOT(search()));
	connect(_SearchForm, SIGNAL(readingSearch()), SLOT(readingSearch()));
	connect(_SearchForm, SIGNAL(kanjiSearch()), SLOT(kanjiSearch()));
	*/

	(void) KStdAction::quit(this, SLOT(close()), actionCollection());
	(void) KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	(void) new KAction(i18n("&Learn"), "pencil", CTRL+Key_L, this, SLOT(createLearn()), actionCollection(), "file_learn");
	(void)new KAction(i18n( "Clear location bar" ), BarIcon("locationbar_erase", 16), 0, Edit, SLOT(clear()), actionCollection(), "clear_search");

	Edit = new EditAction(i18n("Search Edit"), 0, this, SLOT(search()), actionCollection(), "search_edit");
	(void) new KAction(i18n("&Anywhere Search"), "find", 0, this, SLOT(search()), actionCollection(), "search_anywhere");
	(void) new KAction(i18n("&Reading Search"), "find", CTRL+Key_R, this, SLOT(readingSearch()), actionCollection(), "search_reading");
	(void) new KAction(i18n("&Kanji Search"), "find", CTRL+Key_K, this, SLOT(kanjiSearch()), actionCollection(), "search_kanji");
	kanjiCB = new KToggleAction(i18n("Kan&jidic?"), "kanjidic", CTRL+Key_J, this, SLOT(kanjiDictChange()), actionCollection(), "kanji_toggle");
	comCB = new KToggleAction(i18n("&Filter Out Rare"), "filter", CTRL+Key_F, this, SLOT(toggleCom()), actionCollection(), "common");
	connect(comCB, SIGNAL(toggled(bool)), _Dict, SLOT(toggleCom(bool)));
	irCB =  new KToggleAction(i18n("Search &in Results"), "viewmag+", CTRL+Key_I, this, SLOT(toggleIR()), actionCollection(), "in_results");
	connect(irCB, SIGNAL(toggled(bool)), _Dict, SLOT(toggleIR(bool)));

	createGUI();

	statusBar();
	optionDialog = 0;

	slotUpdateConfiguration();
	if (autoCreateLearn)
		createLearn();

	Accel = new KGlobalAccel();
	Accel->insertItem(i18n("Lookup kanji (Kanjidic)"), "LookupKanji", "CTRL+SHIFT+K");
	Accel->insertItem(i18n("Lookup Japanese word (Edict)"), "LookupJpWord", "CTRL+SHIFT+W");
	Accel->insertItem(i18n("Lookup English word (Edict)"), "LookupEngWord", "CTRL+SHIFT+E");
	Accel->insertItem(i18n("Lookup reading (Edict)"), "LookupReading", "CTRL+SHIFT+R");
	Accel->connectItem("LookupKanji", this, SLOT(kanjiSearchAccel()));
	Accel->connectItem("LookupJpWord", this, SLOT(jpWordAccel()));
	Accel->connectItem("LookupEngWord", this, SLOT(engWordAccel()));
	Accel->connectItem("LookupReading", this, SLOT(readingSearchAccel()));
	Accel->readSettings();

	isListMod = false;

	kanjiDictChange();
}

void TopLevel::close()
{
	kdDebug() << "TopLevel::close()\n";

	if (isListMod)
	{
		if (KMessageBox::warningContinueCancel(this, i18n("Unsaved changes to learning list. Are you sure you want to discard these?"), i18n("Unsaved changes"), i18n("Discard"), "DiscardAsk", true) != KMessageBox::Continue)
		{
			statusBar()->message(i18n("Go to the List tab in the Learn dialog to save your learning list"));
			return;
		}
	}

	KConfig *config = kapp->config();
	config->setGroup("app");
	config->writeEntry("com", comCB->isChecked());
	config->writeEntry("kanji", kanjiCB->isChecked());

	kapp->quit();
}

void TopLevel::doSearch()
{
	if (regexp.isEmpty())
	{
		statusBar()->message(i18n("Empty search items"));
		return;
	}
	//kdDebug() << "TopLevel::doSearch()\n";
	_ResultView->clear();

	statusBar()->message(i18n("Searching..."));
	unsigned int fullNum;
	unsigned int num;

	if (!kanjiCB->isChecked())
	{
		if (noInit)
		{
			statusBar()->message(i18n("Error: Regular dictionaries aren't loaded completely"));
			return;
		}
	
		QPtrList<Entry> results = _Dict->search(realregexp, regexp, num, fullNum);
		
		QPtrListIterator<Entry> it(results);
		Entry *curEntry;
		while ((curEntry = it.current()) != 0)
		{
			++it;
			_ResultView->addResult(curEntry, comCB->isChecked());
		}
	}
	else
	{
		if (noKanjiInit)
		{
			statusBar()->message(i18n("Error: Kanji dictionaries aren't loaded completely"));
			return;
		}
	
		QPtrList<Kanji> results = _Dict->kanjiSearch(realregexp, regexp, num, fullNum);

		if (num == 1) // if its only one entry, give compounds too!
		{
			Kanji *curKanji = results.getLast();
			_ResultView->addKanjiResult(curKanji);

			// now show some compounds in which this kanji appears
			
			QString kanji = curKanji->kanji();
			//kdDebug() << "kanji is " << kanji << endl;
		
			bool oldir = _Dict->isir();
			bool oldcom = _Dict->iscom();
			_Dict->toggleCom(true);
			_Dict->toggleIR(false);
			
			unsigned int _num, _fullNum;
			QPtrList<Entry> compounds = _Dict->search(QRegExp(kanji), kanji, _num, _fullNum);
		
			_ResultView->addHeader(i18n("%1 in common compunds").arg(kanji));
				
			QPtrListIterator<Entry> it(compounds);
			Entry *curEntry;
			while ((curEntry = it.current()) != 0)
			{
				//kdDebug() << "entry: " << curEntry->kanji() << endl;
				++it;
				_ResultView->addResult(curEntry, true);
			}
		
			_Dict->toggleCom(oldcom);
			_Dict->toggleIR(oldir);
		}
		else
		{
			QPtrListIterator<Kanji> it(results);
			Kanji *curKanji;
			while ((curKanji = it.current()) != 0)
			{
				++it;
				_ResultView->addKanjiResult(curKanji);
			}
		}
	}

	setResults(num, fullNum);
}

void TopLevel::search()
{
	realregexp = searchItems();
	regexp = Edit->text();
	doSearch();
}

void TopLevel::readingSearch()
{
	realregexp = readingSearchItems(kanjiCB->isChecked());
	regexp = Edit->text();
	doSearch();
}

void TopLevel::kanjiSearch()
{
	realregexp = kanjiSearchItems();
	regexp = Edit->text();
	doSearch();
}

void TopLevel::strokeSearch()
{
	unsigned int strokes = Edit->text().toUInt();
	if (strokes <= 0 || strokes > 60)
	{
		statusBar()->message(i18n("Invalid stroke sount"));
		return;
	}
	regexp = QString("S%1 ").arg(strokes);
	realregexp = QRegExp(regexp);

	// must be in kanjidic mode
	kanjiCB->setChecked(true);

	doSearch();
}

void TopLevel::gradeSearch()
{
	QString text = Edit->text();
	unsigned int grade;

	if (text == "Jouyou")
		grade = 8;
	else if (text == "Jinmeiyou")
		grade = 9;
	else
		grade = text.toUInt();

	if (grade <= 0 || grade > 9)
	{
		statusBar()->message(i18n("Invalid grade"));
		return;
	}
	regexp = QString("G%1 ").arg(grade);
	realregexp = QRegExp(regexp);

	kanjiCB->setChecked(true);

	doSearch();
}

QString TopLevel::clipBoardText() // gets text from clipboard for globalaccels
{
	kapp->clipboard()->setSelectionMode(true);
	QString text = kapp->clipboard()->text();
	kapp->clipboard()->setSelectionMode(false);

	return text;
}

void TopLevel::readingSearchAccel()
{
	kanjiCB->setChecked(false);

	realregexp = readingSearchItems(kanjiCB->isChecked());
	clipBoardText();
	regexp = Edit->text();
	doSearch();
}

void TopLevel::kanjiSearchAccel()
{
	kanjiCB->setChecked(true);

	realregexp = kanjiSearchItems();
	clipBoardText();
	regexp = Edit->text();
	doSearch();
}

void TopLevel::jpWordAccel()
{
	kanjiCB->setChecked(false);

	realregexp = kanjiSearchItems();
	clipBoardText();
	regexp = Edit->text();
	doSearch();
}

void TopLevel::engWordAccel()
{
	kanjiCB->setChecked(false);
	
	realregexp = searchItems();
	clipBoardText();
	regexp = Edit->text();
	doSearch();
}

void TopLevel::setResults(unsigned int results, unsigned int fullNum)
{
	QString str = i18n("%1 results").arg(results);
	
	if (results < fullNum)
		str += i18n(" out of %1").arg(fullNum);

	statusBar()->message(str);
	setCaption(kapp->makeStdCaption(str));
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
	_Dict->toggleCom(com);
	kanjiCB->setChecked(config->readBoolEntry("kanji", false));
	
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

	_Dict->setDictList(DictList);
	_Dict->setDictNameList(DictNameList);

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

	_Dict->setKanjiDictList(DictList);
	_Dict->setKanjiDictNameList(DictNameList);

	loadDict();

	config->setGroup("Learn");
	autoCreateLearn = config->readBoolEntry("startLearn", false);

	config->setGroup("Searching Options");
	wholeWord = config->readBoolEntry("wholeWord", true);
	caseSensitive = config->readBoolEntry("caseSensitive", false);
}

void TopLevel::loadDict()
{
	if (!_Dict->init(false))
		noInit = true;
	else
		noInit = false;

	if (!_Dict->init(true)) // kanji
		noKanjiInit = true;
	else
		noKanjiInit = false;
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
	Learn *_Learn = new Learn(_Dict, 0);
	
	// make all learns have current list
	connect(_Learn, SIGNAL(listChanged()), SLOT(globalListChanged()));
	connect(_Learn, SIGNAL(listDirty()), SLOT(globalListDirty()));
	connect(this, SIGNAL(updateLists()), _Learn, SLOT(readConfiguration()));

	_Learn->show();
}

void TopLevel::globalListChanged()
{
	isListMod = false;
	emit updateLists();
}

void TopLevel::kanjiDictChange()
{
	//bool on = kanjiCB->isChecked();
	//strokeButton->setEnabled(on);
	//gradeButton->setEnabled(on);
}

void TopLevel::globalListDirty()
{
	isListMod = false;
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

QRegExp TopLevel::kanjiSearchItems()
{
	QString text = Edit->text();
	if (text.isEmpty())
	{
		return QRegExp(); //empty
	}

	//CompletionObj->addItem(text);

	QString regexp = "^%1\\W";
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

	//CompletionObj->addItem(text);

	unsigned int contains = text.contains(QRegExp("[A-Za-z0-9_:]"));
	if (contains == text.length())
		regexp = "\\W%1\\W";
	else
		regexp = "%1";

	regexp = regexp.arg(text);
	
	//kdDebug() << "SearchForm::searchItems returning " << regexp << endl;
	return QRegExp(regexp, caseSensitive);
}

void TopLevel::toggleCom()
{
	//_Dict->setCom(comCB->isChecked());
}

void TopLevel::toggleIR()
{
	//_Dict->setIR(irCB->isChecked());
}
#include "kiten.moc"
