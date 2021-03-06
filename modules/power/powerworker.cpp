#include "powerworker.h"

static const char *SuspendAction = "dbus-send --print-reply --dest=com.deepin.SessionManager /com/deepin/SessionManager com.deepin.SessionManager.RequestSuspend";

PowerWorker::PowerWorker(PowerModel *model, QObject *parent)
    : QObject(parent),

      m_powerModel(model),

      m_powerInter(new PowerInter("com.deepin.daemon.Power", "/com/deepin/daemon/Power", QDBusConnection::sessionBus(), this))
{
    m_powerInter->setSync(false);

    connect(m_powerInter, &PowerInter::ScreenBlackLockChanged, m_powerModel, &PowerModel::setScreenBlackLock);
    connect(m_powerInter, &PowerInter::SleepLockChanged, m_powerModel, &PowerModel::setSleepLock);
    connect(m_powerInter, &PowerInter::LidIsPresentChanged, m_powerModel, &PowerModel::setLidPresent);
    connect(m_powerInter, &PowerInter::LidClosedActionChanged, this, &PowerWorker::lidClosedActionChanged);
    connect(m_powerInter, &PowerInter::LinePowerScreenBlackDelayChanged, this, &PowerWorker::setScreenBlackDelayToModel);
    connect(m_powerInter, &PowerInter::LinePowerSleepDelayChanged, this, &PowerWorker::setSleepDelayToModel);
}

void PowerWorker::active()
{
    m_powerInter->blockSignals(false);

    // refersh data
    m_powerModel->setScreenBlackLock(m_powerInter->screenBlackLock());
    m_powerModel->setSleepLock(m_powerInter->sleepLock());
    m_powerModel->setLidPresent(m_powerInter->lidIsPresent());
    m_powerModel->setSleepOnLidClose(isActionSuspend(m_powerInter->lidClosedAction()));

    setScreenBlackDelayToModel(m_powerInter->linePowerScreenBlackDelay());
    setSleepDelayToModel(m_powerInter->linePowerSleepDelay());
}

void PowerWorker::deactive()
{
    m_powerInter->blockSignals(true);
}

void PowerWorker::setScreenBlackLock(const bool lock)
{
    m_powerInter->setScreenBlackLock(lock);
}

void PowerWorker::setSleepLock(const bool lock)
{
    m_powerInter->setSleepLock(lock);
}

void PowerWorker::setSleepOnLidClosed(const bool sleep)
{
    m_powerInter->setLidClosedAction(sleep ? SuspendAction : "");
}

void PowerWorker::setSleepDelay(const int delay)
{
    m_powerInter->setLinePowerSleepDelay(converToDelayDBus(delay));
    m_powerInter->setBatterySleepDelay(converToDelayDBus(delay));
}

void PowerWorker::setScreenBlackDelay(const int delay)
{
    m_powerInter->setLinePowerScreenBlackDelay(converToDelayDBus(delay));
    m_powerInter->setBatteryScreenBlackDelay(converToDelayDBus(delay));
}

void PowerWorker::setSleepDelayToModel(const int delay)
{
    m_powerModel->setSleepDelay(converToDelayModel(delay));
}

void PowerWorker::setScreenBlackDelayToModel(const int delay)
{
    m_powerModel->setScreenBlackDelay(converToDelayModel(delay));
}

void PowerWorker::lidClosedActionChanged(const QString &value)
{
    m_powerModel->setSleepOnLidClose(isActionSuspend(value));
}

bool PowerWorker::isActionSuspend(const QString &action)
{
    return action.contains("RequestSuspend");
}

int PowerWorker::converToDelayModel(int value)
{
    if (value == 0) {
        return 7;
    }

    if (value <= 60) {
        return 1;
    } else if (value <= 300) {
        return 2;
    } else if (value <= 600) {
        return 3;
    } else if (value <= 900) {
        return 4;
    } else if (value <= 1800) {
        return 5;
    } else {
        return 6;
    }
}

int PowerWorker::converToDelayDBus(int value)
{
    switch (value) {
    case 1:
        return 60;
    case 2:
        return 300;
    case 3:
        return 600;
    case 4:
        return 900;
    case 5:
        return 1800;
    case 6:
        return 3600;
    case 7:
        return 0;
    default:
        return 900;
    }
}
