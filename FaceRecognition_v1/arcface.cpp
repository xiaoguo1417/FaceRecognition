#include "arcface.h"

Arcface::Arcface()
{

}

Arcface::~Arcface()
{
    arcfaceNet.clear();
}

bool Arcface::init(const std::string& binPath, const std::string& paramPath)
{
    int ret = arcfaceNet.load_param(paramPath.data());

    if (ret != 0)
    {
        return false;
    }

    ret = arcfaceNet.load_model(binPath.data());

    if (ret != 0)
    {
        return false;
    }

    return true;
}

float* Arcface::getFeature(ncnn::Mat& img)
{
    float* feature = runNet(img);
    return feature;
}

float* Arcface::runNet(ncnn::Mat& img)
{
    ncnn::Extractor ex = arcfaceNet.create_extractor();
    ex.set_light_mode(true);
    ncnn::Mat in;
    ncnn::resize_bilinear(img, in, resize_w_h, resize_w_h);
    ex.input("data", in);
    ncnn::Mat out;
    ex.extract("fc1", out);
    float* feature = new float[feature_dim];
    for (int j = 0; j < 128; j++)
    {
        feature[j] = out[j];
    }
    return feature;
}

double Arcface::calculSimilar(float* feature1, float* feature2)
{
    double ret = 0.0;
    double mod1 = 0.0;
    double mod2 = 0.0;

    for (std::vector<double>::size_type i = 0; i != feature_dim; i++)
    {
        ret += feature1[i] * feature2[i];
        mod1 += feature1[i] * feature1[i];
        mod2 += feature2[i] * feature2[i];
    }

    return (ret / sqrt(mod1) / sqrt(mod2) + 1) / 2.0;

}
