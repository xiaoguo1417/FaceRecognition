#include "mainwidget.h"
#include <QApplication>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDir>

int main(int argc, char *argv[])
{
    //创建文件夹imgs,dbs,records
    QDir *folder = new QDir;
    bool exist = folder->exists("imgs");
    if (!exist){
        folder->mkpath("imgs");
    }
    exist = folder->exists("dbs");
    if (!exist){
        folder->mkpath("dbs");
    }
    exist = folder->exists("records");
    if (!exist){
        folder->mkpath("records");
    }

    //建立数据库
    QFile arcFaceFile("dbs/arcfaceDatabase.db");
    if (!arcFaceFile.exists())
    {
        QSqlDatabase db;
        db = QSqlDatabase::addDatabase("QSQLITE");
        //如果这个数据库不存在，则会在后续操作时自动创建;如果已经存在，则后续的操作会在已有的数据库上进行
        db.setDatabaseName("dbs/arcfaceDatabase.db");
        if(db.open()){
            qDebug()<<"open success";

        }
        QSqlQuery query(db);

        QString basis = "id int primary key, name vachar, regist_time vachar";
        for (int i = 1;i <= 128;i++)
        {
            basis += ", f";
            basis += QString::number(i);
            basis += " float";
        }
        QString creat_sql = "create table users(" + basis + ")";
        query.prepare(creat_sql);
        if(query.exec(creat_sql)){
            qDebug()<<"init table success";
        }else{
                //打印sql语句错误信息
            qDebug()<<"init table error"<<query.lastError();
        }
        QString insert_sql = "insert into users values(?,?,?";
        for (int i = 1;i <= 128;i++)
        {
            insert_sql += ",?";
        }
        insert_sql += ")";

        query.prepare(insert_sql);
        query.addBindValue(0);
        query.addBindValue("admin");
        query.addBindValue("none");
        for (int i = 0;i < 128;i++)
        {
            query.addBindValue(i*0.1);
        }
        query.exec();

        db.close();
    }

    QFile recordFile("dbs/recordDatabase.db");
    if (!recordFile.exists())
    {
        QSqlDatabase db;
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("dbs/recordDatabase.db");
        if(db.open()){
           qDebug()<<"open success";
        }

        QSqlQuery query(db);
        QString creat_sql = "create table records(record_time vachar, id int, name vachar)";
        query.prepare(creat_sql);
        query.exec(creat_sql);

        QString insert_sql = "insert into records values(?,?,?)";
        query.prepare(insert_sql);
        query.addBindValue("none");
        query.addBindValue(0);
        query.addBindValue("admin");
        query.exec();

        db.close();
    }

    QFile adminFile("dbs/adminDatabase.db");
    if (!adminFile.exists())
    {
        QSqlDatabase db;
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("dbs/adminDatabase.db");
        if(db.open()){
           qDebug()<<"open success";
        }

        QSqlQuery query(db);
        QString creat_sql = "create table admin(name vachar, key vachar, threshold_liveface double, threshold_arcface double)";
        query.prepare(creat_sql);
        query.exec(creat_sql);

        QString insert_sql = "insert into admin values(?,?,?,?)";
        query.prepare(insert_sql);
        query.addBindValue("admin");
        query.addBindValue("12345");
        query.addBindValue(0.6);
        query.addBindValue(0.8);
        query.exec();

        db.close();
    }

    QApplication a(argc, argv);
    MainWidget w;
    w.showMaximized();

    return a.exec();
}
