/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "resultsview.h"
#include "kitenconfig.h"
#include <KColorScheme>
#include <QScrollBar>

ResultsView::ResultsView( QWidget *parent, const char *name )
: QTextBrowser( parent )
, _scrollValue( 0 )
{
  Q_UNUSED( name );
  setOpenLinks( false );
  connect(this, &QTextBrowser::anchorClicked, this, [=](const QUrl &url){
      emit urlClicked( url.toString() );
  });
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
  _printText.clear();
}

void ResultsView::doScroll()
{
  verticalScrollBar()->setValue(_scrollValue);
}

/**
 * Flushes the printText to screen
 */
void ResultsView::flush()
{
  const QString content = QString("<head><style>%1</style></head><body>%2</body>").arg(generateCSS()).arg(_printText);
  setHtml( content );
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
                  ".odd { background-color: %10 }"
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
  Q_UNUSED( title )
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
  const QString content = QString("<head><style>%1</style></head><body>%2</body>").arg(generateCSS()).arg(text);
  setHtml( content );
//  qDebug() << "Set HTML to " << content;
}

void ResultsView::setLaterScrollValue( int scrollValue )
{
  this->_scrollValue = scrollValue;
}


