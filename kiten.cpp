#include <kmainwindow.h>
#include <klocale.h>
#include <qcheckbox.h>
#include <kconfig.h>
#include <qwidget.h>
#include <kapp.h>
#include <kstatusbar.h>
#include <kglobalaccel.h>
#include <kdebug.h>
#include <qtimer.h>
#include <qregexp.h>
#include <qlayout.h>
#include <kaction.h>
#include <kstdaction.h>

#include "kiten.h"
#include "optiondialog.h"
#include "widgets.h"
#include "dict.h"

TopLevel::TopLevel(QWidget *parent, const char *name) : KMainWindow(parent, name)
{
	QWidget *dummy = new QWidget(this);
	setCentralWidget(dummy);

	_SearchForm = new SearchForm(dummy, "SearchForm");
	_ResultView = new ResultView(dummy, "ResultView");

	QVBoxLayout *layout = new QVBoxLayout(dummy, 6);
	QHBoxLayout *checkLayout = new QHBoxLayout(layout, 6);

	kanjiCB = new QCheckBox(i18n("KanjiDic?"), dummy, "kanjiCB");
	comCB = new QCheckBox(i18n("Only common entries"), dummy, "comCB");
	checkLayout->addWidget(kanjiCB);
	checkLayout->addWidget(comCB);
	layout->addWidget(_SearchForm);
	layout->addWidget(_ResultView);

	(void) KStdAction::quit(this, SLOT(close()), actionCollection());
	(void) KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());

	connect(_SearchForm, SIGNAL(search()), SLOT(search()));
	connect(_SearchForm, SIGNAL(readingSearch()), SLOT(readingSearch()));
	connect(_SearchForm, SIGNAL(kanjiSearch()), SLOT(kanjiSearch()));

	createGUI();

	statusBar();
	optionDialog = 0;

	_Dict = new Dict();
	connect(comCB, SIGNAL(toggled(bool)), _Dict, SLOT(toggleCom(bool)));

	slotUpdateConfiguration();

	Accel = new KGlobalAccel();
	Accel->insertItem(i18n("Lookup Kanji"), "LookupKanji", "CTRL+SHIFT+K");
	Accel->insertItem(i18n("Lookup Reading"), "LookupReading", "CTRL+SHIFT+R");
	Accel->connectItem("LookupKanji", this, SLOT(kanjiSearch()));
	Accel->connectItem("LookupReading", this, SLOT(readingSearch()));
	Accel->readSettings();
}

void TopLevel::close()
{
	KConfig *config = kapp->config();
	config->setGroup("app");
	config->writeEntry("com", comCB->isChecked());
	config->writeEntry("kanji", kanjiCB->isChecked());

	kapp->quit();
}

void TopLevel::doSearch()
{
	//kdDebug() << "TopLevel::doSearch()\n";
	_ResultView->clear();

	statusBar()->message(i18n("Searching..."));

	if (!kanjiCB->isChecked())
	{
		if (noInit)
		{
			statusBar()->message(i18n("Error: Regular dictionaries aren't loaded completely"));
			return;
		}
	
		unsigned int fullNum;
		unsigned int num;
		QPtrList<Entry> results = _Dict->search(realregexp, regexp, num, fullNum);
		
		setResults(num, fullNum);
		
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
	
		unsigned int fullNum;
		unsigned int num;
		QPtrList<Kanji> results = _Dict->kanjiSearch(realregexp, regexp, num, fullNum);
		
		setResults(num, fullNum);
		
		QPtrListIterator<Kanji> it(results);
		Kanji *curKanji;
		while ((curKanji = it.current()) != 0)
		{
			++it;
			_ResultView->addKanjiResult(curKanji);
		}
	}
}

void TopLevel::search()
{
	realregexp = _SearchForm->searchItems();
	regexp = _SearchForm->lineText();
	doSearch();
}

void TopLevel::readingSearch()
{
	realregexp = _SearchForm->readingSearchItems(kanjiCB->isChecked());
	regexp = _SearchForm->lineText();
	doSearch();
}

void TopLevel::kanjiSearch()
{
	realregexp = _SearchForm->kanjiSearchItems();
	regexp = _SearchForm->lineText();
	doSearch();
}

void TopLevel::setResults(unsigned int results, unsigned int fullNum)
{
	QString str = i18n("%1 results").arg(results);
	
	if (results < fullNum)
		str += i18n(" out of %1").arg(fullNum);

	statusBar()->message(str);
}

void TopLevel::slotUpdateConfiguration()
{
	KConfig *config = kapp->config();

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
	_Dict->setDictList(DictList);
	_Dict->setDictNameList(DictNameList);

	config->setGroup("kanjidic");

	DictList.clear();
	DictNameList = config->readListEntry("__NAMES");

	for (it = DictNameList.begin(); it != DictNameList.end(); ++it)
		DictList.append(config->readEntry(*it));
	_Dict->setKanjiDictList(DictList);
	_Dict->setKanjiDictNameList(DictNameList);

	loadDict();
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
		connect(optionDialog, SIGNAL(valueChanged()), _SearchForm, SLOT(slotUpdateConfiguration()));
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

#include "kiten.moc"
