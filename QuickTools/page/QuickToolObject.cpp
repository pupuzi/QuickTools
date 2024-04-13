#include "QuickToolObject.h"
#include "VideoMoveThread.h"
#include "VideoCompressThread.h"

QuickToolObject::QuickToolObject(QObject *parent) : QObject(parent)
{

}

QuickToolObject::~QuickToolObject()
{

}

void QuickToolObject::setSourcePath(const QString& strSource)
{
    if(m_strSource != strSource)
    {
        m_strSource = strSource;
    }
}

void QuickToolObject::setDestPath(const QString& strDest)
{
    if(m_strDest != strDest)
    {
        m_strDest = strDest;
    }
}

QString QuickToolObject::log()
{
    return m_strLog;
}

void QuickToolObject::setLog(const QString& strLog)
{
    m_strLog = strLog;
}

void QuickToolObject::setText(const QString& strText)
{
    if(m_strText != strText)
    {
        m_strText = strText;
    }
}

void QuickToolObject::copyFiles()
{
    if(!m_strSource.length() || !m_strDest.length() || !m_strText.length())
    {
        m_strLog = "请确认路径与关键字都已填写!";
        emit logChanged();
        return;
    }

    VideoMoveThread* thread = new VideoMoveThread();
    connect(thread, SIGNAL(sig_showLog(QString)), this, SLOT(slot_showLog(QString)));
    thread->startThread(m_strSource, m_strDest, m_strText);
}

void QuickToolObject::compressVideos()
{
    if(!m_strSource.length())
    {
        m_strLog = "请确认文件路径已填写!";
        emit logChanged();
        return;
    }

    VideoCompressThread* thread = new VideoCompressThread();
    connect(thread, SIGNAL(sig_showLog(QString)), this, SLOT(slot_showLog(QString)));
    thread->startThread(m_strSource);
}

void QuickToolObject::updateFilter(const QStringList& list)
{
    m_listFilter.clear();
    m_listFilter.append(list);
}

void QuickToolObject::slot_showLog(QString strLog)
{
    m_strLog = strLog;
    emit logChanged();
}
