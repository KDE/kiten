/* This file is part of kiten, a KDE Japanese Reference Tool
   Copyright (C) 2006 Joseph Kerian <jkerian@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef RADICALFILE_H
#define RADICALFILE_H

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QMultiMap>

#include "radicalbutton.h"

class Radical : public QString {
protected:
	unsigned int stroke_count;
	QSet<QString> kanji;
	QSet<QString> componants;
public:
	Radical() : stroke_count(0) {}
	explicit Radical(const QString &irad, unsigned int iStrokes=0)
		: QString(irad.at(0)), stroke_count(iStrokes) {}
	const QSet<QString>	&getKanji() const {return kanji;}
	void addKanji(const QSet<QString> &newKanji) {kanji += newKanji;}
	unsigned int strokes() const {return stroke_count;}
	bool operator<(const Radical &other) const {
		return this->stroke_count < other.stroke_count;
	}
};

class Kanji : public QString {
protected:
	unsigned int stroke_count;
	QSet<QString> componants;
public:
	Kanji() : stroke_count(0) {}
	Kanji(const QString &ikanji, const QSet<QString> &radicals)
		: QString(ikanji.at(0)), stroke_count(0)     { componants = radicals; }
	const QSet<QString> &getRadicals() const {return componants;}
	void addRadical(const QString &it) {componants +=it;}
	void calculateStrokes(const QList<Radical> &lookup) {
		stroke_count = 0;
		foreach( const QString &it, componants )
			stroke_count += lookup.value(lookup.indexOf(Radical(it))).strokes();
	}
	unsigned int strokes() const
		{return stroke_count;}
	bool operator<(const Kanji &other) const {
		return this->stroke_count < other.stroke_count;
	}
};

class radicalFile {
private:
	QHash<QString, Radical> m_radicals;
	QHash<QString, Kanji> m_kanji;

public:
	explicit radicalFile(QString &radkfile);
	bool loadRadicalFile(QString &radkfile);

	QMultiMap<int,Radical> *mapRadicalsByStrokes() const;
	QSet<Kanji> kanjiContainingRadicals(QSet<QString> &radicallist) const;
	QSet<QString> radicalsInKanji(QSet<Kanji> &kanjilist) const;
};

#endif
