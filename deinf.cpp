#include <kglobal.h>
#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qmap.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include "deinf.h"

Deinf::Deinf()
{
	body = false;

	KStandardDirs *dirs = KGlobal::dirs();
	QString vconj = dirs->findResource("appdata", "vconj");
	if (vconj == QString::null)
	{
		KMessageBox::error(0, i18n("Verb deinflection information not found, so verb de-inflection cannot be used."));
		return;
	}

	QFile f(vconj);
	QString s;
	
	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Verb deinflection information not be loaded, so verb de-inflection cannot be used."));
		return;
	}

	QTextStream t(&f);
	while (!t.eof())
	{
		s = t.readLine();

		loadLine(s);
	}

	f.close();
}

Deinf::~Deinf()
{
}

void Deinf::loadLine(const QString &text)
{
	QChar first = text.at(0);
	if (first == '#')
		return;
	if (first == '$')
	{
		body = true;
		return;
	}
	if (!body)
	{
		unsigned int number = text.left(2).stripWhiteSpace().toUInt();
		QString name = text.right(text.length() - 2).stripWhiteSpace();

		names[number] = name;
		return;
	}
	
	QStringList things(QStringList::split(QChar('\t'), text));

	conj Conj;
	Conj.ending = things.first();
	Conj.replace = (*things.at(1));
	Conj.num = things.last().toUInt();

	list.append(Conj);
}

QString Deinf::deinf(const QString &text, QString &name)
{
	QValueList<conj>::Iterator it;
	unsigned int len = text.length();
	int i;
	for (i = len; i > 0; i--)
	{
		for (it = list.begin(); it != list.end(); ++it)
		{
			//kdDebug() << "ending = " << (*it).ending << endl;
			unsigned int rlen = (*it).ending.length();
			if (rlen >= len)
				continue;

			if (text.right(i).find((*it).ending) == 0)
			{
				name = names[(*it).num];
				return (text.left(len - i) + (*it).replace);
			}
		}
	}

	name = QString::null;
	return text;
}

