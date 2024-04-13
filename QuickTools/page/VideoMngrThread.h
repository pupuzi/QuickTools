#ifndef VIDEOMNGRTHREAD_H
#define VIDEOMNGRTHREAD_H

#include <QThread>

class VideoMngrThread : public QThread
{
    Q_OBJECT

private:
    QString m_strError;
    QStringList m_strNames;

    void printErrLog(int nRet);
    bool compress(const QString& strName);

public:
    VideoMngrThread();
    virtual ~VideoMngrThread();

    void startThread(const QStringList& strNames);
    void startThread(const QString& strName);

protected:
    void run() override;

signals:
    void sig_showLog(QString sigLog);
};

#endif // VIDEOMNGRTHREAD_H
