#include <kapp.h>
#include <kconfig.h>
#include <kkeydialog.h>
#include <kglobalaccel.h>
#include <kglobal.h>
#include <kdebug.h>
#include <klocale.h>
#include <kconfig.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>
#include <kurllabel.h>
#include <klistview.h>

#include <qlayout.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qfileinfo.h>

#include "optiondialog.h"

ConfigureDialog::ConfigureDialog(KGlobalAccel *accel, QWidget *parent, char *name, bool modal)
  :KDialogBase(KDialogBase::TreeList, i18n("Configure"), Apply|Ok|Cancel, Ok, parent, name, modal )
{
	Accel = accel;

	setHelp("kiten/index.html", QString::null);

	QStringList list;
	list = i18n("Dictionaries");
	QFrame *Page0 = addPage(i18n("Dictionaries"));
	QVBoxLayout *descBox = new QVBoxLayout(Page0);
	descBox->addWidget(new QLabel(i18n("Kiten includes Edict for a regular word search.\nFor Kanji searching, Kanjidic is included.\n\nFeel free to add your own extras, by adding them in the\nconfiguration pages below this one."), Page0));
	descBox->addWidget(new KURLLabel("http://www.csse.monash.edu.au/~jwb/edict.html", i18n("Edict information page"), Page0));
	descBox->addWidget(new KURLLabel("http://www.csse.monash.edu.au/~jwb/kanjidic.html", i18n("Kanjidic information page"), Page0));

	list.append(i18n("Edict"));
	QFrame *Page1 = addPage(list);
	QVBoxLayout *dictBox = new QVBoxLayout(Page1);

	DictDictList = new DictList("edict", Page1);
	dictBox->addWidget(DictDictList);

	list = i18n("Dictionaries");
	list.append(i18n("Kanjidic"));
	QFrame *Page2 = addPage(list);
	QVBoxLayout *kanjiDictBox = new QVBoxLayout(Page2);

	KanjiDictList = new DictList("kanjidic", Page2);
	kanjiDictBox->addWidget(KanjiDictList);

	connect(this, SIGNAL(cancelClicked()), this, SLOT(readConfig()));

	QFrame *Page3 = addPage(i18n("Searching"));
	QVBoxLayout *searchBox = new QVBoxLayout(Page3);

	caseSensitiveCB = new QCheckBox(i18n("Case sensitive searches"), Page3);
	searchBox->addWidget(caseSensitiveCB);
	wholeWordCB = new QCheckBox(i18n("Match only whole english word"), Page3);
	searchBox->addWidget(wholeWordCB);

	QFrame *Page4 = addPage(i18n("Global Keys"));
	m_keys = Accel->keyDict();
	QVBoxLayout *layout = new QVBoxLayout(Page4);
	layout->addWidget(new KKeyChooser(&m_keys, Page4));

	readConfig();
}

ConfigureDialog::~ConfigureDialog()
{
}

void ConfigureDialog::readConfig()
{
	KConfig *config = kapp->config();

	DictDictList->readConfig();
	KanjiDictList->readConfig();

	config->setGroup("Dictionaries");

	config->setGroup("Searching Options");
	caseSensitiveCB->setChecked(config->readBoolEntry("caseSensitive", false));
	wholeWordCB->setChecked(config->readBoolEntry("wholeWord", true));
}

void ConfigureDialog::writeConfig()
{
	KConfig *config = kapp->config();

	KanjiDictList->writeConfig();
	DictDictList->writeConfig();

	config->setGroup("Dictionaries");

	config->setGroup("Searching Options");
	config->writeEntry("caseSensitive", caseSensitiveCB->isChecked());
	config->writeEntry("wholeWord", wholeWordCB->isChecked());

	config->sync();

	Accel->setKeyDict(m_keys);
	Accel->writeSettings();
}

void ConfigureDialog::slotOk()
{
	slotApply();
	accept();
}

void ConfigureDialog::slotApply()
{
	writeConfig();
	emit valueChanged();
}

DictList::DictList(const QString &configKey, QWidget *parent, char *name)
	: QWidget(parent, name)
{
	_configKey = configKey;

	KStandardDirs *dirs = KGlobal::dirs();
	QString globaldict = dirs->findResource("appdata", configKey);

	QVBoxLayout *biglayout = new QVBoxLayout(this, 6);
	if (globaldict != QString::null)
	{
		QLabel *already = new QLabel(i18n("Main <strong>%1</strong> already loaded.").arg(configKey), this);
		biglayout->addWidget(already);
	}

	QHBoxLayout *layout = new QHBoxLayout(biglayout, 6);

	List = new KListView(this);
	layout->addWidget(List);

	QVBoxLayout *buttonLayout = new QVBoxLayout(layout, 6);
	AddButton = new QPushButton(i18n("Add"), this);
	buttonLayout->addWidget(AddButton);
	connect(AddButton, SIGNAL(pressed()), SLOT(add()));
	DelButton = new QPushButton(i18n("Delete"), this);
	buttonLayout->addWidget(DelButton);
	connect(DelButton, SIGNAL(pressed()), SLOT(del()));

	List->addColumn("Name");
	List->addColumn("File");

	List->setItemsRenameable(true);
	List->setRenameable(1);
	List->setRenameable(2);
}

void DictList::add()
{
	QListViewItem *item = List->firstChild();
	QString filename = KFileDialog::getOpenFileName(item? QFileInfo(item->text(1)).dirPath(true).append("/") : QString::null);
	QString name = QFileInfo(filename).fileName();

	(void) new QListViewItem(List, name, filename);
}

void DictList::del()
{
	QListViewItem *file = List->selectedItem();
	if (!file)
		return;

	delete file;
}

void DictList::writeConfig()
{
	//kdDebug() << "DictList::writeConfig()\n";
	KConfig *config = kapp->config();
	config->setGroup(_configKey);

	QStringList names;

	QListViewItemIterator it(List);
	for (; it.current(); ++it )
	{
		//kdDebug() << "saving item\n";
		names.append(it.current()->text(0));
		config->writeEntry(it.current()->text(0), it.current()->text(1));
	}
	config->writeEntry("__NAMES", names);
}

void DictList::readConfig()
{
	KConfig *config = kapp->config();
	config->setGroup(_configKey);

	QStringList names = config->readListEntry("__NAMES");

	QStringList::Iterator it;
	for (it = names.begin(); it != names.end(); ++it)
	{
		(void) new QListViewItem (List, *it, config->readEntry(*it));
	}
}

#include "optiondialog.moc"
