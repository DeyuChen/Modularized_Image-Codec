#include "uniform_quantizer.h"

void Uniform_Quantizer::quantize(Mat &src, Mat &dst){
    dst = src / factor;
    for (int i = 0; i < dst.rows; i++){
        for (int j = 0; j < dst.cols; j++){
            dst.at<float>(i, j) = round(dst.at<float>(i, j));
        }
    }
}

void Uniform_Quantizer::scale(Mat &src, Mat &dst){
    dst = src * factor;
}
