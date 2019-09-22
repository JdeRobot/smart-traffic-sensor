//
// Created by frivas on 9/02/17.
//

#include <fstream>
#include "ClassTypeGeneric.h"


ClassTypeGeneric::ClassTypeGeneric(const std::string &classesFile) {
    fillStringClassesVector(classesFile);
}

ClassTypeGeneric::ClassTypeGeneric(const std::string &classesFile, int id) {
    fillStringClassesVector(classesFile);
    classID=classes[id];
}

void ClassTypeGeneric::fillStringClassesVector(const std::string &classesFile) {
    //std::cout<<"classFile: "<<classesFile<<'\n';
    std::ifstream labelFile(classesFile);
    std::string data;
    while(getline(labelFile,data)) {
        classes.push_back(data);
	//std::cout<<"classes: "<<data<<'\n';
    }
    
}

void ClassTypeGeneric::setId(int id) {
    //std::cout<<"setid if: "<<id<<'\n';
    classID=classes[id];
    //std::cout<<"classID: "<<classID<<'\n';
}

