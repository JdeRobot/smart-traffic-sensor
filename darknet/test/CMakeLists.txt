

message("DIR: ${CUDA_INCLUDE_DIRS}")


include_directories(${CMAKE_CURRENT_LIST_DIR}/../ ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../examples ${CMAKE_CURRENT_LIST_DIR}/../src ${CMAKE_CURRENT_LIST_DIR} ${OPENCV_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIR} ${Boost_INCLUDE_DIR})


if(USE_GPU)
  cuda_add_executable(darknetTest main.cpp)
else()
  add_executable(darknetTest main.cpp)
endif()

target_link_libraries(darknetTest
        darknetExamplesLib
        darknetAPILib
        darknetLib
        ${OpenCV_LIBRARIES}
        pthread
        m
        ${CUDA_LIBRARIES}
        ${CUDA_CUBLAS_LIBRARIES}
        ${CUDA_curand_LIBRARY}
    )
