#include "addusers.h"
#include "ui_addusers.h"
#include <QTime>
#include<QDebug>

AddUsers::AddUsers(int m_index, QWidget *parent) :
    index(m_index), QWidget(parent),
    ui(new Ui::AddUsers)
{
    ui->setupUi(this);

    connect(&theTimer, SIGNAL(timeout()), this, SLOT(updateImage()));
    connect(ui->bt_openCamera, SIGNAL(clicked()), this, SLOT(onBtOpenCameraClicked()));
    connect(ui->bt_shoot, SIGNAL(clicked()), this, SLOT(onBtShootClicked()));
    connect(ui->bt_confirm, SIGNAL(clicked()), this, SLOT(onBtConfirmClicked()));
    connect(ui->bt_return, SIGNAL(clicked()), this, SLOT(onBtReturnClicked()));

    show_index = index;
    ui->label_index->setText(QString::number(index));

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

    retinaface.init();
}

AddUsers::~AddUsers()
{
    capture.release();
    delete ui;
}

void AddUsers::updateImage()
{
    capture>>srcImage;
    src = srcImage.clone();//保存RGB原始图像
    //cv::resize(src, src, cv::Size(), 1.2, 1.2, cv::INTER_LINEAR_EXACT );
    cvtColor(srcImage, srcImage, CV_BGR2RGB);//Qt中支持的是RGB图像, OpenCV中支持的是BGR
    //人脸检测
    faces.clear();
    Size minFeatureSize(50, 50);
    count++;
    //cv::resize(srcImage, srcImage, cv::Size(), 0.8, 0.8, cv::INTER_LINEAR_EXACT );
    count %=3;
    if (count == 0){
    //QTime time;
    //time.start();
    faces_cascade.detectMultiScale(srcImage, faces, 1.1f, 2, CASCADE_SCALE_IMAGE, minFeatureSize);
    //qDebug()<<time.elapsed()/1000.0<<"s";
    }

    //retinaface
    /*if (count == 0){
    QTime time;
    time.start();
    faces = retinaface.runRetinafaceNet(srcImage);
    qDebug()<<time.elapsed()/1000.0<<"s";}*/

    if (faces.size() == 0)
    {
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
            //roi = srcImage(largestRect).clone();
            //srcImage(faces[i]).copyTo(roi);
            //圆角矩形绘制
            drawRoundedRectangle(srcImage, largestRect);
            //cv::rectangle(srcImage, largestRect, cv::Scalar(255, 0, 0), 2, 8, 0);
        }
        cv::flip(srcImage, image_fliped, 1);

        if(image_fliped.data)
        {
            this->update();  //发送刷新消息
        }
        return;
    }

    //画矩形框
    /*cv::Point text_lb;
    for (size_t i = 0; i < faces.size(); i++)
    {
        if (faces[i].height > 0 && faces[i].width > 0)
        {
            text_lb = cv::Point(faces[i].x, faces[i].y);
            //std::cout<<faces.size()<<" "<<faces[i].height<<" "<<faces[i].width<<endl;

            int lt_x = min(text_lb.x, srcImage.cols);
            lt_x = max(0, text_lb.x);
            int lt_y = min(text_lb.y, srcImage.rows);
            lt_y = max(0, text_lb.y);
            int h = min(faces[i].height, srcImage.rows - lt_y - 1);
            int w = min(faces[i].width, srcImage.cols - lt_x - 1);

            faces[i] = Rect(lt_x, lt_y, w, h);
            //cv::Mat show_roi;
            //cv::Mat image_roi;
            //cv::Rect rect = Rect(text_lb.x + faces[i].width / 2 - 100,text_lb.y + faces[i].height / 2 - 100, 200, 200);
            //if(rect.x < 0 || rect.y < 0 || rect.x + 200 >= srcImage.cols || rect.y + 200 >= srcImage.rows)
            //{
            //    return;
            //}
            //src(rect).copyTo(show_roi);
            //src(faces[0]).copyTo(image_roi);
            //cv::imwrite("image0.png", src);
            //cv::imwrite("image_roi.png", image_roi);
            //cv::imwrite("show_roi.png", show_roi);

            cv::rectangle(srcImage, faces[i], cv::Scalar(255, 0, 0), 2, 8, 0);
        }
     }*/

    //mtcnn
    /*std::vector<Bbox> faces_info = detect_mtcnn(srcImage);
    if (faces_info.size() == 0)
    {
        return;
    }
    cv::Rect largestRect = Rect(Point(faces_info[0].x1, faces_info[0].y1), Point(faces_info[0].x2, faces_info[0].y2));*/

    largestRect = findLargestRect(faces);
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
        //roi = srcImage(largestRect).clone();
        //srcImage(faces[i]).copyTo(roi);
        //圆角矩形绘制
        drawRoundedRectangle(srcImage, largestRect);
        //cv::rectangle(srcImage, largestRect, cv::Scalar(255, 0, 0), 2, 8, 0);
    }

    cv::flip(srcImage, image_fliped, 1);

    if(image_fliped.data)
    {
        this->update();  //发送刷新消息
    }
}

/**
 * @brief MainWindow::paintEvent
 * @param e
 * 重写方法
 */
void AddUsers::paintEvent(QPaintEvent *e)
{
    //将视频流绘制到label中
    QImage image = QImage((uchar*)(image_fliped.data), image_fliped.cols, image_fliped.rows, QImage::Format_RGB888);
    ui->label_camera->setPixmap(QPixmap::fromImage(image));
    //ui->camera->resize(srcImage);
    ui->label_camera->show();
}

void AddUsers::onBtOpenCameraClicked()
{
    if(capture.isOpened())
    {
        return;
    }

    if(capture.open(0))
    {
        //从摄像头捕获视频
        srcImage = Mat::zeros(capture.get(CV_CAP_PROP_FRAME_HEIGHT), capture.get(CV_CAP_PROP_FRAME_WIDTH), CV_8UC3);
        theTimer.start(40);
    }
    else
    {
        //输出打开失败
        //QMessageBox::critical(this, tr("错误提示"), tr("摄像头打开失败！"), "关闭");
    }
}

void AddUsers::onBtShootClicked()
{
    //写图片
    cv::Point text_lb = cv::Point(largestRect.x, largestRect.y);
    int lt_x = min(text_lb.x, srcImage.cols);
    lt_x = max(0, text_lb.x);
    int lt_y = min(text_lb.y, srcImage.rows);
    lt_y = max(0, text_lb.y);
    int h = min(largestRect.height, srcImage.rows - lt_y - 1);
    int w = min(largestRect.width, srcImage.cols - lt_x - 1);

    largestRect = Rect(lt_x, lt_y, w, h);
    cv::Mat show_temp;
    src(largestRect).copyTo(show_temp);
    cv::flip(show_temp, show_temp, 1);
    cv::imwrite("imgs/show_temp.png", show_temp);
    cv::Mat  show_src;
    cv::flip(src, show_src, 1);
    cv::imwrite("imgs/show_src.png", show_src);

    QImage image = QImage("imgs/show_temp.png");
    ui->label_image->setPixmap(QPixmap::fromImage(image));
    //ui->camera->resize(srcImage);
    ui->label_image->show();
}

void AddUsers::onBtConfirmClicked()
{
    theTimer.stop();
    capture.release();

    //图片image_roi训练模型    
    cv::Mat image_roi = imread("imgs/show_temp.png");
    Arcface arcface;
    arcface.init("models/arcface.bin", "models/arcface.param");
    ncnn::Mat ncnn_roi = ncnn::Mat::from_pixels(image_roi.data, ncnn::Mat::PIXEL_BGR2RGB, image_roi.cols, image_roi.rows);
    float* feature1 = arcface.getFeature(ncnn_roi);

    //保存注册照片
    QString save_name = "imgs/show_img_";
    save_name += QString::number(show_index);
    save_name += ".png";
    cv::imwrite(save_name.toStdString(), image_roi);

    //将名字和序号传回用户管理界面(并保存至相应文件)，训练数据进行记录
    sig_add(ui->lineEdit_name->text(), feature1);

    QMessageBox::about(this, tr("Tips:"), tr(" Add Successfully!"));

    //退出界面
    this->close();
}

void AddUsers::onBtReturnClicked()
{
    //退出界面
    theTimer.stop();
    capture.release();
    this->close();
}

void AddUsers::drawRoundedRectangle(cv::Mat& src, cv::Rect largestRect)
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

cv::Rect AddUsers::findLargestRect(std::vector<cv::Rect>faces)
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
