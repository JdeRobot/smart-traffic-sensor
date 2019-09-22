#ifndef _DARKNET_MODEL_
#define _DARKNET_MODEL_

#include <iostream>
#include <vector>

#include <colorspacesmm.h>
#include "vehicle.h"
#include "singleton.h"

#include <vector>
#include <math.h>
#include <cstdio>
#include <numpy/arrayobject.h>


namespace trafficmonitor{

class DarknetModel: public CSingleton<DarknetModel>{

public:

   /**
    *
    */
   static void init();
   int find_blobs(colorspaces::Image& new_frame, std::vector<Blob*>& new_vehicles);
  
protected:

   friend class CSingleton<DarknetModel>;

private:

   /**
    *
    */
   DarknetModel();
   struct detection {
        cv::Rect boundingBox;
        float probability;
        int classId;
    };
   bool darknet_init(const std::string& labels_file_name,const std::string& netWeights, const std::string &netConfig);
   PyObject *pName, *pModule, *pClass, *pInstance;
   PyObject *pArgs, *pValue, *pmodel;
   std::string classNamesFile;
   std::vector<detection> detections;
   
 
};

}

#endif
