#include <DarknetAPI/DarknetAPI.h>
#include <iostream>
#include "Darknet_model.h"
#include <ClassTypeGeneric.h>
#include <boost/shared_ptr.hpp>
#include <fstream>

std::string NET_WEIGHTS = "../cfg/yolov3-voc_19000.weights";
std::string NET_CONFIG = "../cfg/yolov3-voc.cfg";
std::string TF_LABELLIST_PATH_DARKNET = "../names/label.names";
float const TF_PREDICTION_THRESSHOLD_DARKNET = 0.5;

//std::string DETECTION_FILE = "../detection/";
//std::string DETECTION_IMAGE = "../detection_image/";
//int contador_file_txt = 0;
boost::shared_ptr<DarknetAPI> cnn; 

namespace trafficmonitor{

DarknetModel::DarknetModel()
{
	if (!darknet_init(TF_LABELLIST_PATH_DARKNET, NET_WEIGHTS,NET_CONFIG))
	{
    	std::cout << "Cannot initialize darknet" << std::endl;
   	}
}

// Loads the graph into the session and starts up the labels map
bool DarknetModel::darknet_init(const std::string& labels_file_name,const std::string& netWeights, const std::string &netConfig)
{
	classNamesFile = labels_file_name;
   	cnn = boost::shared_ptr<DarknetAPI>(new DarknetAPI((char*)netConfig.c_str(), (char*)netWeights.c_str()));
   	std::cout << "Loaded Darknet Model" << '\n';
   	return true;
}

int DarknetModel::find_blobs(colorspaces::Image& _frame, std::vector<Blob*>& new_blobs)
{
	int error = 0;
	Mat new_frame(_frame);
   	int blob_counter = 0;
   	double confidence_threshold = 0.4;
   	std::string clase_vehicle;
   	int inputHeight = new_frame.size().height;
   	int inputWidth = new_frame.size().width;
   	cv::Mat outputImg(inputHeight, inputWidth, CV_8UC3);
   	new_frame.convertTo(outputImg, CV_8UC3);
	
   	cv::Mat rgbImage;
   	cv::cvtColor(new_frame,rgbImage,CV_RGB2BGR);
   
    DarknetDetections detections = cnn->process(rgbImage, (float)confidence_threshold);

    ClassTypeGeneric typeConverter(classNamesFile);

    // Crea un fichero de salida
    //imwrite( DETECTION_IMAGE+std::to_string(contador_file_txt)+".jpg", rgbImage );
    // Crea un fichero de salida
    //ofstream fs(DETECTION_FILE+std::to_string(contador_file_txt)+".txt"); 

    for (auto it = detections.data.begin(), end=detections.data.end(); it !=end; ++it){
		typeConverter.setId(it->classId);
		int boxClass = it->classId;
       	float x1 = it->detectionBox.x ;
       	float y1 = it->detectionBox.y ;
       	float x2 = it->detectionBox.x + it->detectionBox.width;
       	float y2 = it->detectionBox.y + it->detectionBox.height ;
		Vehicle* curr_vehicle = static_cast<Vehicle*> (new_blobs[blob_counter]);
		curr_vehicle->init();
       	curr_vehicle->set_id(blob_counter+1);
       	curr_vehicle->set_rect(x1,y1,x2,y2);
           
		tvehicle_category category = INVALID_VEHICLE_CLASS;
		clase_vehicle = typeConverter.getClassString();

		if(clase_vehicle == "car")
			category =  CAR;
		else if(clase_vehicle == "motorcycle")
			category =  MOTORCYCLE;
		else if(clase_vehicle == "van")
			category =  VAN;
		else if(clase_vehicle == "bus")
			category =  BUS;
		else if(clase_vehicle == "truck")
			category =  TRUCK;
		else if(clase_vehicle == "small-truck")
			category =  SMALL_TRUCK;
		else if(clase_vehicle == "tank-truck")
			category =  TANK_TRUCK;

		curr_vehicle->set_category(category);
		curr_vehicle->set_probability(it->probability);
		curr_vehicle->set_klt_vehicle(0);
		if (error == 0)
		{
			/*fs <<category<<" "<< it->detectionBox.x<< " "<< it->detectionBox.y<<" "<< it->detectionBox.width<<" "<< it->detectionBox.height<<" "<<it->probability<<endl;*/
		
			error = 0;
		}
	
		blob_counter++;
    }
    /*fs.close();
    contador_file_txt = contador_file_txt + 1;*/
    
    return blob_counter;

}

} // namespace
