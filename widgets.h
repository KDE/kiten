#ifndef WIDGETS_H
#define WIDGETS_H

#include <qwidget.h>
#include <qtextedit.h>
#include <qptrlist.h>
#include <klineedit.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kmainwindow.h>
#include <qstringlist.h>
#include <qguardedptr.h>
#include <kaction.h>

class QButtonGroup;
class QString;
class QSplitter;
class QTabWidget;
class QListViewItem;
class KListView;
class QRegExp;
class QLabel;
class KPushButton;
class KStatusBar;
class QSpinBox;
class QToolButton;

#include "dict.h"

class ResultView : public QTextEdit
{
	Q_OBJECT
	
	public:
	ResultView(QWidget *parent = 0, const char *name = 0);

	void addResult(Dict::Entry, bool = false);
	void addKanjiResult(Dict::Kanji);

	void addHeader(const QString &, unsigned int degree = 3);
};

class Learn : public KMainWindow
{
	Q_OBJECT
	
	public:
	Learn(Dict::Index *, QWidget *parent = 0, const char *name = 0);
	~Learn();

	signals:
	void listChanged();
	void listDirty();

	public slots:
	void showKanji(QListViewItem *);
	void readConfiguration();
	void writeConfiguration();

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void next();
	void prev();
	void updateGrade();
	void random();
	void add();
	void addAll();
	void externAdd(Dict::Kanji);
	void del();
	void cheat();
	void tabChanged(QWidget *);
	void itemSelectionChanged();

	void updateQuiz();

	void answerClicked(int);
	void qnew();

	private:
	QTabWidget *Tabs;
	QSplitter *listTop;
	QWidget *quizTop;

	ResultView *View;
	QValueList<Dict::Kanji> list;
	QValueListIterator<Dict::Kanji> current;
	KStatusBar *StatusBar;

	Dict::Index *index;

	void update();

	KListView *List;
	KPushButton *Save;
	KPushButton *Del;
	KPushButton *qDel;

	bool isMod;

	// Quiz, an app to itself in a tabwidget :)

	static const int numberOfAnswers;
	QButtonGroup *answers;
	QLabel *qKanji;

	QListViewItem *prevItem;
	QListViewItem *curItem;
	// if init == true, don't emit dirty
	void addItem(QListViewItem *, bool init = false);

	void qupdate();

	int seikai;
	QString shortenString(QString);

	// Creates a random meaning not on the lists and adds the meaning
	// to the list.
	QString randomMeaning(QStringList &oldMeanings);

	KAction *forwardAct;
	KAction *backAct;
	KAction *cheatAct;
	KAction *saveAct;
	KAction *addAct;
	KAction *addAllAct;
	KAction *removeAct;
	KAction *randomAct;
	KListAction *gradeAct;

	int getCurrentGrade(void);
	void setCurrentGrade(int grade);
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
	void insert(QString &);

signals:
    void plugged();

private:
    QGuardedPtr<KLineEdit> m_combo;
    const QObject *m_receiver;
    const char *m_member;
};

#endif
