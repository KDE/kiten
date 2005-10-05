/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#ifndef ASYNDETA_H
#define ASYNDETA_H

class KConfig;

namespace Dict
{
	class Index;
}

class KDE_EXPORT Asyndeta
{
public:
	// creates index
	Asyndeta();

	~Asyndeta();

	Dict::Index *retrieveIndex();

	// read information concerning dictionary files
	// and set the index to use them
	void readKitenConfiguration();

	static QString personalDictionaryLocation();

protected:
	static int refCount;
	static Dict::Index *index;
};

#endif
