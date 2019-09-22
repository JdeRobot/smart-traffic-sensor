CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(cairomm cairomm-1.0)
INCLUDE_DIRECTORIES(${cairomm_INCLUDE_DIRS})
LINK_DIRECTORIES(${cairomm_LIBRARY_DIRS})

PKG_CHECK_MODULES(gtkmm REQUIRED gtkmm-3.0)
INCLUDE_DIRECTORIES(${gtkmm_INCLUDE_DIRS})
LINK_DIRECTORIES(${gtkmm_LIBRARY_DIRS})

PKG_CHECK_MODULES(gsl REQUIRED gsl)
INCLUDE_DIRECTORIES(${gsl_INCLUDE_DIRS})
LINK_DIRECTORIES(${gsl_LIBRARY_DIRS})

PKG_CHECK_MODULES(opencv REQUIRED opencv)
INCLUDE_DIRECTORIES(${opencv_INCLUDE_DIRS})
LINK_DIRECTORIES(${opencv_LIBRARY_DIRS})

LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/lib)

FIND_PACKAGE(Boost REQUIRED program_options filesystem)
OPTION(USE_GPU_DARKNET "Compile with GPU Support" OFF)						## Only Applicable for Darknet

FIND_PATH( DARKNET_INCLUDE_DIR NAMES DarknetAPI/DarknetAPI.h  PATHS ${DARKNET_PATH}/include )
SET(DARKNET_NAMES darknetAPILib darknetExamplesLib darknetLib)

 FOREACH(N ${DARKNET_NAMES})
	FIND_LIBRARY( DARKNET_LIBRARIES_${N} NAMES ${N} PATHS ${DARKNET_PATH}/lib )
	SET(DARKNET_LIBRARIES ${DARKNET_LIBRARIES} ${DARKNET_LIBRARIES_${N}})

ENDFOREACH()

if (DARKNET_INCLUDE_DIR AND DARKNET_LIBRARIES)				## Sometimes it may be found without specifing DARNKNET_PATH

	if (USE_GPU_DARKNET)
		MESSAGE("Darknet must be build with GPU support")
		add_definitions(-DGPU)
		FIND_PACKAGE(CUDA REQUIRED)
	endif(USE_GPU_DARKNET)

	message("DARKNET FOUND")
	message("DARKNET_INCLUDE_DIR: ${DARKNET_INCLUDE_DIR}")
	message("DARKNET_LIBRARIES: ${DARKNET_LIBRARIES}")
	SET(DARKNET_ACTIVE ON)
	ADD_DEFINITIONS(-DDARKNET_ACTIVE)
else()
	#message("DARKNET_INCLUDE_DIR: ${DARKNET_INCLUDE_DIR}")
	#message("DARKNET_LIBRARIES: ${DARKNET_LIBRARIES}")
	if (NOT DARKNET_PATH)
		MESSAGE("To enable Darknet define DARKNET_PATH equal to Darknet Installation Path")
		MESSAGE(WARNING "Darknet Inferencing support is Disabled")
	else()
		MESSAGE("Couldn't find darknet in the DARKNET_PATH specified")
		MESSAGE(WARNING "Darknet Inferencing support is Disabled")
	endif()
endif()


#SET(LIBS_NEEDED -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_ml -lopencv_video -ltensorflow_cc -ltensorflow_framework  ) 
SET(LIBS_NEEDED -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_ml -lopencv_video -lpython2.7) 

SET( SOURCE_FILES arrow.cpp imagecv.cpp progeo.c image_utils.cpp observer.cpp blob.cpp  cvutils.cpp  main.cpp  movement_model.cpp  stats.cpp  vehicle_model.cpp background_model.cpp  camera_model.cpp planar_geom.cpp  trafficmonitor_algorithm.cpp  viewgtk.cpp viewfile.cpp view.cpp classifier.cpp  feature_tracking.cpp proximity_tracking.cpp model.cpp  road_detection.cpp  vehicle.cpp database.cpp trafficmonitor_config.cpp rectify.cpp keras_model.cpp ClassType.cpp ClassTypeGeneric.cpp Darknet_model.cpp tensorflow_model.cpp deep_learning_tracking.cpp)
#keras_model.cpp ClassType.cpp ClassTypeGeneric.cpp

#SET( CMAKE_CXX_FLAGS "-O0 -g -std=c++11 -pg")
SET( CMAKE_CXX_FLAGS " -O3 -std=c++11")



INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/include
  ${cairomm_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIR}
  ${DARKNET_INCLUDE_DIR}
  ${CUDA_INCLUDE_DIRS}
  )

ADD_EXECUTABLE (trafficmonitor ${SOURCE_FILES})

# LIBRARIESD AVAILABLE IN PYTHON_LIBRARIES
TARGET_LINK_LIBRARIES( trafficmonitor
  ${LIBS_NEEDED}
  ${opencv_LIBRARIES}
  ${gtkmm_LIBRARIES}
  ${cairomm_LIBRARIES}
  ${gsl_LIBRARIES}
  ${Boost_LIBRARIES}
  ${DARKNET_LIBRARIES}
  ${CUDA_LIBRARIES}
  ${CUDA_CUBLAS_LIBRARIES}
  ${CUDA_curand_LIBRARY}
  )


include(Deps/numpy/CMakeLists.txt)
include_directories(${PYTHON_INCLUDE_DIRS})

