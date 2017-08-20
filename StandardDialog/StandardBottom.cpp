#include "StandardBottom.h"

StandardBottom::StandardBottom(QWidget *parent) : BaseWidget(parent)
  , m_pHBoxLayout(NULL)
  , m_pBottomWidget(NULL)
{
    m_pHBoxLayout = new QHBoxLayout(this);
    m_pHBoxLayout->setContentsMargins(0,0,0,0);
    m_pHBoxLayout->setSpacing(0);
}

StandardBottom::~StandardBottom()
{

}

void StandardBottom::AddWidget(QWidget *pWidget)
{
    if (!pWidget)
    {
        return;
    }
    m_pBottomWidget = pWidget;
    m_pBottomWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_pBottomWidget->setMouseTracking(true);
    m_pHBoxLayout->addWidget(m_pBottomWidget);
}

bool StandardBottom::GetIsBottomSetted()
{
    if (m_pBottomWidget)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void StandardBottom::RemoveWidget()
{
    if (!m_pBottomWidget)
    {
        return;
    }
    m_pHBoxLayout->removeWidget(m_pBottomWidget);
    m_pBottomWidget = NULL;
}
