#ifndef _TRAFFICMONITOR_ALGORITHM_
#define _TRAFFICMONITOR_ALGORITHM_

#include <string>
#include <vector>
#include <iostream>

#include <colorspacesmm.h>
#include "blob.h"
#include "planar_geom.h"
#include "background_model.h"
#include "trafficmonitor_config.h"
#include "road_detection.h"
#include "classifier.h"
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "tensorflow_model.h"

using std::string;
using std::vector;

namespace trafficmonitor{

/**
 *
 */
class TrafficMonitorAlgorithmState{
public:
   TrafficMonitorAlgorithmState(){};
   vector<Blob*> vehicles;
   vector<Blob*> tmp_blobs;
   colorspaces::Image current_frame;
   colorspaces::Image background_image;
   colorspaces::Image prev_frame;
   Background* background;
   Road* road;
   timeval timestamp;
   std::vector<Tpoint2D> roadPoints;
};

/**
 *
 */
class TrafficMonitorAlgorithm{

public:
   /**
    *
    */
   TrafficMonitorAlgorithm(TrafficMonitorAlgorithmConfig& algcfg);

   /**
    *
    */
   ~TrafficMonitorAlgorithm(){};

   /**
    *
    */
   void init(const TrafficMonitorAlgorithmConfig& config);
   void init();

   /**
    *
    */
   const TrafficMonitorAlgorithmState& get_state() const throw(){ return _state;};

   /**
    *
    */
   void set_cfg(const TrafficMonitorAlgorithmConfig& new_cfg, bool update=true) throw();

   /**
    *
    */
   void save_cfg() throw() { _cfg.save();};

   /**
    *
    */
   void update_cfg();

   /**
    *
    */
   void processMouseMovement(int xCoor, int yCoor);

   /**
    *
    */
   TrafficMonitorAlgorithmConfig& get_cfg() throw() {return _cfg;};

   /**
    *
    */
   bool iteration(const colorspaces::Image& currentImg, const timeval timeStamp);
   void processFirstFrame(const colorspaces::Image& firstFrame, const timeval timeStamp);
   int getMeanDurationTime();

private:

   TrafficMonitorAlgorithmState _state;
   TrafficMonitorAlgorithmConfig _cfg;
   Classifier m_classifier;
   std::vector<long> durationVector;
   boost::posix_time::ptime Time_traffic;
   

   /**
    *
    */
   void classify_vehicles(int klt_vehicle);
   void update_vehicles_trajectories_history();
   bool feature_tracking(bool klt_only);
   bool proximity_tracking();
   bool cnn_tracking();
   bool keras_tracking();
   bool darknet_tracking();


};

typedef std::tr1::shared_ptr<TrafficMonitorAlgorithm> TrafficMonitorAlgorithmPtr;
}//namspace

#endif
