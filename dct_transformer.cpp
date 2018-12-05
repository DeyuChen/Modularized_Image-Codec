#include "dct_transformer.h"

void DCT_Transformer::transform(Mat &src, Mat &dst){
    return transform(src, dst, false);
}

void DCT_Transformer::inverse_transform(Mat &src, Mat &dst){
    return transform(src, dst, true);
}

void DCT_Transformer::transform(Mat &src, Mat &dst, bool inverse){
    transform_channel(src, dst, inverse);

    return;
}

void DCT_Transformer::transform_channel(Mat &src, Mat &dst, bool inverse){
    assert(src.channels() == 1);

    int flag = 0;
    if (inverse){
        flag = DCT_INVERSE;
    }
    
    dct(src, dst, flag);

    return;
}
