
if(UNIX)
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -Xcompiler -fPIC; -O3; -ccbin ${CMAKE_CXX_COMPILER})
else()
    set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -ccbin "\"${CMAKE_CXX_COMPILER}\"")
endif()


file(GLOB_RECURSE darknet_EXAMPLES_SOURCES "*.c")



set (darknet_EXAMPLES_SOURCES
	${darknet_EXAMPLES_SOURCES}
)


include_directories( ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR} ${OPENCV_INCLUDE_DIR} ${RAPIDJSON_INCLUDE_DIR})

if(USE_GPU)
  cuda_add_library (darknetExamplesLib ${darknet_EXAMPLES_SOURCES} SHARED)
else()
  add_library (darknetExamplesLib SHARED ${darknet_EXAMPLES_SOURCES} )
endif(USE_GPU)

target_link_libraries(darknetExamplesLib darknetLib)

install(TARGETS darknetExamplesLib LIBRARY DESTINATION ${LIB_DESTINATION} ARCHIVE DESTINATION ${LIB_DESTINATION} RUNTIME DESTINATION ${LIB_DESTINATION})

file(GLOB filesA *.h)
file(GLOB filesB DarknetAPI/*.h)

install(FILES  ${filesA} DESTINATION  ${HEADER_DESTINATION})
install(FILES  ${filesB} DESTINATION  ${HEADER_DESTINATION}/DarknetAPI)



if(USE_GPU)
  cuda_add_executable(darknet darknet.c)
else()
  add_executable(darknet darknet.c)
endif(USE_GPU)

target_link_libraries(darknet
        darknetAPILib
        darknetExamplesLib
        darknetLib
        ${OpenCV_LIBRARIES}
        pthread
        m
        ${CUDA_LIBRARIES}
        ${CUDA_CUBLAS_LIBRARIES}
        ${CUDA_curand_LIBRARY}
        )
