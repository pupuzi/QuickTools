#ifndef VIDEOCOMPRESSTHREAD_H
#define VIDEOCOMPRESSTHREAD_H

#include <QThread>

class VideoCompressThread : public QThread
{
    Q_OBJECT

private:
    QString m_strError;
    QStringList m_strNames;

    void printErrLog(int nRet);
    bool compress(const QString& strName);

public:
    VideoCompressThread();
    virtual ~VideoCompressThread();

    void startThread(const QStringList& strNames);
    void startThread(const QString& strName);

protected:
    void run() override;

signals:
    void sig_showLog(QString sigLog);
};

#endif // VIDEOMNGRTHREAD_H
