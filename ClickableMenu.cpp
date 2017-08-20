#include "ClickableMenu.h"
#include <QMouseEvent>

ClickableMenu::ClickableMenu(QWidget *parent) :
	QMenu(parent)
{
}

ClickableMenu::ClickableMenu(const QString &title, QWidget *parent) :
    QMenu(title, parent)
{
}

void ClickableMenu::mouseReleaseEvent(QMouseEvent *e)
{
	QAction *action = actionAt(e->pos());
	if (action) {
		action->activate(QAction::Trigger);
		return;
	}
	QMenu::mouseReleaseEvent(e);
}

