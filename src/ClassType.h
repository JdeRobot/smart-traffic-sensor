#ifndef CLASSTYPE_H
#define CLASSTYPE_H

#include <opencv2/opencv.hpp>

struct ClassType {

    cv::Scalar getColor();
    std::string getClassString();
    int getClassID();
    std::vector<std::string> getAllAvailableClasses();
protected:
    float _get_color(int c, int x, int max);
    std::vector<std::string> classes;
    std::string classID;


};


#endif //CLASSTYPE_H
