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

using namespace Deinf;

Index::Index()
{
	KStandardDirs *dirs = KGlobal::dirs();
	QString vconj = dirs->findResource("appdata", "vconj");
	if (vconj == QString::null)
	{
		KMessageBox::error(0, i18n("Verb deinflection information not found, so verb de-inflection cannot be used."));
		return;
	}

	QFile f(vconj);
	
	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Verb deinflection information not be loaded, so verb de-inflection cannot be used."));
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
	for(QString text = t.readLine(); text; text = t.readLine())
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
}

namespace
{
QStringList possibleConjugations(const QString &text)
{
	QStringList endings;
	for(unsigned i = 0; i < text.length(); ++i)
		endings.append(text.right(i));
	return endings;
}
}

QString Index::deinflect(QString text, QString &name)
{
	QStringList endings = possibleConjugations(text);
	for (QValueListIterator <Conjugation> it = list.begin(); it != list.end(); ++it)
	{
		QStringList::Iterator ending = endings.find((*it).ending);
		if(ending != endings.end()) // a match
		{
			name = names[(*it).num];
			text.replace(QRegExp((*ending) + "$"), (*it).replace);
			return text;
		}
	}

	name = QString::null;
	return text;
}

