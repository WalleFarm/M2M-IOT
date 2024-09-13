#ifndef BASESQLITE_H
#define BASESQLITE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include<QNetworkInterface>
#include "drivers/DrvCommon.h"

class BaseSqlite : public QObject
{
    Q_OBJECT

public:
    explicit BaseSqlite(QObject *parent = nullptr);
   ~BaseSqlite(void);
    bool openDataBase(QString db_name, QString conn_name);
    void closeDataBase(void);
    bool isOpened(void);
    bool runSqlQuery(QString str_query);
    bool beginTransaction(void);
    bool endTransaction(void);

protected:
    QSqlQuery m_sqlQuery;
private:
    QSqlDatabase m_sqlDataBase;
    QString m_dbName, m_connName;

signals:

public slots:
};

#endif // BASESQLITE_H
