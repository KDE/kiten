#ifndef RAD_H
#define RAD_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qwidget.h>

class QCheckBox;
class QSpinBox;
class QPushButton;
class QListBox;

class Radical
{
	public:
	Radical(QString = QString::null, unsigned int = 0);

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

	QValueList<Radical> list;
};

class RadWidget : public QWidget
{
	Q_OBJECT

	public:
	RadWidget(Rad *, QWidget *parent = 0, const char *name = 0);
	~RadWidget();

	signals:
	// if totalStrokes == 0, then don't search by total strokes
	void set(QString &radical, unsigned int totalStrokes);

	private slots:
	void updateList(int);
	void apply();
	void totalClicked(void);
	void highlighted(int);
	
	private:
	QSpinBox *strokesSpin;
	QSpinBox *totalSpin;
	QPushButton *ok;
	QPushButton *cancel;
	QCheckBox *totalStrokes;
	QListBox *List;

	Rad *rad;
};

#endif
