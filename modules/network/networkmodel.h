#ifndef NETWORKMODEL_H
#define NETWORKMODEL_H

#include "networkdevice.h"

#include <QObject>
#include <QMap>

namespace dcc {

namespace network {

class NetworkDevice;
class NetworkWorker;
class NetworkModel : public QObject
{
    Q_OBJECT

    friend class NetworkWorker;

public:
    explicit NetworkModel(QObject *parent = 0);
    ~NetworkModel();

    bool vpnEnabled() const { return m_vpnEnabled; }
    const QList<NetworkDevice *> devices() const { return m_devices; }
    const QList<QJsonObject> vpns() const { return m_connections.value("vpn"); }
    const QList<QJsonObject> wireds() const { return m_connections.value("wired"); }
    const QList<QJsonObject> pppoes() const { return m_connections.value("pppoe"); }
    const QString connectionUuid(const QString &connPath) const;

signals:
    void vpnEnabledChanged(const bool enabled) const;
    void deviceListChanged(const QList<NetworkDevice *> devices) const;
    void unhandledConnectionSessionCreated(const QString &device, const QString &sessionPath) const;

private slots:
    void onVPNEnabledChanged(const bool enabled);
    void onDeviceListChanged(const QString &devices);
    void onConnectionListChanged(const QString &conns);
    void onActiveConnectionsChanged(const QString &conns);
    void onConnectionSessionCreated(const QString &device, const QString &sessionPath);
    void onDeviceAPListChanged(const QString &device, const QString &apList);
    void onDeviceAPInfoChanged(const QString &device, const QString &apInfo);
    void onDeviceEnableChaned(const QString &device, const bool enabled);

private:
    bool containsDevice(const QString &devPath) const;
    NetworkDevice *device(const QString &devPath) const;

private:
    bool m_vpnEnabled;
    QList<NetworkDevice *> m_devices;
    QMap<QString, QList<QJsonObject>> m_connections;
};

}   // namespace network

}   // namespace dcc

#endif // NETWORKMODEL_H
