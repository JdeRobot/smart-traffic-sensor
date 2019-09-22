#include <math.h>
#include <iostream>
#include "tensorflow_model.h"
#include <ClassTypeGeneric.h>


std::string TF_PB_PATH = "../cfg/frozen_inference_graph.pb"; 
std::string TF_LABELLIST_PATH = "../names/label_tensorflow.names";
float const TF_PREDICTION_THRESSHOLD1 = 0.2;

namespace trafficmonitor{

TensorflowModel::TensorflowModel()
{
	if (!tensorflow_init(TF_LABELLIST_PATH, TF_PB_PATH))
   	{
    	std::cout << "Cannot initialize tensorflow" << std::endl;
   	}
}


// Loads the graph into the session and starts up the labels map
bool TensorflowModel::tensorflow_init(const std::string& labels_file_name,const std::string& graph_file_name)
{
	classNamesFile = labels_file_name;
	/*return true or false para indicar que se ha iniciado correctamente */
   	std::string file_path = __FILE__;
   	std::string dir_path = file_path.substr(0, file_path.rfind("/"));
   	dir_path = dir_path + "/python_modules";
   	std::string string_to_run = "import sys\nsys.path.append('" + dir_path + "')\n";
  
   	Py_Initialize();
   
   	PyRun_SimpleString(string_to_run.c_str());
   	/* habria que comprobar si entra dentro de init */	
   	init();
   	pName = PyString_FromString("tensorflow_detect");
   	pModule = PyImport_Import(pName);
   	Py_DECREF(pName);
   
   	if (pModule != NULL) {
    	std::cout << "Loading Tensorflow Model" << '\n';
    	pClass = PyObject_GetAttrString(pModule, "TensorFlowDetector");

      	pArgs = PyTuple_New(1);
      	pmodel = PyString_FromString(graph_file_name.c_str());
      	/* pValue reference stolen here: */
      	PyTuple_SetItem(pArgs, 0, pmodel);
      	/* pFunc is a new reference */
      	pInstance = PyInstance_New(pClass, pArgs, NULL);

      	if (pInstance == NULL)
      	{
        	Py_DECREF(pArgs);
	 		PyErr_Print();
         	return false;
       	}

   	} else {
    	if (PyErr_Occurred())
        	PyErr_Print();
      	fprintf(stderr, "Cannot find function \"tensorflow_detect\"\n");
      	return false;
   	}

   	std::cout << "Loaded Tensorflow Model" << '\n';
   	return true;
   
}

void TensorflowModel::init()
{
	import_array();
}

int TensorflowModel::find_blobs(colorspaces::Image& _frame, std::vector<Blob*>& new_blobs)
{
	int error = 0;
   	if(PyErr_CheckSignals() == -1) {
		throw std::runtime_error("Keyboard Interrupt");
	}
   	Mat new_frame(_frame);
   	double confidence_threshold = 0.5;
   	int inputHeight = new_frame.size().height;
   	int inputWidth = new_frame.size().width;
   	cv::Mat outputImg(inputHeight, inputWidth, CV_8UC3);
   	new_frame.convertTo(outputImg, CV_8UC3);
	
   	//std::cout<<"find blobs \n";

   	cv::Mat rgbImage;
   	cv::cvtColor(new_frame,rgbImage,cv::COLOR_BGR2RGB);
   	this->detections.clear();
   	int result = getTensorflowInferences(rgbImage, confidence_threshold);
   	//std::cout << "result" << result << '\n';
   	if (result == 0) {
		std::cout << "Error Occured during getting inferences" << '\n';
   	}

	int blob_counter = 0;
	ClassTypeGeneric typeConverter(classNamesFile);
   	std::string clase_vehicle;

   	for (auto it = detections.begin(), end=detections.end(); it !=end  ; ++it){
		
		if(it->classId < 0)
	 	{
			it->classId = 0;
			error = 1;
		}
		typeConverter.setId(it->classId);

		int boxClass = it->classId;
       	float x1 = it->boundingBox.x ;
       	float y1 = it->boundingBox.y ;
       	float x2 = it->boundingBox.x + it->boundingBox.width;
       	float y2 = it->boundingBox.y + it->boundingBox.height ;
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
        curr_vehicle->set_klt_vehicle(0);
		
		if (error == 0)
		{
			//std::cout<< it->boundingBox.x << " " << it->boundingBox.y << " " << it->boundingBox.height << " " << it->boundingBox.width << "blob_counter"<<blob_counter<<std::endl;
			error = 0;
		}

		blob_counter++;
	
	}

   //cout << "Number of blobs: " << blob_counter << " " << inputHeight << ":" << inputWidth << std::endl;

   return blob_counter;

}

int TensorflowModel::getTensorflowInferences(const cv::Mat& image, double confidence_threshold) {

	int i, num_detections, dims, sizes[3];

	if (image.channels() == 3) {
		dims = 3;
		sizes[0] = image.rows;
		sizes[1] = image.cols;
		sizes[2] = image.channels();

	} else if (image.channels() == 1) {
		dims = 2;
		sizes[0] = image.rows;
		sizes[1] = image.cols;
	} else {
		std::cout << "Invalid Image Passed" << '\n';
		return 0;
	}


	npy_intp _sizes[4];

	for( i = 0; i < dims; i++ )
	{
		_sizes[i] = sizes[i];
	}


	PyObject* mynparr = PyArray_SimpleNewFromData(dims, _sizes, NPY_UBYTE, image.data);
	PyObject* conf = PyFloat_FromDouble(confidence_threshold);
	
	if (!mynparr || !conf) {
		Py_DECREF(pArgs);
		Py_DECREF(pModule);
		fprintf(stderr, "Cannot convert argument\n");
		return 0;
	}

	pValue = PyObject_CallMethodObjArgs(pInstance, PyString_FromString("detect"), mynparr, conf, NULL);

	Py_DECREF(pArgs);
    
	if (pValue != NULL) {
		num_detections = _PyInt_AsInt( PyDict_GetItem(pValue, PyString_FromString("num_detections") ) );
		PyObject* pBounding_boxes = PyDict_GetItem(pValue, PyString_FromString("detection_boxes") );
		PyObject* pDetection_scores = PyDict_GetItem(pValue, PyString_FromString("detection_scores") );
		PyObject* classIds = PyDict_GetItem(pValue, PyString_FromString("detection_classes") );
        	output_result(num_detections, image.cols, image.rows, pBounding_boxes, pDetection_scores, classIds);
	}
	else {
		Py_DECREF(pClass);
		Py_DECREF(pModule);
		PyErr_Print();
		fprintf(stderr,"Call failed\n");

		return 0;
	}
	return 1;
}

void TensorflowModel::output_result(int num_detections, int width, int height, PyObject* bounding_boxes, PyObject* detection_scores, PyObject* classIds)
{

	if( PyArray_Check(bounding_boxes) && PyArray_Check(detection_scores) && PyArray_Check(classIds) ) {
      
		PyArrayObject* bounding_boxes_cont = PyArray_GETCONTIGUOUS( (PyArrayObject*) bounding_boxes );
		PyArrayObject* detection_scores_cont = PyArray_GETCONTIGUOUS( (PyArrayObject*) detection_scores );
		PyArrayObject* classIds_cont = PyArray_GETCONTIGUOUS( (PyArrayObject*) classIds );

		float* bounding_box_data = (float*) bounding_boxes_cont->data; // not copying data
		float* detection_scores_data = (float*) detection_scores_cont->data;
		unsigned char* classIds_data = (unsigned char*) classIds_cont->data;
       
		int i;
		int boxes = 0, scores = 0, classes = 0, masks = 0;

		for( i=0; i<num_detections; i++ ) {

			detections.push_back(detection());
			detections[i].classId = classIds_data[classes++] - 1;  // In TensorFlow id's start from 1 whereas detectionsuite starts from 0s
			detections[i].probability = detection_scores_data[scores++];
			detections[i].boundingBox.y = bounding_box_data[boxes++] * height;
			detections[i].boundingBox.x = bounding_box_data[boxes++] * width;
			detections[i].boundingBox.height = bounding_box_data[boxes++] * height - detections[i].boundingBox.y;
			detections[i].boundingBox.width = bounding_box_data[boxes++] * width - detections[i].boundingBox.x;
			//std::cout<<"classisd "<<detections[i].classId<<" x "<<detections[i].boundingBox.x<<" y "<<detections[i].boundingBox.y<<"probability"<<detections[i].probability<<'\n';

		}

		// clean
		Py_XDECREF(bounding_boxes);
		Py_XDECREF(detection_scores);
		Py_XDECREF(classIds);
	}
}

} // namespace


