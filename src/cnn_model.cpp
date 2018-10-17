
#include <iostream>
#include "cnn_model.h"

std::string TF_PB_PATH = "../cfg/frozen_inference_graph.pb";
std::string TF_LABELLIST_PATH = "./labels.txt";
float const TF_PREDICTION_THRESSHOLD = 0.5;

namespace trafficmonitor{

CNNModel::CNNModel()
{
   if (!tf_init(TF_LABELLIST_PATH, &tf_label_map, TF_PB_PATH, &tf_session))
   {
      std::cout << "Cannot initialize tensorflow" << std::endl;
   }
}

// Loads the graph into the session and starts up the labels map
bool CNNModel::tf_init(const std::string& labels_file_name, std::map<int, std::string>* label_map, const std::string& graph_file_name, std::unique_ptr<tensorflow::Session>* tf_session)
{
   int argc = 0;
   tensorflow::port::InitMain(NULL, &argc, NULL);

   int label_count;
   if (!tf_load_labels(labels_file_name, label_map, &label_count))
   {
      std::cerr << "tf_load_labels ERROR" << std::endl;
   }
   else
   {
      std::cout << "Loaded " << label_count << " dnn class labels" << std::endl;
   }

   tensorflow::Status status = tf_load_graph(graph_file_name, tf_session);
   if (!status.ok())
   {
      std::cerr << "tf_load_graph ERROR: " << status.error_message() << std::endl;
      return false;
   }

   return true;
}

// Takes a file name, and loads a list of labels from it, one per line into the map object. Expects `CLASSID: CLASSNAME` fmt
bool CNNModel::tf_load_labels(const std::string& file_name, std::map<int, std::string>* result, int* found_label_count)
{
   std::ifstream file(file_name);
   if (!file) {
      return false;
   }
   result->clear();
   *found_label_count = 0;
   std::string line;
   while (std::getline(file, line)) {
      std::string::size_type sz;   // alias of size_t
      int i_decimal = std::stoi(line, &sz);
      (*result)[i_decimal] = line.substr(sz+2); // +2 to account for ':' and following space
      (*found_label_count)++;
   }
   return true;
}

// Reads a model graph definition from disk, and creates a session object you can use to run it.
tensorflow::Status CNNModel::tf_load_graph(const std::string& graph_file_name, std::unique_ptr<tensorflow::Session>* session)
{
   tensorflow::GraphDef graph_def;
   tensorflow::Status load_graph_status = tensorflow::ReadBinaryProto(tensorflow::Env::Default(), graph_file_name, &graph_def);
   if (!load_graph_status.ok())
   {
      return tensorflow::errors::NotFound("Failed to load compute graph at '", graph_file_name, "'");
   }
   session->reset(tensorflow::NewSession(tensorflow::SessionOptions()));
   return (*session)->Create(graph_def);
}


int CNNModel::find_blobs(colorspaces::Image& _frame, std::vector<Blob*>& new_blobs)
{
   Mat new_frame(_frame);

   // avoid memory copies using strategy from https://github.com/tensorflow/tensorflow/issues/8033#issuecomment-332029092
   int inputHeight = new_frame.size().height;
   int inputWidth = new_frame.size().width;
   tensorflow::Tensor imgTensorWithSharedData(tensorflow::DT_UINT8, {1, inputHeight, inputWidth, new_frame.channels()});
   uint8_t *p = imgTensorWithSharedData.flat<uint8_t>().data();
   cv::Mat outputImg(inputHeight, inputWidth, CV_8UC3, p);
   new_frame.convertTo(outputImg, CV_8UC3);

   // Run tensorflow
   cv::TickMeter tm;
   tm.start();
   std::vector<tensorflow::Tensor> outputs;
   tensorflow::Status run_status = tf_session->Run({{"image_tensor:0", imgTensorWithSharedData}},
                                                      {"detection_boxes:0", "detection_scores:0", "detection_classes:0", "num_detections:0"},
                                                      {},
                                                      &outputs);
   if (!run_status.ok())
   {
      std::cerr << "tf_session->Run Error: " << run_status << std::endl;
   }
   tm.stop();
   std::cout << "Inference time, ms: " << tm.getTimeMilli()  << std::endl;

   tensorflow::TTypes<float>::Flat scores = outputs[1].flat<float>();
   tensorflow::TTypes<float>::Flat classes = outputs[2].flat<float>();
   tensorflow::TTypes<float>::Flat num_detections = outputs[3].flat<float>();
   auto boxes = outputs[0].flat_outer_dims<float,3>();

   int detectionsCount = (int)(num_detections(0));
   int blob_counter = 0;

   std::cout << "Total detections before threshold: " << detectionsCount << std::endl;
   for(int i = 0; i < detectionsCount && i < new_blobs.size(); ++i)
   {
      if (scores(i) > TF_PREDICTION_THRESSHOLD)
      {
         int boxClass = (int)classes(i);


         float x1 = float(inputWidth) * boxes(0,i,1);
         float y1 = float(inputHeight) * boxes(0,i,0);

         float x2 = float(inputWidth) * boxes(0,i,3);
         float y2 = float(inputHeight) * boxes(0,i,2);

         std::ostringstream label;

         Vehicle* curr_vehicle = static_cast<Vehicle*> (new_blobs[blob_counter]);
         curr_vehicle->init();
         curr_vehicle->set_id(blob_counter+1);
         curr_vehicle->set_rect(x1,y1,x2,y2);

         tvehicle_category category = INVALID_VEHICLE_CLASS;
         switch (boxClass)
         {
         case 1: category =  CAR; break;
         case 2: category =  MOTORCYCLE; break;
         case 3: category =  VAN; break;
         case 4: category =  BUS; break;
         case 5: category =  TRUCK;  break;
         case 6: category =  TRUCK; break;
         case 7: category =  TRUCK; break;
         }

         curr_vehicle->set_category(category);

         label << tf_label_map[boxClass] << ", confidence: " << (scores(i)  * 100) << "%";
         std::cout << "Detection " << (i+1) << ": class: " << boxClass << " cat:" << category << " " << label.str() << ", box: (" << x1 << "," << y1 << "), (" << x2 << "," << y2 << ")" << std::endl;

         // curr_vehicle->print();
         //std::cout << "Rect " << x1 << " " << y1 << " " << x2-x1 << " " << y2-y1 << std::endl;

         blob_counter++;
      }
   }

   cout << "Number of blobs: " << blob_counter << " " << inputHeight << ":" << inputWidth << std::endl;

   return blob_counter;
}

} // namespace
