#ifndef VIDEOMOVETHREAD_H
#define VIDEOMOVETHREAD_H

#include <QMap>
#include <QThread>
#include <QFileInfo>

class VideoMoveThread : public QThread
{
    Q_OBJECT

private:
    QString m_strSource;
    QString m_strDest;
    QString m_strName;

    QMap<QString, QFileInfo> m_listFileInfo;
    void GetFileInfos(const QString& strFilePath);
    QString GetCurrentLog(const QString& strLog);

public:
    VideoMoveThread();
    virtual ~VideoMoveThread();

    void startThread(const QString& strSource, const QString& strDest, const QString& strName);

protected:
    void run() override;

signals:
    void sig_showLog(QString sigLog);
};

#endif // FILEMOVETHREAD_H
