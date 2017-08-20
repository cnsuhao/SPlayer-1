#pragma once

#include <QMenu>

class ClickableMenu : public QMenu
{
	Q_OBJECT
public:
    explicit ClickableMenu(QWidget *parent = 0);
    explicit ClickableMenu(const QString& title, QWidget *parent = 0);

protected:
	virtual void mouseReleaseEvent(QMouseEvent *);
	
};
