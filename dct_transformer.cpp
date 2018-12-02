#include "dct_transformer.h"

void DCT_Transformer::transform(Mat &src, Mat &dst){
    return transform(src, dst, false);
}

void DCT_Transformer::inverse_transform(Mat &src, Mat &dst){
    return transform(src, dst, true);
}

void DCT_Transformer::transform(Mat &src, Mat &dst, bool inverse){
    int nchannel = src.channels();
    if (nchannel == 1){
        transform_channel(src, dst, inverse);
    }
    else {
        vector<Mat> csrc(nchannel), cdst(nchannel);
        split(src, csrc);
        for (int i = 0; i < nchannel; i++){
            transform_channel(csrc[i], cdst[i], inverse);
        }
        merge(cdst, dst);
    }

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
