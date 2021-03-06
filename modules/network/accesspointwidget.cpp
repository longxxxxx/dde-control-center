#include "accesspointwidget.h"
#include "labels/normallabel.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::network;

AccessPointWidget::AccessPointWidget(QWidget *parent)
    : SettingsItem(parent),

      m_lockIcon(new QLabel),
      m_strengthIcon(new QLabel),
      m_apName(new NormalLabel),
      m_detailBtn(new DImageButton),

      m_mainLayout(new QVBoxLayout),

      m_connected(false)
{
    m_lockIcon->setFixedSize(16, 16);
    m_strengthIcon->setFixedSize(16, 16);

    QHBoxLayout *basicInfoLayout = new QHBoxLayout;
    basicInfoLayout->addWidget(m_lockIcon);
    basicInfoLayout->addWidget(m_strengthIcon);
    basicInfoLayout->addWidget(m_apName);
    basicInfoLayout->addStretch();
    basicInfoLayout->addWidget(m_detailBtn);
    basicInfoLayout->setSpacing(5);
    basicInfoLayout->setContentsMargins(0, 5, 0, 5);

    m_mainLayout->addLayout(basicInfoLayout);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_mainLayout);
    setObjectName("AccessPointWidget");

    connect(m_detailBtn, &DImageButton::clicked, [=] { emit requestEdit(m_path); });
}

void AccessPointWidget::setEncyrpt(const bool encyrpt)
{
    if (!encyrpt)
        m_lockIcon->clear();
    else
        m_lockIcon->setPixmap(QPixmap(":/network/themes/dark/icons/wireless/security.svg"));
}

void AccessPointWidget::setAPName(const QString &name)
{
    m_apName->setText(name);
}

void AccessPointWidget::setPath(const QString &path)
{
    m_path = path;
}

void AccessPointWidget::setStrength(const int strength)
{
    m_strength = strength;

    if (m_strength < 0)
        return m_strengthIcon->clear();

    const QString iconName = QString(":/network/themes/dark/icons/wireless/wireless-%1-symbolic.svg").arg(strength / 10 & ~1);

    m_strengthIcon->setPixmap(iconName);
}

void AccessPointWidget::setConnected(const bool connected)
{
    m_connected = connected;
}

void AccessPointWidget::setEditable(const bool editable)
{
    m_detailBtn->setVisible(editable);
}
