
if(UNIX)
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -Xcompiler -fPIC; -O3; -ccbin ${CMAKE_CXX_COMPILER})
else()
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -ccbin "\"${CMAKE_CXX_COMPILER}\"")
endif()

IF (USE_GPU)
	file(GLOB_RECURSE gpu_SRCS "*.cu")
ENDIF()

file(GLOB_RECURSE darknet_SOURCES "*.c")
file(GLOB_RECURSE darknet_HEADERS "*.h")

set (darknet_SOURCES
	${darknet_SOURCES}
  PARENT_SCOPE
#	DarknetAPI/DarknetAPI
#	DarknetAPI/DarknetAPIConversions
#	DarknetAPI/DarknetDetection
)


include_directories( ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR} ${OPENCV_INCLUDE_DIR} ${OpenCV_INCLUDE_DIRS} ${RAPIDJSON_INCLUDE_DIR})




if(USE_GPU)
  cuda_add_library (darknetLib ${darknet_SOURCES} ${gpu_SRCS} SHARED)
   target_link_libraries(darknetLib
        ${CUDA_LIBRARIES}
        ${CUDA_CUBLAS_LIBRARIES}
        ${CUDA_curand_LIBRARY}
    )
else()
  add_library (darknetLib SHARED ${darknet_HEADERS} ${darknet_SOURCES})
endif(USE_GPU)

install(TARGETS darknetLib LIBRARY DESTINATION ${LIB_DESTINATION} ARCHIVE DESTINATION ${LIB_DESTINATION} RUNTIME DESTINATION ${LIB_DESTINATION})

file(GLOB filesA *.h)
file(GLOB filesB DarknetAPI/*.h)

install(FILES  ${filesA} DESTINATION  ${HEADER_DESTINATION})
install(FILES  ${filesB} DESTINATION  ${HEADER_DESTINATION}/DarknetAPI)
