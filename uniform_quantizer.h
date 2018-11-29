#ifndef UNIFORM_QUANTIZER_H
#define UNIFORM_QUANTIZER_H

#include "quantizer.h"

class Uniform_Quantizer : public Quantizer {
public:
    Uniform_Quantizer(float factor) : factor(factor){}
    void quantize(Mat &src, Mat &dst);
    void scale(Mat &src, Mat &dst);

private:
    float factor;
};

#endif
