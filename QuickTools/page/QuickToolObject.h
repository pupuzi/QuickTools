#ifndef QUICKTOOLOBJECT_H
#define QUICKTOOLOBJECT_H

#include <QObject>
#include <QtQml>

class QuickToolObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString log READ log WRITE setLog NOTIFY logChanged)

private:
    QString m_strText;
    QString m_strSource;
    QString m_strDest;
    QString m_strLog;

    QStringList m_listFilter;

public:
    explicit QuickToolObject(QObject *parent = nullptr);
    virtual ~QuickToolObject();

    QString log();
    void setLog(const QString& strLog);

    Q_INVOKABLE void setText(const QString& strText);
    Q_INVOKABLE void setSourcePath(const QString& strSource);
    Q_INVOKABLE void setDestPath(const QString& strDest);
    Q_INVOKABLE void copyFiles();
    Q_INVOKABLE void compressVideos();

    Q_INVOKABLE void updateFilter(const QStringList& list);

public slots:
    void slot_showLog(QString strLog);

signals:
    void logChanged();
};

#endif // QUICKTOOLOBJECT_H
