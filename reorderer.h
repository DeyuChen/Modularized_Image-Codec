#ifndef REORDERER_H
#define REORDERER_H

#include <opencv2/opencv.hpp>

using namespace cv;

class Reorderer {
public:
    virtual void reorder_2D_to_1D(Mat &src, Mat &dst) = 0;
    virtual void reorder_1D_to_2D(Mat &src, Mat &dst, int width, int height) = 0;
};

#endif
