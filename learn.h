#ifndef LEARN_H
#define LEARN_H

#include <kmainwindow.h>
#include <kurl.h>
#include <qstringlist.h>

#include "dict.h"
#include "widgets.h"

class KAction;
class KConfig;
class KListView;
class KPushButton;
class QButtonGroup;
class QLabel;
class QListViewItem;
class QSplitter;
class QTabWidget;

class Learn : public KMainWindow
{
	Q_OBJECT
	
	public:
	Learn(Dict::Index *, QWidget *parent = 0, const char *name = 0);
	~Learn();

	bool closeWindow();

	signals:
	void destroyed(Learn *);

	public slots:
	void showKanji(QListViewItem *);
	void updateQuizConfiguration();

	protected:
	void closeEvent(QCloseEvent *);

	private slots:
	void read(const KURL &);
	void write(const KURL &);
	void next();
	void prev();
	void update();
	void updateGrade();
	void random();
	void add();
	void addAll();
	void add(Dict::Entry, bool noEmit = false);
	void del();
	void cheat();
	void tabChanged(QWidget *);
	void itemSelectionChanged();
	void save();
	void saveAs();

	void setDirty();
	void setClean();

	void open();

	void print();

	void updateQuiz();
	void answerClicked(int);
	void qnew();

	private:
	KURL filename;
	bool warnClose();

	QTabWidget *Tabs;
	QSplitter *listTop;
	QWidget *quizTop;

	ResultView *View;
	QValueList<Dict::Entry> list;
	QValueListIterator<Dict::Entry> current;

	Dict::Index *index;

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
	// if noEmit == true, don't emit dirty
	void addItem(QListViewItem *, bool noEmit = false);

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

#endif
