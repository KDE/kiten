#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmap.h>
#include <qregexp.h>
#include <qstringlist.h>
#include <qtextcodec.h>
#include <qvaluelist.h>
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
	QString vconj = dirs->findResource("appdata", "vconj");
	if (vconj.isNull())
	{
		KMessageBox::error(0, i18n("Verb deinflection information not found, so verb deinflection cannot be used."));
		return;
	}

	QFile f(vconj);

	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Verb deinflection information could not be loaded, so verb deinflection cannot be used."));
		return;
	}

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("eucJP"));

	for(QString text = t.readLine(); !t.eof() && text.at(0) != '$'; text = t.readLine())
	{
		if(text.at(0) != '#')
		{
			unsigned int number = text.left(2).stripWhiteSpace().toUInt();
			QString name = text.right(text.length() - 2).stripWhiteSpace();

			names[number] = name;
		}
	}
	for(QString text = t.readLine(); !text.isEmpty(); text = t.readLine())
	{
		if(text.at(0) != '#')
		{
			QStringList things(QStringList::split(QChar('\t'), text));

			Conjugation conj;
			conj.ending = things.first();
			conj.replace = (*things.at(1));
			conj.num = things.last().toUInt();

			list.append(conj);
		}
	}

	f.close();
	loaded = true;
}

namespace
{
QStringList possibleConjugations(const QString &text)
{
	QStringList endings;
	for (unsigned i = 0; i < text.length(); ++i)
		endings.append(text.right(i));
	return endings;
}
}

QStringList Deinf::Index::deinflect(QString text, QStringList &name)
{
	load();
	QStringList endings = possibleConjugations(text);
	QStringList ret;

	for (QValueListIterator <Conjugation> it = list.begin(); it != list.end(); ++it)
	{
		QStringList matches(endings.grep(QRegExp(QString("^") + (*it).ending)));

		if (matches.size() > 0) // a match
		{
			name.append(names[(*it).num]);

			//kdDebug() << "match ending: " << (*it).ending << "; replace: " << (*it).replace << "; name: " << names[(*it).num] << endl;

			QString tmp(text);
			tmp.replace(QRegExp((*it).ending + "*", false, true), (*it).replace);
			ret.append(tmp);
		}
	}

	return ret;
}

