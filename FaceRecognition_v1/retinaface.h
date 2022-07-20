#ifndef RETINAFACE_H
#define RETINAFACE_H

#include<opencv2/opencv.hpp>
#include "net.h"

using namespace cv;
using namespace std;

//人脸检测
struct RBOX
{
    Rect box;
    float confidence;
    int index;
};
static bool sort_score(RBOX box1, RBOX box2)
{
    return box1.confidence > box2.confidence ? true : false;
}

//输出模型的图像尺寸
const int inpWidth = 1280;
const int inpHeight = 1280;

//网络基本属性
const float rerinaface_steps[3] = { 8.0, 16.0, 32.0 };//特征层步长
const int rerinaface_feature_maps[6] = { 160,160,80,80,40,40 };
const float rerinaface_min_sizes[3][2] = { {16.0, 32.0}, {64.0, 128.0}, {256.0, 512.0} };
const float rerinaface_variances[2] = { 0.1, 0.2 };

class Retinaface
{
public:
    Retinaface();
    vector<Rect> runRetinafaceNet(Mat inputImg);
    void init();

private:
    ncnn::Net retinafaceNet;

    float Retinaface_img_zoom;//图像缩放比例(宽和高的缩放比例)
    vector<float> confidences;//置信度
    vector<Rect> boxes;//位置框
    float get_iou_value(Rect rect1, Rect rect2);
    void nms_boxes(vector<Rect>& boxes, vector<float>& confidences, float confThreshold, float nmsThreshold, vector<int>& indices);
    void drawPred(float conf, int left, int top, int right, int bottom, Mat& frame);
    Mat Retinaface_resize(Mat& input_img);
};

#endif // RETINAFACE_H
