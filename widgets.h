#ifndef WIDGETS_H
#define WIDGETS_H

#include <klineedit.h>
#include <kmainwindow.h>
#include <qguardedptr.h>
#include <ktextbrowser.h>
#include <qwidget.h>

class KAction;
class KListView;
class KStatusBar;

#include "dict.h"
#include "rad.h"

class ResultView : public KTextBrowser
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
	void setBasicMode(bool yes) { basicMode = yes; }

	void updateFont();

	private:
	QString putchars(const QString &);
	QString printText;

	bool links;
	bool basicMode;
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
