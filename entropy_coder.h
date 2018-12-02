#ifndef ENTROPY_CODER_H
#define ENTROPY_CODER_H

#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>

using namespace cv;
using namespace std;

class Entropy_Coder {
public:
    virtual void encode(string &in, stringstream &out) = 0;
    virtual void decode(string &in, stringstream &out) = 0;
};

#endif
