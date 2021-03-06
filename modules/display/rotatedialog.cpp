#include "rotatedialog.h"
#include "displayworker.h"
#include "monitor.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QApplication>
#include <QWindow>
#include <QX11Info>
#include <QPainter>
#include <QPaintEvent>

#include <xcb/xproto.h>

static void BlurWindowBackground(const WId windowId, const QRect &region)
{
    xcb_connection_t *connection = QX11Info::connection();
    const char *name = "_NET_WM_DEEPIN_BLUR_REGION";
    xcb_intern_atom_cookie_t cookie = xcb_intern_atom(connection,
                                      0,
                                      strlen(name),
                                      name);

    xcb_intern_atom_reply_t *reply = xcb_intern_atom_reply(connection,
                                     cookie,
                                     NULL);
    if (reply) {
        const int data[] = {region.x(), region.y(), region.width(), region.height()};

        xcb_change_property_checked(connection,
                                    XCB_PROP_MODE_REPLACE,
                                    windowId,
                                    reply->atom,
                                    XCB_ATOM_CARDINAL,
                                    32,
                                    4,
                                    data);
        xcb_flush(connection);

        free(reply);
    }
}

using namespace dcc::display;

const QString Tips(QT_TR_NOOP("Left click to rotate and right click to exit"));

RotateDialog::RotateDialog(Monitor *mon, QWidget *parent)
    : QDialog(parent),

      m_mon(mon)
{
    connect(m_mon, &Monitor::wChanged, this, &RotateDialog::setFixedWidth);
    connect(m_mon, &Monitor::hChanged, this, &RotateDialog::setFixedHeight);
    connect(m_mon, &Monitor::xChanged, [=] (const int x) { move(x, y()); });
    connect(m_mon, &Monitor::yChanged, [=] (const int y) { move(x(), y); });

    setMouseTracking(true);
    setFixedWidth(m_mon->w());
    setFixedHeight(m_mon->h());
    move(m_mon->x(), m_mon->y());
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint);

#ifndef QT_DEBUG
    qApp->setOverrideCursor(Qt::BlankCursor);
#endif
}

void RotateDialog::mousePressEvent(QMouseEvent *e)
{
    QDialog::mousePressEvent(e);

    switch (e->button())
    {
    case Qt::RightButton:   accept();       break;
    case Qt::LeftButton:    rotate();       break;
    default:;
    }
}

void RotateDialog::mouseMoveEvent(QMouseEvent *e)
{
    QDialog::mouseMoveEvent(e);

    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::resizeEvent(QResizeEvent *e)
{
    const int l = 100;
    const QRect r((width() - l) / 2, (height() - l) / 2, l, l);

    BlurWindowBackground(winId(), r);

    QDialog::resizeEvent(e);
}

void RotateDialog::leaveEvent(QEvent *e)
{
    QDialog::leaveEvent(e);

    QCursor::setPos(rect().center() + pos());
}

void RotateDialog::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);

    QFontMetrics fm(font());
    const int tw = fm.width(Tips);
    const int w = width();
    const int h = height();
    const int margin = 100;

    QPainter painter(this);
    painter.fillRect(rect(), QColor(127, 127, 127, 255 * .6));

    QPixmap rotatePixmap(":/display/themes/common/icon/rotate.png");
    painter.drawPixmap(rect().center() - rotatePixmap.rect().center(), rotatePixmap);

    // bottom
    painter.drawText((w - tw) / 2, h - margin, Tips);

    // left
    painter.rotate(90);
    painter.drawText((h - tw) / 2, -margin, Tips);

    // top
    painter.rotate(90);
    painter.drawText((w + tw) / -2, -margin, Tips);

    // right
    painter.rotate(90);
    painter.drawText((h + tw) / -2, w - margin, Tips);
}

void RotateDialog::rotate()
{
    const auto rotates = m_mon->rotateList();
    const auto rotate = m_mon->rotate();

    Q_ASSERT(rotates.contains(rotate));

    const quint16 nextValue = rotates[(rotates.indexOf(rotate) + 1) % rotates.size()];

    emit requestRotate(m_mon, nextValue);
}
