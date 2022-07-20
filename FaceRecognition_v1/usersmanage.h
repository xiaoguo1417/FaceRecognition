#ifndef USERSMANAGE_H
#define USERSMANAGE_H

#include <QWidget>
#include "addusers.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>

namespace Ui {
class UsersManage;
}

class UsersManage : public QWidget
{
    Q_OBJECT

public:
    explicit UsersManage(QWidget *parent = 0);
    ~UsersManage();

private:
    Ui::UsersManage *ui;
    AddUsers* add;
    void initTableWidget();
    void initTableWidget2();

private slots:
    void onBtManageClicked();
    void onBtRecordClicked();
    void onBtSetClicked();
    void onBtAboutClicked();
    void onBtReturnClicked();

    void onBtAddClicked();
    void onBtDeleteClicked();

    void onBtSaveClicked();
    void onBtFindClicked();

    void recvName(QString n, float* feature);

    void onBtConfirm1Clicked();
    void onBtConfirm2Clicked();
    void onBtConfirm3Clicked();
};

#endif // USERSMANAGE_H
