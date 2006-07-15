#ifndef KITENEDIT_H
#define KITENEDIT_H

#include <kaction.h>
#include <kcombobox.h>
#include <kcompletion.h>
#include <kdemacros.h>

class KDE_EXPORT KitenEdit : public KAction 
{
	Q_OBJECT
	public:
		KitenEdit(KActionCollection *parent, QWidget *bar);
		~KitenEdit();
		
		/* access and mutate contents */
		KComboBox *ComboBox();
		KCompletion *Completion();
		QString text();
		void setText(const QString &text);
	
	private:
		KComboBox *comboBox;
		KCompletion *completion;
};

#endif
