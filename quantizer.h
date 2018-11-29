#ifndef QUANTIZER_H
#define QUANTIZER_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Quantizer {
public:
    virtual void quantize(Mat &src, Mat &dst) = 0;
    virtual void scale(Mat &src, Mat &dst) = 0;
};

#endif
