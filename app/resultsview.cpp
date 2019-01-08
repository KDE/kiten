/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
 * Copyright (C) 2006 Eric Kjeldergaard <kjelderg@gmail.com>                 *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 *
 * USA                                                                       *
 *****************************************************************************/

#include "resultsview.h"

#include "kitenconfig.h"

#include <KColorScheme>
#include <KHTMLView>

/* Needed by ResultsView only */
#include <QScrollBar>

ResultsView::ResultsView( QWidget *parent, const char *name )
: KHTMLPart( parent, parent )
, _scrollValue( 0 )
{
  ////////setReadOnly(true);
  /* TODO: configurably underlined links */
//	setLinkUnderline(false); //KDE4 CHANGE
  ////////basicMode = false;

  // don't let ktextbrowser internally handle link clicks
  ////////setNotifyClick(true);
  connect( view(), &KHTMLView::finishedLayout,
             this,   &ResultsView::doScroll );
}

/**
 * As the name implies, it appends @param text to the printText
 */
void ResultsView::append( const QString &text )
{
  _printText.append( text );
}

/**
 * This clears the printText
 */
void ResultsView::clear()
{
  _printText = QLatin1String("");
}

QString ResultsView::deLinkify( DOM::Node node )
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

void ResultsView::doScroll()
{
  view()->verticalScrollBar()->setValue(_scrollValue);
}

/**
 * Flushes the printText to screen
 */
void ResultsView::flush()
{
  begin();
  setUserStyleSheet( generateCSS() );
  write( _printText );
  end();
//KDE4 CHANGE	setCursorPosition( 0, 0 );
  ////////ensureCursorVisible();
}

QString ResultsView::generateCSS()
{
  KColorScheme scheme( QPalette::Active );
  QFont font = KitenConfigSkeleton::self()->font();

  return QString( ".Word { font-size: %9px }"
                  ".Entry { font-size: %8px; color: %1; font-family: \"%7\"; }"
                  //"div.Entry {display: inline; }"
                  ".DictionaryHeader { color: %2; border-bottom: solid %3 }"
                  ".CommonHeader { color: %2; text-align:center }"
                  ".UncommonHeader { color: %2; text-align:center }"
                  "a{ text-decoration: none; }"
                  "a:link { color: %4; }"
                  "a:visited {color: %5} "
                  "a:hover {color: %6 } "
                  "a:active {color: %6}"
                  ".Entry:hover { background-color: %10 }"
                  "query { color: %6 }" )
          .arg( scheme.foreground().color().name() )
          .arg( scheme.foreground( KColorScheme::InactiveText ).color().name() )
          .arg( scheme.shade( KColorScheme::MidlightShade ).name() )
          .arg( scheme.foreground( KColorScheme::LinkText ).color().name() )
          .arg( scheme.foreground( KColorScheme::VisitedText ).color().name() )
          .arg( scheme.foreground( KColorScheme::ActiveText ).color().name() )
          .arg( font.family() )
          .arg( font.pointSize() ) // the text size
          .arg( font.pointSize() + 10 ) // a larger size for kanji
          .arg( scheme.background( KColorScheme::AlternateBackground ).color().name() );
}

/**
 * Prints the contents of the resultview to a
 * printer with @param title as the title.
 */
void ResultsView::print( const QString &title )
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

void ResultsView::setBasicMode( bool yes )
{
  _basicMode = yes;
}

/**
 * Non-buffered write of contents to screen
 */
void ResultsView::setContents( const QString &text )
{
  begin();
  setUserStyleSheet( generateCSS() );
  write( text );
  end();

  qDebug() << "Set CSS to " << generateCSS();
//KDE4 CHANGE	setCursorPosition( 0,0 );
  ////////ensureCursorVisible();
}

void ResultsView::setLaterScrollValue( int scrollValue )
{
  this->_scrollValue = scrollValue;
}

bool ResultsView::urlSelected( const QString &url,
                        int button,
                        int state,
                        const QString &_target,
                        const KParts::OpenUrlArguments& args,
                        const KParts::BrowserArguments& browserArgs )
{
  //qDebug() << nodeUnderMouse().parentNode().parentNode().parentNode().toHTML();
  emit urlClicked( url );
  return KHTMLPart::urlSelected( url, button, state, _target, args, browserArgs );
}


