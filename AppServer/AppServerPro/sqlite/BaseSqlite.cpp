#include "BaseSqlite.h"

BaseSqlite::BaseSqlite(QObject *parent) : QObject(parent)
{
}

BaseSqlite::~BaseSqlite()
{
//    closeDataBase();
}

void BaseSqlite::closeDataBase(void)
{
//    qDebug()<<m_dbName<<" ## "<<m_connName<<"closeDataBase";
//    QSqlDatabase::removeDatabase(m_connName);
    m_sqlDataBase.close();
}

//打开数据库
bool BaseSqlite::openDataBase(QString db_name, QString conn_name)
{
    if(m_sqlDataBase.isOpen())
    {
        qDebug()<<m_dbName<<" "<<m_connName<< " is opened.";
        return false;
    }
    m_dbName=db_name;
    m_connName=conn_name;

    if(QSqlDatabase::contains(m_connName))
      m_sqlDataBase = QSqlDatabase::database(m_connName);
    else
      m_sqlDataBase = QSqlDatabase::addDatabase("QSQLITE", m_connName);

//    m_sqlDataBase = QSqlDatabase::addDatabase("QSQLITE", conn_name);
    m_sqlDataBase.setDatabaseName(db_name);
    if(!m_sqlDataBase.open())
    {
        qDebug()<<m_dbName<<" "<<m_connName<< "Error: Failed to connect database." << m_sqlDataBase.lastError();
        return false;
    }
    else
    {
        m_sqlQuery = QSqlQuery(m_sqlDataBase);
//        qDebug()<<m_dbName<<" "<<m_connName<< "Succeed to open database." ;
    }
    return true;
}

bool BaseSqlite::isOpened(void)
{
    return m_sqlDataBase.isOpen();
}

//执行语句
bool BaseSqlite::runSqlQuery(QString str_query)
{
//    QSqlQuery query(m_sqlDataBase);

    if(!m_sqlQuery.exec(str_query))
    {
        qDebug()<<m_sqlQuery.lastError();
    }
    else
    {
        return true;
    }
    return false;
}

//启动事务
bool BaseSqlite::beginTransaction(void)
{
    QString str_query = "BEGIN TRANSACTION";
    if(runSqlQuery(str_query)==false)
    {
        return false;
    }
    return true;
}

//停止事务
bool BaseSqlite::endTransaction(void)
{
    QString str_query = "COMMIT";
    if(runSqlQuery(str_query)==false)
    {
        return false;
    }
    return true;
}



