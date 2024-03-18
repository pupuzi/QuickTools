#include "FileMoveObject.h"
#include "FileMoveThread.h"

FileMoveObject::FileMoveObject(QObject *parent) : QObject(parent)
{

}

FileMoveObject::~FileMoveObject()
{

}

void FileMoveObject::setSourcePath(const QString& strSource)
{
    if(m_strSource != strSource)
    {
        m_strSource = strSource;
    }
}

void FileMoveObject::setDestPath(const QString& strDest)
{
    if(m_strDest != strDest)
    {
        m_strDest = strDest;
    }
}

QString FileMoveObject::log()
{
    return m_strLog;
}

void FileMoveObject::setLog(const QString& strLog)
{
    m_strLog = strLog;
}

void FileMoveObject::setText(const QString& strText)
{
    if(m_strText != strText)
    {
        m_strText = strText;
    }
}

void FileMoveObject::copyFiles()
{
    if(!m_strSource.length() || !m_strDest.length() || !m_strText.length())
    {
        m_strLog = "请确认路径与关键字都已填写!";
        emit logChanged();
        return;
    }

    FileMoveThread* thread = new FileMoveThread();
    connect(thread, SIGNAL(sig_showLog(QString)), this, SLOT(slot_showLog(QString)));
    thread->startThread(m_strSource, m_strDest, m_strText);
}

void FileMoveObject::updateFilter(const QStringList& list)
{
    m_listFilter.clear();
    m_listFilter.append(list);
}

void FileMoveObject::slot_showLog(QString strLog)
{
    m_strLog = strLog;
    emit logChanged();
}
