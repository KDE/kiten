#ifndef RAD_H
#define RAD_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qwidget.h>

class QSpinBox;
class QPushButton;
class QListBox;

class Radical
{
	public:
	Radical(QString, unsigned int);

	QString radical() { return _Radical; };
	unsigned int strokes() { return Strokes; };
	QString kanji() { return Kanji; };

	void addKanji(QString &);

	private:
	QString _Radical;
	unsigned int Strokes;
	QString Kanji;
};

class Rad : public QObject
{
	Q_OBJECT
	
	public:
	Rad();
	~Rad();

	QStringList radByStrokes(unsigned int);
	QStringList kanjiByRad(QString &);

	private:
	void loadLine(QString &);

	QPtrList<Radical> list;

	Radical *curRadical;
};

class RadWidget : public QWidget
{
	Q_OBJECT

	public:
	RadWidget(Rad *, QWidget *parent = 0, const char *name = 0);
	~RadWidget();

	signals:
	void set(QString &, unsigned int);

	private slots:
	void updateList(int);
	void apply();
	
	private:
	QSpinBox *strokesSpin;
	QSpinBox *nonradSpin;
	QPushButton *ok;
	QPushButton *cancel;
	QListBox *List;

	Rad *rad;
};

#endif
