cmake_minimum_required(VERSION 2.8)

PROJECT(DARKNET C CXX)

####################################################################
####################################################################
######           DEFINE DARKNET CMAKE OPTIONS                 ######
####################################################################
####################################################################
OPTION(BUILD_python "Build Python wrapper" ON)
OPTION(USE_GPU "Compile with GPU support" ON)

SET(DARKNET_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/src ${CMAKE_CURRENT_LIST_DIR}/examples ${CMAKE_CURRENT_LIST_DIR}/)

IF (USE_GPU)
	add_definitions(-DGPU)
ENDIF()

####################################################################
####################################################################
######           DEFINE LOCAL CMAKE REPO AND TOOLS            ######
####################################################################
####################################################################


IF (NOT DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
ELSEIF (CMAKE_BUILD_TYPE STREQUAL "" )
    SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
ENDIF()


####################################################################
####################################################################



####################################################################
######           DEPENDENCIES                                 ######
####################################################################
# ---[ Python


#Cuda
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Boost REQUIRED)


IF(USE_GPU)
	FIND_PACKAGE(CUDA REQUIRED)
ENDIF(USE_GPU)


####################################################################
####################################################################
######           SET COMPILLER OPTIONS                        ######
####################################################################
####################################################################
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-fPIC)
####################################################################
####################################################################

set(LIB_DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" CACHE STRING "Library directory name" FORCE)
set(HEADER_DESTINATION "${CMAKE_INSTALL_PREFIX}/include" CACHE STRING "include directory name" FORCE)


add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(DarknetAPI)
add_subdirectory(test)

add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_SOURCE_DIR}/uninstall.cmake"
)
