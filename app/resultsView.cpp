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

#include "resultsView.h"
#include "kitenconfig.h"

/* Needed by ResultView only */
#include <QtGui/QApplication>
#include <QtGui/QPainter>

#include <kconfig.h>
#include <kmenu.h>
#include <kactionmenu.h>
#include <klocale.h>
#include <kactioncollection.h>

/* TODO: make this a KHTML thing instead of a KTextBrowser? */
ResultView::ResultView(QWidget *parent, const char *name)
	: KHTMLPart(parent, parent)
{
	//Set right-click functionality
	connect(this, SIGNAL(popupMenu( const QString& , const QPoint& )), this, SLOT(openPopupMenu( const QString& , const QPoint& )));
	popupActions = new KActionCollection(widget());
	addToExportListAction = popupActions->addAction("add_to_export_list_popup");
        addToExportListAction->setText(i18n("&Add to export list"));
	popupMenu = popupActions->add<KActionMenu>("popup");
	popupMenu->addAction(addToExportListAction);

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
void ResultView::print(const QString &title)
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
				i18n("<h1>Search for \"%1\"</h1>",title) + printText,
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

			qApp->processEvents();
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

bool ResultView::urlSelected(const QString & 	url,
		int 	button,
		int 	state,
		const QString & 	_target,
		const KParts::OpenUrlArguments& 	args,
		const KParts::BrowserArguments& 	browserArgs )
{
	kDebug() << nodeUnderMouse().parentNode().parentNode().parentNode().toHTML();
	emit urlClicked(url);
	return KHTMLPart::urlSelected(url, button, state, _target, args, browserArgs);
}

DOM::Node ResultView::findEntryNode(DOM::Node node)
{
	while (!node.isNull() && node.attributes().getNamedItem("class").nodeValue() != "Entry")
	{
		node = node.parentNode();
	}
	return node;
}

DOM::Node ResultView::findChildWithClassValue(DOM::Node node, const QString& classValue)
{
	/* Do a Breadth first search looking for a node with the attribute
	 * class="classValue" */
	QList<DOM::Node> traverse;
	for (int i = 0; i < node.childNodes().length(); ++i)
	{
		traverse.push_back(node.childNodes().item(i));
	}
	DOM::Node returnNode;
	while (traverse.size() > 0)
	{
		returnNode = traverse.takeFirst();
		if (returnNode.attributes().getNamedItem("class").nodeValue().string() == classValue)
		{
			return returnNode;
		}

		for (int i = 0; i < returnNode.childNodes().length(); ++i)
		{
			traverse.push_back(returnNode.childNodes().item(i));
		}
	}
	return DOM::Node();
}

QString ResultView::deLinkify(DOM::Node node)
{
	//TODO: make this function more flexible (ie, accept non-link-content as
	//well.)
	QString word;
	for (int i = 0; i < node.childNodes().length(); ++i)
	{
		if (node.childNodes().item(i).nodeName() != "a") return QString();
		if (!node.childNodes().item(i).hasChildNodes()) return QString();
		word += node.childNodes().item(i).childNodes().item(0).nodeValue().string();
	}
	return word;
}

void ResultView::openPopupMenu( const QString& url, const QPoint& point )
{
	QAction *clickedAction = popupMenu->menu()->exec(point);
	if (clickedAction == addToExportListAction)
	{
		//find the entry node that contains the node under the mouse
		DOM::Node entryNode = findEntryNode(nodeUnderMouse());
		int index = entryNode.attributes().getNamedItem("index").nodeValue().toInt();

		emit entrySpecifiedForExport(index);

	} else
	{
		kDebug() << "No menu action clicked.";
	}

}

#include "resultsView.moc"
