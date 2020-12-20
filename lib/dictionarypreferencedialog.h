/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KITEN_DICTIONARYPREFERENCEDIALOG_H
#define KITEN_DICTIONARYPREFERENCEDIALOG_H

#include <QWidget>

#include "kiten_export.h"

/**
 * @short This abstract base class specifies the interface for dictionary
 * preference dialogs in user applications. The DictionaryManager class can be
 * asked for a list of these objects for the current session, and the appropriate
 * signal/slot connections can be made to handle dictionary preferences transparently
 * to the user application.
 *
 * One annoying caveat is that the user application still has to add preference elements
 * to it's own kcfg file, or the preference code will crash when trying to work with it.
 * Sadly, at the moment the only way to figure out what needs to be added to the kcfg is
 * to read the code, or the kiten.kcfg implementation.
 *
 * @author Joseph Kerian \<jkerian@gmail.com\>
 */
class KITEN_EXPORT DictionaryPreferenceDialog : public QWidget
{
  Q_OBJECT

  public:
    /**
     * Basic constructor. Used by internal classes only. Implement if you are
     * adding your own dictionary type.
     *
     * @param parent the parent widget, as per normal Qt Widget handling
     * @param name the name of your widget, as understood by the preference code
     */
    DictionaryPreferenceDialog( QWidget *parent, const QString &name );
    /**
     * Basic destructor
     */
    virtual ~DictionaryPreferenceDialog();
    /**
     * Get the dictionary type name associated with this dialog
     */
    QString name() const;

  public Q_SLOTS:
    /**
     * Connect the signal of your preferences dialog to this updateWidgets slot, to handle reading
     * preference settings cleanly. You can also call this slot directly from your own updateWidgets slot.
     */
    virtual void updateWidgets() = 0;
    /**
     * Connect the signal of your preferences dialog to this updateWidgetsDefault slot, to handle setting
     * preference settings back to their default settings easily.
     */
    virtual void updateWidgetsDefault() = 0;
    /**
     * Connect the signal of your preferences dialog to this updateSettings slot, to handle saving
     * user's preference information.
     */
    virtual void updateSettings() = 0;

  Q_SIGNALS:
    /**
     * When the user edits something on this preference page, this signal should be emitted
     */
    void widgetChanged();

  protected:
    /**
     * A place to store the name, passed in the constructor
     */
    QString m_name;
};

#endif
