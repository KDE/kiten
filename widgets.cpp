#include <qwidget.h>
#include <kcompletion.h>
#include <kdebug.h>
#include <klocale.h>
#include <kconfig.h>
#include <kapp.h>
#include <kiconloader.h>
#include <klineedit.h>
#include <kpushbutton.h>
#include <qtoolbutton.h>
#include <qtextedit.h>
#include <qstring.h>
#include <qregexp.h>
#include <qlayout.h>

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
