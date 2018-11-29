#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Transformer {
public:
    virtual void transform(Mat &src, Mat &dst) = 0;
    virtual void inverse_transform(Mat &src, Mat &dst) = 0;
};

#endif
