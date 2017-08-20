#include <QApplication>
#include <QMouseEvent>
#include "StandardDialog.h"

StandardDialog::StandardDialog(QWidget *parent) : BaseWidget(parent)
  , m_bDialogMaxed(false)
  , m_bFixedSize(false)
  , m_bHeaderPressed(false)
  , m_bStayOnTop(false)
  , m_bCanMove(true)
  , m_pHeader(NULL)
  , m_pBottom(NULL)
  , m_pVBoxLayout(NULL)
{
    m_pHeader = new StandardHeader(this);
    m_pBottom = new StandardBottom(this);

    m_pVBoxLayout = new QVBoxLayout(this);
    m_pVBoxLayout->setContentsMargins(0,0,0,0);
    m_pVBoxLayout->setSpacing(0);
    m_pVBoxLayout->addWidget(m_pHeader);
    m_pVBoxLayout->addWidget(m_pBottom);

    connect(m_pHeader,SIGNAL(Signal_LogoClicked()),this,SLOT(Slot_LogoClicked()));
    connect(m_pHeader, SIGNAL(Signal_StayOnTopClicked()), this
            , SLOT(Slot_StayOnTopClicked()));
    connect(m_pHeader,SIGNAL(Signal_SkinClicked()),this,SLOT(Slot_SkinClicked()));
    connect(m_pHeader,SIGNAL(Signal_MinClicked()),this,SLOT(Slot_MinClicked()));
    connect(m_pHeader,SIGNAL(Signal_MaxClicked()),this,SLOT(Slot_MaxClicked()));
    connect(m_pHeader,SIGNAL(Signal_CloseClicked()),this,SLOT(Slot_CloseClicked()));
    connect(m_pHeader,SIGNAL(Signal_MouseDoubleClicked()),this
            ,SLOT(Slot_MouseDoubleClicked()));
    connect(m_pHeader,SIGNAL(Signal_MousePressed()),this,SLOT(Slot_HeaderPressed()));
    connect(m_pHeader,SIGNAL(Signal_MouseReleased()),this,SLOT(Slot_HeaderReleased()));
}

StandardDialog::~StandardDialog()
{

}

StandardHeader *StandardDialog::GetHeader()
{
    return m_pHeader;
}

StandardBottom *StandardDialog::GetBottom()
{
    return m_pBottom;
}

void StandardDialog::SetFixed(bool bFixedSize)
{
    m_bFixedSize = bFixedSize;
}

bool StandardDialog::GetFixed()
{
    return m_bFixedSize;
}

void StandardDialog::setWindowMaximize(bool canMaximize)
{
    if (canMaximize && m_bDialogMaxed)
    {
        return;
    }
    if (!canMaximize && !m_bDialogMaxed)
    {
        return;
    }
    if (canMaximize)
    {
        m_bDialogMaxed = false;
    }
    else
    {
        m_bDialogMaxed = true;
    }
    Slot_MaxClicked();
}

bool StandardDialog::getIsMaximized()
{
    return m_bDialogMaxed;
}

void StandardDialog::setGeometryAsMaximizedWindow()
{
    setGeometry(qApp->desktop()->availableGeometry());
}

void StandardDialog::setRestoreGeometry(QRect Geometry)
{
    m_restoreGeometry = Geometry;
}

void StandardDialog::setRestoreSize(QSize Size)
{
    m_restoreSize = Size;
}

void StandardDialog::exitFromFullscreenMode()
{
    m_bDialogMaxed = true;
    Slot_MaxClicked();
}

QRect StandardDialog::getRestoreGeometry()
{
    return m_restoreGeometry;
}

QSize StandardDialog::getRestoreSize()
{
    return m_restoreSize;
}

void StandardDialog::setWindowCanMove(bool canMove)
{
    m_bCanMove = canMove;
}

bool StandardDialog::getWindowCanMove()
{
    return m_bCanMove;
}

void StandardDialog::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        return;
    }
    if (!m_bCanMove)
    {
        return;
    }
    m_sourcePos = event->globalPos();
    BaseWidget::mousePressEvent(event);
}

//实现拖动以及伸缩窗口效果
void StandardDialog::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bDialogMaxed && !m_bHeaderPressed)
    {
        return;
    }
    if (m_bDialogMaxed && m_bHeaderPressed)
    {
        QRect m_currentRect;
        if (event->pos().x() <= (width() / 3))
        {
            m_currentRect.setLeft(0);
        }
        else if (event->pos().x() >= (width() / 3 * 2))
        {
            m_currentRect.setLeft(QApplication::desktop()->width() - m_restoreSize.width());
        }
        else
        {
            m_currentRect.setLeft(QApplication::desktop()->width() / 2 - m_restoreSize.width() / 2);
        }
        m_currentRect.setTop(0);;
        m_currentRect.setSize(m_restoreSize);
        setGeometry(m_currentRect);
        m_pHeader->SetMaxImage();
        m_bDialogMaxed = false;
    }
    QPoint pt = event->pos();
    if(!m_sourcePos.isNull())
    {
        pt = mapFromGlobal(m_sourcePos);
    }
    bool bResize = true;
    QRect dialogGeometry = geometry();
    int dx = event->globalX() - m_sourcePos.x();
    int dy = event->globalY() - m_sourcePos.y();
    //qDebug() << "StandardDialog::mouseMoveEvent::" << dx << "__" << dy <<"__"<<pt << "__" << rect();
    //伸缩窗口的有效边界距离
    int nMaxValidValue = 5;
    if(!m_bFixedSize)
    {
        //TopLeft
        if(pt.x() <= nMaxValidValue && pt.y() <= nMaxValidValue)
        {
            //qDebug() << "TopLeft";
            setCursor(Qt::SizeFDiagCursor);
            dialogGeometry.setLeft(dialogGeometry.left()+dx);
            dialogGeometry.setTop(dialogGeometry.top()+dy);
        }
        //TopRight
        else if((rect().right()-pt.x())<=nMaxValidValue && pt.y() <= nMaxValidValue)
        {
            //qDebug() << "TopRight";
            setCursor(Qt::SizeBDiagCursor);
            dialogGeometry.setRight(dialogGeometry.right()+dx);
            dialogGeometry.setTop(dialogGeometry.top()+dy);
        }
        //BottomRight
        else if((rect().right()-pt.x())<=nMaxValidValue && (rect().bottom()-pt.y())<=nMaxValidValue)
        {
            //qDebug() << "BottomRight";
            setCursor(Qt::SizeFDiagCursor);
            dialogGeometry.setRight(dialogGeometry.right()+dx);
            dialogGeometry.setBottom(dialogGeometry.bottom()+dy);
        }
        //BottomLeft
        else if(pt.x() <= nMaxValidValue && (rect().bottom()-pt.y())<=nMaxValidValue)
        {
            //qDebug() << "BottomLeft";
            setCursor(Qt::SizeBDiagCursor);
            dialogGeometry.setLeft(dialogGeometry.left()+dx);
            dialogGeometry.setBottom(dialogGeometry.bottom()+dy);
        }
        //Left
        else if(pt.x() <= nMaxValidValue)
        {
            //qDebug() << "Left";
            setCursor(Qt::SizeHorCursor);
            dialogGeometry.setLeft(dialogGeometry.left()+dx);
        }
        //Top
        else if(pt.y() <= nMaxValidValue)
        {
            //qDebug() << "Top";
            setCursor(Qt::SizeVerCursor);
            dialogGeometry.setTop(dialogGeometry.top()+dy);
        }
        //Right
        else if((rect().right()-pt.x())<=nMaxValidValue)
        {
            //qDebug() << "Right";
            setCursor(Qt::SizeHorCursor);
            dialogGeometry.setRight(dialogGeometry.right()+dx);
        }
        //Bottom
        else if((rect().bottom()-pt.y())<=nMaxValidValue)
        {
            //qDebug() << "Bottom";
            setCursor(Qt::SizeVerCursor);
            dialogGeometry.setBottom(dialogGeometry.bottom()+dy);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
            bResize = false;
        }
    }
    else
    {
        setCursor(Qt::ArrowCursor);
        bResize = false;
    }
    if(m_bPress)
    {
        if(bResize)//伸缩窗口
        {
            if(!m_sourcePos.isNull())
            {
                setGeometry(dialogGeometry);
            }
        }
        else
        {
            if(!m_sourcePos.isNull())//移动窗口
            {
                //qDebug() << "Move";
                move(pos()+(event->globalPos() - m_sourcePos));
            }
        }
        m_sourcePos = event->globalPos();
    }
}

void StandardDialog::mouseReleaseEvent(QMouseEvent *event)
{
    m_sourcePos = QPoint();

    BaseWidget::mouseReleaseEvent(event);
}

void StandardDialog::Slot_LogoClicked()
{

}

void StandardDialog::Slot_StayOnTopClicked()
{
    if (m_bStayOnTop)
    {
        m_pHeader->SetStayOnTopImage();
    }
    else
    {
        m_pHeader->SetStayedOnTopImage();
    }
    m_bStayOnTop = !m_bStayOnTop;
}

void StandardDialog::Slot_SkinClicked()
{

}

void StandardDialog::Slot_MinClicked()
{
    showMinimized();
}

//m_restoreGeometry：：保留用于还原显示的窗口几何信息
void StandardDialog::Slot_MaxClicked()
{
    if(m_bDialogMaxed)//还原
    {
        setGeometry(m_restoreGeometry);
        m_pHeader->SetMaxImage();
    }
    else    //最大化
    {
        m_restoreSize = size();
        m_restoreGeometry = geometry();
        setGeometry(qApp->desktop()->availableGeometry());
        m_pHeader->SetRestoreImage();
    }
    m_bDialogMaxed = !m_bDialogMaxed;
}

void StandardDialog::Slot_CloseClicked()
{
    close();
}

void StandardDialog::Slot_MouseDoubleClicked()
{
    Slot_MaxClicked();
}

void StandardDialog::Slot_HeaderPressed()
{
    m_bHeaderPressed = true;
}

void StandardDialog::Slot_HeaderReleased()
{
    m_bHeaderPressed = false;
}

void StandardDialog::SetCenterWidget(QWidget *pCenterWidget)
{
    if(pCenterWidget)
    {
        pCenterWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        int index = m_pVBoxLayout->indexOf(m_pHeader);
        m_pVBoxLayout->insertWidget(index+1,pCenterWidget);
    }
}

void StandardDialog::SetBottomWidget(QWidget *pBottomWidget)
{
    if(pBottomWidget)
    {
        pBottomWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
        m_pBottom->AddWidget(pBottomWidget);
    }
}

void StandardDialog::RemoveBottomWidget()
{
    m_pBottom->RemoveWidget();
}

