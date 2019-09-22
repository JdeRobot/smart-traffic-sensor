#ifndef _TENSORFLOW_MODEL_
#define _TENSORFLOW_MODEL_

#include <iostream>
#include <vector>
#include <math.h>
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

class TensorflowModel: public CSingleton<TensorflowModel>{

public:

   /**
    *
    */
   static void init();
   int find_blobs(colorspaces::Image& new_frame, std::vector<Blob*>& new_vehicles);
   int getTensorflowInferences(const cv::Mat& image, double confidence_threshold);
   void output_result(int num_detections, int width, int height, PyObject* bounding_boxes, PyObject* detection_scores, PyObject* classIds );

  
protected:

   friend class CSingleton<TensorflowModel>;

private:

   /**
    *
    */
   TensorflowModel();
   struct detection {
        cv::Rect boundingBox;
        float probability;
        int classId;
   
    };
   bool tensorflow_init(const std::string& labels_file_name, const std::string& graph_file_name);
   PyObject *pName, *pModule, *pClass, *pInstance;
   PyObject *pArgs, *pValue, *pmodel;
   std::string classNamesFile;
   std::vector<detection> detections;
   bool hasMasks;
 
};

}

#endif
