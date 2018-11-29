#ifndef MATRIX_QUANTIZER_H
#define MATRIX_QUANTIZER_H

#include "quantizer.h"

class Matrix_Quantizer : public Quantizer {
public:
    void quantize(Mat &src, Mat &dst);
    void scale(Mat &src, Mat &dst);
    void setMatrix(const Mat &_matrix){matrix = _matrix.clone();}

private:
    Mat matrix;
};

class Matrix_Quantizer_8x8 : public Matrix_Quantizer {
public:
    Matrix_Quantizer_8x8(int quality);
};

#endif
