#ifndef FILEMOVEOBJECT_H
#define FILEMOVEOBJECT_H

#include <QObject>
#include <QtQml>

class FileMoveObject : public QObject
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
    explicit FileMoveObject(QObject *parent = nullptr);
    virtual ~FileMoveObject();

    QString log();
    void setLog(const QString& strLog);

    Q_INVOKABLE void setText(const QString& strText);
    Q_INVOKABLE void setSourcePath(const QString& strSource);
    Q_INVOKABLE void setDestPath(const QString& strDest);
    Q_INVOKABLE void copyFiles();

    Q_INVOKABLE void updateFilter(const QStringList& list);

public slots:
    void slot_showLog(QString strLog);

signals:
    void logChanged();
};

#endif // FILEMOVEOBJECT_H
