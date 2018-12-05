#include "zigzag_reorderer.h"

void ZigZag_Reorderer::reorder_2D_to_1D(Mat &src, Mat &dst){
    dst.create(1, src.rows * src.cols, src.type());

    int x = 0, y = 0, i = 1;
    int xm = src.cols - 1, ym = src.rows - 1;
    dst.at<float>(0, 0) = src.at<float>(0, 0);
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

        dst.at<float>(0, i++) = src.at<float>(y, x);
    }
}

void ZigZag_Reorderer::reorder_1D_to_2D(Mat &src, Mat &dst, int width, int height){
    assert(src.cols == width * height);
    dst.create(height, width, src.type());

    int x = 0, y = 0, i = 1;
    int xm = dst.cols - 1, ym = dst.rows - 1;
    dst.at<float>(0, 0) = src.at<float>(0, 0);
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

        dst.at<float>(y, x) = src.at<float>(0, i++);
    }
}
