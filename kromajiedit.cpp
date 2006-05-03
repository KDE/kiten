/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include <kdebug.h>
#include <kstandarddirs.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <qfile.h>
#include <q3popupmenu.h>
#include <qtextcodec.h>
//Added by qt3to4:
#include <QTextStream>
#include <QKeyEvent>

#include "kromajiedit.h"

KRomajiEdit::KRomajiEdit(QWidget *parent, const char *name)
	: KLineEdit(parent)
{
	setObjectName(name);
	kana = "unset";

	KStandardDirs *dirs = KGlobal::dirs();
	QString romkana = dirs->findResource("data", "kiten/romkana.cnv");
	if (romkana.isNull())
	{
		KMessageBox::error(0, i18n("Romaji information file not installed, so Romaji conversion cannot be used."));
		return;
	}

	QFile f(romkana);

	if (!f.open(QIODevice::ReadOnly))
	{
		KMessageBox::error(0, i18n("Romaji information could not be loaded, so Romaji conversion cannot be used."));
	}

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("eucJP"));
	while (!t.atEnd())
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
			QString romaji(things.at(1));

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

void KRomajiEdit::completionMenuActivated( QAction* act)
{
    if ( act == englishAction )
        kana = "english";
    else if ( act == kanaAction )
        kana = "hiragana";
}

// TODO allow editing not only at end
void KRomajiEdit::keyPressEvent(QKeyEvent *e)
{
	bool shift = e->state() & Qt::ShiftModifier;
	QString ji = e->text();

	if (shift && e->key() == Qt::Key_Space) // switch hiragana/english
	{
		if (kana == "hiragana")
			kana = "english";
		else if (kana == "english")
			kana = "hiragana";

		return;
	}

	if (kana == "english" || ji.isEmpty())
	{
		KLineEdit::keyPressEvent(e);
		return;
	}

	if (shift) // shift for katakana
	{
		if (kana == "hiragana")
			kana = "katakana";
	}

	//kDebug() << "--------------------\n";

	QString curEng;
	QString curKana;
	QString _text = text();

	int i;
	unsigned int len = _text.length();
	//kDebug() << "length = " << len << endl;
	for (i = len - 1; i >= 0; i--)
	{
		QChar at = _text.at(i);

		//kDebug() << "at = " << QString(at) << endl;

		if (at.row() == 0 && at != '.')
		{
			//kDebug() << "prepending " << QString(at) << endl;
			curEng.prepend(at);
		}
		else
			break;
	}
	i++;

	//kDebug() << "i is " << i << ", length is " << len << endl;
	curKana = _text.left(i);

	ji.prepend(curEng);
	ji = ji.lower();
	//kDebug() << "ji = " << ji << endl;

	QString replace;

	//kDebug () << "kana is " << kana << endl;
	if (kana == "hiragana")
		replace = hiragana[ji];
	else if (kana == "katakana")
		replace = katakana[ji];

	//kDebug() << "replace = '" << replace << "'" << endl;

	if (!replace.isEmpty()) // if (replace has something in it)
	{
		//kDebug() << "replace isn't empty\n";

		setText(curKana + replace);

		if (kana == "katakana")
			kana = "hiragana";
		return;
	}
	else
	{
		//kDebug() << "replace is empty\n";
    QString farRight(ji.right(1));
		//kDebug() << "ji = " << ji << endl;
		//kDebug() << "farRight = " << farRight << endl;

		// test if we need small tsu
		if (ji.at(0) == farRight.at(0) && ji.length() > 1) // if two letters are same, and we can add a twoletter length kana
		{
			if (kana == "hiragana")
				setText(curKana + hiragana[ji.at(0) == 'n'? "n'" : "t-"] + farRight.at(0));
			else
				setText(curKana + katakana[ji.at(0) == 'n'? "n'" : "t-"] + farRight.at(0));

			if (kana == "katakana")
				kana = "hiragana";
			return;
		}

		// test for hanging n
		QString newkana;
		if (kana == "hiragana")
		{
			newkana = hiragana[farRight];
			//kDebug() << "newkana = " << newkana << endl;
			if (ji.at(0) == 'n' && !newkana.isEmpty())
			{
				//kDebug() << "doing the n thing\n";

				setText(curKana + hiragana["n'"] + newkana);

				if (kana == "katakana")
					kana = "hiragana";
				return;
			}
		}
		else
		{
			newkana = katakana[farRight];
			if (ji.at(0) == 'n' && !newkana.isEmpty())
			{
				//kDebug() << "doing the n thing - katakana\n";

				setText(curKana + katakana["n'"] + newkana);

				if (kana == "katakana")
					kana = "hiragana";
				return;
			}
		}
	}

	if ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) // take care of pending n
	{
		if (kana == "hiragana")
		{
			if (_text[len-1] == 'n')
				setText(curKana + hiragana["n'"]);
		}
		else
		{
			if (_text[len-1] == 'N')
				setText(curKana + katakana["n'"]);
		}
	}

	KLineEdit::keyPressEvent(e); // don't think we'll get here :)
}

void KRomajiEdit::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu *popup = KLineEdit::createStandardContextMenu();
    popup->insertSeparator();
    englishAction = popup->addAction( i18n("English"));
    kanaAction = popup->addAction(i18n("Kana"));
    if (kana == "english")
       englishAction->setChecked(true);
    else if (kana == "hiragana")
        kanaAction->setChecked(true);
    connect( popup, SIGNAL( triggered ( QAction* ) ),
             this, SLOT( completionMenuActivated( QAction* ) ) );
    popup->exec(e->globalPos());
    delete popup;
}


#include "kromajiedit.moc"
