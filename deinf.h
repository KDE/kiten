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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef DEFINF_H
#define DEFINF_H


namespace Deinf
{

struct Conjugation
{
	QString ending;
	QString replace;
	unsigned int num;
};

class Index
{
	public:
	Index();

	QStringList deinflect(QString, QStringList &);

	private:
	void load();
	QMap<unsigned int, QString> names;
	QValueList<Conjugation> list;
	bool loaded;
};

}

#endif
