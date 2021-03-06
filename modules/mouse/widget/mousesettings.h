#ifndef MOUSESETTINGS_H
#define MOUSESETTINGS_H

#include "translucentframe.h"
#include <QWidget>
#include <QVBoxLayout>

class QSlider;
namespace dcc
{

namespace widgets
{
class TitledSliderItem;
class SwitchWidget;
class SettingsGroup;
class SettingsHead;
class TranslucentFrame;
}
namespace mouse
{
class MouseModelMouseSettings;
class MouseSettings : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit MouseSettings(const QString &title, QWidget *parent = 0);
    void setModel(MouseModelMouseSettings *const baseSettings);

signals:
    void setDefault();
    void requestSetSwitch(const bool state);
    void requestSetSliderValue(const int &value);

public slots:
    void setSwitchTitle(const QString &title);
    void setSwitchState(const bool state);
    void setSliderValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    widgets::SettingsHead   *m_title;
    widgets::SettingsGroup *m_mainGroup;
    MouseModelMouseSettings *m_baseSettings;
    widgets::TitledSliderItem *m_speedSlider;
    widgets::SwitchWidget *m_switchWidget;
};
}
}


#endif // MOUSESETTINGS_H
