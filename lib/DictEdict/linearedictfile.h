/*
    This file is part of Kiten, a KDE Japanese Reference Tool
    SPDX-FileCopyrightText: 2001 Jason Katz-Brown <jason@katzbrown.com>
    SPDX-FileCopyrightText: 2008 Joseph Kerian <jkerian@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KITEN_LINEAREDICTFILE_H
#define KITEN_LINEAREDICTFILE_H

#include <QString>
#include <QStringList>
#include <QVector>

/**
 * A class for managing the EDICT formatted dictionaries with their
 * dictionary files. This class is not really designed for subclassing.
 */
class /* NO_EXPORT */ LinearEdictFile
{
  public:
    /**
     * Create and initialize this object
     */
    LinearEdictFile();
    ~LinearEdictFile();

    /**
     * Load a file, generate the index if it doesn't already exist
     */
    bool loadFile( const QString &filename );

    /**
     * Test if the file was properly loaded
     */
    bool valid() const;

    /**
     * Get everything that looks remotely like a given search string
     */
    QVector<QString> findMatches( const QString &searchString ) const;

  private:
    QStringList m_edict;
    bool        m_properlyLoaded;
};

#endif
