#pragma execution_character_set("utf-8")

#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "verification.h"

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    connect(ui->bt_open, SIGNAL(clicked()), this, SLOT(onBtOpenClicked()));
    connect(ui->bt_close, SIGNAL(clicked()), this, SLOT(onBtCloseClicked()));
    connect(ui->bt_manage, SIGNAL(clicked()), this, SLOT(onBtManageClicked()));
    connect(ui->bt_exit, SIGNAL(clicked()), this, SLOT(onBtExitClicked()));
    connect(ui->bt_about, SIGNAL(clicked()), this, SLOT(onBtAboutClicked()));

    connect(&theTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
    connect(sub_thread, SIGNAL(sig_finish(int, QString, double)), this, SLOT(recvResult(int, QString, double)));
    connect(sub_thread, SIGNAL(sig_fakeface()), this, SLOT(recvFake()));

    //初始化级联分类器
    try
    {
        faces_cascade = cv::CascadeClassifier("models/haarcascade_frontalface_default.xml");
    }
    catch (cv::Exception e)
    {
        std::cout << e.msg <<std::endl;
    }
    if(faces_cascade.empty())
    {
        std::cout << "无法加载"<<std::endl;
    }

    getFaceThreshold();

}

MainWidget::~MainWidget()
{
    if(capture.isOpened())
        capture.release();
    delete ui;
}

void MainWidget::updateImage()
{
    if (display)
    {
        QTime dieTime = QTime::currentTime().addMSecs(3000);
        while( QTime::currentTime() < dieTime ) {}
        display = false;
    }

    ui->label_picture->clear();
    ui->label_result->clear();
    ui->label_index->clear();
    ui->label_name->clear();
    ui->label_signtime->clear();

    //时间
    QDateTime timeCurrent = QDateTime::currentDateTime();
    QString time = timeCurrent.toString("[yy-MM-dd hh:mm:ss]");
    ui->label_time->setText(time);

    capture>>srcImage;
    cv::flip(srcImage, srcImage, 1);
    cv::Mat roi;
    //if (!srcImage.empty())
    //{
        //cv::imwrite("src.jpg", srcImage);
    //}

    //人脸检测
    //cv::resize(srcImage, srcImage, cv::Size(), 0.8, 0.8, cv::INTER_LINEAR_EXACT );
    cv::Size minFeatureSize(50, 50);
    cvtColor(srcImage, srcImage, CV_BGR2RGB);//Qt中支持的是RGB图像, OpenCV中支持的是BGR
    count++;
    count %= 3;
    if (count == 0)
    {
    faces_cascade.detectMultiScale(srcImage, faces, 1.1f, 2, cv::CASCADE_SCALE_IMAGE, minFeatureSize);
    }
    //mtcnn
    /*std::vector<Bbox> faces_info = detect_mtcnn(srcImage);
    if (faces_info.size() == 0)
    {
        return;
    }
    cv::Rect largestRect = Rect(Point(faces_info[0].x1, faces_info[0].y1), Point(faces_info[0].x2, faces_info[0].y2));*/

    //找最大的框
    if (faces.size() == 0)
    {
        if(srcImage.data)
        {
            this->update();  //发送刷新消息
        }
        return;
    }
    cv::Rect largestRect = findLargestRect(faces);

    //画矩形框
    /*cv::Point text_lb;
    for (size_t i = 0; i < faces.size(); i++)
    {
        if (faces[i].height > 0 && faces[i].width > 0)
        {
            //防止roi超出范围
            text_lb = cv::Point(faces[i].x, faces[i].y);
            int lt_x = cv::min(text_lb.x, srcImage.cols);
            lt_x = cv::max(0, text_lb.x);
            int lt_y = cv::min(text_lb.y, srcImage.rows);
            lt_y = cv::max(0, text_lb.y);
            int h = cv::min(faces[i].height, srcImage.rows - lt_y - 1);
            int w = cv::min(faces[i].width, srcImage.cols - lt_x - 1);

            faces[i] = cv::Rect(lt_x, lt_y, w, h);
            roi = srcImage(faces[i]).clone();
            //srcImage(faces[i]).copyTo(roi);
            cv::rectangle(srcImage, faces[i], cv::Scalar(255, 0, 0), 2, 8, 0);
        }
     }
    cv::imwrite("roi.jpg", roi);*/

    cv::Point rect_lb;
    if (largestRect.height > 0 && largestRect.width > 0)
    {
        //防止roi超出范围
        rect_lb = cv::Point(largestRect.x, largestRect.y);
        int lt_x = cv::min(rect_lb.x, srcImage.cols);
        lt_x = cv::max(0, rect_lb.x);
        int lt_y = cv::min(rect_lb.y, srcImage.rows);
        lt_y = cv::max(0, rect_lb.y);
        int h = cv::min(largestRect.height, srcImage.rows - lt_y - 1);
        int w = cv::min(largestRect.width, srcImage.cols - lt_x - 1);

        largestRect = cv::Rect(lt_x, lt_y, w, h);
        roi = srcImage(largestRect).clone();
        //srcImage(faces[i]).copyTo(roi);
        //圆角矩形绘制
        drawRoundedRectangle(srcImage, largestRect);
        //cv::rectangle(srcImage, largestRect, cv::Scalar(255, 0, 0), 2, 8, 0);
    }

    //cv::flip(srcImage, image_fliped, 1);
    if (open_thread)
    {
        Mat srcImg;
        cv::cvtColor(srcImage, srcImg, CV_RGB2BGR);
        sub_thread->setImg(srcImg, largestRect);
        sub_thread->start();
        open_thread = false;
    }

    if(srcImage.data)
    {
        this->update();  //发送刷新消息
    }
}

void MainWidget::paintEvent(QPaintEvent *e)
{
    if (suspend)
    {
        return;
    }
    //将视频流绘制到label中
    QImage image = QImage((uchar*)(srcImage.data), srcImage.cols, srcImage.rows, QImage::Format_RGB888);
    ui->label_camera->setPixmap(QPixmap::fromImage(image));
    ui->label_camera->show();
}

void MainWidget::onBtOpenClicked()
{
    open_thread=true;
    suspend = false;
    if(capture.isOpened())
    {
        return;
    }

    if(capture.open(0))
    {
        //从摄像头捕获视频
        srcImage = cv::Mat::zeros(capture.get(CV_CAP_PROP_FRAME_HEIGHT), capture.get(CV_CAP_PROP_FRAME_WIDTH), CV_8UC3);
        std::cout<<capture.get(CV_CAP_PROP_FRAME_HEIGHT)<<"   "<<capture.get(CV_CAP_PROP_FRAME_WIDTH)<<std::endl;
        theTimer.start(40);
    }
    else
    {
        //输出打开失败
        QMessageBox::critical(this, QString::fromLocal8Bit("提示："), QString::fromLocal8Bit("摄像头打开失败！"), "关闭");
    }
}

void MainWidget::onBtCloseClicked()
{
    suspend = true;
    theTimer.stop();
    open_thread=false;
    if(capture.isOpened())
    {
        capture.release();
    }
    ui->label_picture->clear();
    ui->label_result->clear();
    ui->label_index->clear();
    ui->label_name->clear();
    ui->label_signtime->clear();
    ui->label_camera->clear();
}

void MainWidget::onBtManageClicked()
{
    Verification* v = new Verification();
    v->show();
    v->move(this->width()/2-v->width()/2, this->height()/2-v->height()/2);
}

void MainWidget::recvResult(int index, QString name,  double similarity)
{
    open_thread = true;
    if(similarity< threshold_face)
    {
        if (suspend)
        {
            theTimer.stop();
            onBtCloseClicked();
        }
        return;
    }

    QString picture_name = "imgs/show_img_";
    picture_name += QString::number(index);
    picture_name += ".png";
    QImage image = QImage(picture_name);
    ui->label_picture->setPixmap(QPixmap::fromImage(image));
    ui->label_result->setText(QString::fromLocal8Bit("Sign Successfully!"));
    ui->label_index->setText(QString::number(index));
    ui->label_name->setText(name);
    QDateTime timeCurrent = QDateTime::currentDateTime();
    ui->label_signtime->setText(timeCurrent.toString("[yy-MM-dd hh:mm:ss]"));


    display = true;
    if (suspend)
    {
        theTimer.stop();
        onBtCloseClicked();
    }

    //插入打卡记录数据库
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("dbs/recordDatabase.db");
    if(db.open()){
       qDebug()<<"open success";
    }

    QSqlQuery query(db);
    QString insert_sql = "insert into records values(?,?,?)";
    query.prepare(insert_sql);
    query.addBindValue(timeCurrent.toString("[yy-MM-dd hh:mm:ss]"));
    query.addBindValue(index);
    query.addBindValue(name);
    query.exec();

    db.close();
}

void MainWidget::recvFake()
{
    open_thread = true;
    display = true;

    ui->label_result->setText("fake!");
}

void MainWidget::onBtExitClicked()
{
    onBtCloseClicked();
    this->close();
}

void MainWidget::onBtAboutClicked()
{
    about = new About();
    about->show();
    about->move(this->width()/2-about->width()/2, this->height()/2-about->height()/2);
}

//找到最大的人脸框
cv::Rect MainWidget::findLargestRect(std::vector<cv::Rect>faces)
{
    int largestIndex = 0;
    int largestArea = faces[largestIndex].width * faces[largestIndex].height;

    for (int i = 0;i < faces.size();i++)
    {
        int nowArea = faces[i].width * faces[i].height;
        if (nowArea > largestArea)
        {
            largestArea = nowArea;
            largestIndex = i;
        }
    }

    return faces[largestIndex];
}

//绘制圆角矩形
 void MainWidget::drawRoundedRectangle(cv::Mat& src, cv::Rect largestRect)
 {
     double filletFactor = 0.2;//圆角系数
     double lineFactor = (1-filletFactor*2-0.2)/2;

     double filletRadius = largestRect.width * filletFactor / 2;
     int width = largestRect.br().x - largestRect.tl().x;
     int height = largestRect.br().y - largestRect.tl().y;

     cv::Point p1 = cv::Point(largestRect.x+filletFactor*width / 2, largestRect.y + filletFactor*height / 2);
     cv::Point p2 = cv::Point(largestRect.x+(1-filletFactor / 2)*width, largestRect.y + filletFactor / 2*height);
     cv::Point p3 = cv::Point(largestRect.x+(1-filletFactor / 2)*width, largestRect.y + (1-filletFactor / 2)*height);
     cv::Point p4 = cv::Point(largestRect.x+filletFactor / 2*width, largestRect.y + (1-filletFactor / 2)*height);

     cv::ellipse(src, p1, Size(filletRadius, filletRadius), 0, 180, 270, Scalar(0, 149, 255), 2, 8);
     cv::line(src, cv::Point(p1.x, largestRect.tl().y), cv::Point(p1.x+lineFactor*width, largestRect.tl().y), Scalar(0, 149, 255), 2, 8);
     cv::line(src, cv::Point(largestRect.tl().x, p1.y), cv::Point(largestRect.tl().x, p1.y+lineFactor*height), Scalar(0, 149, 255), 2, 8);
     cv::ellipse(src, p2, Size(filletRadius, filletRadius), 0, 270, 360, Scalar(0, 149, 255), 2, 8);
     cv::line(src, cv::Point(p2.x, largestRect.tl().y), cv::Point(p2.x-lineFactor*width, largestRect.tl().y), Scalar(0, 149, 255), 2, 8);
     cv::line(src, cv::Point(largestRect.br().x, p2.y), cv::Point(largestRect.br().x, p2.y+lineFactor*height), Scalar(0, 149, 255), 2, 8);
     cv::ellipse(src, p3, Size(filletRadius, filletRadius), 0, 0, 90, Scalar(0, 149, 255), 2, 8);
     cv::line(src, cv::Point(p3.x, largestRect.br().y), cv::Point(p3.x-lineFactor*width, largestRect.br().y), Scalar(0, 149, 255), 2, 8);
     cv::line(src, cv::Point(largestRect.br().x, p3.y), cv::Point(largestRect.br().x, p3.y-lineFactor*height), Scalar(0, 149, 255), 2, 8);
     cv::ellipse(src, p4, Size(filletRadius, filletRadius), 0, 90, 180, Scalar(0, 149, 255), 2, 8);
     cv::line(src, cv::Point(p4.x, largestRect.br().y), cv::Point(p4.x+lineFactor*width, largestRect.br().y), Scalar(0, 149, 255), 2, 8);
     cv::line(src, cv::Point(largestRect.tl().x, p4.y), cv::Point(largestRect.tl().x, p4.y-lineFactor*height), Scalar(0, 149, 255), 2, 8);
 }

 void MainWidget::getFaceThreshold()
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
             threshold_face = query.value(3).toDouble();
         }
     }
     db.close();
 }
