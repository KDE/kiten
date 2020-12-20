/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2011 Daniel E. Moctezuma <democtezuma@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KANJIBROWSER_H
#define KANJIBROWSER_H

#include <KXmlGuiWindow>

class DictFileKanjidic;
class KanjiBrowserConfigSkeleton;
class KanjiBrowserView;

class KanjiBrowser : public KXmlGuiWindow
{
  friend class KanjiBrowserView;

  Q_OBJECT

  public:
         KanjiBrowser();
        ~KanjiBrowser();

  private slots:
    /**
     * Change/update the status bar.
     */
    void changeStatusBar( const QString &text );
    /**
     * Show a font preferences dialog.
     */
    void showPreferences();

  private:
    /**
     * Load KANJIDIC dictionary and parse the necessary information and
     * calls KanjiBrowserView::setupView() member function to finish the setup.
     * This should be called only once at initial setup of KanjiBrowser or to
     * reload the dictionary in case of updates.
     */
    void loadKanji();

    KanjiBrowserConfigSkeleton *_config;
    KanjiBrowserView           *_view;
    DictFileKanjidic           *_dictFileKanjidic;
};

#endif
