#include "fontsettingswidget.h"
#include "contentwidget.h"
#include "nextpagewidget.h"
#include "settingsgroup.h"
#include "../../model/fontmodel.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::personalization;

FontSettingsWidget::FontSettingsWidget(QWidget *parent)
    :TranslucentFrame(parent)
{
    m_mainlayout = new QVBoxLayout;
    m_fontWidget = new SettingsGroup;
    m_standard    = new NextPageWidget;
    m_mono       = new NextPageWidget;

    m_standard->setTitle(tr("StandardFont"));
    m_mono->setTitle(tr("MonospacedFont"));

    m_standard->setAccessibleName("StandardFont");
    m_mono->setAccessibleName("MonospacedFont");

    m_fontWidget->appendItem(m_standard);
    m_fontWidget->appendItem(m_mono);

    m_mainlayout->addWidget(m_fontWidget);
    m_mainlayout->setMargin(0);

    setLayout(m_mainlayout);
    setObjectName("FontSettingsWidget");

    connect(m_standard, &NextPageWidget::clicked, this, &FontSettingsWidget::showStandardFont);
    connect(m_mono, &NextPageWidget::clicked, this, &FontSettingsWidget::showMonoFont);
}

void FontSettingsWidget::setFontStandModel(FontModel *const model)
{
    m_standard->setValue(model->getFontName());
}

void FontSettingsWidget::setFontMonoModel(FontModel *const model)
{
    m_mono->setValue(model->getFontName());
}
