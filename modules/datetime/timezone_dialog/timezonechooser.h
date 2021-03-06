/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TIMEZONECHOOSER_H
#define TIMEZONECHOOSER_H

#include "blurredframe.h"

class QPushButton;
class QComboBox;

namespace installer {
class TimezoneMap;
}

namespace dcc {
namespace widgets {
class SearchInput;
}
}

namespace dcc {
namespace datetime {

class TimeZoneChooser : public dcc::widgets::BlurredFrame
{
    Q_OBJECT
public:
    explicit TimeZoneChooser();

signals:
    void confirmed(const QString &zone);
    void cancelled();

private:
    installer::TimezoneMap *m_map;
    dcc::widgets::SearchInput *m_searchInput;
    QPushButton *m_cancelBtn;
    QPushButton *m_confirmBtn;
};

} // namespace datetime
} // namespace dcc

#endif // TIMEZONECHOOSER_H
