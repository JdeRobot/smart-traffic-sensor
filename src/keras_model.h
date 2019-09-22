#ifndef _KERAS_MODEL_
#define _KERAS_MODEL_

#include <iostream>
#include <vector>

#include <colorspacesmm.h>
#include "vehicle.h"
#include "singleton.h"

#include <Python.h>
#include <boost/python.hpp>
#include <vector>
#include <math.h>
#include <cstdio>
#include <numpy/arrayobject.h>

namespace trafficmonitor{

class KerasModel: public CSingleton<KerasModel>{

public:

   /**
    *
    */
   static void init();
   int find_blobs(colorspaces::Image& new_frame, std::vector<Blob*>& new_vehicles);
   int getKerasInferences(const cv::Mat& image, double confidence_threshold);
   void output_result(PyObject* result, int sizes[] );
 
  
protected:

   friend class CSingleton<KerasModel>;

private:

   /**
    *
    */
   KerasModel();
   struct detection {
        cv::Rect boundingBox;
        float probability;
        int classId;
    };
   bool keras_init(const std::string& labels_file_name, const std::string& graph_file_name);
   PyObject *pName, *pModule, *pClass, *pInstance;
   PyObject *pArgs, *pValue, *pmodel;
   std::string classNamesFile;
   std::vector<detection> detections;
 
};

}

#endif
