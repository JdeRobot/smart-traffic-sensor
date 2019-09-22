
#ifndef CLASSTYPEGENERIC_H
#define CLASSTYPEGENERIC_H


#include "ClassType.h"

struct ClassTypeGeneric: public ClassType{
    ClassTypeGeneric(const std::string& classesFile);
    ClassTypeGeneric(const std::string& classesFile, int id);
    void setId(int id);
    void fillStringClassesVector(const std::string& classesFile);
    std::string classID_generic;
    std::vector<std::string> classes;

};


#endif //CLASSTYPEGENERIC_H
