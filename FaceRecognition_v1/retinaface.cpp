#include "retinaface.h"

Retinaface::Retinaface()
{

}

void Retinaface::init()
{
    //加载转换并且量化后的网络
    retinafaceNet.load_param("models/Retinaface_mobilenet-sim.param");
    retinafaceNet.load_model("models/Retinaface_mobilenet-sim.bin");

}

vector<Rect> Retinaface::runRetinafaceNet(Mat inputImg)
{
    //读取图片数据
    Mat input_orignal = inputImg.clone();
    inputImg = Retinaface_resize(inputImg);
    Mat input_clone = inputImg.clone();

    //ncnn中数据归一化处理
    ncnn::Mat input1 = ncnn::Mat::from_pixels(input_clone.data, ncnn::Mat::PIXEL_BGR2RGB, input_clone.cols, input_clone.rows);
    const float mean_vals[3] = { 104.0f,117.0f,123.0f };
    const float norm_vals[3] = { 1.0f,1.0f,1.0f };
    input1.substract_mean_normalize(mean_vals, norm_vals);

    // ncnn前向计算
    ncnn::Extractor extractor = retinafaceNet.create_extractor();
    extractor.input("images", input1);

    //取决于模型的输出有几个
    ncnn::Mat outs0, outs1;
    extractor.extract("out0", outs0);//位置信息4
    extractor.extract("out1", outs1);//置信度信息2

    //解码
    float* pdata_0 = outs0;//位置信息
    float* pdata_1 = outs1;//置信度信息
    float confidence_threshold = 0.5;//置信度阈值
    float nmsThreshold = 0.3;//IOU阈值
    int pdata_num = 0;//数据位置信息
    for (int n = 0; n < 3; n++)//三个特征层
    {
        for (int i = 0; i < rerinaface_feature_maps[n * 2]; i++)
        {
            for (int j = 0; j < rerinaface_feature_maps[n * 2 + 1]; j++)
            {
                for (int k = 0; k < 2; k++)//每个特征图对应两个不同的min_sizes，
                {
                    float box_score = outs1[pdata_num * 2 + 1];//置信度大小
                    if (box_score > confidence_threshold)//当大于置信度阈值
                    {
                        confidences.push_back(box_score);//置信度存储
                        //位置信息解码
                        float s_kx, s_ky, cx, cy;//s_kx, s_ky为 feature map 大小，后面两个为：每个框的中心坐标
                        s_kx = rerinaface_min_sizes[n][k] / inpWidth;
                        s_ky = rerinaface_min_sizes[n][k] / inpHeight;
                        cx = (j + 0.5) * rerinaface_steps[n] / inpWidth;
                        cy = (i + 0.5) * rerinaface_steps[n] / inpHeight;

                        float pdata_0_0, pdata_0_1, pdata_0_2, pdata_0_3;//中心、宽高解码
                        pdata_0_0 = pdata_0[pdata_num * 4];
                        pdata_0_1 = pdata_0[pdata_num * 4 + 1];
                        pdata_0_2 = pdata_0[pdata_num * 4 + 2];
                        pdata_0_3 = pdata_0[pdata_num * 4 + 3];

                        pdata_0_0 = cx + pdata_0_0 * rerinaface_variances[0] * s_kx;
                        pdata_0_1 = cy + pdata_0_1 * rerinaface_variances[0] * s_ky;
                        pdata_0_2 = s_kx * exp(pdata_0_2 * rerinaface_variances[1]);
                        pdata_0_3 = s_ky * exp(pdata_0_3 * rerinaface_variances[1]);

                        pdata_0_0 -= pdata_0_2 / 2;
                        pdata_0_1 -= pdata_0_3 / 2;
                        pdata_0_2 += pdata_0_0;
                        pdata_0_3 += pdata_0_1;

                        //将特征图上的坐标转移到实际尺寸的缩放后的图上
                        float x1 = pdata_0_0 * inpWidth;
                        float y1 = pdata_0_1 * inpHeight;
                        float x2 = pdata_0_2 * inpWidth;
                        float y2 = pdata_0_3 * inpHeight;
                        //将目标框从缩放图中的尺寸还原到原始图像尺寸上
                        //目标框对于中心点的偏移量
                        int x1_offset, x2_offset, y1_offset, y2_offset;
                        x1_offset = 640 - x1;
                        x2_offset = 640 - x2;
                        y1_offset = 640 - y1;
                        y2_offset = 640 - y2;
                        //尺寸复原到原图
                        float left = input_orignal.cols / 2 - ((float)x1_offset) * Retinaface_img_zoom;
                        float top = input_orignal.rows / 2 - ((float)y1_offset) * Retinaface_img_zoom;
                        float right = input_orignal.cols / 2 - ((float)x2_offset) * Retinaface_img_zoom;
                        float bottom = input_orignal.rows / 2 - ((float)y2_offset) * Retinaface_img_zoom;
                        boxes.push_back(Rect(ceil(left), ceil(top), ceil(right - left), ceil(bottom - top)));
                    }
                    pdata_num++;
                }
            }
        }
    }

    //采用非最大抑制，消除冗余重叠的高可信度框
    vector<int> indices;
    // 	dnn::NMSBoxes(boxes, confidences, confidence_threshold, nmsThreshold, indices);
    nms_boxes(boxes, confidences, confidence_threshold, nmsThreshold, indices);
    /*for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        drawPred(confidences[idx], box.x, box.y,
            box.x + box.width, box.y + box.height, input_orignal);
    }*/
    vector<Rect>faces;
    for (size_t i = 0; i < indices.size(); ++i)
    {
        faces.push_back(boxes[indices[i]]);
    }
    return faces;
}

//绘制预测边界
void Retinaface::drawPred(float conf, int left, int top, int right, int bottom, Mat& frame)
{
    rectangle(frame, Point(left, top), Point(right, bottom), cv::Scalar(0, 0, 255), 1);
}
//输入图像缩放方式
Mat Retinaface::Retinaface_resize(Mat& input_img)
{
    //Mat input_img = imread(input_image_path);//输入
    Mat output_img(inpWidth, inpHeight, CV_8UC3);//输出
    int input_width = input_img.cols;
    int input_height = input_img.rows;
    if (input_width >= input_height)
    {
        Retinaface_img_zoom = ((float)input_width) / 1280;
        resize(input_img, input_img, Size(1280, input_height / Retinaface_img_zoom));
        copyMakeBorder(input_img, output_img, (1280 - input_img.rows) / 2, (1280 - input_img.rows) / 2, 0, 0, BORDER_CONSTANT, cv::Scalar(128, 128, 128));
    }
    else
    {
        Retinaface_img_zoom = ((float)input_height) / 1280;
        resize(input_img, input_img, Size(input_width / Retinaface_img_zoom, 1280));
        copyMakeBorder(input_img, output_img, 0, 0, (1280 - input_img.cols) / 2, (1280 - input_img.cols) / 2, BORDER_CONSTANT, cv::Scalar(128, 128, 128));
    }
    resize(output_img, output_img, Size(inpWidth, inpHeight));
    return output_img;
}
//非极大值抑制函数
float Retinaface::get_iou_value(Rect rect1, Rect rect2)
{
    int xx1, yy1, xx2, yy2;

    xx1 = max(rect1.x, rect2.x);
    yy1 = max(rect1.y, rect2.y);
    xx2 = min(rect1.x + rect1.width - 1, rect2.x + rect2.width - 1);
    yy2 = min(rect1.y + rect1.height - 1, rect2.y + rect2.height - 1);

    int insection_width, insection_height;
    insection_width = max(0, xx2 - xx1 + 1);
    insection_height = max(0, yy2 - yy1 + 1);

    float insection_area, union_area, iou;
    insection_area = float(insection_width) * insection_height;
    union_area = float(rect1.width * rect1.height + rect2.width * rect2.height - insection_area);
    iou = insection_area / union_area;
    return iou;
}
void Retinaface::nms_boxes(vector<Rect>& boxes, vector<float>& confidences, float confThreshold, float nmsThreshold, vector<int>& indices)
{
    RBOX bbox;
    vector<RBOX> bboxes;
    int i, j;
    for (i = 0; i < boxes.size(); i++)
    {
        bbox.box = boxes[i];
        bbox.confidence = confidences[i];
        bbox.index = i;
        bboxes.push_back(bbox);
    }
    sort(bboxes.begin(), bboxes.end(), sort_score);//通过得分对阈值排序
    while (bboxes.size() > 0)
    {
        indices.push_back(bboxes[0].index);
        int index_num = 1;
        while (index_num < bboxes.size())
        {
            float iou = get_iou_value(bboxes[0].box, bboxes[index_num].box);
            if (iou > nmsThreshold)//当计算值大于设置IOU时
            {
                bboxes.erase(bboxes.begin() + index_num);
            }
            else
                index_num++;
        }
        bboxes.erase(bboxes.begin());
    }
}

