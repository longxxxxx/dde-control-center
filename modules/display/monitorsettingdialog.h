#ifndef MONITORSETTINGDIALOG_H
#define MONITORSETTINGDIALOG_H

#include "monitor.h"
#include "displaymodel.h"
#include "dccslider.h"
#include "settingslistwidget.h"

#include <QDialog>
#include <QPushButton>
#include <QHBoxLayout>

#include <dimagebutton.h>

using Dtk::Widget::DImageButton;

using dcc::DCCSlider;

class MonitorControlWidget;
class MonitorSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MonitorSettingDialog(DisplayModel *model, QWidget *parent = nullptr);
    explicit MonitorSettingDialog(Monitor *monitor, QWidget *parent = nullptr);
    ~MonitorSettingDialog();

signals:
    void requestMerge() const;
    void requestSplit() const;
    void requestRecognize() const;
    void requestSetPrimary(const int index) const;
    void requestMonitorRotate(Monitor *mon) const;
    void requestSetMonitorMode(Monitor *mon, const int mode) const;
    void requestSetMonitorBrightness(Monitor *mon, const double brightness) const;
    void requestSetMonitorPosition(Monitor *mon, const int x, const int y) const;

private:
    void init();
    void initPrimary();

    void reloadMonitorObject(Monitor *monitor);
    void reloadOtherScreensDialog();

private slots:
    void mergeScreens();
    void splitScreens();
    void updateScreensRelation();
    void onPrimaryChanged();
    void onMonitorRectChanged();
    void onMonitorModeChanged();
    void onMonitorBrightnessChanegd(const double brightness);
    void onMonitorModeListChanged(const QList<Resolution> &modeList);
    void onMonitorModeSelected(const int index);

    void onBrightnessSliderChanged(const int value);

private:
    bool m_primary;

    DisplayModel *m_model;
    Monitor *m_monitor;

    Dtk::Widget::DImageButton *m_rotateBtn;
    dcc::DCCSlider *m_lightSlider;
    SettingsListWidget *m_resolutionsWidget;
    SettingsListWidget *m_primarySettingsWidget;
    MonitorControlWidget *m_ctrlWidget;
    QHBoxLayout *m_btnsLayout;
    QVBoxLayout *m_mainLayout;

    QTimer *m_smallDelayTimer;

    QList<MonitorSettingDialog *> m_otherDialogs;
};

#endif // MONITORSETTINGDIALOG_H
