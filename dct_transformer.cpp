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

    dst.create(src.rows, src.cols, CV_32F);

    int row_rmd = src.rows % blocksize;
    int col_rmd = src.cols % blocksize;
    int x = 0;
    for (int i = 0; i < src.rows / blocksize; i++, x += blocksize){
        int y = 0;
        for (int j = 0; j < src.cols / blocksize; j++, y += blocksize){
            dct(src(Rect(x, y, blocksize, blocksize)), dst(Rect(x, y, blocksize, blocksize)), flag);
        }
        if (col_rmd){
            dct(src(Rect(x, y, blocksize, col_rmd)), dst(Rect(x, y, blocksize, col_rmd)), flag);
        }
    }
    if (row_rmd){
        int y = 0;
        for (int j = 0; j < src.cols / blocksize; j++, y += blocksize){
            dct(src(Rect(x, y, row_rmd, blocksize)), dst(Rect(x, y, row_rmd, blocksize)), flag);
        }
        if (col_rmd){
            dct(src(Rect(x, y, row_rmd, col_rmd)), dst(Rect(x, y, row_rmd, col_rmd)), flag);
        }
    }

    return;
}
