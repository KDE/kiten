#include <qwidget.h>
#include <qevent.h>
#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qfile.h>
#include <qpopupmenu.h>
#include <qcstring.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qstringlist.h>
#include <qtextcodec.h>

#include "kromajiedit.h"

KRomajiEdit::KRomajiEdit(QWidget *parent, const char *name)
	: KLineEdit(parent, name)
{
	kana = "unset";

	KStandardDirs *dirs = KGlobal::dirs();
	QString romkana = dirs->findResource("appdata", "romkana.cnv");
	if (romkana == QString::null)
	{
		KMessageBox::error(0, i18n("Romaji information file not installed, so Romaji conversion cannot be used."));
		return;
	}

	QFile f(romkana);
	
	if (!f.open(IO_ReadOnly))
	{
		KMessageBox::error(0, i18n("Romaji information could not be loaded, so Romaji conversion cannot be used."));
	}

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("eucJP"));
	while (!t.eof())
	{
		QString s = t.readLine();

		QChar first = s.at(0);
		if (first == '#') // comment!
		{
			// nothing
		}
		else if (first == '$') // header
		{
			if (kana == "unset")
				kana = "hiragana";
			else
				kana = "katakana";
		}
		else // body
		{
			QStringList things(QStringList::split(QChar(' '), s));
			QString thekana(things.first());
			QString romaji(*things.at(1));

			if (kana == "hiragana")
				hiragana[romaji] = thekana;
			else if (kana == "katakana")
				katakana[romaji] = thekana;
		}
	}
	f.close();

	kana = "english";
}

KRomajiEdit::~KRomajiEdit()
{
}

void KRomajiEdit::setKana(int _kana)
{
	switch (_kana)
	{
		case 0:
		kana = "english";
		break;
		case 1:
		kana = "hiragana";
		break;
		//case 2:
		//kana = "katakana";
		//break;
	}
}

void KRomajiEdit::keyPressEvent(QKeyEvent *e)
{
	bool shift = e->state() & ShiftButton;

	if (shift && e->key() == Key_Space) // switch hiragana/english
	{
		if (kana == "hiragana")
			kana = "english";
		else if (kana == "english")
			kana = "hiragana";

		return;
	}

	if (kana == "english")
	{
		KLineEdit::keyPressEvent(e);
		return;
	}

	if (shift) // shift for katakana
	{
		if (kana == "hiragana")
			kana = "katakana";
	}

	//kdDebug() << "--------------------\n";
	QString ji = e->text();

	QString curEng;
	QString curKana;
	QString _text = text();

	int i;
	unsigned int len = _text.length();
	//kdDebug() << "length = " << len << endl;
	for (i = len - 1; i >= 0; i--)
	{
		QChar at = _text.at(i);

		//kdDebug() << "at = " << QString(at) << endl;

		if (at.row() == 0 && at != '.')
		{
			//kdDebug() << "prepending " << QString(at) << endl;
			curEng.prepend(at);
		}
		else
			break;
	}
	i++;

	//kdDebug() << "i is " << i << ", length is " << len << endl;
	curKana = _text.left(i);

	ji.prepend(curEng);
	ji = ji.lower();
	//kdDebug() << "ji = " << ji << endl;

	QString replace;

	//kdDebug () << "kana is " << kana << endl;
	if (kana == "hiragana")
		replace = hiragana[ji];
	else if (kana == "katakana")
		replace = katakana[ji];

	//kdDebug() << "replace = " << replace << endl;

	if (!!replace) // if (replace has something in it)
	{
		//kdDebug() << "replace isn't empty\n";

		setText(curKana + replace);

		if (kana == "katakana")
			kana = "hiragana";
		return;
	}
	else
	{
		//kdDebug() << "replace is empty\n";
		QString farRight(ji.right(ji.length() - 1));
		//kdDebug() << "farRight = " << farRight << endl;

		// test if we need small tsu
		if (ji.at(0) == farRight.at(0) && farRight.length() > 1) // if two letters are same, and we can add a twoletter length kana
		{
			if (kana == "hiragana")
				setText(curKana + hiragana["t-"] + hiragana[farRight]);
			else
				setText(curKana + katakana["t-"] + katakana[farRight]);

			if (kana == "katakana")
				kana = "hiragana";
			return;
		}

		// test for hanging n
		QString newkana;
		if (kana == "hiragana")
		{
			newkana = hiragana[farRight];
			//kdDebug() << "newkana = " << newkana << endl;
			if (ji.at(0) == 'n' && !!newkana)
			{
				//kdDebug() << "doing the n thing\n";
				
				setText(curKana + hiragana["n'"] + newkana);

				if (kana == "katakana")
					kana = "hiragana";
				return;
			}
		}
		else
		{
			newkana = katakana[farRight];
			if (ji.at(0) == 'n' && !!newkana)
			{
				//kdDebug() << "doing the n thing - katakana\n";
				
				setText(curKana + katakana["n'"] + newkana);

				if (kana == "katakana")
					kana = "hiragana";
				return;
			}
		}
	}

	KLineEdit::keyPressEvent(e); // don't think we'll get here :)
}

QPopupMenu *KRomajiEdit::createPopupMenu()
{
    QPopupMenu *popup = KLineEdit::createPopupMenu();
    popup->insertSeparator();
    popup->insertItem(i18n("English"), 0);
    popup->insertItem(i18n("Kana"), 1);

    if (kana == "english")
		popup->setItemChecked(0, true);
    else if (kana == "hiragana")
		popup->setItemChecked(1, true);

    connect(popup, SIGNAL(activated(int)), SLOT(setKana(int)));

    emit aboutToShowContextMenu(popup);
    return popup;
}

#include "kromajiedit.moc"
