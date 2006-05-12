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
#include <k3listview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprinter.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <qfileinfo.h>
#include <q3paintdevicemetrics.h>
#include <qpainter.h>
#include <qregexp.h>
#include <q3simplerichtext.h>
#include <qtextcodec.h>
//Added by qt3to4:
#include <QTextStream>
#include <Q3PtrList>

#include <cassert>

#include "kitenconfig.h"
#include "dict.h"
#include "kromajiedit.h"
#include "widgets.h"

ResultView::ResultView(bool showLinks, QWidget *parent, const char *name)
	: KTextBrowser(parent, name)
{
	setReadOnly(true);
	//setLinkUnderline(false);
	basicMode = false;
	links = showLinks;

	// don't let ktextbrowser internally handle link clicks
	setNotifyClick(true);
}

void ResultView::addResult(Dict::Entry result, bool common)
{
	if (result.dictName() != "__NOTSET")
	{
		addHeader((common ? i18n("Common results from %1", result.dictName())
		                  : i18n("Results from %1", result.dictName())),
		          5);
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
		if ((*it).contains("(P)"))
		{
			if (common)
				continue;
			else
			{
				html += QString("<strong>") + i18n("Common") + QString("</strong>   ");
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

void ResultView::addKanjiResult(Dict::Entry result, bool common, Radical rad)
{
	if (result.dictName() != "__NOTSET")
	{
		addHeader((common ? i18n("Common results from %1", result.dictName())
		                  : i18n("Results from %1", result.dictName())),
		          5);
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
		// this isn't a number of times, it's simply an index of
		// probability
		html = html.arg(i18n("Probability rank #%1", freq));

	html += "<br />";

	QStringList::Iterator it;
	QStringList Readings = result.readings();
	for (it = Readings.begin(); it != Readings.end(); ++it)
	{
		if ((*it) == "T1")
		{
			if (basicMode)
				break;

			html.truncate(html.length() - 2); // get rid of last ,
			html += i18n("<br />In names: ");
		}
		else
		{
			if ((*it) == "T2")
			{
				if (basicMode)
					break;

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
	
	KLocalizedString tmp = ki18n("Grade Level: %1. Strokes: %2.");
	switch (result.grade())
	{
		case 0:
		tmp = tmp.subs(i18n("None"));
		break;
		case 8:
		tmp = tmp.subs(i18n("In Jouyou"));
		break;
		case 9:
		tmp = tmp.subs(i18n("In Jinmeiyou"));
		break;
		default:
		tmp = tmp.subs(result.grade());
	}
	html += tmp.subs(result.strokes()).toString();

	if (result.miscount() != 0)
		html.append(i18n(" Common Miscount: %1.", result.miscount()));

	if (!rad.radical().isEmpty())
		html.append(i18n(" Largest radical: %1, with %2 strokes.", QString("<a href=\"__radical:%1\">%2</a>").arg(rad.radical()).arg(rad.radical()), rad.strokes()));

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

	for (unsigned i = 0; i < len; i++)
	{
		if (Dict::textType(QString(text.at(i))) == Dict::Text_Kanji)
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
	setHtml(printText);
	QTextCursor cursor = textCursor ();
	cursor.movePosition( QTextCursor::Start );
	ensureCursorVisible();
}

void ResultView::print(QString title)
{
#warning "kde4: porting";
#if 0
	KPrinter printer;
	printer.setFullPage(true);
	if (printer.setup(this, i18n("Print Japanese Reference")))
	{
		QPainter p(&printer);
		Q3PaintDeviceMetrics metrics(p.device());
		int dpix = metrics.logicalDpiX();
		int dpiy = metrics.logicalDpiY();
		const int margin = 72; // pt

		QRect body(dpix, dpiy, metrics.width() - margin * dpix / margin * 2, metrics.height() - margin * dpiy / margin * 2);

		Q3SimpleRichText richText(title.isNull()? printText : i18n("<h1>Search for \"%1\"</h1>", title) + printText, font(), context(), styleSheet(), mimeSourceFactory(), body.height(), Qt::black, false);
		richText.setWidth(&p, body.width());
		QRect view(body);
		int page = 1;

		QColorGroup goodColorGroup = QColorGroup(colorGroup());
		goodColorGroup.setColor(QPalette::Link, Qt::black);

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
#endif
}

void ResultView::updateFont()
{
	setFont(Config::font());
}

/////////////////////////////////////////////////////
// nice EDICT dictionary editor

eEdit::eEdit(const QString &_filename, QWidget *parent, const char *name)
	: KMainWindow(parent, name)
	, filename(_filename)
{
	List = new K3ListView(this);
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
	List->setColumnWidthMode(0, Q3ListView::Maximum);
	List->setColumnWidthMode(1, Q3ListView::Maximum);
	List->setColumnWidthMode(2, Q3ListView::Maximum);
	List->setColumnWidthMode(3, Q3ListView::Maximum);
	List->setMultiSelection(true);
	List->setDragEnabled(true);

	saveAct = KStdAction::save(this, SLOT(save()), actionCollection());
	removeAct = new KAction(i18n("&Delete"), actionCollection(), "del");
	connect( removeAct, SIGNAL( triggered(bool) ), this, SLOT(del()) ); 
	removeAct->setShortcut(Qt::CTRL + Qt::Key_X);
	removeAct->setIcon(KIcon("edit_remove"));

	KAction * disableAct = new KAction(i18n("&Disable Dictionary"), actionCollection(), "disable");
	connect( disableAct, SIGNAL( triggered(bool) ), this, SLOT(disable()) ); 

	addAct = new KAction(i18n("&Add"), actionCollection(), "add");
	connect( addAct, SIGNAL( triggered(bool) ), this, SLOT(add()) ); 
	addAct->setShortcut(Qt::CTRL + Qt::Key_A);
	addAct->setIcon(KIcon("edit_add"));

	(void) KStdAction::close(this, SLOT(close()), actionCollection());

	createGUI("eeditui.rc");
	//closeAction->plug(toolBar());

	openFile(filename);

	isMod = false;
}

eEdit::~eEdit()
{
}

void eEdit::add()
{
	if (List)
		new K3ListViewItem(List);
}

void eEdit::openFile(const QString &file)
{
	QFile f(file);
	if (!f.open(QIODevice::ReadOnly))
		return;

	QTextStream t(&f);
	QString s;

	while (!t.atEnd())
	{
		s = t.readLine();
		if (s.left(1) == "#" || s.isEmpty())
			continue;
		Dict::Entry entry = Dict::parse(s);
		QString meanings = Dict::prettyMeaning(entry.meanings());
		bool common = meanings.contains(QString("(P)"));
		meanings.replace(QRegExp("; "), "/");
		meanings.replace(QRegExp("/\\(P\\)"), "");
		new Q3ListViewItem(List, entry.kanji(), Dict::prettyKanjiReading(entry.readings()), meanings, common? i18n("yes") : i18n("no"));
	}
}

void eEdit::save()
{
	QFile f(filename);
	if (!f.open(QIODevice::WriteOnly))
		return;
	QTextStream t(&f);

	t << "# Generated by Kiten's EDICT editor" << endl << "# http://katzbrown.com/kiten" << endl << endl;

	Q3ListViewItemIterator it(List);
	for (; it.current(); ++it)
	{
		QString kanji = it.current()->text(0);
		QString reading = it.current()->text(1);
		QString text = kanji.isEmpty()? reading : kanji;

		QString meanings = it.current()->text(2);
		if (meanings.right(1) != "/")
			meanings.append("/");
		if (meanings.left(1) != "/")
			meanings.prepend("/");

		QString commonString = it.current()->text(3).toLower();
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
	proc << KStandardDirs::findExe("kitengen") << filename << KGlobal::dirs()->saveLocation("data", "kiten/xjdx/", true) + QFileInfo(filename).baseName() + ".xjdx";
	// TODO: put up a status dialog and event loop instead of blocking
	proc.start(KProcess::Block, KProcess::NoCommunication);

	statusBar()->showMessage(i18n("Saved"));
	isMod = false;
}

void eEdit::disable()
{
	int result = KMessageBox::warningYesNo(this, i18n("Disabling your personal dictionary will delete its contents.\n\n(You can however always create your dictionary again.)"), QString(), i18n("Disable"), KStdGuiItem::cancel(), "DisableAsk", KMessageBox::Notify);
	if (result == KMessageBox::No)
		return;

	QFile::remove(filename);
	delete this;
}

void eEdit::del()
{
	QList<Q3ListViewItem*> selected = List->selectedItems();
	assert(selected.count());

	foreach ( Q3ListViewItem* listViewItem, selected ) {
		delete listViewItem;
	}
	isMod = true;
}

/////////////////////////////////////////////////////
// sorta taken from konqy

EditAction::EditAction(const QString& text, int accel, const QObject *receiver, const char *member, KActionCollection* parent, const char* name)
    : KAction(text, parent, name),
      m_receiver(receiver),
      m_member(member)
{
	connect( this, SIGNAL( triggered(bool) ), receiver, SLOT(member()) ); 
	this->setShortcut(accel);
}

EditAction::~EditAction()
{
}

int EditAction::plug( QWidget *w, int index )
{
	//  if ( !w->inherits( "KToolBar" ) )
	//    return -1;

	KToolBar *toolBar = (KToolBar *)w;

// 	int id = KAction::getToolButtonID();

	KRomajiEdit *comboBox = new KRomajiEdit(toolBar, "search edit");
// 	toolBar->insertWidget( id, 70, comboBox, index );
  m_combo = comboBox;
  toolBar->addWidget(m_combo);
	connect( m_combo, SIGNAL( returnPressed()), m_receiver, m_member );

// 	addContainer(toolBar, id);

// 	connect( toolBar, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

// 	toolBar->setItemAutoSized( id, true );

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

// 	int idx = findContainer( w );

// 	toolBar->removeItem( itemId( idx ) );

// 	removeContainer( idx );
	m_combo = 0L;
}

void EditAction::clear()
{
	m_combo->clear();
	m_combo->setFocus();
}

void EditAction::insert(const QString &text)
{
	m_combo->insert(text);
}

void EditAction::setText(const QString &text)
{
	m_combo->setText(text);
}

QPointer<KLineEdit> EditAction::editor()
{
	return m_combo;
}

#include "widgets.moc"
