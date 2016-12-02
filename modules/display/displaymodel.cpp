#include "displaymodel.h"

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
{

}

const QList<Resolution> DisplayModel::monitorsModeList() const
{

}

Monitor *DisplayModel::primaryMonitor() const
{
    for (auto mon : m_monitors)
        if (mon->name() == m_primary)
            return mon;

    return nullptr;
}

bool DisplayModel::monitorsIsIntersect() const
{
    if (m_monitors.size() < 2)
        return false;

    // only support 2 screens
    Q_ASSERT(m_monitors.size() == 2);

    return m_monitors.first()->rect().intersects(m_monitors.last()->rect());
}

void DisplayModel::setScreenHeight(const int h)
{
    if (m_screenHeight != h)
    {
        m_screenHeight = h;
        emit screenHeightChanged(m_screenHeight);
    }
}

void DisplayModel::setScreenWidth(const int w)
{
    if (m_screenWidth != w)
    {
        m_screenWidth = w;
        emit screenWidthChanged(m_screenWidth);
    }
}

void DisplayModel::setPrimary(const QString &primary)
{
    if (m_primary != primary)
    {
        m_primary = primary;
        emit primaryScreenChanged(m_primary);
    }
}

void DisplayModel::monitorAdded(Monitor *mon)
{
    m_monitors.append(mon);

    emit monitorListChanged();
}
