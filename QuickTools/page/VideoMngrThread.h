#ifndef VIDEOMNGRTHREAD_H
#define VIDEOMNGRTHREAD_H

#include <QThread>

class VideoMngrThread : public QThread
{
    Q_OBJECT

private:
    QString m_strError;
    QStringList m_strNames;

    bool compress_h265(const QString& strName);

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
