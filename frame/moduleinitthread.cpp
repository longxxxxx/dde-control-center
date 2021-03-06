#include "moduleinitthread.h"
#include "modulewidget.h"

ModuleInitThread::ModuleInitThread(dcc::ModuleInterface *inter, QObject *parent)
    : QThread(parent),

      m_module(inter)
{

}

void ModuleInitThread::run()
{
    m_module->initialize();
    m_module->moduleDeactive();

    emit moduleInitFinished(m_module);
}
