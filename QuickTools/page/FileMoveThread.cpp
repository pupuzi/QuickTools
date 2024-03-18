#include "FileMoveThread.h"
#include <QDir>
#include <QDateTime>

FileMoveThread::FileMoveThread()
{

}

FileMoveThread::~FileMoveThread()
{

}

void FileMoveThread::GetFileInfos(const QString& strFilePath)
{
    QDir dir(strFilePath);
    foreach(QFileInfo info, dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs))
    {
        QString strPath = strFilePath + "/" + info.filePath();
        GetFileInfos(strPath);
    }

    foreach(QFileInfo info, dir.entryList(QDir::Files))
    {
        m_listFileInfo.insert(strFilePath + "/" + info.fileName(), info);
    }
}

QString FileMoveThread::GetCurrentLog(const QString& strLog)
{
    QString strTime = QDateTime::currentDateTime().toString("[yyyy-MM-dd HH:mm:ss.zzz]");
    return QString("%1 %2").arg(strTime).arg(strLog);
}

void FileMoveThread::startThread(const QString& strSource, const QString& strDest, const QString& strName)
{
    m_strDest = strDest;
    m_strSource = strSource;
    m_strName = strName;

    start();
}

void FileMoveThread::run()
{
    emit sig_showLog(GetCurrentLog("开始遍历文件..."));

    GetFileInfos(m_strSource);

    QString strPath = m_strDest + "/" + m_strName;
    QDir dir(strPath);
    if(!dir.exists())
    {
        dir.mkdir(strPath);
    }

    emit sig_showLog(GetCurrentLog("开始按关键字复制文件..."));

    QMap<QString, QFileInfo>::Iterator iter;
    long nIndex = 0;
    for(iter = m_listFileInfo.begin(); iter != m_listFileInfo.end(); iter++)
    {
        QString strFileName = iter.value().fileName().toUpper();
        QString strTempClass = m_strName.toUpper();
        if(strFileName.indexOf(strTempClass) != -1)
        {
            QString strFilePath = strPath + "/" + iter.value().fileName();
            QFile file(iter.key());
            file.copy(strFilePath);
            emit sig_showLog(GetCurrentLog(iter.value().fileName()));
        }
        nIndex++;
    }

    emit sig_showLog(GetCurrentLog("复制文件结束!"));
}
