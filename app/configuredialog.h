/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CONFIGUREDIALOG_H
#define CONFIGUREDIALOG_H

#include <KConfigDialog>

class KitenConfigSkeleton;
class QWidget;

class ConfigureDialog : public KConfigDialog
{
    Q_OBJECT

public:
    explicit ConfigureDialog(QWidget *parent = nullptr, KitenConfigSkeleton *config = nullptr);
    ~ConfigureDialog() override = default;

Q_SIGNALS:
    void settingsChangedSignal();
    void updateWidgetsSignal();
    void updateWidgetsDefaultSignal();
    void updateSettingsSignal();

private Q_SLOTS:
    void updateConfiguration();
    void updateSettings() override;
    void updateWidgets() override;
    void updateWidgetsDefault() override;

private:
    bool isDefault() override;
    QWidget *makeDictionaryPreferencesPage(QWidget *, KitenConfigSkeleton *);
    QWidget *makeDictionaryFileSelectionPage(QWidget *, KitenConfigSkeleton *);
    QWidget *makeSortingPage(QWidget *, KitenConfigSkeleton *);
};

#endif
