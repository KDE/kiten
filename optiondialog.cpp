#include <kapplication.h>
#include <qvgroupbox.h>
#include <kkeydialog.h>
#include <kfontdialog.h>
#include <qtextcodec.h>
#include <kglobalaccel.h>
#include <kglobal.h>
#include <kdebug.h>
#include <klocale.h>
#include <qcombobox.h>
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
	descBox->addStretch();
	descBox->addWidget(new KURLLabel("http://www.csse.monash.edu.au/~jwb/edict.html", i18n("Edict information page"), Page0));
	descBox->addWidget(new KURLLabel("http://www.csse.monash.edu.au/~jwb/kanjidic.html", i18n("Kanjidic information page"), Page0));
	descBox->addStretch();

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
	searchBox->addStretch();

	QFrame *Page4 = addPage(i18n("Learn"));
	QVBoxLayout *learnBox = new QVBoxLayout(Page4);
	startLearnCB = new QCheckBox(i18n("Start learn on Kiten startup"), Page4);
	learnBox->addWidget(startLearnCB);
	learnBox->addStretch();

	QStringList quizTypes(i18n("Kanji"));
	quizTypes.append(i18n("Meaning"));
	quizTypes.append(i18n("Reading"));

	QVGroupBox *quizzing = new QVGroupBox(i18n("Quizzing"), Page4);
	quizzing->setAlignment(QGroupBox::AlignRight);
	learnBox->addWidget(quizzing);

	(void) new QLabel(i18n("Clue"), quizzing);
	quizOn = new QComboBox(quizzing);
	quizOn->insertStringList(quizTypes);
	(void) new QLabel(i18n("Possible answers"), quizzing);
	guessOn = new QComboBox(quizzing);
	guessOn->insertStringList(quizTypes);
	learnBox->addStretch();

	QFrame *Page5 = addPage(i18n("Global Keys"));
	QVBoxLayout *layout = new QVBoxLayout(Page5);
	Chooser = new KKeyChooser(Accel, Page5);
	layout->addWidget(Chooser);

	QFrame *Page6 = addPage(i18n("Font"));
	QVBoxLayout *fontLayout = new QVBoxLayout(Page6);
	font = new KFontChooser(Page6);
	font->setSampleText(QTextCodec::codecForName("eucJP")->toUnicode(QCString("Result View Font - いろはにほへと 漢字")));
	fontLayout->addWidget(font);

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

	config->setGroup("General");
	QFont defaultFont(config->readFontEntry("font"));
	config->setGroup("Font");
	font->setFont(config->readFontEntry("font", &defaultFont));

	config->setGroup("Dictionaries");

	config->setGroup("Searching Options");
	caseSensitiveCB->setChecked(config->readBoolEntry("caseSensitive", false));
	wholeWordCB->setChecked(config->readBoolEntry("wholeWord", true));

	config->setGroup("Learn");
	startLearnCB->setChecked(config->readBoolEntry("startLearn", false));
	quizOn->setCurrentItem(config->readNumEntry("Quiz On", 0));
	guessOn->setCurrentItem(config->readNumEntry("Guess On", 1));
}

void ConfigureDialog::writeConfig()
{
	KConfig *config = kapp->config();

	KanjiDictList->writeConfig();
	DictDictList->writeConfig();

	config->setGroup("Font");
	config->writeEntry("font", font->font());

	config->setGroup("Dictionaries");

	config->setGroup("Searching Options");
	config->writeEntry("caseSensitive", caseSensitiveCB->isChecked());
	config->writeEntry("wholeWord", wholeWordCB->isChecked());

	config->setGroup("Learn");
	config->writeEntry("startLearn", startLearnCB->isChecked());
	config->writeEntry("Quiz On", quizOn->currentItem());
	config->writeEntry("Guess On", guessOn->currentItem());

	config->sync();

	Chooser->commitChanges();
	Accel->writeSettings(KGlobal::config());
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

//////////////////////////////////////////////////////////////////////

DictList::DictList(const QString &configKey, QWidget *parent, char *name)
	: QWidget(parent, name)
{
	_configKey = configKey;

	KStandardDirs *dirs = KGlobal::dirs();
	QString globaldict = dirs->findResource("appdata", configKey);

	QVBoxLayout *biglayout = new QVBoxLayout(this, 0, KDialog::spacingHint());
	useGlobal = new QCheckBox(i18n("Use preinstalled %1").arg(configKey), this);
	biglayout->addWidget(useGlobal);
	useGlobal->setEnabled(globaldict != QString::null);

	QHBoxLayout *layout = new QHBoxLayout(biglayout, KDialog::spacingHint());

	List = new KListView(this);
	layout->addWidget(List);

	QVBoxLayout *buttonLayout = new QVBoxLayout(layout, KDialog::spacingHint());
	AddButton = new QPushButton(i18n("Add"), this);
	buttonLayout->addWidget(AddButton);
	connect(AddButton, SIGNAL(clicked()), SLOT(add()));
	DelButton = new QPushButton(i18n("Delete"), this);
	buttonLayout->addWidget(DelButton);
	connect(DelButton, SIGNAL(clicked()), SLOT(del()));
	buttonLayout->addStretch();

	List->addColumn(i18n("Name"));
	List->addColumn(i18n("File"));

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
	KConfig *config = kapp->config();
	config->setGroup(_configKey);

	QStringList names;

	QListViewItemIterator it(List);
	for (; it.current(); ++it )
	{
		names.append(it.current()->text(0));
		config->writeEntry(it.current()->text(0), it.current()->text(1));
	}
	config->writeEntry("__NAMES", names);

	config->writeEntry("__useGlobal", useGlobal->isChecked());
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

	useGlobal->setChecked(config->readBoolEntry("__useGlobal", true));
}

#include "optiondialog.moc"
