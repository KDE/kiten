#ifndef KROMAJIEDIT_H
#define KROMAJIEDIT_H

#include <klineedit.h>
#include <qcstring.h>

class QKeyEvent;
class QWidget;
class QPopupMenu;

class KRomajiEdit : public KLineEdit
{
	Q_OBJECT
	
	public:
	KRomajiEdit(QWidget *parent, const char *name);
	~KRomajiEdit();

	public slots:
	void setKana(int);

	protected:
	void keyPressEvent(QKeyEvent *e);
	QPopupMenu *createPopupMenu();
	
	private:
	QMap<QString, QString> hiragana;
	QMap<QString, QString> katakana;

	QCString kana;
};

#endif
