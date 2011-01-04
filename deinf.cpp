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

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <tqfile.h>
#include <tqregexp.h>
#include <tqtextcodec.h>
#include "deinf.h"

Deinf::Index::Index()
{
	loaded = false;
}

void Deinf::Index::load()
{
	if (loaded)
		return;

	KStandardDirs *dirs = KGlobal::dirs();
	TQString vconj = dirs->findResource("data", "kiten/vconj");
	if (vconj.isNull())
	{
		KMessageBox::error(0, i18n("Verb deinflection information not found, so verb deinflection cannot be used."));
		return;
	}

	TQFile f(vconj);

	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Verb deinflection information could not be loaded, so verb deinflection cannot be used."));
		return;
	}

	TQTextStream t(&f);
	t.setCodec(TQTextCodec::codecForName("eucJP"));

	for(TQString text = t.readLine(); !t.eof() && text.at(0) != '$'; text = t.readLine())
	{
		if(text.at(0) != '#')
		{
			unsigned int number = text.left(2).stripWhiteSpace().toUInt();
			TQString name = text.right(text.length() - 2).stripWhiteSpace();

			names[number] = name;
		}
	}
	for(TQString text = t.readLine(); !text.isEmpty(); text = t.readLine())
	{
		if(text.at(0) != '#')
		{
			TQStringList things(TQStringList::split(TQChar('\t'), text));

			Conjugation conj;
			conj.ending = things.first();
			conj.tqreplace = (*things.at(1));
			conj.num = things.last().toUInt();

			list.append(conj);
		}
	}

	f.close();
	loaded = true;
}

namespace
{
TQStringList possibleConjugations(const TQString &text)
{
	TQStringList endings;
	for (unsigned i = 0; i < text.length(); ++i)
		endings.append(text.right(i));
	return endings;
}
}

TQStringList Deinf::Index::deinflect(const TQString &text, TQStringList &name)
{
	load();
	TQStringList endings = possibleConjugations(text);
	TQStringList ret;

	for (TQValueListIterator <Conjugation> it = list.begin(); it != list.end(); ++it)
	{
		TQStringList matches(endings.grep(TQRegExp(TQString("^") + (*it).ending)));

		if (matches.size() > 0) // a match
		{
			name.append(names[(*it).num]);

			//kdDebug() << "match ending: " << (*it).ending << "; tqreplace: " << (*it).tqreplace << "; name: " << names[(*it).num] << endl;

			TQString tmp(text);
			tmp.tqreplace(TQRegExp((*it).ending + "*", false, true), (*it).tqreplace);
			ret.append(tmp);
		}
	}

	return ret;
}

