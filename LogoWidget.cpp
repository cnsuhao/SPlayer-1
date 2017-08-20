#include "LogoWidget.h"
#include "common/Common.h"
#include <QPainter>

LogoWidget::LogoWidget(QWidget *parent) : BaseWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_pOpenFile = new SOpenFileButton(this);
    QImage image(Common::getImagePath(":/Image/OpenFileButton.png"));
    m_pOpenFile->setFixedHeight(image.height());
    m_pOpenFile->setFixedWidth(image.width()/4);
    m_pOpenFile->AppendImage(image);
    m_pOpenFile->setToolTip(tr("Open file"));

    createCentralButtonMenu();

    connect(m_pOpenFile, SIGNAL(Signal_Clicked()), this
            , SIGNAL(onOpenLocalFileButtonClicked()));
    connect(m_pOpenFile, SIGNAL(onArrowButtonClicked()), this, SLOT(onShowPopupMenu()));
}

LogoWidget::~LogoWidget()
{
    if (m_pCentralButtonMenu)
    {
        m_pCentralButtonMenu->close();
        delete m_pCentralButtonMenu;
        m_pCentralButtonMenu = NULL;
    }
}

void LogoWidget::onShowPopupMenu()
{
    if (!m_pCentralButtonMenu)
    {
        return;
    }
    QPoint pt = m_pOpenFile->rect().bottomLeft();
    m_pCentralButtonMenu->exec(m_pOpenFile->mapToGlobal(QPoint(pt.x(), pt.y() + 1)));
}

void LogoWidget::SetLogoImage(const QString &imageFilePath)
{
    m_logoImage = QImage(imageFilePath);
    update();
}

void LogoWidget::paintEvent(QPaintEvent *event)
{
    if (!m_logoImage.isNull())
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.drawImage(QPoint(0,0),m_logoImage.scaled(size()));
    }
    BaseWidget::paintEvent(event);
}

void LogoWidget::resizeEvent(QResizeEvent *event)
{
    m_pOpenFile->move(rect().center().x() - m_pOpenFile->width()/2,rect().center().y() + m_pOpenFile->height()+10);
    BaseWidget::resizeEvent(event);
}

void LogoWidget::createCentralButtonMenu()
{
    m_pCentralButtonMenu = new QMenu(this);
    QAction *openLocalFileAction = m_pCentralButtonMenu->addAction(tr("Open file"));
    connect(openLocalFileAction, SIGNAL(triggered()), this
            , SIGNAL(onOpenLocalFileButtonClicked()));
    QAction *openURLAction = m_pCentralButtonMenu->addAction(tr("Open URL"));
    connect(openURLAction, SIGNAL(triggered()), this, SIGNAL(onOpenURLButtonClicked()));
}

SOpenFileButton::SOpenFileButton(QWidget *parent) : SButton(parent)
{

}

SOpenFileButton::~SOpenFileButton()
{

}

void SOpenFileButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->pos().x() > 94)
    {
        this->ResetButtonState();
        this->update();
        emit onArrowButtonClicked();
    }
    else
    {
        SButton::mouseReleaseEvent(event);
    }
}
