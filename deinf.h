#ifndef DEFINF_H
#define DEFINF_H

#include <qstring.h>
#include <qstringlist.h>
#include <qvaluelist.h>
#include <qmap.h>

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
