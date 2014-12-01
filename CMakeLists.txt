project(broception)
cmake_minimum_required(VERSION 2.6)
aux_source_directory(./src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

#execute_process(COMMAND broccoli-config --libs OUTPUT_VARIABLE BROCCOLI_LIBS)
#execute_process(COMMAND broccoli-config --cflags OUTPUT_VARIABLE BROCCOLI_CFLAGS)
set (BROCCOLI_INCLUDES "/usr/local/bro/include")
set (BROCCOLI_LIBS "/usr/local/bro/lib/libbroccoli.so")

add_definitions(-DBROCCOLI)
include_directories(${BROCCOLI_INCLUDES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${BROCCOLI_LIBS})
