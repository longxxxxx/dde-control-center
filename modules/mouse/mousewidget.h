#ifndef MOUSEWIDGET_H
#define MOUSEWIDGET_H

#include "modulewidget.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace dcc
{
namespace widgets{
class SettingsGroup;
}
namespace mouse
{
class BaseSettings;
class MouseSettings;
class ThinkpadSettings;
class MouseModel;
class MouseWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit MouseWidget();
    void setModel(MouseModel *const model);

signals:
    void setDefault();
    void requestSetLeftHand(const bool state);
    void requestSetNaturalScroll(const bool state);
    void requestSetDisTyping(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetTapClick(const bool state);
    void requestSetDouClick(const int &value);
    void requestSetMouseMotionAcceleration(const int &value);
    void requestSetTouchpadMotionAcceleration(const int &value);
    void requestSetTrackPointMotionAcceleration(const int &value);

private:
    widgets::SettingsGroup *m_userGroup;
    QPushButton *m_nextPage;
    BaseSettings *m_baseSettings;
    MouseSettings *m_mouseSettings;
    MouseSettings *m_touchSettings;
    ThinkpadSettings *m_ThinkapdSettings;
};
}
}

#endif // MOUSEWIDGET_H
