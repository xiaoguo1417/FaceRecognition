#ifndef SUBTHREAD_H
#define SUBTHREAD_H

#include <QObject>
#include <QThread>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#include <QVector>
#include <QString>

#include"liveface.h"
#include "arcface.h"
#include "addusers.h"

class SubThread : public QThread
{
    Q_OBJECT
public:
    explicit SubThread(QObject *parent = 0);
    void setImg(const cv::Mat& in_src, cv::Rect in_roi);


signals:
    sig_finish(int, QString, double);
    sig_fakeface();

public slots:

protected:
    void run() override;

private:
    Arcface arcface;
    liveface live;

    cv::Mat src;
    cv::Rect roi_rect;

    void loadLiveModel(liveface& live);
    LiveFaceBox Bbox2LiveFaceBox(const Bbox& box);
    LiveFaceBox Rect2LiveFaceBox(const Rect& rect);

    double threshold_live;
    void getLiveThreshold();
};

#endif // SUBTHREAD_H
