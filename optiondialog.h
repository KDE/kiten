#ifndef OPTIONDIALOG_H
#define OPTIONDIALOG_H

#include <kdialogbase.h>
#include <qwidget.h>

class KListView;
class QString;
class QLabel;
class QPushButton;
class QCheckBox;
class KGlobalAccel;
class DictList;

class ConfigureDialog : public KDialogBase
{
	Q_OBJECT

	public:
	ConfigureDialog(KGlobalAccel *accel, QWidget *parent = 0, char *name = 0, bool modal = 0);
	~ConfigureDialog();

	protected slots:
	virtual void slotOk();
	virtual void slotApply();

	private slots:
	void readConfig();
	void writeConfig();

	private:
	DictList *DictDictList;
	DictList *KanjiDictList;
	QCheckBox *wholeWordCB;
	QCheckBox *caseSensitiveCB;
	QCheckBox *startLearnCB;

	KGlobalAccel *Accel;
	KKeyEntryMap m_keys;
	
	signals:
	void valueChanged();
};

class DictList : public QWidget
{
	Q_OBJECT
	public:
	DictList(const QString &configkey, QWidget *parent = 0, char *name = 0);
	void readConfig();
	void writeConfig();

	private slots:
	void add();
	void del();

	private:
	QPushButton *AddButton;
	QPushButton *DelButton;

	KListView *List;

	QString _configKey;
};

#endif
