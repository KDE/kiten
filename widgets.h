#ifndef WIDGETS_H
#define WIDGETS_H

#include <qwidget.h>
#include <qtextbrowser.h>
#include <qptrlist.h>
#include <kaction.h>
#include <kstdaction.h>
#include <klineedit.h>
#include <kmainwindow.h>
#include <qstringlist.h>
#include <qguardedptr.h>
#include <kaction.h>

#include "rad.h"

class QButtonGroup;
class QString;
class QSplitter;
class QTabWidget;
class QListViewItem;
class KListView;
class QRegExp;
class KConfig;
class QLabel;
class KPushButton;
class KStatusBar;
class QSpinBox;
class QToolButton;

#include "dict.h"

class ResultView : public QTextBrowser
{
	Q_OBJECT
	
	public:
	ResultView(bool, QWidget *parent = 0, const char *name = 0);

	void addResult(Dict::Entry, bool = false);
	void addKanjiResult(Dict::Entry, bool = false, Radical = Radical());

	void addHeader(const QString &, unsigned int degree = 3);

	public slots:
	void print(QString = QString::null);
	void append(const QString &);
	void flush();
	void clear();

	void updateFont();

	private:
	QString putchars(const QString &);
	QString printText;

	bool links;
};

class eEdit : public KMainWindow
{
	Q_OBJECT
	
	public:
	eEdit(const QString &, QWidget *parent = 0, const char *name = 0);
	~eEdit();

	private slots:
	void add();
	void save();
	void del();
	void disable();
	void openFile(const QString &);

	private:
	KListView *List;
	QString filename;
	KStatusBar *StatusBar;
	KAction *addAct;
	KAction *removeAct;
	KAction *saveAct;
	bool isMod;
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
	void updateQuizConfiguration();

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void next();
	void prev();
	void update();
	void updateGrade();
	void random();
	void add();
	void addAll();
	void externAdd(Dict::Entry);
	void del();
	void cheat();
	void tabChanged(QWidget *);
	void itemSelectionChanged();
	void saveAs();

	void open();
	void openDefault();

	void print();

	void updateQuiz();
	void answerClicked(int);
	void qnew();

	private:
	QString filename;
	bool warnClose();
	KConfig *config;

	QTabWidget *Tabs;
	QSplitter *listTop;
	QWidget *quizTop;

	ResultView *View;
	QValueList<Dict::Entry> list;
	QValueListIterator<Dict::Entry> current;
	KStatusBar *StatusBar;

	Dict::Index *index;

	KListView *List;
	KPushButton *Save;
	KPushButton *Del;
	KPushButton *qDel;

	bool isMod;
	bool noRead;

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
	bool nogood;
	QString shortenString(QString);

	// Creates a random meaning not on the lists and adds the meaning
	// to the list.
	QString randomMeaning(QStringList &oldMeanings);

	KAction *forwardAct;
	KAction *printAct;
	KAction *backAct;
	KAction *cheatAct;
	KAction *saveAct;
	KAction *openAct;
	KAction *openDefaultAct;
	KAction *saveAsAct;
	KAction *addAct;
	KAction *addAllAct;
	KAction *removeAct;
	KAction *randomAct;
	KListAction *gradeAct;

	int getCurrentGrade(void);
	void setCurrentGrade(int grade);

	int quizOn;
	int guessOn;
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
	void setText(const QString &text);

	QGuardedPtr<KLineEdit> editor();

	public slots:
	void clear();
	void insert(QString);

	signals:
	void plugged();

private:
    QGuardedPtr<KLineEdit> m_combo;
    const QObject *m_receiver;
    const char *m_member;
};

#endif
