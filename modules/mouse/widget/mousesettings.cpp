#include "mousesettings.h"
#include "settingsitem.h"
#include "switchwidget.h"
#include "settingsgroup.h"
#include "../mousemodel.h"
#include "titledslideritem.h"
#include "settingshead.h"
#include "mouse/model/mousemodelmousesettings.h"
#include "dccslider.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::mouse;

MouseSettings::MouseSettings(const QString &title, QWidget *parent)
    : TranslucentFrame(parent)
{
    m_mainGroup = new SettingsGroup;
    m_mainLayout = new QVBoxLayout;

    m_title    = new SettingsHead;
    m_title->setTitle(title);
    m_title->setEditEnable(false);

    QStringList list;
    list << tr("Slow") << "" << "" << "" << "" << "" << tr("Fast");
    m_speedSlider = new TitledSliderItem(tr("Pointer Speed"));
    m_speedSlider->slider()->setType(DCCSlider::Vernier);
    m_speedSlider->slider()->setTickPosition(QSlider::TicksBelow);
    m_speedSlider->slider()->setRange(0, 6);
    m_speedSlider->slider()->setTickInterval(1);
    m_speedSlider->slider()->setPageStep(1);

    m_speedSlider->setAnnotations(list);

    m_mainGroup->appendItem(m_title);
    m_mainGroup->appendItem(m_speedSlider);

    m_switchWidget = new SwitchWidget;
    m_mainGroup->appendItem(m_switchWidget);

    m_mainLayout->addWidget(m_mainGroup);
    m_mainLayout->setMargin(0);
    setLayout(m_mainLayout);
    connect(m_switchWidget, &SwitchWidget::checkedChanegd, this, &MouseSettings::requestSetSwitch);

    QSlider *spSlider;
    spSlider = m_speedSlider->slider();
    connect(spSlider, &QSlider::valueChanged, this, &MouseSettings::requestSetSliderValue);
    setObjectName("MouseSettings");
}

void MouseSettings::setModel(MouseModelMouseSettings *const baseSettings)
{
    m_baseSettings = baseSettings;

    connect(m_baseSettings, &MouseModelMouseSettings::switchChanged, this, &MouseSettings::setSwitchState);
    connect(m_baseSettings, &MouseModelMouseSettings::sliderValueChanged, this, &MouseSettings::setSliderValue);
    connect(m_baseSettings, &MouseModelMouseSettings::existChanged, this, &MouseSettings::setVisible);

    setSwitchState(m_baseSettings->getSwitchState());
    setSliderValue(m_baseSettings->getSliderValue());
    setVisible(baseSettings->getExist());
}

void MouseSettings::setSwitchTitle(const QString &title)
{
    blockSignals(true);
    m_switchWidget->setTitle(title);
    blockSignals(false);
}

void MouseSettings::setSwitchState(const bool state)
{
    blockSignals(true);
    m_switchWidget->setChecked(state);
    blockSignals(false);
}

void MouseSettings::setSliderValue(const int &value)
{
    QSlider *spSlider = m_speedSlider->slider();
    spSlider->blockSignals(true);
    spSlider->setValue(value);
    spSlider->blockSignals(false);
}
