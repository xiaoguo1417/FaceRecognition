#ifndef ADDUSERS_H
#define ADDUSERS_H

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
#include <QMessageBox>

#include "arcface.h"
#include "retinaface.h"
#include "mtcnn.h"

using namespace std;
using namespace cv;

namespace Ui {
class AddUsers;
}

class AddUsers : public QWidget
{
    Q_OBJECT

public:
    AddUsers(int m_index, QWidget *parent = 0);
    ~AddUsers();
    int index;
    void drawRoundedRectangle(cv::Mat& src, cv::Rect largestRect);
    cv::Rect findLargestRect(std::vector<cv::Rect>faces);

protected:
    void paintEvent(QPaintEvent *e);

private:
    Ui::AddUsers *ui;
    //摄像头
    cv::VideoCapture capture;
    QTimer theTimer;
    cv::Mat srcImage;
    cv::Mat image_fliped;
    cv::Mat image_roi;
    cv::Mat src;
    int show_index;

    //级联分类器
    cv::CascadeClassifier faces_cascade;
    //检测出的人脸
    std::vector<cv::Rect> faces;
    int count = 0;
    cv::Rect largestRect;
    //retinaface
    Retinaface retinaface;

signals:
    void sig_add(QString name, float* feature);

private slots:
    void onBtOpenCameraClicked();
    void updateImage();
    void onBtShootClicked();
    void onBtConfirmClicked();
    void onBtReturnClicked();
};

#endif // ADDUSERS_H
