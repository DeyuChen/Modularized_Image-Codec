#ifndef ZIGZAG_REORDERER_H
#define ZIGZAG_REORDERER_H

#include "reorderer.h"

class ZigZag_Reorderer : public Reorderer {
public:
    void reorder_2D_to_1D(Mat &src, Mat &dst);
    void reorder_1D_to_2D(Mat &src, Mat &dst, int width, int height);
};

#endif
