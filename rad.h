#ifndef RAD_H
#define RAD_H

#include <qstring.h>
#include <qobject.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qwidget.h>

class QCheckBox;
class QLabel;
class QListBoxItem;
class QSpinBox;
class KPushButton;
class KListBox;
class QButtonGroup;

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
	QStringList kanjiByRad(const QString &);
	QStringList kanjiByRad(const QStringList &);
	Radical radByKanji(QString);
	unsigned int strokesByRad(QString);

	private:
	void loadLine(QString &);
	void load();

	QValueList<Radical> list;
	bool loaded;
};

class RadWidget : public QWidget
{
	Q_OBJECT

	public:
	RadWidget(Rad *, QWidget *parent = 0, const char *name = 0);
	~RadWidget();

	signals:
	// if totalStrokes == 0, then don't search by total strokes
	void set(const QStringList &radical, unsigned int totalStrokes, unsigned int totalStrokesErr);

	private slots:
	void updateList(int);
	void apply();
	void totalClicked(void);
	void selectionChanged();
	void hotlistClicked(int);
	void addToSelected(const QString &);
	void executed(QListBoxItem *);
	void removeSelected();
	void clearSelected();
	
	private:
	QSpinBox *strokesSpin;
	QSpinBox *totalSpin;
	QSpinBox *totalErrSpin;
	QLabel *totalErrLabel;
	KPushButton *ok;
	KPushButton *cancel;
	KPushButton *remove;
	KPushButton *clear;
	QButtonGroup *hotlistGroup;
	QCheckBox *totalStrokes;
	KListBox *List;
	KListBox *selectedList;
	QStringList selected;

	Rad *rad;

	unsigned int hotlistNum;
	QStringList hotlist;

	void numChanged();
};

#endif
