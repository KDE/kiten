/*
    This file is part of Kiten, a KDE Japanese Reference Tool...
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2005 Paul Temple <paul.temple@gmx.net>
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configuredialog.h"

#include <KLocalizedString>

#include <QString>
#include <QStringList>
#include <QTabWidget>
#include <QWidget>

#include "kitenconfig.h"

// Generated from UI files
#include "ui_configfont.h"
#include "ui_configsearching.h"

// Our template for managing individual dict type's settings
#include "dictionarypreferencedialog.h"
// The dictionary file selector widget
#include "configdictionaryselector.h"
// The sorting selection widget
#include "configsortingpage.h"
// To get the list of dictionary Types and to interface with dictType objects
#include "dictionarymanager.h"

ConfigureDialog::ConfigureDialog(QWidget *parent, KitenConfigSkeleton *config)
    : KConfigDialog(parent, QStringLiteral("Settings"), config)
{
    // TODO: Figure out why these pages are unmanaged... is this needed?
    addPage(makeDictionaryFileSelectionPage(nullptr, config), i18n("Dictionaries"), QStringLiteral("help-contents"));

    QWidget *widget;
    widget = new QWidget();
    Ui::ConfigSearching cs;
    cs.setupUi(widget);
    addPage(widget, i18n("Searching"), QStringLiteral("edit-find"));

    widget = new QWidget();
    Ui::ConfigFont cf;
    cf.setupUi(widget);
    addPage(widget, i18n("Font"), QStringLiteral("preferences-desktop-font"));

    addPage(makeDictionaryPreferencesPage(nullptr, config),
            i18nc("@title:group the settings for the dictionary display", "Display"),
            QStringLiteral("format-indent-more"));
    addPage(makeSortingPage(nullptr, config), i18n("Results Sorting"), QStringLiteral("format-indent-more"));

    setHelp(QString(), QStringLiteral("kiten"));

    connect(this, &KConfigDialog::widgetModified, this, &ConfigureDialog::updateConfiguration);
}

void ConfigureDialog::updateConfiguration()
{
    Q_EMIT settingsChanged(QString());
}

QWidget *ConfigureDialog::makeDictionaryFileSelectionPage(QWidget *parent, KitenConfigSkeleton *config)
{
    auto layoutWidget = new QWidget(parent);
    auto layout = new QVBoxLayout(layoutWidget);
    layout->setContentsMargins({});

    auto tabWidget = new QTabWidget(layoutWidget);
    tabWidget->setDocumentMode(true);
    layout->addWidget(tabWidget);

    for (const QString &dict : config->dictionary_list()) {
        QWidget *newTab = new ConfigDictionarySelector(dict, tabWidget, config);
        if (newTab) {
            connect(newTab, SIGNAL(widgetChanged()), this, SIGNAL(widgetModified()));
            connect(this, SIGNAL(updateWidgetsSignal()), newTab, SLOT(updateWidgets()));
            connect(this, SIGNAL(updateWidgetsDefaultSignal()), newTab, SLOT(updateWidgetsDefault()));
            connect(this, SIGNAL(updateSettingsSignal()), newTab, SLOT(updateSettings()));
            tabWidget->addTab(newTab, dict);
        }
    }

    return layoutWidget;
}

QWidget *ConfigureDialog::makeDictionaryPreferencesPage(QWidget *parent, KitenConfigSkeleton *config)
{
    auto layoutWidget = new QWidget(parent);

    auto layout = new QVBoxLayout();
    layout->setContentsMargins(style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                               style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                               style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                               style()->pixelMetric(QStyle::PM_LayoutBottomMargin));
    layoutWidget->setLayout(layout);

    QStringList dictTypes = DictionaryManager::listDictFileTypes();

    auto tabWidget = new QTabWidget();
    layout->addWidget(tabWidget);

    QMap<QString, DictionaryPreferenceDialog *> dialogList = DictionaryManager::generatePreferenceDialogs(config, parent);

    for (DictionaryPreferenceDialog *dialog : dialogList) {
        connect(this, &ConfigureDialog::updateWidgetsSignal, dialog, &DictionaryPreferenceDialog::updateWidgets);
        connect(this, &ConfigureDialog::updateWidgetsDefaultSignal, dialog, &DictionaryPreferenceDialog::updateWidgetsDefault);
        connect(this, &ConfigureDialog::updateSettingsSignal, dialog, &DictionaryPreferenceDialog::updateSettings);

        tabWidget->addTab(dialog, dialog->name());
    }

    return layoutWidget;
}

QWidget *ConfigureDialog::makeSortingPage(QWidget *parent, KitenConfigSkeleton *config)
{
    auto newPage = new ConfigSortingPage(parent, config);

    connect(newPage, &ConfigSortingPage::widgetChanged, this, &KConfigDialog::widgetModified);

    connect(this, &ConfigureDialog::updateWidgetsSignal, newPage, &ConfigSortingPage::updateWidgets);
    connect(this, &ConfigureDialog::updateWidgetsDefaultSignal, newPage, &ConfigSortingPage::updateWidgetsDefault);
    connect(this, &ConfigureDialog::updateSettingsSignal, newPage, &ConfigSortingPage::updateSettings);

    return newPage;
}

void ConfigureDialog::updateWidgets()
{
    Q_EMIT updateWidgetsSignal();
}

void ConfigureDialog::updateWidgetsDefault()
{
    Q_EMIT updateWidgetsDefaultSignal();
}

void ConfigureDialog::updateSettings()
{
    Q_EMIT updateSettingsSignal();
}

bool ConfigureDialog::isDefault()
{
    return false;
    // Always show the defaults button.... perhaps make a workaround later
}

#include "moc_configuredialog.cpp"
