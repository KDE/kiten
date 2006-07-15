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

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qfile.h>
#include <qregexp.h>
#include <qtextcodec.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <QTextStream>
#include "deinf.h"

//This is a very primative form of information hiding
//But C++ can get stupid with static QT objects...
//So this turns out to be much, much easier
typedef struct Conjugation
{
	QString ending; //The ending we are replacing
	QString replace; //The replacement (dictionary form) ending
	QString *label; //What this type of replacement is called
}Conjugation;

//Declare our constants
Q3IntDict<QString> names;
Q3ValueList<Conjugation> list;

deinf::deinf()
{
	load();
}

bool deinf::load()
{
	if (list.count() > 0)
		return true;

	list.clear();
	names.clear();
	//Find the file
	KStandardDirs *dirs = KGlobal::dirs();
	QString vconj = dirs->findResource("data", "kiten/vconj");
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
	for(QString text = t.readLine(); !t.atEnd() && text.at(0) != '$'; text = t.readLine()) //KDE4 CHANGE
	{
		if(text.at(0) != '#')
		{
			long number = text.left(2).trimmed().toULong();
			QString *name = new QString(text.right(text.length() - 2).trimmed());
			names.insert(number, name);
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
			conj.replace = (things.at(1)); //KDE4 CHANGE
			conj.label = names.find(things.last().toULong());

			list.append(conj);
		}
	}

	f.close();
	names.setAutoDelete(true);

	return true;
}


QStringList deinf::deinflect(const QString &text, QStringList &name)
{
	if(!load()) //Load the dictionary system if we havne't done so already
		return QStringList();
		
	QStringList ret;

	Q3ValueListIterator<Conjugation> it = list.begin();
	for ( ; it != list.end(); ++it)
	{
		if(text.endsWith((*it).ending)) {
			name.append((*(*it).label)); // Add to our list of explanations

			//kdDebug() << "match ending: " << (*it).ending << "; replace: " << (*it).replace << "; name: " << names[(*it).num] << endl;

			QString tmp(text);
			tmp.truncate(text.length() - (*it).ending.length());
			tmp = tmp + (*it).replace;
			ret.append(tmp);
		}
	}

	return ret;
}

