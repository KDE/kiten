#ifndef WIDGETS_H
#define WIDGETS_H

#include <qwidget.h>
#include <qtextedit.h>
#include <qptrlist.h>
#include <klineedit.h>
#include <qstringlist.h>
#include <qguardedptr.h>
#include <kaction.h>

class KCompletion;
class Entry;
class Kanji;
class QString;
class QTabWidget;
class QListViewItem;
class KListView;
class QRegExp;
class QLabel;
class KPushButton;
class KStatusBar;
class QSpinBox;
class Dict;
class QToolButton;

class ResultView : public QTextEdit
{
	Q_OBJECT
	
	public:
	ResultView(QWidget *parent = 0, const char *name = 0);

	void addResult(Entry *, bool = false);
	void addKanjiResult(Kanji *);

	void addHeader(const QString &);
};

class Learn : public QWidget
{
	Q_OBJECT
	
	public:
	Learn(Dict *, QWidget *parent = 0, const char *name = 0);
	~Learn();

	signals:
	void listChanged();
	void listDirty();

	public slots:
	void showKanji(QListViewItem *);
	void readConfiguration();

	private slots:
	void next();
	void prev();
	void qprev();
	void updateGrade();
	void random();
	void gradeChange(int);
	void add();
	void del();
	void qdelete();
	void updateCaption(QListViewItem *);
	void close();
	void cheat();
	void writeConfiguration();

	void q1();
	void q2();
	void q3();
	void q4();
	void q5();
	void qnew();

	private:
	QTabWidget *Tabs;

	KPushButton *UpdateGrade;
	QSpinBox *GradeSpin;
	ResultView *View;
	KPushButton *Next;
	KPushButton *Prev;
	KPushButton *Rand;
	KPushButton *Cheat;
	QPtrList<Kanji> list;
	KStatusBar *StatusBar;

	KPushButton *Add;
	KPushButton *Close;

	Dict *dict;

	void update(Kanji *curKanji= 0);

	KListView *List;
	KPushButton *Save;
	KPushButton *Del;
	KPushButton *qDel;

	bool isMod;

	// Quiz, an app to itself in a tabwidget :)
	
	KPushButton *qPrev;
	KPushButton *Q1;
	KPushButton *Q2;
	KPushButton *Q3;
	KPushButton *Q4;
	KPushButton *Q5;
	QLabel *qKanji;

	QListViewItem *prevItem;
	QListViewItem *curItem;

	void qupdate();

	int seikai;
	QString shortenString(QString);

	QString randomMeaning();
	QStringList oldMeanings;
};

class EditAction : public KAction
{
  Q_OBJECT
public:
    EditAction( const QString& text, int accel, const QObject *receiver, const char *member, QObject* parent, const char* name );
    ~EditAction();

    virtual int plug( QWidget *w, int index = -1 );

    virtual void unplug( QWidget *w );

    QString text() { return m_combo->text(); }

public slots:
	void clear();

signals:
    void plugged();

private:
    QGuardedPtr<KLineEdit> m_combo;
    const QObject *m_receiver;
    const char *m_member;
};

#endif
