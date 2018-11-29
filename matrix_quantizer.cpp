#include "matrix_quantizer.h"

void Matrix_Quantizer::quantize(Mat &src, Mat &dst){
    int nchannel = src.channels();
    vector<Mat> cdst(nchannel);
    split(src, cdst);
    for (int k = 0; k < nchannel; k++){
        divide(cdst[k], matrix, cdst[k]);
        for (int i = 0; i < src.rows; i++){
            for (int j = 0; j < src.cols; j++){
                cdst[k].at<float>(i, j) = round(cdst[k].at<float>(i, j));
            }
        }
    }
    merge(cdst, dst);
}

void Matrix_Quantizer::scale(Mat &src, Mat &dst){
    int nchannel = src.channels();
    vector<Mat> cdst(nchannel);
    split(src, cdst);
    for (int i = 0; i < nchannel; i++){
        multiply(cdst[i], matrix, cdst[i]);
    }
    merge(cdst, dst);
}

Matrix_Quantizer_8x8::Matrix_Quantizer_8x8(int quality){
    vector<float> arr(64);
    switch (quality){
        case 50:
        default:
            arr = { 16,  11,  10,  16,  24,  40,  51,  61,
                    12,  12,  14,  19,  26,  58,  60,  55,
                    14,  13,  16,  24,  40,  57,  69,  56,
                    14,  17,  22,  29,  51,  87,  80,  62,
                    18,  22,  37,  56,  68, 109, 103,  77,
                    24,  35,  55,  64,  81, 104, 113,  92,
                    49,  64,  78,  87, 103, 121, 120, 101,
                    72,  92,  95,  98, 112, 100, 103,  99};
    }

    Mat matrix(8, 8, CV_32F, arr.data());
    setMatrix(matrix);
}
