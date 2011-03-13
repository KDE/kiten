/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>
 Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>
 Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>
 Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>

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
#include <QtGui/QScrollBar>

#include <kconfig.h>
#include <kmenu.h>
#include <kactionmenu.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kcolorscheme.h>

ResultView::ResultView( QWidget *parent, const char *name )
: KHTMLPart( parent, parent )
, scrollValue( 0 )
{
  ////////setReadOnly(true);
  /* TODO: configurably underlined links */
//	setLinkUnderline(false); //KDE4 CHANGE
  ////////basicMode = false;

  // don't let ktextbrowser internally handle link clicks
  ////////setNotifyClick(true);
  connect( view(), SIGNAL( finishedLayout() ),
             this,   SLOT( doScroll() ) );
}

/**
 * As the name implies, it appends @param text to the printText
 */
inline void ResultView::append( const QString &text )
{
  printText.append(text);
}

/**
 * This clears the printText
 */
void ResultView::clear()
{
  printText = "";
}

/**
 * Flushes the printText to screen
 */
void ResultView::flush()
{
  begin();
  setUserStyleSheet( generateCSS() );
  write( printText );
  end();
//KDE4 CHANGE	setCursorPosition( 0, 0 );
  ////////ensureCursorVisible();
}

/**
 * Non-buffered write of contents to screen
 */
void ResultView::setContents( const QString &text )
{
  begin();
  setUserStyleSheet( generateCSS() );
  write( text );
  end();

  kDebug() << "Set CSS to " << generateCSS();
//KDE4 CHANGE	setCursorPosition( 0,0 );
  ////////ensureCursorVisible();
}

/**
 * Prints the contents of the resultview to a
 * printer with @param title as the title.
 */
void ResultView::print( const QString &title )
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

bool ResultView::urlSelected( const QString &url,
                        int button,
                        int state,
                        const QString &_target,
                        const KParts::OpenUrlArguments& args,
                        const KParts::BrowserArguments& browserArgs )
{
  //kDebug() << nodeUnderMouse().parentNode().parentNode().parentNode().toHTML();
  emit urlClicked( url );
  return KHTMLPart::urlSelected( url, button, state, _target, args, browserArgs );
}

QString ResultView::deLinkify( DOM::Node node )
{
  //TODO: make this function more flexible (ie, accept non-link-content as
  //well.)
  QString word;
  for ( int i = 0; i < node.childNodes().length(); ++i )
  {
    if ( node.childNodes().item(i).nodeName() != "a" )
    {
      return QString();
    }
    if ( ! node.childNodes().item(i).hasChildNodes() )
    {
      return QString();
    }

    word += node.childNodes().item(i).childNodes().item( 0 ).nodeValue().string();
  }
  return word;
}

QString ResultView::generateCSS()
{
  KColorScheme scheme( QPalette::Active );
  QFont font = KitenConfigSkeleton::self()->font();

  return QString( ".Word { font-size: %9px }"
                  ".Entry { font-size: %8px; color: %1; font-family: \"%7\"; }"
                  //"div.Entry {display: inline; }"
                  ".DictionaryHeader { color: %2; border-bottom: solid %3 }"
                  "a{ text-decoration: none; }"
                  "a:link { color: %4; }"
                  "a:visited {color: %5} "
                  "a:hover {color: %6 } "
                  "a:active {color: %6}"
                  ".Entry:hover { background-color: %10 }"
                  "query { color: %6 }" )
          .arg( scheme.foreground().color().name() )
          .arg( scheme.foreground (KColorScheme::InactiveText ).color().name() )
          .arg( scheme.shade( KColorScheme::MidlightShade ).name() )
          .arg( scheme.foreground( KColorScheme::LinkText ).color().name() )
          .arg( scheme.foreground( KColorScheme::VisitedText ).color().name() )
          .arg( scheme.foreground( KColorScheme::ActiveText ).color().name() )
          .arg( font.family() )
          .arg( font.pointSize() ) // the text size
          .arg( font.pointSize() + 10 ) // a larger size for kanji
          .arg( scheme.background( KColorScheme::AlternateBackground ).color().name() );
}

void ResultView::setLaterScrollValue( int scrollValue )
{
  this->scrollValue = scrollValue;
}

void ResultView::doScroll()
{
  view()->verticalScrollBar()->setValue(scrollValue);
}

#include "resultsView.moc"
