#include "timezonectrlwidget.h"

#include <QHBoxLayout>

TimezoneCtrlWidget::TimezoneCtrlWidget(QWidget *parent) : QWidget(parent)
{
    m_TipsLabel = new DynamicLabel;
    m_TipsLabel->hideLabel();

    m_addBtn = new ImageNameButton("add");
    m_delBtn = new ImageNameButton("delete");
    m_cancelBtn = new DTextButton(tr("Close"));
    m_confirmBtn = new DTextButton(tr("Confirm"));
    m_addTimezoneBtn = new DTextButton(tr("Add"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(m_TipsLabel);
    mainLayout->addWidget(m_delBtn);
    mainLayout->addWidget(m_addBtn);
    mainLayout->addWidget(m_cancelBtn);
    mainLayout->addWidget(m_confirmBtn);
    mainLayout->addWidget(m_addTimezoneBtn);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    toNormalMode();

    connect(m_delBtn, &ImageNameButton::clicked, this, &TimezoneCtrlWidget::removeTimezone);
    connect(m_delBtn, &ImageNameButton::clicked, this, &TimezoneCtrlWidget::toDelTimezoneMode);
    connect(m_confirmBtn, &DTextButton::clicked, this, &TimezoneCtrlWidget::toNormalMode);
    connect(m_confirmBtn, &DTextButton::clicked, this, &TimezoneCtrlWidget::removeAccept);
    connect(m_cancelBtn, &DTextButton::clicked, this, &TimezoneCtrlWidget::addTimezoneCancel);
    connect(m_cancelBtn, &DTextButton::clicked, this, &TimezoneCtrlWidget::toNormalMode);
    connect(m_addTimezoneBtn, &DTextButton::clicked, this, &TimezoneCtrlWidget::addTimezoneAccept);
    connect(m_addTimezoneBtn, &DTextButton::clicked, this, &TimezoneCtrlWidget::toNormalMode);
    connect(m_addBtn, &ImageNameButton::clicked, this, &TimezoneCtrlWidget::addTimezone);
    connect(m_addBtn, &ImageNameButton::clicked, this, &TimezoneCtrlWidget::toAddTimezoneMode);
    connect(m_delBtn, &DImageButton::stateChanged, [this] () -> void {
                m_TipsLabel->setText(tr("Remove Timezone"));
                if (m_delBtn->getState() == DImageButton::Hover)
                    m_TipsLabel->showLabel();
                else
                    m_TipsLabel->hideLabel();
            });
    connect(m_addBtn, &ImageNameButton::stateChanged, [this] () -> void {
                m_TipsLabel->setText(tr("Add Timezone"));
                if (m_addBtn->getState() == DImageButton::Hover)
                    m_TipsLabel->showLabel();
                else
                    m_TipsLabel->hideLabel();
            });
}

void TimezoneCtrlWidget::toNormalMode()
{
    m_addBtn->show();
    m_delBtn->show();
    m_cancelBtn->hide();
    m_confirmBtn->hide();
    m_addTimezoneBtn->hide();
}

void TimezoneCtrlWidget::toDelTimezoneMode()
{
    m_addBtn->hide();
    m_delBtn->hide();
    m_cancelBtn->hide();
    m_confirmBtn->show();
    m_addTimezoneBtn->hide();
}

void TimezoneCtrlWidget::toAddTimezoneMode()
{
    m_addBtn->hide();
    m_delBtn->hide();
    m_cancelBtn->show();
    m_confirmBtn->hide();
    m_addTimezoneBtn->hide();
}

void TimezoneCtrlWidget::setAcceptOrCancel(bool accept)
{
    m_addTimezoneBtn->setVisible(!accept);
    m_cancelBtn->setVisible(accept);
}
