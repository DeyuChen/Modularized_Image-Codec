#ifndef DCT_TRANSFORMER_H
#define DCT_TRANSFORMER_H

#include "transformer.h"

using namespace cv;

class DCT_Transformer : public Transformer {
public:
    void transform(Mat &src, Mat &dst);
    void inverse_transform(Mat &src, Mat &dst);
private:
    void transform(Mat &src, Mat &dst, bool inverse);
    void transform_channel(Mat &src, Mat &dst, bool inverse);
};

#endif
