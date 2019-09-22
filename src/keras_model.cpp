
#include <iostream>
#include "keras_model.h"
#include <ClassTypeGeneric.h>

std::string TF_PB_PATH1 = "../cfg/ssd300adam_1000_cambio_ancho_coco_120_inference.h5";
std::string TF_LABELLIST_PATH1 = "../names/label.names";
float const TF_PREDICTION_THRESSHOLD1 = 0.2;

namespace trafficmonitor{

KerasModel::KerasModel()
{
	if (!keras_init(TF_LABELLIST_PATH1, TF_PB_PATH1))
    {
    	std::cout << "Cannot initialize keras" << std::endl;
    }
}

// Loads the graph into the session and starts up the labels map
bool KerasModel::keras_init(const std::string& labels_file_name,const std::string& graph_file_name)
{
	classNamesFile = labels_file_name;

    std::string file_path = __FILE__;
    std::string dir_path = file_path.substr(0, file_path.rfind("/"));
    dir_path = dir_path + "/python_modules";
    std::string string_to_run = "import sys\nsys.path.append('" + dir_path + "')\n";
 
    Py_Initialize();
   
    PyRun_SimpleString(string_to_run.c_str());
 
    init();
    pName = PyString_FromString("keras_detect");
    pModule = PyImport_Import(pName);
    Py_DECREF(pName);
   
    if (pModule != NULL) {
		std::cout << "Loading Keras Model" << '\n';
	    pClass = PyObject_GetAttrString(pModule, "KerasDetector");

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
      	fprintf(stderr, "Cannot find function \"keras_detect\"\n");
      	return false;
    }

    std::cout << "Loaded Keras Model" << '\n';
    return true;
   
}

void KerasModel::init()
{
	import_array();
}

int KerasModel::find_blobs(colorspaces::Image& _frame, std::vector<Blob*>& new_blobs)
{
	int error = 0;
    if(PyErr_CheckSignals() == -1) {
		throw std::runtime_error("Keyboard Interrupt");
   	}
   	Mat new_frame(_frame);
   	double confidence_threshold = 0.4;
   	int inputHeight = new_frame.size().height;
   	int inputWidth = new_frame.size().width;
   	cv::Mat outputImg(inputHeight, inputWidth, CV_8UC3);
   	new_frame.convertTo(outputImg, CV_8UC3);
	
   	cv::Mat rgbImage;
   	cv::cvtColor(new_frame,rgbImage,cv::COLOR_BGR2RGB);
   	this->detections.clear();
   	int result = getKerasInferences(rgbImage, confidence_threshold);

   	if (result == 0) {
		std::cout << "Error Occured during getting inferences" << '\n';
   	}


   	int blob_counter = 0;
   	ClassTypeGeneric typeConverter(classNamesFile);
   	std::string clase_vehicle;

   	for (auto it = detections.begin(), end=detections.end(); it !=end  ; ++it)
   	{
		
		if(it->classId < 0)
 		{
			it->classId = 2;
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
			error = 0;
		}
	
		blob_counter++;
	
	}
   
	//cout << "Number of blobs: " << blob_counter << " " << inputHeight << ":" << inputWidth << std::endl;

   	return blob_counter;

}

int KerasModel::getKerasInferences(const cv::Mat& image, double confidence_threshold) {

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
		output_result(pValue, sizes);
		Py_DECREF(pValue);

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

void KerasModel::output_result(PyObject* result, int sizes[])
{

	int* dims;

	if( PyArray_Check(result)) {

		PyArrayObject* result_cont = PyArray_GETCONTIGUOUS( (PyArrayObject*) result );
		float* result_data = (float*) result_cont->data; // not copying data
        dims = (int*) PyArray_SHAPE(result_cont);

		int i;
		int k = 0;

		for( i=0; i<dims[0]; i++ ) {
	
			detections.push_back(detection());
			detections[i].classId = (int) result_data[k++];  // In Keras id's start from 1 whereas detectionsuite starts from 0s
			detections[i].probability = result_data[k++];
			detections[i].boundingBox.x = result_data[k++] * sizes[1];
			detections[i].boundingBox.y = result_data[k++] * sizes[0];
			detections[i].boundingBox.width = result_data[k++] * sizes[1] - detections[i].boundingBox.x;
			detections[i].boundingBox.height = result_data[k++] * sizes[0] - detections[i].boundingBox.y;

		}

		// clean
		Py_XDECREF(result_cont);
	}
}


} // namespace


