#include "verification.h"
#include "ui_verification.h"
#include "usersmanage.h"

Verification::Verification(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Verification)
{
    ui->setupUi(this);

    connect(ui->bt_close, SIGNAL(clicked()),this, SLOT(close()));
    connect(ui->bt_ok, SIGNAL(clicked()),this, SLOT(onBtOkClicked()));

    getPassword();
}

Verification::~Verification()
{
    delete ui;
}

void Verification::onBtOkClicked()
{
    //if (ui->lineEdit_password->text() == "")
    if (ui->lineEdit_password->text() == password)
    {
        UsersManage* manege = new UsersManage();
        manege->showMaximized();
        this->close();
    }
    else
    {
        //输出错误提示
        ui->lineEdit_password->clear();
    }
}

void Verification::getPassword()
{
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
        }
    }
     db.close();
}
