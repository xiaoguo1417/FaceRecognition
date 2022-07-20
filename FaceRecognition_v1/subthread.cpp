#include "subthread.h"
#include<QDebug>
#include<QTime>

SubThread::SubThread(QObject *parent) : QThread(parent)
{
    arcface.init("models/arcface.bin", "models/arcface.param");
    loadLiveModel(live);
    getLiveThreshold();
}

void SubThread::setImg(const cv::Mat& in_src, cv::Rect in_roi)
{
     src = in_src;
     roi_rect = in_roi;
}

void SubThread::run()
{
    //test_model
    /*Mat test=imread("imgs/show_src.png");
    LiveFaceBox test_live_face_box = Rect2LiveFaceBox(Rect(200,207,191,191));
    double test_confidence = live.Detect(test, test_live_face_box);*/
    /*cv::Mat test1 = imread("imgs/show_img_1.png");
    ncnn::Mat test_ncnn_roi1 = ncnn::Mat::from_pixels(test1.data, ncnn::Mat::PIXEL_BGR2RGB, test1.cols, test1.rows);
    float* test_feature1 = arcface.getFeature(test_ncnn_roi1);
    cv::Mat test2 = imread("imgs/show_img_2.png");
    ncnn::Mat test_ncnn_roi2 = ncnn::Mat::from_pixels(test2.data, ncnn::Mat::PIXEL_BGR2RGB, test2.cols, test2.rows);
    float* test_feature2 = arcface.getFeature(test_ncnn_roi2);
    double test_similarity = arcface.calculSimilar(test_feature1, test_feature2);
    test_similarity++;*/

    cv::Mat roi = src(roi_rect);
    cv::imwrite("imgs/arcface_roi.png", roi);
    //子线程进行计算
    //cv::Mat roi = cv::imread("roi.jpg", cv::IMREAD_GRAYSCALE);
    if (!roi.data)
    {
        return;
    }
    QTime time1;
    time1.start();
    //活体检测
    //LiveFaceBox live_face_box = Bbox2LiveFaceBox(roi);
    LiveFaceBox live_face_box = Rect2LiveFaceBox(roi_rect);
    double confidence = live.Detect(src, live_face_box);
    qDebug()<<"live_confidence = "<<confidence;
    qDebug()<<time1.elapsed()/1000.0<<"s";
    if (confidence < threshold_live)
    {
        sig_fakeface();
        return;
    }

    QTime time2;
    time2.start();
    //arcface人脸识别
    cv::Mat arcface_roi = cv::imread("imgs/arcface_roi.png");
    ncnn::Mat ncnn_roi = ncnn::Mat::from_pixels(arcface_roi.data, ncnn::Mat::PIXEL_BGR2RGB, arcface_roi.cols, arcface_roi.rows);
    float* feature1 = arcface.getFeature(ncnn_roi);
    qDebug()<<time2.elapsed()/1000.0<<"s";

    //遍历寻找
    int max_index = -1;
    double max_similarity = 0;
    float *feature = new float[128];
    QString max_name ="";

    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/arcfaceDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }

    QSqlQuery query=QSqlQuery(db);
    query.prepare("select * from users");
    if(query.exec())
    {
        while (query.next())
        {
            int index =  query.value(0).toInt();
            if (index == 0)
            {
                continue;
            }
            QString name = query.value(1).toString();

            for (int i = 3;i < 128+3;i++)
            {
                feature[i -3] = query.value(i).toFloat();
            }
            double similarity = arcface.calculSimilar(feature, feature1);
            qDebug()<<"similarity ="<<similarity;
            if (similarity > max_similarity)
            {
                max_similarity = similarity;
                max_index = index;
                max_name = name;
            }
        }
    }
    db.close();

    qDebug()<<"max_index ="<<max_index;
    sig_finish(max_index, max_name, max_similarity);
}

void SubThread::loadLiveModel(liveface& live)
{
    //Live detection configs
    struct ModelConfig config1 = { 2.7f,0.0f,0.0f,80,80,"liveface_1",false };
    struct ModelConfig config2 = { 4.0f,0.0f,0.0f,80,80,"liveface_2",false };
    vector<struct ModelConfig> configs;
    configs.push_back(config1);
    configs.push_back(config2);
    live.LoadModel(configs);
}

LiveFaceBox SubThread::Bbox2LiveFaceBox(const Bbox& box)
{
    float x_ = box.x1;
    float y_ = box.y1;
    float x2_ = box.x2;
    float y2_ = box.y2;
    int x = (int)x_;
    int y = (int)y_;
    int x2 = (int)x2_;
    int y2 = (int)y2_;
    struct LiveFaceBox  live_box = { x_,y_,x2_,y2_ };
    return live_box;
}

LiveFaceBox SubThread::Rect2LiveFaceBox(const Rect& rect)
{
    float x_ = rect.tl().x;
    float y_ = rect.tl().y;
    float x2_ = rect.br().x;
    float y2_ = rect.br().y;
    struct LiveFaceBox  live_box = { x_,y_,x2_,y2_ };
    return live_box;
}

void SubThread::getLiveThreshold()
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
            threshold_live = query.value(2).toDouble();
        }
    }
    db.close();
}


