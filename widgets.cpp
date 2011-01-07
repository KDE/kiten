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
#include <klistview.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kprinter.h>
#include <kprocess.h>
#include <kstandarddirs.h>
#include <kstatusbar.h>
#include <tqfileinfo.h>
#include <tqpaintdevicemetrics.h>
#include <tqpainter.h>
#include <tqregexp.h>
#include <tqsimplerichtext.h>
#include <tqtextcodec.h>

#include <cassert>

#include "kitenconfig.h"
#include "dict.h"
#include "kromajiedit.h"
#include "widgets.h"

ResultView::ResultView(bool showLinks, TQWidget *parent, const char *name)
	: KTextBrowser(parent, name)
{
	setReadOnly(true);
	setLinkUnderline(false);
	basicMode = false;
	links = showLinks;

	// don't let ktextbrowser internally handle link clicks
	setNotifyClick(true);
}

void ResultView::addResult(Dict::Entry result, bool common)
{
	if (result.dictName() != "__NOTSET")
	{
		addHeader((common? i18n("Common results from %1") : i18n("Results from %1")).arg(result.dictName()), 5);
		return;
	}
	if (result.header() != "__NOTSET")
	{
		addHeader(result.header());
		return;
	}

	TQString html;
	if (result.kanaOnly())
		html = TQString("<p><font size=\"+2\">%1</font>  ").arg(result.firstReading());
	else
		html = TQString("<p><font size=\"+2\">%1</font>: %2  ").arg(putchars(result.kanji())).arg(result.firstReading());

	TQStringList::Iterator it;
	TQStringList Meanings = result.meanings();
	for (it = Meanings.begin(); it != Meanings.end(); ++it)
	{
		if ((*it).find("(P)") >= 0)
		{
			if (common)
				continue;
			else
			{
				html += TQString("<strong>") + i18n("Common") + TQString("</strong>   ");
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
		addHeader((common? i18n("Common results from %1") : i18n("Results from %1")).arg(result.dictName()), 5);
		return;
	}
	if (result.header() != "__NOTSET")
	{
		addHeader(result.header());
		return;
	}

	TQString html;
	html = TQString("<p><font size=\"+3\">%1</font>: %2  ").arg(putchars(result.kanji()));

	unsigned int freq = result.freq();
	if (freq == 0) // does it have a frequency?
		html = html.arg(i18n("Rare"));
	else
		// this isn't a number of times, it's simply an index of
		// probability
		html = html.arg(i18n("Probability rank #%1").arg(freq));

	html += "<br />";

	TQStringList::Iterator it;
	TQStringList Readings = result.readings();
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

	TQStringList Meanings = result.meanings();
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
		html.append(i18n(" Largest radical: %1, with %2 strokes.").arg(TQString("<a href=\"__radical:%1\">%2</a>").arg(rad.radical()).arg(rad.radical())).arg(rad.strokes()));

	html += "</p>";

	append(html);
}

void ResultView::addHeader(const TQString &header, unsigned int degree)
{
	append(TQString("<h%1>%2</h%3>").arg(degree).arg(header).arg(degree));
}

TQString ResultView::putchars(const TQString &text)
{
	if (!links)
		return text;

	unsigned int len = text.length();
	TQString ret;

	for (unsigned i = 0; i < len; i++)
	{
		if (Dict::textType(TQString(text.at(i))) == Dict::Text_Kanji)
			ret.append(TQString("<a href=\"%1\">%1</a>").arg(text.at(i)).arg(text.at(i)));
		else
			ret.append(text.at(i));
	}
	
	return ret;
}

void ResultView::append(const TQString &text)
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
	setCursorPosition(0, 0);
	ensureCursorVisible();
}

void ResultView::print(TQString title)
{
	KPrinter printer;
	printer.setFullPage(true);
	if (printer.setup(this, i18n("Print Japanese Reference")))
	{
		TQPainter p(&printer);
		TQPaintDeviceMetrics metrics(p.device());
		int dpix = metrics.logicalDpiX();
		int dpiy = metrics.logicalDpiY();
		const int margin = 72; // pt
	
		TQRect body(dpix, dpiy, metrics.width() - margin * dpix / margin * 2, metrics.height() - margin * dpiy / margin * 2);
	
		TQSimpleRichText richText(title.isNull()? printText : i18n("<h1>Search for \"%1\"</h1>").arg(title) + printText, font(), context(), styleSheet(), mimeSourceFactory(), body.height(), Qt::black, false);
		richText.setWidth(&p, body.width());
		TQRect view(body);
		int page = 1;

		TQColorGroup goodColorGroup = TQColorGroup(colorGroup());
		goodColorGroup.setColor(TQColorGroup::Link, Qt::black);

		do
		{
			richText.draw(&p, body.left(), body.top(), view, goodColorGroup);
			view.moveBy(0, body.height());
			p.translate(0, -body.height());

			TQFont myFont(font());
			myFont.setPointSize(9);
			p.setFont(myFont);
			TQString footer(TQString("%1 - Kiten").arg(TQString::number(page)));
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
	setFont(Config::self()->font());
}

/////////////////////////////////////////////////////
// nice EDICT dictionary editor

eEdit::eEdit(const TQString &_filename, TQWidget *parent, const char *name)
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
	List->setColumnWidthMode(0, TQListView::Maximum);
	List->setColumnWidthMode(1, TQListView::Maximum);
	List->setColumnWidthMode(2, TQListView::Maximum);
	List->setColumnWidthMode(3, TQListView::Maximum);
	List->setMultiSelection(true);
	List->setDragEnabled(true);

	saveAct = KStdAction::save(this, TQT_SLOT(save()), actionCollection());
	removeAct = new KAction(i18n("&Delete"), "edit_remove", CTRL + Key_X, this, TQT_SLOT(del()), actionCollection(), "del");
	(void) new KAction(i18n("&Disable Dictionary"), 0, this, TQT_SLOT(disable()), actionCollection(), "disable");
	addAct = new KAction(i18n("&Add"), "edit_add", CTRL + Key_A, this, TQT_SLOT(add()), actionCollection(), "add");
	(void) KStdAction::close(this, TQT_SLOT(close()), actionCollection());

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
		new KListViewItem(List);
}

void eEdit::openFile(const TQString &file)
{
	TQFile f(file);
	if (!f.open(IO_ReadOnly))
		return;

	TQTextStream t(&f);
	TQString s;

	while (!t.eof())
	{ 
		s = t.readLine();
		if (s.left(1) == "#" || s.isEmpty())
			continue;
		Dict::Entry entry = Dict::parse(s);
		TQString meanings = Dict::prettyMeaning(entry.meanings());
		bool common = meanings.find(TQString("(P)")) >= 0;
		meanings.replace(TQRegExp("; "), "/");
		meanings.replace(TQRegExp("/\\(P\\)"), "");
		new TQListViewItem(List, entry.kanji(), Dict::prettyKanjiReading(entry.readings()), meanings, common? i18n("yes") : i18n("no"));
	}
}

void eEdit::save()
{
	TQFile f(filename);
	if (!f.open(IO_WriteOnly))
		return;
	TQTextStream t(&f);

	t << "# Generated by Kiten's EDICT editor" << endl << "# http://katzbrown.com/kiten" << endl << endl;
	
	TQListViewItemIterator it(List);
	for (; it.current(); ++it)
	{
		TQString kanji = it.current()->text(0);
		TQString reading = it.current()->text(1);
		TQString text = kanji.isEmpty()? reading : kanji;

		TQString meanings = it.current()->text(2);
		if (meanings.right(1) != "/")
			meanings.append("/");
		if (meanings.left(1) != "/")
			meanings.prepend("/");

		TQString commonString = it.current()->text(3).lower();
		bool common = (commonString == "true" || commonString == "yes" || commonString == "1" || commonString == "common");

		text.append(" ");
		if (!kanji.isEmpty())
			text.append(TQString("[%1] ").arg(reading));
		text.append(meanings);

		if (common)
			text.append("(P)/");

		t << text << endl;
	}

	f.flush();

	// find the index generator executable
	KProcess proc;
	proc << KStandardDirs::findExe("kitengen") << filename << KGlobal::dirs()->saveLocation("data", "kiten/xjdx/", true) + TQFileInfo(filename).baseName() + ".xjdx";
	// TODO: put up a status dialog and event loop instead of blocking
	proc.start(KProcess::Block, KProcess::NoCommunication);
	
	statusBar()->message(i18n("Saved"));
	isMod = false;
}

void eEdit::disable()
{
	int result = KMessageBox::warningYesNo(this, i18n("Disabling your personal dictionary will delete its contents.\n\n(You can however always create your dictionary again.)"), TQString::null, i18n("Disable"), KStdGuiItem::cancel(), "DisableAsk", true);
	if (result == KMessageBox::No)
		return;

	TQFile::remove(filename);
	delete this;
}

void eEdit::del()
{
	TQPtrList<TQListViewItem> selected = List->selectedItems();
	assert(selected.count());

	for(TQPtrListIterator<TQListViewItem> i(selected); *i; ++i)
		delete *i;

	isMod = true;
}

/////////////////////////////////////////////////////
// sorta taken from konqy

EditAction::EditAction(const TQString& text, int accel, const TQObject *receiver, const char *member, TQObject* parent, const char* name)
    : KAction(text, accel, parent, name)
{
	m_receiver = receiver;
	m_member = member;
}

EditAction::~EditAction()
{
}

int EditAction::plug( TQWidget *w, int index )
{
	//  if ( !w->inherits( "KToolBar" ) )
	//    return -1;

	KToolBar *toolBar = (KToolBar *)w;

	int id = KAction::getToolButtonID();

	KRomajiEdit *comboBox = new KRomajiEdit(toolBar, "search edit");
	toolBar->insertWidget( id, 70, comboBox, index );
	connect( comboBox, TQT_SIGNAL( returnPressed()), m_receiver, m_member );

	addContainer(toolBar, id);

	connect( toolBar, TQT_SIGNAL( destroyed() ), this, TQT_SLOT( slotDestroyed() ) );

	toolBar->setItemAutoSized( id, true );

	m_combo = comboBox;

	emit plugged();

	//TQWhatsThis::add( comboBox, whatsThis() );

	return containerCount() - 1;
}

void EditAction::unplug( TQWidget *w )
{
//  if ( !w->inherits( "KToolBar" ) )
//    return;

	KToolBar *toolBar = (KToolBar *)w;

	int idx = findContainer( w );

	toolBar->removeItem( itemId( idx ) );

	removeContainer( idx );
	m_combo = 0L;
}

void EditAction::clear()
{
	m_combo->clear();
	m_combo->setFocus();
}

void EditAction::insert(const TQString &text)
{
	m_combo->insert(text);
}

void EditAction::setText(const TQString &text)
{
	m_combo->setText(text);
}

TQGuardedPtr<KLineEdit> EditAction::editor()
{
	return m_combo;
}

#include "widgets.moc"
