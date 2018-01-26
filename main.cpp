#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include "people.h"

bool iou(People *p1, People* p2) {
    return 5 * (p1->pos & p2->pos).area() > (p1->pos | p2->pos).area();
}

int main() {
    auto fgbg = cv::createBackgroundSubtractorMOG2();
    constexpr double minArea = 500;
    constexpr double maxArea = 4000;
    constexpr int nTolCnt = 25;
    constexpr int nTolChangeCnt = 0;
    constexpr int nSample = 100;
    constexpr int boundaryTol = 10;

    constexpr int vmin = 10, vmax = 256, smin = 30;

    int nPeopleIn = 0;
    int nPeopleOut = 0;
    std::vector<double> pSample;
    int averageArea = 0;


    auto *nil = new People();
    nil->left = nil;
    nil->right = nil;

    People *peopleList = nil;

    cv::VideoCapture cap("./peopleCounter.avi");

    std::vector<cv::Point2i> contor{{221, 128}, {445, 178}, {397, 366}, {314, 364}, {197, 311}};
    cv::Mat prevFrame;
    cv::Mat frame;
    std::vector<std::vector<cv::Point2i>> contours;
    cv::Mat fgmask;

    int cnt = 0;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
//    std::cout << kernel << std::endl;
    std::vector<cv::Rect2i> detectedContours;
    cv::Rect2i wholeBox(0, 0, cap.get(CV_CAP_PROP_FRAME_WIDTH) - 1, cap.get(CV_CAP_PROP_FRAME_HEIGHT) - 1);
    while (cap.isOpened()) {
        cap >> frame;
//        std::cout << cap.get(CV_CAP_PROP_FRAME_COUNT) << std::endl;
//        std::cout << cap.get(CV_CAP_PROP_FPS) << std::endl;
//        std::cout << cap.get(CV_CAP_PROP_POS_MSEC) << std::endl;
        if (frame.empty()) {
//            std::cout << cnt << std::endl;
            std::cout << "open video error" << std::endl;
            break;
        }

        cv::Mat gray;
        cv::cvtColor(frame, gray, CV_BGR2GRAY);
//        std::cout << gray.channels() << std::endl;
//        cv::polylines(frame, contor, true, cv::Scalar(0, 0, 255), 1, 8);
//        cv::imshow("img", gray);
//        cv::waitKey(0);

        fgbg->apply(gray, fgmask);

        cv::medianBlur(fgmask,fgmask, 5);
        cv::threshold(fgmask, fgmask, 127, 255, CV_THRESH_BINARY);

        cv::morphologyEx(fgmask, fgmask, cv::MORPH_CLOSE, kernel);
        cv::morphologyEx(fgmask, fgmask, cv::MORPH_OPEN, kernel);
//        cv::imshow("img", fgmask);
//        cv::waitKey(10);
        cv::findContours(fgmask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
//        cv::drawContours(frame, contours)
//        for (int i = 0; i < contours.size(); i++) {
//            cv::drawContours(frame, contours, i, cv::Scalar(0, 0, 255), 4);
//        }
//        cv::imshow("img", frame);
//        cv::waitKey(10);


        int N = contours.size();

//        for (int i = 0; i < N; i++) {
//            cv::drawContours(frame, contours, i, cv::Scalar(0, 0, 255), 1);
//        }

//        for (int i = 0; i < N; i++) {
//            detectedContours.push_back(cv::boundingRect(contours[i]));
//        }

        if (cnt == nSample) {
            for (auto x : pSample) {
                averageArea += x;
            }
            averageArea /= nSample;
        }

        for (auto &c : contours) {
            double area = cv::contourArea(c);
            if (pSample.size() < nSample) {
                if (area < minArea || area > maxArea) {
                    continue;
                } else {
                    pSample.push_back(area);
                }
            } else {
                if (area < averageArea / 2 || area > averageArea * 3) {
                    continue;
                }
            }
            detectedContours.push_back(cv::boundingRect(c));
        }

        int M = detectedContours.size();
        std::vector<int> marked(M, 0);

        People *p = peopleList->right;
//        while (p != nil) {
//            if (p->right != nil) {
//                if (iou(p, p->right)) {
//                    p->right->left = p->left;
//                    p->left->right = p->right;
//                    delete p;
//                }
//            }
//            p = p->right;
//        }
//        p = peopleList->right;
        while (p != nil) {

            bool isFound = false;

            cv::Mat hsv, hue, mask, hist, backproj;
            cv::cvtColor(prevFrame, hsv, CV_BGR2HSV);
            cv::inRange(hsv, cv::Scalar(0, smin, vmin), cv::Scalar(180, 256, vmax), mask);
            int ch[] = {0, 0};
            hue.create(hsv.size(), hsv.depth());
            cv::mixChannels(&hsv, 1, &hue, 1, ch, 1);
            cv::Mat roi(hue, p->pos), maskroi(mask, p->pos);
            int hsize = 16;
            float hranges[] = {0, 180};
            const float *phranges = hranges;
            cv::calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
            cv::normalize(hist, hist, 0, 255, cv::NORM_MINMAX);

            cv::calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
            backproj &= mask;
            cv::RotatedRect camPos = cv::CamShift(backproj, p->pos,
                                           cv::TermCriteria(cv::TermCriteria::EPS |
                                                                    cv::TermCriteria::COUNT, 10, 1));

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////// code coming from pedestrianCounter for camshift ///////////////////////////////////////
//// /////////////// you can run the following for comparison        ///////////////////////////////////////

//            cv::Mat prevGray;
//            cv::cvtColor(prevFrame, prevGray, CV_BGR2GRAY);
//            cv::Mat hsv_roi = cv::Mat(prevGray, p->pos);
//            cv::Mat mask;
//            cv::inRange(hsv_roi, 1, 256, mask);
//            cv::Mat roi_hist;
//            int Channels[] = {0};
//            int HistSize[] = {100};
//            float Arange[] = {0, 256};
//            const float *HistRange[] = {Arange};
//            cv::calcHist(&hsv_roi, 1, Channels, mask, roi_hist, 1, HistSize, HistRange);
//            cv::normalize(roi_hist, roi_hist, 0, 255, cv::NORM_MINMAX);
//            cv::Mat dst;
//            cv::calcBackProject(&gray, 1, Channels, roi_hist, dst, HistRange, 1);
//            cv::RotatedRect camPos = cv::CamShift(dst, p->pos,
//                                           cv::TermCriteria(cv::TermCriteria::EPS |
//                                                                    cv::TermCriteria::COUNT, 1, 1));
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////


            for (int i = 0; i < M; i++) {
                if (marked[i] == 1) {
                    continue;
                }

                if (detectedContours[i].x + boundaryTol < camPos.center.x && camPos.center.x < detectedContours[i].x + detectedContours[i].width - boundaryTol &&
                        detectedContours[i].y + boundaryTol < camPos.center.y && camPos.center.y < detectedContours[i].y + detectedContours[i].height - boundaryTol) {
                    p->pos = detectedContours[i];
                    p->speed = cv::Point2i(camPos.center) - p->center;
                    p->center = camPos.center;
                    p->trackings.push_back(p->center);
                    p->missingCnt = 0;
                    marked[i] = 1;

                    isFound = true;

                    bool prevStatus = p->isIn;
                    bool curStatus = cv::pointPolygonTest(contor, camPos.center, false) > 0;
                    p->isIn = curStatus;

                    if (prevStatus == curStatus) {
                        p->interDur++;
                    } else {
                        if (p->interDur > nTolChangeCnt) {

                            if (prevStatus) {
                                cv::drawContours(frame, std::vector<std::vector<cv::Point2i>>(1, contor), 0, cv::Scalar(255, 0, 0), 1);

                                int len = p->trackings.size();
                                for (int i = 0; i < len - 1; i++) {
                                    cv::line(frame, p->trackings[i], p->trackings[i+1], cv::Scalar(0, 0, 255), 3);
                                }
                                cv::rectangle(frame, p->pos, cv::Scalar(0, 0, 255));

                                cv::imshow("img", frame);
                                cv::waitKey(0);
                                cv::imwrite("img" + std::to_string(cnt) + ".jpg", frame);
                                nPeopleOut++;
                            } else {
                                cv::drawContours(frame, std::vector<std::vector<cv::Point2i>>(1, contor), 0, cv::Scalar(255, 0, 0), 1);
                                int len = p->trackings.size();
                                for (int i = 0; i < len - 1; i++) {
                                    cv::line(frame, p->trackings[i], p->trackings[i+1], cv::Scalar(0, 0, 255), 3);
                                }
                                cv::rectangle(frame, p->pos, cv::Scalar(0, 0, 255));

                                cv::imshow("img", frame);
                                cv::waitKey(0);
                                cv::imwrite("img" + std::to_string(cnt) + ".jpg", frame);
                                nPeopleIn++;
                            }
                        }
                        p->interDur = 0;
                    }
                    break;
                }
            }

            if (!isFound) {
                if (p->missingCnt > nTolCnt) {
                    auto tmp = p->right;
                    p->left->right = p->right;
                    p->right->left = p->left;
                    delete p;
                    p = tmp;
                    continue;
                } else {
                    p->missingCnt++;
                    p->pos.x += p->speed.x;
                    p->pos.y += p->speed.y;
                    p->center.x += p->speed.x;
                    p->center.y += p->speed.y;
                    p->trackings.push_back(p->center);
                    p->pos &= wholeBox;

                    if (pSample.size() < nSample) {
                        if (p->pos.area() < minArea) {
                            auto tmp = p->right;
                            p->right->left = p->left;
                            p->left->right = p->right;
                            delete p;
                            p = tmp;
                            continue;
                        }
                    } else {
                        if (p->pos.area() < averageArea / 2) {
                            std::cout << "deleted people " << p->pos.area() << std::endl;
//                            int len = p->trackings.size();
//                            for (int i = 0; i < len - 1; i++) {
//                                cv::line(frame, p->trackings[i], p->trackings[i+1], cv::Scalar(0, 0, 255), 3);
//                            }
//                            cv::rectangle(frame, p->pos, cv::Scalar(0, 0, 255), 2);
//
//                            cv::imshow("img", frame);
//                            cv::waitKey(0);

                            auto tmp = p->right;
                            p->right->left = p->left;
                            p->left->right = p->right;
                            delete p;
                            p = tmp;
                            continue;
                        }
                    }
                }
            }
            p = p->right;
        }

        for (int i = 0; i < M; i++) {
            if (!marked[i]) {
                auto* np = new People();
                np->pos = detectedContours[i];
                np->center.x = np->pos.x + np->pos.width / 2;
                np->center.y = np->pos.y + np->pos.height / 2;
                np->isIn = cv::pointPolygonTest(contor, np->center, false) > 0;
                np->roi = cv::Mat(frame, np->pos);
                np->trackings.push_back(np->center);
                np->momentOnTrack = cap.get(CV_CAP_PROP_POS_MSEC);

                //link new to the end of the list
                np->left = nil->left;
                np->right = nil;
                nil->left->right = np;
                nil->left = np;
            }
        }

        detectedContours.clear();
        prevFrame = frame;
        cnt++;
    }

    auto p = peopleList->right;
    while (p != nil) {
        std::cout << "deleted people " << p->pos.area() << std::endl;
        auto tmp = p->right;
        delete p;
        p = tmp;
    }
    delete nil;

    std::cout << "people in " << nPeopleIn << std::endl;
    std::cout << "people out " << nPeopleOut << std::endl;
    
    auto p = peopleList->right;
    while (p != nil) {
        auto tmp = p->right;
        delete p;
        p = tmp;
    }
    delete nil;
    cv::destroyAllWindows();
    return 0;
}
