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
class KAction;
class KanjiBrowser;
class QListWidgetItem;

class KanjiBrowserView : public QWidget, private Ui::KanjiBrowserView
{
  Q_OBJECT

  public:
         KanjiBrowserView( QWidget *parent );
        ~KanjiBrowserView();

    /**
     * Initial setup.
     */
    void setupView(   KanjiBrowser *parent
                    , const QHash< QString, QPair<int, int> > &kanji
                    , QList<int> &kanjiGrades
                    , QList<int> &strokeCount );

  signals:
    /**
     * Emited when the status bar changed.
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
     */
    void changeStrokeCount( const int strokes );
    /**
     * Search for an item (kanji) in KANJIDIC.
     */
    void searchKanji( QListWidgetItem *item );

  private:
    /**
     * QFont to CSS font style convertion.
     */
    QString convertToCSS( const QFont &font );
    /**
     * Reload the KListWidget items.
     */
    void reloadKanjiList();
    /**
     * Shows the information of a kanji as HTML in a QTextBrowser.
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

    KanjiBrowser                     *_parent;
    KAction                          *_goToKanjiInfo;
    EntryKanjidic                    *_currentKanji;
    QHash< QString, QPair<int, int> > _kanji;
    QList<int>                        _gradeList;
    QList<int>                        _strokesList;
    QList<int>                        _currentGradeList;
    QList<int>                        _currentStrokesList;
    QVariant                          _kanjiSize;
    QFont                             _kanaFont;
    QFont                             _labelFont;
};

#endif
