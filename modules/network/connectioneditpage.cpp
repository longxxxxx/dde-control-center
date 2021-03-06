#include "connectioneditpage.h"
#include "connectionsessionmodel.h"
#include "translucentframe.h"
#include "settingsgroup.h"
#include "settingsheaderitem.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "lineeditwidget.h"
#include "comboboxwidget.h"
#include "filechoosewidget.h"

#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonArray>
#include <QComboBox>

using namespace dcc::widgets;
using namespace dcc::network;

const QString JsonEncoding(const QString &str)
{
    QJsonObject o;
    o["v"] = str;
    QJsonDocument d;
    d.setObject(o);

    const QString encoding = d.toJson(QJsonDocument::Compact);

    return encoding.mid(5, encoding.size() - 5 + 1 - 2);
}

ConnectionEditPage::ConnectionEditPage(QWidget *parent)
    : ContentWidget(parent),

      m_cancelBtn(new QPushButton),
      m_acceptBtn(new QPushButton),
      m_sectionsLayout(new QVBoxLayout),

      m_recreateUITimer(new QTimer(this))
{
    m_sectionsLayout->setSpacing(10);

    m_cancelBtn->setText(tr("Cancel"));
    m_acceptBtn->setText(tr("Save"));

    m_recreateUITimer->setSingleShot(true);
    m_recreateUITimer->setInterval(100);

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(m_cancelBtn);
    btnsLayout->addWidget(m_acceptBtn);
    btnsLayout->setSpacing(0);
    btnsLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(m_sectionsLayout);
    mainLayout->addLayout(btnsLayout);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);

    connect(m_recreateUITimer, &QTimer::timeout, this, &ConnectionEditPage::recreateUI);
    connect(m_cancelBtn, &QPushButton::clicked, this, &ConnectionEditPage::back);
    connect(m_acceptBtn, &QPushButton::clicked, this, &ConnectionEditPage::accept);
}

ConnectionEditPage::~ConnectionEditPage()
{
    emit requestCancelSession();

    for  (const auto &v : m_optionWidgets.values())
        for (auto *w : v)
            if (w)
                w->deleteLater();
    for (auto *w : m_sectionWidgets.values())
        if (w)
            w->deleteLater();
}

void ConnectionEditPage::setModel(ConnectionSessionModel *model)
{
    m_sessionModel = model;

    connect(m_sessionModel, &ConnectionSessionModel::keysChanged, m_recreateUITimer, static_cast<void (QTimer::*)()>(&QTimer::start));
    connect(m_sessionModel, &ConnectionSessionModel::visibleItemsChanged, this, &ConnectionEditPage::refershUI);
    connect(m_sessionModel, &ConnectionSessionModel::errorsChanged, this, &ConnectionEditPage::onErrorsChanged);

    m_recreateUITimer->start();
}

void ConnectionEditPage::onDeviceRemoved()
{
    emit back();
}

void ConnectionEditPage::recreateUI()
{
    // delete all widgets
    for (const auto &v : m_optionWidgets)
        qDeleteAll(v.values());
    qDeleteAll(m_sectionWidgets.values());
    m_optionWidgets.clear();
    m_sectionWidgets.clear();

    // construct new widgets
    const auto keys = m_sessionModel->keys();
    for (auto it(keys.cbegin()); it != keys.cend(); ++it)
    {
        SettingsGroup *grp = new SettingsGroup;
        grp->setHeaderVisible(true);
        grp->headerItem()->setTitle(m_sessionModel->sectionName(it.key()));

        m_sectionWidgets[it.key()] = grp;

        const auto keys = it.value();
        for (auto its(keys.cbegin()); its != keys.cend(); ++its)
            createOptionWidgets(it.key(), its.value());
    }

    refershUI();
}

void ConnectionEditPage::refershUI()
{
    const QString name = m_sessionModel->keysInfo("vs-general", "id").value("Value").toString();
    setTitle(name);

    // hide all widgets
    while (QLayoutItem *item = m_sectionsLayout->takeAt(0))
    {
        SettingsGroup *grp = qobject_cast<SettingsGroup *>(item->widget());
        Q_ASSERT(grp);

        grp->hide();
        grp->clear();

        delete item;
    }

    for (const auto section : m_sessionModel->sections())
    {
        SettingsGroup *grp = m_sectionWidgets.value(section);
        if (!grp)
            continue;

        // add section widgets
        const auto visibleKeys = m_sessionModel->sectionKeys(section);
        for (const auto &vKey : visibleKeys)
        {
            SettingsItem *item = m_optionWidgets[section][vKey.value("Key").toString()];
            if (item)
                grp->appendItem(item);
        }


        m_sectionsLayout->addWidget(grp);
        grp->setVisible(true);
    }
}

void ConnectionEditPage::onErrorsChanged(const NetworkErrors &errors)
{
    // clear old errors
    for (const auto &v : m_optionWidgets)
        for (auto *i : v)
            i->setIsErr(false);

    for (auto it(errors.cbegin()); it != errors.cend(); ++it)
    {
        const auto section = m_sessionModel->virtualSectionName(it.key());
        const auto eItems = it.value();
        for (auto its(eItems.begin()); its != eItems.end(); ++its)
            if (m_optionWidgets[section].contains(its.key()))
                m_optionWidgets[section][its.key()]->setIsErr();
    }
}

void ConnectionEditPage::createOptionWidgets(const QString &section, const QJsonObject &keyObject)
{
    const QString vKey = keyObject.value("Key").toString();
    const QString vType = keyObject.value("WidgetType").toString();
    const QJsonObject vInfo = m_sessionModel->keysInfo(section, vKey);

    // delete old widgets
    if (m_optionWidgets[section][vKey] != nullptr)
    {
        m_optionWidgets[section][vKey]->deleteLater();
        m_optionWidgets[section].remove(vKey);
    }

    SettingsItem *item = nullptr;
    if (vType == "EditLineSwitchButton")
        item = createSwitchWidget(keyObject, vInfo);
    else if (vType == "EditLineTextInput" || vType == "EditLineIpv4Input" || vType == "EditLineSpinner" || vType == "EditLineMissingPackage")
        item = createEditWidget(keyObject, vInfo, false);
    else if (vType == "EditLinePasswordInput")
        item = createEditWidget(keyObject, vInfo, true);
    else if (vType == "EditLineComboBox")
        item = createComboWidget(keyObject, vInfo, false);
    else if (vType == "EditLineEditComboBox")
        item = createComboWidget(keyObject, vInfo, true);
    else if (vType == "EditLineFileChooser")
        item = createFileChooserWidget(keyObject, vInfo);

    if (item)
        m_optionWidgets[section][vKey] = item;
    else
        qWarning() << "type not handled: " << keyObject << vInfo;
}

SettingsItem *ConnectionEditPage::createSwitchWidget(const QJsonObject &keyObject, const QJsonObject &infoObject)
{
    SwitchWidget *w = new SwitchWidget;

    w->setTitle(keyObject.value("Name").toString());
    w->setChecked(infoObject.value("Value").toBool());

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(w, &SwitchWidget::checkedChanegd, [=](const bool checked) {
        emit requestChangeSettings(section, vKey, checked ? "true" : "false");
    });

    return w;
}

SettingsItem *ConnectionEditPage::createEditWidget(const QJsonObject &keyObject, const QJsonObject &infoObject, const bool password)
{
    LineEditWidget *w = new LineEditWidget;
    QLineEdit *e = w->textEdit();

    w->setTitle(keyObject.value("Name").toString());
    e->setText(infoObject.value("Value").toString());
    e->setReadOnly(keyObject.value("Readonly").toBool());
    if (password)
        e->setEchoMode(QLineEdit::Password);

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(e, &QLineEdit::editingFinished, [=] {
        emit requestChangeSettings(section, vKey, JsonEncoding(e->text()));
    });

    return w;
}

SettingsItem *ConnectionEditPage::createComboWidget(const QJsonObject &keyObject, const QJsonObject &infoObject, const bool editable)
{
    ComboBoxWidget *w = new ComboBoxWidget;

    for (const auto &v : infoObject.value("Values").toArray())
    {
        const auto vObject = v.toObject();
        const auto value = vObject.value("Value").toString();
        w->appendOption(vObject.value("Text").toString(), value);
    }

    w->setEditable(editable);
    w->setTitle(keyObject.value("Name").toString());
    w->setCurrent(infoObject.value("Value").toString());

    const QString section = keyObject.value("Section").toString();
    const QString vKey = keyObject.value("Key").toString();
    connect(w, &ComboBoxWidget::requestPage, this, &ConnectionEditPage::requestNextPage);
    connect(w, &ComboBoxWidget::dataChanged, [=](const QVariant &data) {
        emit requestChangeSettings(section, vKey, JsonEncoding(data.toString()));
    });

    return w;
}

SettingsItem *ConnectionEditPage::createFileChooserWidget(const QJsonObject &keyObject, const QJsonObject &infoObject)
{
    Q_UNUSED(infoObject);

    FileChooseWidget *w = new FileChooseWidget;

    w->setTitle(keyObject.value("Name").toString());

    return w;
}
