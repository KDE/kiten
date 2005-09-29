/**
 This file is part of Kiten, a KDE Japanese Reference Tool...
 Copyright (C) 2001  Jason Katz-Brown <jason@katzbrown.com>
	        (C) 2005 Paul Temple <paul.temple@gmx.net>

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
#include <kapplication.h>
#include <qvgroupbox.h>
#include <kfontdialog.h>
#include <qtextcodec.h>
#include <klocale.h>
#include <qcombobox.h>
#include <kfiledialog.h>
#include <kstandarddirs.h>
#include <kurllabel.h>
#include <krun.h>

#include <qlayout.h>
#include <qcheckbox.h>
#include <qpushbutton.h>

#include "optiondialog.h"
#include "configfont.h"
#include "configlearn.h"
#include "configsearching.h"

ConfigureDialog::ConfigureDialog(QWidget *parent, const char *name) : KConfigDialog(parent, name, Config::self())
{
	configDic = new ConfigDictionaries(0, "dictionaries_page");
	connect(configDic, SIGNAL(widgetChanged()), this, SLOT(updateButtons()));
	addPage(configDic, i18n("Dictionaries"), "contents");
	addPage(new ConfigSearching(0, "searching_page"), i18n("Searching"), "find");
	addPage(new ConfigLearn(0, "learn_page"), i18n("Learn"), "pencil");
	ConfigFont* configFont = new ConfigFont(0, "font_page");
	configFont->kcfg_font->setSampleText(i18n("Result View Font")+QString::fromUtf8(" - いろはにほへと 漢字"));
	addPage(configFont, i18n("Font"), "fonts");
}

ConfigureDialog::~ConfigureDialog()
{
}


void ConfigureDialog::updateWidgets()
{
	configDic->updateWidgets();
}

 void ConfigureDialog::updateWidgetsDefault()
{
	configDic->updateWidgetsDefault();
}

 void ConfigureDialog::updateSettings()
{
	bool changed = hasChanged();
	configDic->updateSettings();
	if (changed) KConfigDialog::settingsChangedSlot();
}

//////////////////////////////////////////////////////////////////////

bool ConfigureDialog::hasChanged()
{
	return configDic->hasChanged();
}

bool ConfigureDialog::isDefault()
{
	 return configDic->isDefault();
}


#include "optiondialog.moc"
