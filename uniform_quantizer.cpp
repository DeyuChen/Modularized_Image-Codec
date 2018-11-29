#include "uniform_quantizer.h"

void Uniform_Quantizer::quantize(Mat &src, Mat &dst){
    dst = src / factor;
    for (int i = 0; i < dst.rows; i++){
        for (int j = 0; j < dst.cols; j++){
            dst.at<Vec3f>(i, j)[0] = round(dst.at<Vec3f>(i, j)[0]);
            dst.at<Vec3f>(i, j)[1] = round(dst.at<Vec3f>(i, j)[1]);
            dst.at<Vec3f>(i, j)[2] = round(dst.at<Vec3f>(i, j)[2]);
        }
    }
}

void Uniform_Quantizer::scale(Mat &src, Mat &dst){
    dst = src * factor;
}
