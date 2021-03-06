#include "keyboardwork.h"
#include "shortcutitem.h"
#include <QTime>
#include <QDebug>

namespace dcc {
namespace keyboard{
KeyboardWork::KeyboardWork(KeyboardModel *model, QObject *parent)
    : QObject(parent),
      m_model(model)
{
    qRegisterMetaType<LocaleInfo>("LocaleInfo");
    qDBusRegisterMetaType<LocaleInfo>();

    m_keyboardInter = new KeyboardInter("com.deepin.daemon.InputDevices",
                                        "/com/deepin/daemon/InputDevice/Keyboard",
                                        QDBusConnection::sessionBus(), this);

    m_langSelector = new LangSelector("com.deepin.daemon.LangSelector",
                                      "/com/deepin/daemon/LangSelector",
                                      QDBusConnection::sessionBus(), this);

    m_keybindInter = new KeybingdingInter("com.deepin.daemon.Keybinding",
                                          "/com/deepin/daemon/Keybinding",
                                          QDBusConnection::sessionBus(), this);

    connect(m_keybindInter, SIGNAL(Added(QString,int)), this,SLOT(onAdded(QString,int)));
    connect(m_keybindInter, SIGNAL(KeyEvent(bool,QString)), this, SIGNAL(KeyEvent(bool,QString)));
    connect(m_keyboardInter, SIGNAL(UserLayoutListChanged(QStringList)), this, SIGNAL(UserLayoutListChanged(QStringList)));
    connect(m_keyboardInter, SIGNAL(CurrentLayoutChanged(QString)), this, SIGNAL(curLayout(QString)));
    connect(m_langSelector, SIGNAL(serviceValidChanged(bool)), this ,SLOT(onValid(bool)));
    connect(m_langSelector, SIGNAL(CurrentLocaleChanged(QString)), this, SIGNAL(curLang(QString)));
    connect(m_keyboardInter, SIGNAL(RepeatDelayChanged(uint)), m_model, SIGNAL(delayChanged(uint)));
    connect(m_keyboardInter, SIGNAL(RepeatIntervalChanged(uint)), m_model, SIGNAL(speedChanged(uint)));
    connect(m_keyboardInter, SIGNAL(CapslockToggleChanged(bool)), m_model, SLOT(setCapsLock(bool)));

    m_model->setCapsLock(m_keyboardInter->capslockToggle());

    m_keyboardInter->setSync(false);
    m_langSelector->setSync(false);
    m_keybindInter->setSync(false);
}

void KeyboardWork::getProperty()
{
    QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_keybindInter->List(), this);
    connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
            SLOT(onRequestShortcut(QDBusPendingCallWatcher*)));
}

KeyboardLayoutList KeyboardWork::layoutLists() const
{
    QDBusPendingReply<KeyboardLayoutList> list = m_keyboardInter->LayoutList();
    KeyboardLayoutList tmp_map = list.value();

    return tmp_map;
}

void KeyboardWork::modifyShortcut(ShortcutInfo *info, const QString &key, bool clear)
{
    if (!info) {
        return;
    }
    QString str;
    QString accels = info->accels;
    if (info->accels != tr("None")) {
        bool result = m_keybindInter->ModifiedAccel(info->id, info->type, info->accels, false, str).value();//remove
        if(!result)
        {
            info->accels = str;
            if(clear && info->item)
                info->item->repaint();
        }

    }
    if(!clear)
    {
        if (!key.isEmpty() && key != tr("None")) {
            bool result = m_keybindInter->ModifiedAccel(info->id, info->type, key, true, str).value();
            if(!result)
            {
                info->accels = key;
                if(info->item)
                    info->item->repaint();
                emit searchChangd(info, info->name+accels);
            }
        }
    }
}

bool KeyboardWork::addCustonShortcut(const QString &name, const QString &command, const QString &accels, bool &result)
{
    m_keybindInter->Add(name, command, accels, result);

    return !result;
}

void KeyboardWork::grabScreen()
{
    m_keybindInter->GrabScreen();
}

bool KeyboardWork::checkAvaliable(const QString &key)
{
    return m_keybindInter->CheckAvaliable(key);
}

QString KeyboardWork::query(const QString &id, qint32 type)
{
    QDBusPendingReply<QString> reply = m_keybindInter->Query(id, type);
    reply.waitForFinished();
    return reply.value();
}

void KeyboardWork::delShortcut(ShortcutInfo* info)
{
    m_keybindInter->Delete(info->id, info->type);
}

void KeyboardWork::setRepeatDelay(int value)
{
    m_keyboardInter->setRepeatDelay(value);
}

uint KeyboardWork::repeatDelay() const
{
    return m_keyboardInter->repeatDelay();
}

void KeyboardWork::setRepeatInterval(int value)
{
    m_keyboardInter->setRepeatInterval(((1000 - value) + 200) / 10);
}

uint KeyboardWork::repeatInterval() const
{
    return m_keyboardInter->repeatInterval();
}

void KeyboardWork::setCapsLock(bool value)
{
    m_keyboardInter->setCapslockToggle(value);
}

void KeyboardWork::addUserLayout(const QString &value)
{
    m_keyboardInter->AddUserLayout(value);

    emit addLayout(value);
}

void KeyboardWork::delUserLayout(const QString &value)
{
    m_keyboardInter->DeleteUserLayout(value);

    emit delLayout(value);
}

void KeyboardWork::onValid(bool value)
{
    m_datas.clear();
    if(value)
    {
        QDBusPendingCallWatcher *result = new QDBusPendingCallWatcher(m_langSelector->GetLocaleList(), this);
        connect(result, SIGNAL(finished(QDBusPendingCallWatcher*)), this,
                SLOT(onLocaleListFinish(QDBusPendingCallWatcher*)));
    }
}

void KeyboardWork::onLocaleListFinish(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<LocaleList> reply = *watch;
    if(reply.isError())
    {
        qDebug()<<Q_FUNC_INFO<<reply.error().message();
        watch->deleteLater();
        return;
    }

    QString key = m_langSelector->currentLocale();
    emit curLang(key);
    LocaleList list = reply.value();
    for(int i = 0; i<list.count(); i++)
    {
        MetaData md;
        md.setKey(list.at(i).id);
        md.setText(list.at(i).name);
        m_datas.append(md);
    }
    emit langValid(m_datas);
    watch->deleteLater();
}

void KeyboardWork::onRequestShortcut(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;
    if(reply.isError())
    {
        watch->deleteLater();
        return;
    }

    QString info = reply.value();
    emit shortcutInfo(info);
    watch->deleteLater();
}

void KeyboardWork::onAdded(const QString &in0, int in1)
{
    emit custonInfo(m_keybindInter->Query(in0, in1));
}

QStringList KeyboardWork::userLayout() const
{
    return m_keyboardInter->userLayoutList();
}

void KeyboardWork::setLayout(const QString &value)
{
    m_keyboardInter->setCurrentLayout(value);
    emit curLayout(value);
}

QString KeyboardWork::curLayout() const
{
    QString str =m_keyboardInter->currentLayout();

    return str;
}

void KeyboardWork::setLang(const QString &value)
{
    m_langSelector->SetLocale(value);
}

QString KeyboardWork::curLang() const
{
    return m_langSelector->currentLocale();
}
}
}
