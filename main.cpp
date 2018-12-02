#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <unordered_map>

#include "dct_transformer.h"
#include "matrix_quantizer.h"

using namespace cv;
using namespace std;

class Reorderer {
public:
    virtual void reorder_2D_to_1D(Mat &src, Mat &dst) = 0;
    virtual void reorder_1D_to_2D(Mat &src, Mat &dst, int width, int height) = 0;
};

template <class T>
class ZigZag_Reorderer : public Reorderer {
public:
    void reorder_2D_to_1D(Mat &src, Mat &dst);
    void reorder_1D_to_2D(Mat &src, Mat &dst, int width, int height);
};

template <class T>
void ZigZag_Reorderer<T>::reorder_2D_to_1D(Mat &src, Mat &dst){
    dst.create(1, src.rows * src.cols, src.type());

    int x = 0, y = 0, i = 1;
    int xm = src.cols - 1, ym = src.rows - 1;
    dst.at<T>(0, 0) = src.at<T>(0, 0);
    while (x < xm || y < ym){
        if ((x + y) % 2){
            if (y == ym)
                x++;
            else if (x == 0)
                y++;
            else {
                x--;
                y++;
            }
        }
        else {
            if (x == xm)
                y++;
            else if (y == 0)
                x++;
            else {
                x++;
                y--;
            }
        }

        dst.at<T>(0, i++) = src.at<T>(y, x);
    }
}

template <class T>
void ZigZag_Reorderer<T>::reorder_1D_to_2D(Mat &src, Mat &dst, int width, int height){
    assert(src.cols == width * height);
    dst.create(height, width, src.type());

    int x = 0, y = 0, i = 1;
    int xm = src.cols - 1, ym = src.rows - 1;
    dst.at<T>(0, 0) = src.at<T>(0, 0);
    while (x < xm || y < ym){
        if ((x + y) % 2){
            if (y == ym)
                x++;
            else if (x == 0)
                y++;
            else {
                x--;
                y++;
            }
        }
        else {
            if (x == xm)
                y++;
            else if (y == 0)
                x++;
            else {
                x++;
                y--;
            }
        }

        dst.at<T>(y, x) = src.at<T>(0, i++);
    }
}

class Codec {
public:
    Codec(int channels, int blocksize = 8);
    void encode(Mat &src, Mat &dst);
    void decode(Mat &src, Mat &dst);

private:
    void encode_block(Mat &src, Mat &dst);
    void decode_block(Mat &src, Mat &dst);
    string encode_quantization_table(Mat &src);
    string encode_huffman_table();

    Transformer *transformer;
    Quantizer *quantizer;
    Reorderer *reorderer;

    int channels;
    int blocksize;
};

Codec::Codec(int channels, int blocksize)
    : channels(channels), blocksize(blocksize)
{
    transformer = new DCT_Transformer();
    quantizer = new Matrix_Quantizer_8x8(50);
    if (channels == 1){
        reorderer = new ZigZag_Reorderer<float>();
    }
    else if (channels == 3){
        reorderer = new ZigZag_Reorderer<Vec3f>();
    }
}

void Codec::encode(Mat &src, Mat &dst){
    src.convertTo(dst, CV_32F, 1, -128);

    Mat block;
    int row_rmd = dst.rows % blocksize;
    int col_rmd = dst.cols % blocksize;
    int y = 0;
    for (int i = 0; i < dst.rows / blocksize; i++, y += blocksize){
        int x = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, x += blocksize){
            block = dst(Rect(x, y, blocksize, blocksize));
            encode_block(block, block);
        }
        if (col_rmd){
            block = dst(Rect(x, y, col_rmd, blocksize));
            encode_block(block, block);
        }
    }
    if (row_rmd){
        int x = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, x += blocksize){
            block = dst(Rect(x, y, blocksize, row_rmd));
            encode_block(block, block);
        }
        if (col_rmd){
            block = dst(Rect(x, y, col_rmd, row_rmd));
            encode_block(block, block);
        }
    }
}

void Codec::decode(Mat &src, Mat &dst){
    dst = src;

    Mat block;
    int row_rmd = dst.rows % blocksize;
    int col_rmd = dst.cols % blocksize;
    int y = 0;
    for (int i = 0; i < dst.rows / blocksize; i++, y += blocksize){
        int x = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, x += blocksize){
            block = dst(Rect(x, y, blocksize, blocksize));
            decode_block(block, block);
        }
        if (col_rmd){
            block = dst(Rect(x, y, col_rmd, blocksize));
            decode_block(block, block);
        }
    }
    if (row_rmd){
        int x = 0;
        for (int j = 0; j < dst.cols / blocksize; j++, x += blocksize){
            block = dst(Rect(x, y, blocksize, row_rmd));
            decode_block(block, block);
        }
        if (col_rmd){
            block = dst(Rect(x, y, col_rmd, row_rmd));
            decode_block(block, block);
        }
    }

    dst.convertTo(dst, CV_8UC3, 1, 128);
}

void Codec::encode_block(Mat &src, Mat &dst){
    Mat tmp;
    transformer->transform(src, tmp);
    quantizer->quantize(tmp, dst);
    reorderer->reorder_2D_to_1D(dst, tmp);
}

void Codec::decode_block(Mat &src, Mat &dst){
    Mat tmp;
    quantizer->scale(src, tmp);
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

    Codec codec(image.channels());
    Mat image2, fimage, freq;

    codec.encode(image, fimage);
    fimage.convertTo(freq, CV_8UC1);
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
