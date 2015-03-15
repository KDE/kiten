/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool                 *
 * Copyright (C) 2011 Daniel E. Moctezuma <democtezuma@gmail.com>            *
 *                                                                           *
 * This library is free software; you can redistribute it and/or             *
 * modify it under the terms of the GNU Library General Public               *
 * License as published by the Free Software Foundation; either              *
 * version 2 of the License, or (at your option) any later version.          *
 *                                                                           *
 * This library is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public License *
 * along with this library; see the file COPYING.LIB.  If not, write to      *
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,      *
 * Boston, MA 02110-1301, USA.                                               *
 *****************************************************************************/

#ifndef KANJIBROWSERVIEW_H
#define KANJIBROWSERVIEW_H

#include "ui_kanjibrowserview.h"

class EntryKanjidic;
class QAction;
class KanjiBrowser;
class QListWidgetItem;

class KanjiBrowserView : public QWidget, private Ui::KanjiBrowserView
{
  Q_OBJECT

  public:
        /**
         * Constructor.
         *
         * @param parent parent QWidget
         */
         KanjiBrowserView( QWidget *parent );
        ~KanjiBrowserView();

    /**
     * Initial setup.
     *
     * @param parent      parent to which we are going to add some QActions
     * @param kanji       hash containing kanji with its grades and number of strokes
     * @param kanjiGrades sorted list of grades found in KANJIDIC
     * @param strokeCount sorted list of strokes found in KANJIDIC
     */
    void setupView(   KanjiBrowser *parent
                    , const QHash< QString, QPair<int, int> > &kanji
                    , QList<int> &kanjiGrades
                    , QList<int> &strokeCount );

  signals:
    /**
     * Emitted when the status bar changed.
     *
     * @param text new text to put in the status bar
     */
    void statusBarChanged( const QString &text );

  public slots:
    /**
     * Load the font settings.
     */
    void loadSettings();

  private slots:
    /**
     * Called when the user changed the grade
     * of a kanji to be shown in the ComboBox.
     *
     * @param grade kanji grade to filter
     */
    void changeGrade( const int grade );
    /**
     * Change StackedWidget to "Kanji Information" page.
     */
    void changeToInfoPage();
    /**
     * Change StackedWidget to "Kanji List" page.
     */
    void changeToListPage();
    /**
     * Called when the user changed the strokes
     * of a kanji to be shown in the ComboBox.
     *
     * @param strokes number of strokes of a kanji to filter
     */
    void changeStrokeCount( const int strokes );
    /**
     * Search for an item (kanji) in KANJIDIC.
     *
     * @param item item to search in the dictionary
     */
    void searchKanji( QListWidgetItem *item );

  private:
    /**
     * QFont to CSS font style convertion.
     *
     * @param font font to be convert
     */
    QString convertToCSS( const QFont &font );
    /**
     * Reload the KListWidget items.
     */
    void reloadKanjiList();
    /**
     * Shows the information of a kanji as HTML in a QTextBrowser.
     *
     * @param kanji kanji that will be displayed as HTML
     */
    void showKanjiInformation( const EntryKanjidic *kanji );

    /**
     * Enumerations of our possible states in QStackedWidget.
     */
    enum Page
    {
      List,
      Info
    };

    enum Grade
    {
      AllJouyouGrades = 0,
      Grade7          = 7,
      Jinmeiyou       = 9
    };

    enum StrokesCount
    {
      NoStrokeLimit
    };

    /**
     * We need this as we are going to add some QActions to it.
     */
    KanjiBrowser                     *_parent;
    /**
     * We need to update this action's text from different functions.
     */
    QAction                          *_goToKanjiInfo;
    /**
     * Keep track of the current kanji being displayed in the Kanji Information page.
     */
    EntryKanjidic                    *_currentKanji;
    /**
     * A hash containing all the kanji (found in KANJIDIC) we need to filter.
     */
    QHash< QString, QPair<int, int> > _kanji;
    /**
     * A list containing all the kanji grades found in KANJIDIC.
     */
    QList<int>                        _gradeList;
    /**
     * A list containing all the number of strokes found in KANJIDIC.
     */
    QList<int>                        _strokesList;
    /**
     * Current kanji grades selected by the user to be filtered.
     */
    QList<int>                        _currentGradeList;
    /**
     * Current number of strokes selected by the user to be filtered.
     */
    QList<int>                        _currentStrokesList;
    /**
     * Font size of the kanji displayed in the Kanji Information page.
     */
    QVariant                          _kanjiSize;
    /**
     * Font used in kana (onyomi and kunyomi pronunciations of a kanji).
     */
    QFont                             _kanaFont;
    /**
     * Font used in information labels of a kanji (Grades, Strokes, etc.).
     */
    QFont                             _labelFont;
};

#endif
