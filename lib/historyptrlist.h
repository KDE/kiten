/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2006 Joseph Kerian <jkerian@gmail.com>
    SPDX-FileCopyrightText: 2006 Eric Kjeldergaard <kjelderg@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KITEN_HISTORYPTRLIST_H
#define KITEN_HISTORYPTRLIST_H

#include <QStringList>

#include "kiten_export.h"

class EntryList;

class KITEN_EXPORT HistoryPtrList
{
  public:
    /**
     * Construct a HistoryPtrList, this should be done early on
     */
    HistoryPtrList();
    /**
     * Destructor... kill all lists before going
     */
    virtual ~HistoryPtrList();
    /**
     * Add an item to the end of the history list and set it as
     * the current displayed item.
     */
    void addItem( EntryList *newItem );
    /**
     * Return a list of the entries. Note that this is usually
     * just a QStringList of all of the EntryList's DictQuery->toString() calls.
     */
    QStringList toStringList();
    /**
     * Return a list of the entries prior to the current one (not including
     * the current entry.
     */
    QStringList toStringListPrev();
    /**
     * Return a summary list that only includes those after the current
     */
    QStringList toStringListNext();
    /**
     * Add one to the current location, convenient for 'forward' buttons
     */
    void next( int distance = 1 );
    /**
     * Sub one from the current location, the counterpart to next()
     */
    void prev( int distance = 1 );
    /**
     * Return the current numerical 0-based location
     */
    int index();
    /**
     * Return the item at the location given by the parameter, and set it
     * to be the current history list item.
     */
    /**
     * Return the current item
     */
    EntryList *current();
    /**
     * Set the current item
     */
    void setCurrent( int i );
    /**
     * Return the total number of items in the list
     */
    int count();

  private:
    class Private;
    Private* const d;
};

#endif
