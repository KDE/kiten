#ifndef DEFINF_H
#define DEFINF_H

#include <qstring.h>
#include <qvaluelist.h>
#include <qmap.h>

struct conj
{
	QString ending;
	QString replace;
	unsigned int num;
};

class Deinf
{
	public:
	Deinf();
	~Deinf();

	QString deinf(const QString &, QString &);

	private:
	QString curLine;
	QMap<unsigned int, QString> names;
	QValueList<conj> list;

	void loadLine(const QString &);

	bool body;
};

#endif
