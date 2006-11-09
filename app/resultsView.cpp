/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
		(C) 2005 Paul Temple <paul.temple@gmx.net>
		(C) 2006 Joseph Kerian <jkerian@gmail.com>
		(C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>

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

/* Needed by ResultView only */
#include <kapplication.h>
#include <kconfig.h>
#include <kprinter.h>
#include <qpainter.h>
#include "kitenconfig.h"
#include <klocale.h>
#include "resultsView.h"

/* TODO: make this a KHTML thing instead of a KTextBrowser? */
ResultView::ResultView(QWidget *parent, const char *name)
	: KHTMLPart(parent, parent)
{
	////////setReadOnly(true);
	/* TODO: configurably underlined links */
//	setLinkUnderline(false); //KDE4 CHANGE
	////////basicMode = false;

	// don't let ktextbrowser internally handle link clicks
	////////setNotifyClick(true);
}

/** As the name implies, it appends @p text to the printText */
inline void ResultView::append(const QString &text)
{
	printText.append(text);
}

/** This clears the printText */
void ResultView::clear()
{
	printText = "";
}

/** Flushes the printText to screen */
void ResultView::flush()
{
	begin();
	write(printText);
	end();
//KDE4 CHANGE	setCursorPosition(0, 0);
	////////ensureCursorVisible();
}

/** Non-buffered write of contents to screen */
void ResultView::setContents(const QString &text)
{
	begin();
	write(text);
	end();
//KDE4 CHANGE	setCursorPosition(0,0);
	////////ensureCursorVisible();
}

/** Prints the contents of the resultview to a printer with @p title as the 
  title. */
void ResultView::print(QString title)
{
	/* KDE4 CHANGE
	KPrinter printer;
	printer.setFullPage(true);
	if (printer.setup(this, i18n("Print Japanese Reference")))
	{
		QPainter p(&printer);
		//Q3PaintDeviceMetrics metrics(p.device());
		int dpix = metrics.logicalDpiX();
		int dpiy = metrics.logicalDpiY();
		const int margin = 72; // pt
	
		QRect body(dpix, dpiy, metrics.width() - margin * dpix / margin * 2, 
				metrics.height() - margin * dpiy / margin * 2);
	
		//Q3SimpleRichText richText(title.isNull()? 
				printText : 
				i18n("<h1>Search for \"%1\"</h1>").arg(title) + printText, 
				font(), context(), styleSheet(), mimeSourceFactory(),
				body.height(), Qt::black, false);
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
			p.drawText(view.right() - p.fontMetrics().width(footer), 
					view.bottom() + p.fontMetrics().ascent() + 5, footer);

			if (view.top() >= richText.height())
				break;

			printer.newPage();
			page++;

			kapp->processEvents();
		}
		while (true);
    }
*/
}

/** updates the font.  Used on font change */
void ResultView::updateFont()
{
	////////setFont(KitenConfigSkeleton::self()->font());
}

void ResultView::urlSelected(const QString & 	url, 
		int 	button, 
		int 	state, 
		const QString & 	_target, 
		KParts::URLArgs 	args )
{
	kDebug() << url << endl;
	emit urlClicked(url);
}

#include "resultsView.moc"
