/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
	       (C) 2005 Paul Temple <paul.temple@gmx.net>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kconfigdialog.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <kedittoolbar.h>
#include <kglobalaccel.h>
#include <kiconloader.h>
#include <kkeydialog.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>

#include <qclipboard.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <qtimer.h>

#include "kiten.h"
#include "learn.h"
#include "kitenconfig.h"
#include "optiondialog.h"

#include <cassert>

TopLevel::TopLevel(QWidget *parent, const char *name) : KMainWindow(parent, name)
{
#if KDE_VERSION > 305
	    setStandardToolBarMenuEnabled(true);
#endif

	config = Config::self();
	config->readConfig();
	Accel = new KGlobalAccel(this);
	(void) Accel->insert("Lookup Kanji (Kanjidic)", i18n("Lookup Kanji (Kanjidic)"), i18n("Gives detailed information about Kanji currently on clipboard."), CTRL + ALT + Key_K, CTRL + ALT + Key_K, this, SLOT(kanjiSearchAccel()));
	(void) Accel->insert("Lookup English/Japanese word", i18n("Lookup English/Japanese Word"), i18n("Looks up current text on clipboard in the same way as if you used Kiten's regular search."), CTRL + ALT + Key_S, CTRL + ALT + Key_S, this, SLOT(searchAccel()));
	Accel->readSettings(KGlobal::config());
	Accel->updateConnections();

	_ResultView = new ResultView(true, this, "_ResultView");
	setCentralWidget(_ResultView);

	(void) KStdAction::quit(this, SLOT(close()), actionCollection());
	(void) KStdAction::print(this, SLOT(print()), actionCollection());
	(void) KStdAction::preferences(this, SLOT(slotConfigure()), actionCollection());
	KStdAction::keyBindings(guiFactory(), SLOT(configureShortcuts()), actionCollection());
	(void) new KAction(i18n("&Learn"), "pencil", CTRL+Key_L, this, SLOT(createLearn()), actionCollection(), "file_learn");
	(void) new KAction(i18n("&Dictionary Editor..."), "edit", 0, this, SLOT(createEEdit()), actionCollection(), "dict_editor");
	(void) new KAction(i18n("Ra&dical Search..."), "gear", CTRL+Key_R, this, SLOT(radicalSearch()), actionCollection(), "search_radical");
	Edit = new EditAction(i18n("Search Edit"), 0, this, SLOT(search()), actionCollection(), "search_edit");
	(void) new KAction(i18n("&Clear Search Bar"), BarIcon("locationbar_erase", 16), CTRL+Key_N, Edit, SLOT(clear()), actionCollection(), "clear_search");
	(void) new KAction(i18n("S&earch"), "key_enter", 0, this, SLOT(search()), actionCollection(), "search");
	(void) new KAction(i18n("Search with &Beginning of Word"), 0, this, SLOT(searchBeginning()), actionCollection(), "search_beginning");
	(void) new KAction(i18n("Search &Anywhere"), 0, this, SLOT(searchAnywhere()), actionCollection(), "search_anywhere");
	(void) new KAction(i18n("Stro&kes"), "paintbrush", CTRL+Key_S, this, SLOT(strokeSearch()), actionCollection(), "search_stroke");
	(void) new KAction(i18n("&Grade"), "leftjust", CTRL+Key_G, this, SLOT(gradeSearch()), actionCollection(), "search_grade");
	kanjiCB = new KToggleAction(i18n("&Kanjidic"), "kanjidic", CTRL+Key_K, this, SLOT(kanjiDictChange()), actionCollection(), "kanji_toggle");
	deinfCB = new KToggleAction(i18n("&Deinflect Verbs in Regular Search"), 0, this, SLOT(kanjiDictChange()), actionCollection(), "deinf_toggle");
	comCB = new KToggleAction(i18n("&Filter Rare"), "filter", CTRL+Key_F, this, SLOT(toggleCom()), actionCollection(), "common");
	autoSearchToggle = new KToggleAction(i18n("&Automatically Search Clipboard Selections"), "find", 0, this, SLOT(kanjiDictChange()), actionCollection(), "autosearch_toggle");
	irAction =  new KAction(i18n("Search &in Results"), "find", CTRL+Key_I, this, SLOT(resultSearch()), actionCollection(), "search_in_results");
	(void) KStdAction::configureToolbars(this, SLOT(configureToolBars()), actionCollection());
	addAction = new KAction(i18n("Add &Kanji to Learning List"), 0, this, SLOT(addToList()), actionCollection(), "add");
	addAction->setEnabled(false);
	(void) new KAction(i18n("Configure &Global Shortcuts..."), "configure_shortcuts", 0, this, SLOT(configureGlobalKeys()), actionCollection(), "options_configure_keybinding");

	historyAction = new KListAction(i18n("&History"), 0, 0, 0, actionCollection(), "history");
	connect(historyAction, SIGNAL(activated(int)), this, SLOT(goInHistory(int)));
	backAction = KStdAction::back(this, SLOT(back()), actionCollection());
	forwardAction = KStdAction::forward(this, SLOT(forward()), actionCollection());
	backAction->setEnabled(false);
	forwardAction->setEnabled(false);
	currentResult = resultHistory.end();
	currentResultIndex = 0;

	createGUI();

	StatusBar = statusBar();
	optionDialog = 0;

	comCB->setChecked(config->com());
	kanjiCB->setChecked(config->kanji());
	autoSearchToggle->setChecked(config->autosearch());
	deinfCB->setChecked(config->deinf());

	updateConfiguration();

	if (config->startLearn())
		createLearn();

	resize(600, 400);
	applyMainWindowSettings(KGlobal::config(), "TopLevelWindow");

	connect(_ResultView, SIGNAL(linkClicked(const QString &)), SLOT(ressearch(const QString &)));
	connect(kapp->clipboard(), SIGNAL(selectionChanged()), this, SLOT(autoSearch()));

	QTimer::singleShot(10, this, SLOT(finishInit()));
}

TopLevel::~TopLevel()
{
    delete optionDialog;
    optionDialog = 0;
}

void TopLevel::finishInit()
{
	// if it's the application's first time starting, 
	// the app group won't exist and we show demo
	if (!kapp->config()->hasGroup("app"))
	{
		if (kanjiCB->isChecked())
		  	Edit->setText(QString::fromUtf8("辞"));
		else
		  	Edit->setText(QString::fromUtf8("辞書"));

		search();
	}

	Edit->clear(); // make sure the edit is focused initially
	StatusBar->message(i18n("Welcome to Kiten"));
	setCaption(QString::null);
}

bool TopLevel::queryClose()
{
	for (QPtrListIterator<Learn> i(learnList); *i;)
	{
		(*i)->show();
		if (!(*i)->closeWindow())
			return false; // cancel
		Learn *old = *i;
		++i;
		learnList.remove(old);
	}

	config->setCom(comCB->isChecked());
	config->setKanji(kanjiCB->isChecked());
	config->setAutosearch(autoSearchToggle->isChecked());
	config->setDeinf(deinfCB->isChecked());
	config->writeConfig();

	saveMainWindowSettings(KGlobal::config(), "TopLevelWindow");
	return true;
}

void TopLevel::addToList()
{
	if (learnList.isEmpty())
		createLearn();
	else
		StatusBar->message(i18n("%1 added to learn list of all open learn windows").arg(toAddKanji.kanji()));

	emit add(toAddKanji);
}

void TopLevel::doSearch(const QString &text, QRegExp regexp)
{
	if (text.isEmpty())
	{
		StatusBar->message(i18n("Empty search items"));
		return;
	}

	StatusBar->message(i18n("Searching..."));

	Dict::SearchResult results;
	if (kanjiCB->isChecked())
	{
		results = _Asyndeta.retrieveIndex()->searchKanji(regexp, text, comCB->isChecked());
	}
	else
	{
		results = _Asyndeta.retrieveIndex()->search(regexp, text, comCB->isChecked());

		// do again... bad because sometimes reading is kanji
		if ((readingSearch || beginningReadingSearch) && (results.count < 1))
		{
			//kdDebug() << "doing again\n";

			if (beginningReadingSearch)
				regexp = kanjiSearchItems(true);
			else if (readingSearch)
				regexp = kanjiSearchItems();

			results = _Asyndeta.retrieveIndex()->search(regexp, text, comCB->isChecked());
		}
	}

	addHistory(results);
	handleSearchResult(results);
	readingSearch = false;
}

void TopLevel::doSearchInResults(const QString &text, QRegExp regexp)
{
	if (text.isEmpty())
	{
		StatusBar->message(i18n("Empty search items"));
		return;
	}

	StatusBar->message(i18n("Searching..."));
	Dict::SearchResult results = _Asyndeta.retrieveIndex()->searchPrevious(regexp, text, *currentResult, comCB->isChecked());
	addHistory(results);
	handleSearchResult(results);
	readingSearch = false;
}

void TopLevel::handleSearchResult(Dict::SearchResult results)
{
	Edit->setText(results.text);
	setResults(results.count, results.outOf);

	addAction->setEnabled(false);
	_ResultView->clear();

	Dict::Entry first = Dict::firstEntry(results);

	if (results.count > 0)
		kanjiCB->setChecked(first.extendedKanjiInfo());
	else
	{
		_ResultView->flush();
		return;
	}

	if (first.extendedKanjiInfo())
	{
		if (results.count == 1) // if its only one entry, give compounds too!
		{
			toAddKanji = first;
			_ResultView->addKanjiResult(toAddKanji, results.common, _Rad.radByKanji(toAddKanji.kanji()));

			addAction->setEnabled(true);

			_ResultView->append(QString("<p>%1</p>").arg(i18n("HTML Entity: %1").arg(QString("&amp;#x%1;").arg(QString::number(toAddKanji.kanji().at(0).unicode(), 16))))); // show html entity

			// now show some compounds in which this kanji appears
			QString kanji = toAddKanji.kanji();

			_ResultView->addHeader(i18n("%1 in compounds").arg(kanji));

			Dict::SearchResult compounds = _Asyndeta.retrieveIndex()->search(QRegExp(kanji), kanji, true);
			bool common = true;
			if (compounds.count <= 0)
			{
				compounds = _Asyndeta.retrieveIndex()->search(QRegExp(kanji), kanji, false);
				_ResultView->addHeader(i18n("(No common compounds)"), 4);
				common = false;
			}

			for (QValueListIterator<Dict::Entry> it = compounds.list.begin(); it != compounds.list.end(); ++it)
			{
				//kdDebug() << "adding " << (*it).kanji() << endl;
				_ResultView->addResult(*it, common);
				kapp->processEvents();
			}
		}
		else
		{
			for (QValueListIterator<Dict::Entry> it = results.list.begin(); it != results.list.end(); ++it)
			{
				kapp->processEvents();
				_ResultView->addKanjiResult(*it, results.common);
			}
		}
	}
	else
	{
		for (QValueListIterator<Dict::Entry> it = results.list.begin(); it != results.list.end(); ++it)
		{
			kapp->processEvents();
			_ResultView->addResult(*it, comCB->isChecked());
		}
	}

	_ResultView->flush();
}

void TopLevel::searchBeginning()
{
	QString text = Edit->text();
	QRegExp regexp;

	switch (Dict::textType(text))
	{
	case Dict::Text_Latin:
		regexp = QRegExp(QString("\\W").append(text));
		break;

	case Dict::Text_Kana:
		if (kanjiCB->isChecked())
		{
			regexp = QRegExp(QString("\\W").append(text));
		}
		else
		{
			beginningReadingSearch = true;
			regexp = QRegExp(QString("\\[").append(text));
		}
		break;

	case Dict::Text_Kanji:
		regexp = QRegExp(QString("^").append(text));
	}

	doSearch(text, regexp);
}

void TopLevel::searchAnywhere()
{
	doSearch(Edit->text(), QRegExp(Edit->text()));
}

void TopLevel::resultSearch()
{
	search(true);
}

// called when a kanji is clicked on in result view
void TopLevel::ressearch(const QString &text)
{
	//kdDebug() << "ressearch(" << text << endl;
	if (text.startsWith("__radical:"))
	{
		QString radical = text.section(":", 1, 1).right(1);
		//kdDebug() << "radical is " << radical << endl;
		radicalSearch()->addRadical(radical);
		return;
	}
	Edit->setText(text);
	kanjiCB->setChecked(true);
	search();
}

void TopLevel::search(bool inResults)
{
	QString text = Edit->text();
	QRegExp regexp;

	switch (Dict::textType(text))
	{
	case Dict::Text_Latin:
		regexp = searchItems();
		break;

	case Dict::Text_Kana:
		regexp = readingSearchItems(kanjiCB->isChecked());
		readingSearch = true;
		break;

	case Dict::Text_Kanji:
		if (deinfCB->isChecked()) // deinflect
		{
			bool common = comCB->isChecked();
			QStringList names;
			QStringList res(_DeinfIndex.deinflect(text, names));

			if (res.size() > 0)
			{
				Dict::SearchResult hist;
				hist.count = 0;
				hist.outOf = 0;
				hist.common = common;
				hist.text = text;

				QStringList done;

				res.prepend(text);
				names.prepend(i18n("No deinflection"));

				QStringList::Iterator nit = names.begin();
				for (QStringList::Iterator it = res.begin(); it != res.end(); ++it, ++nit)
				{
					if (done.contains(*it) > 0)
						continue;

					//kdDebug() << "currently on deinflection " << *it << endl;
					Dict::SearchResult results = _Asyndeta.retrieveIndex()->search(QRegExp(QString("^") + (*it) + "\\W"), *it, common);

					if (results.count < 1) // stop if it isn't in the dictionary
						continue;

					hist.list.append(Dict::Entry(*nit, true));

					hist.list += results.list;
					hist.results += results.results;

					hist.count += results.count;
					hist.outOf += results.outOf;

					done.append(*it);
				}

				handleSearchResult(hist);
				addHistory(hist);
				return;
			}
		}

		regexp = kanjiSearchItems();
	}

	if (inResults)
		doSearchInResults(text, regexp);
	else
		doSearch(text, regexp);
}

void TopLevel::strokeSearch()
{
	QString strokesString;

	bool ok = false;
	QString text = Edit->text().stripWhiteSpace();
	unsigned int strokes = text.toUInt(&ok);

	if (!ok)
	{
		/*
		if (text.find("-") < 0)
		{
			StatusBar->message(i18n("For a range between 4 and 8 strokes, use '4-8'"));
			return;
		}

		unsigned int first = text.section('-', 0, 0).toUInt(&ok);
		if (!ok)
		{
			StatusBar->message(i18n("First number not parseable\n"));
			return;
		}

		unsigned int second = text.section('-', 1, 1).toUInt(&ok);
		if (!ok)
		{
			StatusBar->message(i18n("Second number not parseable\n"));
			return;
		}

		bool already = false;
		for (int i = first; i <= second; ++i)
		{
			if (already)
				strokesString.append('|');

			already = true;

			strokesString.append(QString::number(i));
		}

		strokesString.append(')');
		strokesString.prepend("(?:");

		//kdDebug() << "strokesString is " << strokesString << endl;
		*/

		StatusBar->message(i18n("Unparseable number"));
		return;
	}
	else if (strokes <= 0 || strokes > 60)
	{
		StatusBar->message(i18n("Invalid stroke count"));
		return;
	}
	else
	{
		strokesString = QString::number(strokes);
	}


	QRegExp regexp = QRegExp(text);

	// must be in kanjidic mode
	kanjiCB->setChecked(true);

	doSearch(QString("S%1 ").arg(strokesString), regexp);
}

void TopLevel::gradeSearch()
{
	QString editText = Edit->text().stripWhiteSpace();
	unsigned int grade;

	if (editText.lower() == "jouyou")
		grade = 8;
	else if (editText.lower() == "jinmeiyou")
		grade = 9;
	else
		grade = editText.toUInt();

	if (grade <= 0 || grade > 9)
	{
		StatusBar->message(i18n("Invalid grade"));
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
	QString text = kapp->clipboard()->text().stripWhiteSpace();
	kapp->clipboard()->setSelectionMode(false);

	return text;
}

QString TopLevel::filteredClipboardText()
{
	QString newText = clipBoardText();
	QString currentText = Edit->text();
	if (newText.length() < 80 && newText.find(':') < 0 && newText.find('#') < 0 && newText.find("-") != 0 && newText.find("+") < 0 && currentText.find(newText) < 0)
		return newText;
	else
		return QString::null;
}

void TopLevel::autoSearch()
{
	if (autoSearchToggle->isChecked())
		searchAccel();
}

void TopLevel::searchAccel()
{
	QString newText = filteredClipboardText();
	if (!newText.isNull())
	{
		kanjiCB->setChecked(false);

		raise();

		Edit->setText(newText);
		search();
	}
}

void TopLevel::kanjiSearchAccel()
{
	QString newText = filteredClipboardText();
	if (!newText.isNull())
	{
		kanjiCB->setChecked(true);

		raise();

		Edit->setText(newText);
		search();
	}
}

void TopLevel::setResults(unsigned int results, unsigned int fullNum)
{
	QString str = i18n("%n result","%n results",results);

	if (results < fullNum)
		str += i18n(" out of %1").arg(fullNum);

	StatusBar->message(str);
	setCaption(str);
}

void TopLevel::slotConfigurationChanged()
{
	updateConfiguration();
}

void TopLevel::updateConfiguration()
{
	_Asyndeta.readKitenConfiguration();
	_ResultView->updateFont();
}

void TopLevel::slotConfigure()
{
	if (ConfigureDialog::showDialog("settings"))
		return;
	
	//ConfigureDialog didn't find an instance of this dialog, so lets create it :
	optionDialog = new ConfigureDialog(this, "settings");
	connect(optionDialog, SIGNAL(hidden()),this,SLOT(slotConfigureHide()));
	connect(optionDialog, SIGNAL(settingsChanged()), this, SLOT(slotConfigurationChanged()));
	connect(optionDialog, SIGNAL(valueChanged()), this, SIGNAL(quizConfChanged()));
	optionDialog->show();

	return;
}

void TopLevel::slotLearnConfigure()
{
	slotConfigure();
	optionDialog->showPage(2);
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
	Learn *_Learn = new Learn(_Asyndeta.retrieveIndex(), 0);

	connect(_Learn, SIGNAL(destroyed(Learn *)), this, SLOT(learnDestroyed(Learn *)));
	connect(_Learn, SIGNAL(linkClicked(const QString &)), this, SLOT(ressearch(const QString &)));
	connect(_Learn, SIGNAL(configureLearn()), this, SLOT(slotLearnConfigure()));
	connect(this, SIGNAL(quizConfChanged()), _Learn, SLOT(updateQuizConfiguration()));
	connect(this, SIGNAL(add(Dict::Entry)), _Learn, SLOT(add(Dict::Entry)));

	learnList.append(_Learn);

	_Learn->show();
}

void TopLevel::learnDestroyed(Learn *learn)
{
	learnList.remove(learn);
}

void TopLevel::createEEdit()
{
	eEdit *_eEdit = new eEdit(_Asyndeta.personalDictionaryLocation(), this);
	_eEdit->show();
}

void TopLevel::kanjiDictChange()
{
	// Do we even *need* something here?
}

QRegExp TopLevel::readingSearchItems(bool kanji)
{
	QString text = Edit->text();
	if (text.isEmpty()) // abandon search
		return QRegExp(); //empty

	//CompletionObj->addItem(text);
	QString regexp;
	if (kanji)
		regexp = " %1 ";
	else
		regexp = "\\[%1\\]";

	regexp = regexp.arg(text);

	return QRegExp(regexp, Config::caseSensitive());
}

QRegExp TopLevel::kanjiSearchItems(bool beginning)
{
	QString text = Edit->text();

	if (text.isEmpty())
		return QRegExp(); //empty

	QString regexp;
	if (beginning)
		regexp = "^%1";
	else
		regexp = "^%1\\W";

	regexp = regexp.arg(text);

	return QRegExp(regexp, Config::caseSensitive());
}

QRegExp TopLevel::searchItems()
{
	QString regexp;
	QString text = Edit->text();
	if (text.isEmpty())
		return QRegExp(); //empty

	unsigned int contains = text.contains(QRegExp("[A-Za-z0-9_:]"));
	if (Config::wholeWord() && contains == text.length())
		regexp = "\\W%1\\W";
	else
		regexp = "%1";

	regexp = regexp.arg(text);

	return QRegExp(regexp, Config::caseSensitive());
}

void TopLevel::toggleCom()
{
}

void TopLevel::configureToolBars()
{
	saveMainWindowSettings(KGlobal::config(), "TopLevelWindow");
	KEditToolbar dlg(actionCollection(), "kitenui.rc");
	connect(&dlg, SIGNAL(newToolbarConfig()), SLOT(newToolBarConfig()));
	dlg.exec();
}

void TopLevel::newToolBarConfig()
{
	createGUI("kitenui.rc");
	applyMainWindowSettings(KGlobal::config(), "TopLevelWindow");
}

RadWidget *TopLevel::radicalSearch()
{
	RadWidget *rw = new RadWidget(&_Rad, 0, "rw");
	connect(rw, SIGNAL(set(const QStringList &, unsigned int, unsigned int)), this, SLOT(radSearch(const QStringList &, unsigned int, unsigned int)));
	rw->show();
	return rw;
}

void TopLevel::radSearch(const QStringList &_list, unsigned int strokes, unsigned int errorMargin)
{
	//kdDebug() << "TopLevel::radSearch\n";

	QStringList list(_Rad.kanjiByRad(_list));

	QStringList::iterator it;

	Dict::SearchResult hist;
	hist.count = 0;
	hist.outOf = 0;
	hist.common = comCB->isChecked();

	QString prettyRadicalString;
	bool already = false;
	for (QStringList::ConstIterator it = _list.begin(); it != _list.end(); ++it)
	{
		if (already)
			prettyRadicalString.append(", ");
		prettyRadicalString.append(*it);

		already = true;
	}

	hist.text = i18n("Radical(s): %1").arg(prettyRadicalString);

	if (strokes)
		hist.list.append(Dict::Entry(i18n("Kanji with radical(s) %1 and %2 strokes").arg(prettyRadicalString).arg(strokes), true));
	else
		hist.list.append(Dict::Entry(i18n("Kanji with radical(s) %1").arg(prettyRadicalString), true));

	QString strokesString;
	if (strokes)
	{
		strokesString = QString::number(strokes);
		for (unsigned i = 1; i <= errorMargin; ++i)
		{
			strokesString.append('|');
			strokesString.prepend('|');
			strokesString.append(QString::number(strokes + i));
			strokesString.prepend(QString::number(strokes - i));
		}

		strokesString.append(')');
		strokesString.prepend("(?:");

		//kdDebug() << "strokesString is " << strokesString << endl;
	}

	for (it = list.begin(); it != list.end(); ++it)
	{

		Dict::SearchResult results = _Asyndeta.retrieveIndex()->searchKanji(QRegExp(strokes? (QString("S%1 ").arg(strokesString)) : (QString("^") + (*it)) ), (*it), comCB->isChecked());
		hist.outOf += results.outOf;

		if (results.count < 1)
			continue;

		hist.list.append(Dict::firstEntry(results));
		hist.results.append(Dict::firstEntryText(results));
		hist.count += results.count;
	}

	addHistory(hist);
	handleSearchResult(hist);
}

void TopLevel::back(void)
{
	assert(currentResult != resultHistory.begin());
	--currentResult;
	--currentResultIndex;
	enableHistoryButtons();
	handleSearchResult(*currentResult);
	historySpotChanged();
}

void TopLevel::forward(void)
{
	assert(currentResult != resultHistory.end());
	++currentResult;
	++currentResultIndex;
	enableHistoryButtons();
	handleSearchResult(*currentResult);
	historySpotChanged();
}

void TopLevel::goInHistory(int index)
{
	currentResult = resultHistory.at(resultHistory.count() - historyAction->items().count() + index);
	currentResultIndex = index;
	enableHistoryButtons();
	handleSearchResult(*currentResult);
	historySpotChanged();
}

void TopLevel::historySpotChanged()
{
	historyAction->setCurrentItem(currentResultIndex);
}

void TopLevel::addHistory(Dict::SearchResult result)
{
	QStringList newHistoryList = historyAction->items();

	// remove from back till we hit currentResult
	while (resultHistory.fromLast() != currentResult)
	{
		resultHistory.pop_back();
		newHistoryList.pop_back();
	}

	resultHistory.append(result);
	newHistoryList.append(result.text);

	// make history menu a reasonable length
	while (newHistoryList.count() > 20)
		newHistoryList.pop_front();

	historyAction->setItems(newHistoryList);

	currentResult = resultHistory.end();
	--currentResult;
	currentResultIndex = resultHistory.count() - 1;

	historySpotChanged();

	enableHistoryButtons();

	if (resultHistory.size() > 50)
		resultHistory.pop_front();
}

void TopLevel::enableHistoryButtons()
{
	backAction->setEnabled(currentResult != resultHistory.begin());
	forwardAction->setEnabled(++currentResult != resultHistory.end());
	--currentResult;
}

void TopLevel::print()
{
	_ResultView->print((*currentResult).text);
}

void TopLevel::configureGlobalKeys()
{
	KKeyDialog::configure(Accel, this);
}

#include "kiten.moc"
