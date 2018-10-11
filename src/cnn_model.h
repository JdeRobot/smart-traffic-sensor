#ifndef _CNN_MODEL_
#define _CNN_MODEL_

#include <iostream>
#include <vector>

#include <colorspacesmm.h>
#include "vehicle.h"
#include "singleton.h"

// Opencv
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// Tensorflow
#include <tensorflow/core/platform/init_main.h>
#include <tensorflow/core/public/session.h>
#include <tensorflow/core/framework/tensor_shape.h>

namespace trafficmonitor{

class CNNModel: public CSingleton<CNNModel>{

public:

   /**
    *
    */
   int find_blobs(colorspaces::Image& new_frame, std::vector<Blob*>& new_vehicles);

protected:

   friend class CSingleton<CNNModel>;

private:

   /**
    *
    */
   CNNModel();

   tensorflow::Status tf_load_graph(const std::string& graph_file_name, std::unique_ptr<tensorflow::Session>* session);
   bool tf_init(const std::string& labels_file_name, std::map<int, std::string>* label_map, const std::string& graph_file_name, std::unique_ptr<tensorflow::Session>* tf_session);
   bool tf_load_labels(const std::string& file_name, std::map<int, std::string>* result, int* found_label_count);

   std::map<int, std::string> tf_label_map;
   std::unique_ptr<tensorflow::Session> tf_session;
};

}

#endif
