#include "kitenEdit.h"

#include <kdebug.h>
KitenEdit::KitenEdit(KActionCollection *parent, QWidget *bar) : 
	KAction(parent, "search_edit")
{
	comboBox = new KComboBox(true, bar);
	completion = comboBox->completionObject();
	kDebug() << comboBox << endl;
}

KitenEdit::~KitenEdit()
{
}

QString KitenEdit::text()
{
	return comboBox->currentText();
}

void KitenEdit::setText(const QString &text)
{
	//comboBox->setEditText(text);
}

KComboBox *KitenEdit::ComboBox()
{
	return comboBox;
}

KCompletion *KitenEdit::Completion()
{
	return completion;
}

#include "kitenEdit.moc"
