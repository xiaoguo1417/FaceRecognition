#ifndef ARCFACE_H
#define ARCFACE_H

#include "string"
#include "net.h"
#include "vector"

const int feature_dim = 128;
const int resize_w_h = 112;

class Arcface
{
public:
    Arcface();
    ~Arcface();
    bool init(const std::string& binPath, const std::string& paramPath);
    float* getFeature(ncnn::Mat& img);
    double calculSimilar(float* feat1, float* feat2);
private:
    ncnn::Net arcfaceNet;
    float* runNet(ncnn::Mat& img);
};

#endif // ARCFACE_H
