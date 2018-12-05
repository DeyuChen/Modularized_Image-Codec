#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <unordered_map>

#include "dct_transformer.h"
#include "matrix_quantizer.h"
#include "zigzag_reorderer.h"

using namespace cv;
using namespace std;

class Codec {
public:
    Codec(int blocksize = 8);
    void encode(Mat &src, Mat &dst);
    void decode(Mat &src, Mat &dst, int width, int height);

private:
    void encode_block(Mat &src, Mat &dst);
    void decode_block(Mat &src, Mat &dst, int width, int height);
    string encode_quantization_table(Mat &src);
    string encode_huffman_table();

    Transformer *transformer;
    Quantizer *quantizer;
    Reorderer *reorderer;

    int blocksize;
};

Codec::Codec(int blocksize)
    : blocksize(blocksize)
{
    transformer = new DCT_Transformer();
    quantizer = new Matrix_Quantizer_8x8(50);
    reorderer = new ZigZag_Reorderer();
}

void Codec::encode(Mat &src, Mat &dst){
    int nchannel = src.channels();
    if (nchannel > 1){
        vector<Mat> csrc(nchannel), cdst(nchannel);
        split(src, csrc);
        for (int i = 0; i < nchannel; i++){
            encode(csrc[i], cdst[i]);
        }
        merge(cdst, dst);

        return;
    }

    Mat tmp;
    src.convertTo(tmp, CV_32F, 1, -128);
    dst.create(1, tmp.rows * tmp.cols, tmp.type());

    Mat src_block, dst_block;
    int row_rmd = tmp.rows % blocksize;
    int col_rmd = tmp.cols % blocksize;
    int head = 0;
    int y = 0;
    for (int i = 0; i < tmp.rows / blocksize; i++, y += blocksize){
        int x = 0;
        for (int j = 0; j < tmp.cols / blocksize; j++, x += blocksize){
            src_block = tmp(Rect(x, y, blocksize, blocksize));
            dst_block = dst(Rect(head, 0, blocksize * blocksize, 1));
            encode_block(src_block, dst_block);
            head += blocksize * blocksize;
        }
        if (col_rmd){
            src_block = tmp(Rect(x, y, col_rmd, blocksize));
            dst_block = dst(Rect(head, 0, col_rmd * blocksize, 1));
            encode_block(src_block, dst_block);
            head += col_rmd * blocksize;
        }
    }
    if (row_rmd){
        int x = 0;
        for (int j = 0; j < tmp.cols / blocksize; j++, x += blocksize){
            src_block = tmp(Rect(x, y, blocksize, row_rmd));
            dst_block = dst(Rect(head, 0, blocksize * row_rmd, 1));
            encode_block(src_block, dst_block);
            head += blocksize * row_rmd;
        }
        if (col_rmd){
            src_block = tmp(Rect(x, y, col_rmd, row_rmd));
            dst_block = dst(Rect(head, 0, col_rmd * row_rmd, 1));
            encode_block(src_block, dst_block);
            head += col_rmd * row_rmd;
        }
    }
}

void Codec::decode(Mat &src, Mat &dst, int width, int height){
    int nchannel = src.channels();
    if (nchannel > 1){
        vector<Mat> csrc(nchannel), cdst(nchannel);
        split(src, csrc);
        for (int i = 0; i < nchannel; i++){
            decode(csrc[i], cdst[i], width, height);
        }
        merge(cdst, dst);

        return;
    }

    dst.create(height, width, src.type());


    Mat src_block, dst_block;
    int row_rmd = dst.rows % blocksize;
    int col_rmd = dst.cols % blocksize;
    int head = 0;
    int y = 0;
    for (int i = 0; i < dst.rows / blocksize; i++, y += blocksize){
        int x = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, x += blocksize){
            src_block = src(Rect(head, 0, blocksize * blocksize, 1));
            dst_block = dst(Rect(x, y, blocksize, blocksize));
            decode_block(src_block, dst_block, blocksize, blocksize);
            head += blocksize * blocksize;
        }
        if (col_rmd){
            src_block = src(Rect(head, 0, col_rmd * blocksize, 1));
            dst_block = dst(Rect(x, y, col_rmd, blocksize));
            decode_block(src_block, dst_block, col_rmd, blocksize);
            head += col_rmd * blocksize;
        }
    }
    if (row_rmd){
        int x = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, x += blocksize){
            src_block = src(Rect(head, 0, blocksize * row_rmd, 1));
            dst_block = dst(Rect(x, y, blocksize, row_rmd));
            decode_block(src_block, dst_block, blocksize, row_rmd);
            head += blocksize * row_rmd;
        }
        if (col_rmd){
            src_block = src(Rect(head, 0, col_rmd * row_rmd, 1));
            dst_block = dst(Rect(x, y, col_rmd, row_rmd));
            decode_block(src_block, dst_block, col_rmd, row_rmd);
            head += col_rmd * row_rmd;
        }
    }

    dst.convertTo(dst, CV_8UC1, 1, 128);
}

void Codec::encode_block(Mat &src, Mat &dst){
    Mat tmp;
    transformer->transform(src, tmp);
    quantizer->quantize(tmp, tmp);
    reorderer->reorder_2D_to_1D(tmp, dst);
}

void Codec::decode_block(Mat &src, Mat &dst, int width, int height){
    Mat tmp;
    reorderer->reorder_1D_to_2D(src, tmp, width, height);
    quantizer->scale(tmp, tmp);
    transformer->inverse_transform(tmp, dst);
}

int main(int argc, char** argv){
    if (argc < 2){
        cout << "Please specify image path" << endl;
        return -1;
    }
    
    string imageName(argv[1]);

    Mat image;
    image = imread(imageName.c_str(), IMREAD_COLOR);

    if (image.empty()){
        cout <<  "Could not open or find the image" << endl;
        return -1;
    }

    Codec codec;
    Mat image2, fimage, freq;

    codec.encode(image, fimage);
    //fimage.convertTo(freq, CV_8UC1);
    codec.decode(fimage, image2, image.cols, image.rows);

    namedWindow("Original Image", WINDOW_AUTOSIZE);
    //namedWindow("Frequency Domain Image", WINDOW_AUTOSIZE);
    namedWindow("Decoded Image", WINDOW_AUTOSIZE);
    imshow("Original Image", image);
    //imshow("Frequency Domain Image", freq);
    imshow("Decoded Image", image2);
    waitKey(0);
    
    return 0;
}
