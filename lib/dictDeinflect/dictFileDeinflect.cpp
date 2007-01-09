/* This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
           (C) 2006  Joseph Kerian <jkerian@gmail.com>
			  (C) 2006  Eric Kjeldergaard <kjelderg@gmail.com>

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

#include <QtCore/QHash>
#include <QtCore/QList>

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qfile.h>
#include <qregexp.h>
#include <qtextcodec.h>
#include <QTextStream>
#include "dictFileDeinflect.h"

//This is a very primative form of information hiding
//But C++ can get stupid with static QT objects...
//So this turns out to be much, much easier
//TODO: Fix this for thread safety/functionality (I'm presuming it's broken atm)
typedef struct Conjugation
{
	QString ending; //The ending we are replacing
	QString replace; //The replacement (dictionary form) ending
	QString label; //What this type of replacement is called
}Conjugation;

//Declare our constants
QHash<long,QString> names;
QList<Conjugation> list;

bool dictFileDeinflect::loadDictionary(const QString &file, const QString &name)
{
	if (list.count() > 0)
		return true;

	list.clear();
	names.clear();
	//Find the file
	KStandardDirs *dirs = KGlobal::dirs();
	QString vconj = dirs->findResource("data", file);
	if (vconj.isNull())
	{
		KMessageBox::error(0, i18n("Verb deinflection information not found, so verb deinflection cannot be used."));
		return false;
	}

	//Open the file
	QFile f(vconj);
	if (!f.open(QIODevice::ReadOnly))
	{
		KMessageBox::error(0, i18n("Verb deinflection information could not be loaded, so verb deinflection cannot be used."));
		return false;
	}


	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("eucJP"));

	//The file starts out with a number -> name list of the conjugation types
	//In the format "#[#].\tNAME\n"
	//The next section beginning is flagged with a $ at the beginning of the line
	for(QString text = t.readLine(); !t.atEnd() && text.at(0) != '$';
			text = t.readLine())
	{
		if(text.at(0) != '#')
		{
			long number = text.left(2).trimmed().toULong();
			QString name = text.right(text.length() - 2).trimmed();
			names[number] = name;
		}
	}

	//Now for the actual conjugation data
	//Format is "ENDING_TO_REPLACE\tREPLACEMENT\tNUMBER_FROM_LIST_ABOVE\n"
	for(QString text = t.readLine(); !text.isEmpty(); text = t.readLine())
	{
		if(text.at(0) != '#') //We still have these comments everywhere
		{
			QStringList things( text.split(QChar('\t') ));

			Conjugation conj;
			conj.ending = things.first();
			conj.replace = (things.at(1));
			conj.label = names.find(things.last().toULong()).value();

			list.append(conj);
		}
	}

	f.close();

	dictionaryName = name;

	return true;
}


EntryList *dictFileDeinflect::doSearch(const dictQuery &query)
{
	if(dictionaryName.isEmpty())
		return NULL;

	QStringList ret;
	foreach(Conjugation it, list)
	{
		if(text.endsWith(it.ending)) {
			name.append((it.label)); // Add to our list of explanations

			//kdDebug() << "match ending: " << (*it).ending << "; replace: " << (*it).replace << "; name: " << names[(*it).num] << endl;

			QString tmp(text);
			tmp.truncate(text.length() - (it).ending.length());
			tmp = tmp + (it).replace;
			ret.append(tmp);
		}
	}

	return ret;
}
