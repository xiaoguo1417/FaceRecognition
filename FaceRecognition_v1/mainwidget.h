#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <QPaintEvent>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QImage>
#include <QDateTime>
#include <QMessageBox>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "mtcnn.h"
#include "subthread.h"
#include "about.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    cv::Rect findLargestRect(std::vector<cv::Rect>faces);
    void drawRoundedRectangle(cv::Mat& src, cv::Rect largestRect);


private:
    Ui::MainWidget *ui;
    //摄像头
    cv::VideoCapture capture;
    QTimer theTimer;
    cv::Mat srcImage;
    cv::Mat image_fliped;
    int index = 0;
    //级联分类器
    cv::CascadeClassifier faces_cascade;
    //检测出的人脸
    std::vector<cv::Rect> faces;
    SubThread* sub_thread = new SubThread();
    bool open_thread = true;
    bool suspend = false;//暂停
    bool display = false;
    int count = 0;

    About* about;

    double threshold_face;
    void getFaceThreshold();

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void onBtOpenClicked();
    void onBtCloseClicked();
    void onBtManageClicked();
    void onBtAboutClicked();
    void onBtExitClicked();

    void updateImage();
    void recvResult(int index, QString name, double similarity);
    void recvFake();
};

#endif // MAINWIDGET_H
