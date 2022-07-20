#include "usersmanage.h"
#include "ui_usersmanage.h"

UsersManage::UsersManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UsersManage)
{
    ui->setupUi(this);

    connect(ui->bt_manage, SIGNAL(clicked()), this, SLOT(onBtManageClicked()));
    connect(ui->bt_record, SIGNAL(clicked()), this, SLOT(onBtRecordClicked()));
    connect(ui->bt_set, SIGNAL(clicked()), this, SLOT(onBtSetClicked()));
    connect(ui->bt_about, SIGNAL(clicked()), this, SLOT(onBtAboutClicked()));
    connect(ui->bt_return, SIGNAL(clicked()), this, SLOT(onBtReturnClicked()));

    connect(ui->bt_add, SIGNAL(clicked()), this, SLOT(onBtAddClicked()));
    connect(ui->bt_delete, SIGNAL(clicked()), this, SLOT(onBtDeleteClicked()));
    connect(ui->bt_save, SIGNAL(clicked()), this, SLOT(onBtSaveClicked()));

    connect(ui->bt_find, SIGNAL(clicked()), this, SLOT(onBtFindClicked()));

    connect(ui->bt_confirm1, SIGNAL(clicked()), this, SLOT(onBtConfirm1Clicked()));
    connect(ui->bt_confirm2, SIGNAL(clicked()), this, SLOT(onBtConfirm2Clicked()));
    connect(ui->bt_confirm3, SIGNAL(clicked()), this, SLOT(onBtConfirm3Clicked()));

    initTableWidget();
    initTableWidget2();

    QString password;
    double threshold_live;
    double threshold_face;
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/adminDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }
    QSqlQuery query(db);

    query.prepare("select * from admin");
    if(query.exec())
    {
        while (query.next())
        {
            password = query.value(1).toString();
            threshold_live = query.value(2).toDouble();
            threshold_face = query.value(3).toDouble();
        }
    }
     db.close();
    ui->lineEdit_password->setText(password);
    ui->lineEdit_face->setText(QString::number(threshold_face, 'f', 2));
    ui->lineEdit_live->setText(QString::number(threshold_live, 'f', 2));
}

UsersManage::~UsersManage()
{
    delete ui;
}

void UsersManage::initTableWidget()
{
    //tablewidget
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWidget->setColumnCount(4);
    int width = 969;
    ui->tableWidget->setColumnWidth(0,(width - 200) / 4);
    ui->tableWidget->setColumnWidth(1,(width - 200) / 4);  //设置行列大小
    ui->tableWidget->setColumnWidth(2,(width - 200) / 2);
    ui->tableWidget->setColumnWidth(3,200);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(200);
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->horizontalHeader()->setFixedHeight(40);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() << "index"  << "name" << "regist_time"<<"picture");

    //打开数据库进行读取填写
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/arcfaceDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }
    QSqlQuery query(db);

    query.prepare("select * from users");
    if(query.exec())
    {
        while (query.next())
        {
            int index =  query.value(0).toInt();
            QString name = query.value(1).toString();
            QString recode = query.value(2).toString();
            int row = ui->tableWidget->rowCount();
            ui->tableWidget->setRowCount(row + 1);

            QTableWidgetItem *item = new QTableWidgetItem(QString::number(index));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(row, 0, item);
            item = new QTableWidgetItem(name);
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(row, 1, item);
            item = new QTableWidgetItem(recode);
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(row, 2, item);
            QLabel *label_show_image = new QLabel();     //创建lable
            QString show_name = "imgs/show_img_";
            show_name += QString::number(index);
            show_name += ".png";
            QPixmap show_img = QPixmap(show_name);
            if (!show_img.isNull())
            {
                label_show_image->setPixmap(show_img);    //加载图片
                label_show_image->setAlignment(Qt::AlignCenter);      //设置居中
                //label_show_image->setAlignment(Qt::AlignVCenter);
                ui->tableWidget->setCellWidget(row,3,label_show_image);     //显示
            }
        }
    }
    db.close();
}

void UsersManage::initTableWidget2()
{
    //tablewidget
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWidget_2->setColumnCount(3);
    int width = 969;
    ui->tableWidget_2->setColumnWidth(0,width / 4);
    ui->tableWidget_2->setColumnWidth(1,width / 4);  //设置行列大小
    ui->tableWidget_2->setColumnWidth(2,width / 2);
    ui->tableWidget_2->verticalHeader()->hide();
    ui->tableWidget_2->horizontalHeader()->setFixedHeight(40);
    ui->tableWidget_2->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "index"  << "name" << "rrecord_time");

    //打开数据库进行读取填写
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/recordDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }
    QSqlQuery query(db);

    query.prepare("select * from records");
    if(query.exec())
    {
        while (query.next())
        {
            QString record = query.value(0).toString();
            int index =  query.value(1).toInt();
            QString name = query.value(2).toString();

            int row = ui->tableWidget_2->rowCount();
            ui->tableWidget_2->setRowCount(row + 1);

            QTableWidgetItem *item = new QTableWidgetItem(QString::number(index));
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(row, 0, item);
            item = new QTableWidgetItem(name);
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(row, 1, item);
            item = new QTableWidgetItem(record);
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget_2->setItem(row, 2, item);
        }
    }
    db.close();
}

void UsersManage::onBtManageClicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void UsersManage::onBtRecordClicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void UsersManage::onBtSetClicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void UsersManage::onBtAboutClicked()
{
    //ui->stackedWidget->setCurrentIndex(3);
}

void UsersManage::onBtReturnClicked()
{
    this->close();
}

void UsersManage::onBtAddClicked()
{
    add = new AddUsers(ui->tableWidget->rowCount());
    add->showMaximized();
    connect(add, SIGNAL(sig_add(QString, float*)), this, SLOT(recvName(QString, float*)));
}

void UsersManage::onBtDeleteClicked()
{
    int rowIndex = ui->tableWidget->currentRow();
    //数据库中进行删除
    QSqlDatabase db;
    if(QSqlDatabase::contains("qt_sql_default_connection"))
        db = QSqlDatabase::database("qt_sql_default_connection");
    else
        db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/arcfaceDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }

    QSqlQuery query=QSqlQuery(db);

    QString delete_sql = "delete from users where id = ?";
    query.prepare(delete_sql);
    query.addBindValue(rowIndex);
    if(!query.exec())
    {
        qDebug()<<query.lastError();
    }

    if (rowIndex != ui->tableWidget->rowCount() && rowIndex != 0)
    {
        for (int r = rowIndex + 1;r <= ui->tableWidget->rowCount() - 1;r++)
        {
            QString na = ui->tableWidget->item(r,1)->text();
            QString update_sql = "update users set id = :id where name = :name";
            query.prepare(update_sql);
            query.bindValue(":name", na);
            query.bindValue(":id",r-1);
            query.exec();
        }
    }

    db.close();

    //删除对应图片
    QString show_name = "imgs/show_img_";
    show_name += QString::number(rowIndex);
    show_name += ".png";
    QFile file(show_name);
    bool ok = file.remove();

    //图片重命名
    if (rowIndex < ui->tableWidget->rowCount())
    {
        for (int r = rowIndex + 1;r <= ui->tableWidget->rowCount() - 1;r++)
        {
            QString pic_name = "imgs/show_img_";
            pic_name += QString::number(r);
            pic_name += ".png";
            QFile file(pic_name);
            QString re_name = "imgs/show_img_";
            re_name += QString::number(r - 1);
            re_name += ".png";
            bool ok2 = file.rename(re_name);
        }
    }

    for(int i = ui->tableWidget->rowCount() - 1;i >= 0; i--)
    {
        ui->tableWidget->removeRow(i);
    }
    initTableWidget();
}

void UsersManage::recvName(QString n, float* feature)
{
    QDateTime timeCurrent = QDateTime::currentDateTime();
    QString time = timeCurrent.toString("[yy-MM-dd hh:mm:ss]");

    int row = ui->tableWidget->rowCount();
    //ui->tableWidget->setRowCount(row + 1);

    /*QTableWidgetItem *item = new QTableWidgetItem(QString::number(row));
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(row, 0, item);
    item = new QTableWidgetItem(n);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(row, 1, item);
    item = new QTableWidgetItem(time);
    item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(row, 2, item);*/


    //添加到数据库
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/arcfaceDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }

    QSqlQuery query=QSqlQuery(db);
    QString insert_sql = "insert into users values(?,?,?";
    for (int i = 1;i <= 128;i++)
    {
        insert_sql += ",?";
    }
    insert_sql += ")";
    query.prepare(insert_sql);
    query.addBindValue(row);
    query.addBindValue(n);
    query.addBindValue(time);
    for (int i = 0;i < 128;i++)
    {
        query.addBindValue(feature[i]);
    }
    query.exec();
    db.close();


    for(int i = ui->tableWidget->rowCount() - 1;i >= 0; i--)
    {
        ui->tableWidget->removeRow(i);
    }
    initTableWidget();

}

void UsersManage::onBtSaveClicked()
{
    QDateTime datetime = QDateTime::currentDateTime();
    QString timestr = datetime.toString("ddHHmmzzz");
    QString fileName = "records/record_" + timestr + ".csv";
    QFile file(fileName);

    if(file.open(QFile::WriteOnly | QFile::Truncate))        // 打开文件
    {
        QTextStream out(&file);    // 输入流
        out<<"index"<<","<<"name"<<","<<"record"<<"\n";

        //打开数据库进行读取输出
        QSqlDatabase db;
        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("dbs/recordDatabase.db");
        if(db.open()){
           qDebug()<<"open success";
        }
        QSqlQuery query(db);

        query.prepare("select * from records");
        if(query.exec())
        {
            while (query.next())
            {
                QString record = query.value(0).toString();
                int index =  query.value(1).toInt();
                QString name = query.value(2).toString();
                out<<index<<","<<name<<","<<record<<"\n";
            }
        }
        db.close();

    }

    QMessageBox::about(this, tr("Tips:"), tr(" Save Successfully!"));
}

void UsersManage::onBtFindClicked()
{
    QString str_find = ui->lineEdit_find->text();
    if (str_find.isEmpty())
    {
        for(int i = ui->tableWidget_2->rowCount() - 1;i >= 0; i--)
        {
            ui->tableWidget_2->removeRow(i);
        }
        initTableWidget2();
        return;
    }

    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/recordDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }

    QVector<int>indexes;
    QVector<QString>names;
    QVector<QString>records;
    QSqlQuery query(db);
    query.prepare("select * from records");
    if(query.exec())
    {
        while (query.next())
        {
            QString record = query.value(0).toString();
            int index =  query.value(1).toInt();
            QString name = query.value(2).toString();
            if (name.contains(str_find))
            {
                indexes.push_back(index);
                names.push_back(name);
                records.push_back(record);
            }
        }
    }
    db.close();

    //重新写表
    //tablewidget
    for(int i = ui->tableWidget_2->rowCount() - 1;i >= 0; i--)
    {
        ui->tableWidget_2->removeRow(i);
    }

    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWidget_2->setColumnCount(3);
    int width = 969;
    ui->tableWidget_2->setColumnWidth(0,width / 4);
    ui->tableWidget_2->setColumnWidth(1,width / 4);  //设置行列大小
    ui->tableWidget_2->setColumnWidth(2,width / 2);
    ui->tableWidget_2->verticalHeader()->hide();
    ui->tableWidget_2->horizontalHeader()->setFixedHeight(40);
    ui->tableWidget_2->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget_2->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->tableWidget_2->setHorizontalHeaderLabels(QStringList() << "index"  << "name" << "record_time");

    for (int i = 0;i < indexes.size();i++)
    {
        int row = ui->tableWidget_2->rowCount();
        ui->tableWidget_2->setRowCount(row + 1);

        QTableWidgetItem *item = new QTableWidgetItem(QString::number(indexes[i]));
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(row, 0, item);
        item = new QTableWidgetItem(names[i]);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(row, 1, item);
        item = new QTableWidgetItem(records[i]);
        item->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget_2->setItem(row, 2, item);
    }
}

void UsersManage::onBtConfirm1Clicked()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/adminDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }
    QSqlQuery query(db);
    QString sql=QString("update admin set key=%1").arg(ui->lineEdit_password->text());
    if(query.exec(sql))
    {
        QMessageBox::about(this, tr("Tips:"), tr("Update Successfully!"));
    }
}

void UsersManage::onBtConfirm2Clicked()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/adminDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }
    QSqlQuery query(db);
    QString sql=QString("update admin set threshold_arcface=%1").arg(ui->lineEdit_face->text().toDouble());
    if(query.exec(sql))
    {
        QMessageBox::about(this, tr("Tips:"), tr("Update Successfully!"));
    }
}

void UsersManage::onBtConfirm3Clicked()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/adminDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }
    QSqlQuery query(db);
    QString sql=QString("update admin set threshold_liveface=%1").arg(ui->lineEdit_live->text().toDouble());
    if(query.exec(sql))
    {
        QMessageBox::about(this, tr("Tips:"), tr("Update Successfully!"));
    }
}
