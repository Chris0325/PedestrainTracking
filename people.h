//
// Created by chris on 18-1-22.
//

#ifndef MYTEST_PEOPLE_H
#define MYTEST_PEOPLE_H

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

struct People {
    cv::Rect2i pos;
    cv::Point2i center;
    cv::Point2i speed{0, 0};
    bool isIn;
    int interDur;
    int missingCnt;
    cv::Mat roi;
    People *left;
    People *right;

    float momentOnTrack;
    std::vector<cv::Point2i> trackings;
    People() :
            pos(0, 0, 0, 0), isIn(true), missingCnt(0),
            interDur(0), left(nullptr), right(nullptr), momentOnTrack(0) {
        ;
    }

    ~People() = default;
};



#endif //MYTEST_PEOPLE_H
