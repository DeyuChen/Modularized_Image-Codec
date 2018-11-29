#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

#include "dct_transformer.h"
#include "matrix_quantizer.h"

using namespace cv;
using namespace std;

class Codec {
public:
    Codec(int blocksize = 8);
    void encode(Mat &src, Mat &dst);
    void decode(Mat &src, Mat &dst);

private:
    void encode_block(Mat &src, Mat &dst);
    void decode_block(Mat &src, Mat &dst);

    Transformer *transformer;
    Quantizer *quantizer;

    int blocksize;
};

Codec::Codec(int blocksize) : blocksize(blocksize){
    transformer = new DCT_Transformer();
    quantizer = new Matrix_Quantizer_8x8(50);
}

void Codec::encode(Mat &src, Mat &dst){
    src.convertTo(dst, CV_32F, 1, -128);

    Mat block;
    int row_rmd = dst.rows % blocksize;
    int col_rmd = dst.cols % blocksize;
    int x = 0;
    for (int i = 0; i < dst.rows / blocksize; i++, x += blocksize){
        int y = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, y += blocksize){
            block = dst(Rect(x, y, blocksize, blocksize));
            encode_block(block, block);
        }
        if (col_rmd){
            block = dst(Rect(x, y, blocksize, col_rmd));
            encode_block(block, block);
        }
    }
    if (row_rmd){
        int y = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, y += blocksize){
            block = dst(Rect(x, y, row_rmd, blocksize));
            encode_block(block, block);
        }
        if (col_rmd){
            block = dst(Rect(x, y, row_rmd, col_rmd));
            encode_block(block, block);
        }
    }
}

void Codec::decode(Mat &src, Mat &dst){
    dst = src;

    Mat block;
    int row_rmd = dst.rows % blocksize;
    int col_rmd = dst.cols % blocksize;
    int x = 0;
    for (int i = 0; i < dst.rows / blocksize; i++, x += blocksize){
        int y = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, y += blocksize){
            block = dst(Rect(x, y, blocksize, blocksize));
            decode_block(block, block);
        }
        if (col_rmd){
            block = dst(Rect(x, y, blocksize, col_rmd));
            decode_block(block, block);
        }
    }
    if (row_rmd){
        int y = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, y += blocksize){
            block = dst(Rect(x, y, row_rmd, blocksize));
            decode_block(block, block);
        }
        if (col_rmd){
            block = dst(Rect(x, y, row_rmd, col_rmd));
            decode_block(block, block);
        }
    }

    dst.convertTo(dst, CV_8UC3, 1, 128);
}

void Codec::encode_block(Mat &src, Mat &dst){
    transformer->transform(src, dst);
    quantizer->quantize(src, dst);
}

void Codec::decode_block(Mat &src, Mat &dst){
    quantizer->scale(src, dst);
    transformer->inverse_transform(src, dst);
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

    Mat image2, fimage;
    Codec codec(8);

    image2.create(image.rows, image.cols, CV_8UC3);

    codec.encode(image, fimage);
    //transformer.transform(fimage, fimage);
    //quantizer.quantize(fimage, fimage);

    Mat freq;
    fimage.convertTo(freq, CV_8UC1);

    //quantizer.scale(fimage, fimage);
    //transformer.inverse_transform(fimage, fimage);
    //fimage.convertTo(image2, CV_8UC3, 1, 128);
    codec.decode(fimage, image2);


    namedWindow("Original Image", WINDOW_AUTOSIZE);
    namedWindow("Frequency Domain Image", WINDOW_AUTOSIZE);
    namedWindow("Decoded Image", WINDOW_AUTOSIZE);
    imshow("Original Image", image);
    imshow("Frequency Domain Image", freq);
    imshow("Decoded Image", image2);
    waitKey(0);
    
    return 0;
}
