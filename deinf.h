#ifndef DEFINF_H
#define DEFINF_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>

namespace Deinf
{

struct Result
{
	QString ending;
	QString replace;
	unsigned int num;
};

class Index
{
	public:
	Index();

	QString deinflect(QString, QString &);

	private:
	QMap<unsigned int, QString> names;
	QValueList<Result> list;

	void loadLine(const QString &);
};

}

#endif
