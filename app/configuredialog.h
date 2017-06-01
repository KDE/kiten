/*****************************************************************************
 * This file is part of Kiten, a KDE Japanese Reference Tool...              *
 * Copyright (C) 2001 Jason Katz-Brown <jason@katzbrown.com>                 *
 * Copyright (C) 2005 Paul Temple <paul.temple@gmx.net>                      *
 * Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>                      *
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

#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <KConfigDialog>

class KitenConfigSkeleton;
class QWidget;

class ConfigureDialog : public KConfigDialog
{
  Q_OBJECT

  public:
    explicit ConfigureDialog(  QWidget *parent = 0
                             , KitenConfigSkeleton *config = NULL );
    virtual ~ConfigureDialog();

  signals:
    void settingsChangedSignal();
    void updateWidgetsSignal();
    void updateWidgetsDefaultSignal();
    void updateSettingsSignal();

  private slots:
    void updateConfiguration();
    void updateSettings() Q_DECL_OVERRIDE;
    void updateWidgets() Q_DECL_OVERRIDE;
    void updateWidgetsDefault() Q_DECL_OVERRIDE;

  private:
    bool     isDefault() Q_DECL_OVERRIDE;
    QWidget *makeDictionaryPreferencesPage( QWidget *, KitenConfigSkeleton* );
    QWidget *makeDictionaryFileSelectionPage( QWidget *, KitenConfigSkeleton* );
    QWidget *makeSortingPage( QWidget *, KitenConfigSkeleton* );
};

#endif
