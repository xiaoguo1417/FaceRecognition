#ifndef VERIFICATION_H
#define VERIFICATION_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace Ui {
class Verification;
}

class Verification : public QWidget
{
    Q_OBJECT

public:
    explicit Verification(QWidget *parent = 0);
    ~Verification();
     void getPassword();

private:
    Ui::Verification *ui;
    QString password;

private slots:
    void onBtOkClicked();
};

#endif // VERIFICATION_H
