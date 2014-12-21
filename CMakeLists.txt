project(broception)
cmake_minimum_required(VERSION 2.6)
aux_source_directory(./src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

#  Add in the bro scripts
file (GLOB RES_FILES ./src/*.bro)

#   Look for our dependencies
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
find_package(Threads)
find_package(X11)

add_subdirectory ("aux/soapybrain/src")

#   Define our dependence on the Enginecommon lib
set (EXTRA_LIBS ${EXTRA_LIBS} EngineCommon)
#   Define our dependence on OpenGL GLEW and GLUT
set (EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLUT_LIBRARY})

#   Use the C++11 std
add_definitions(-std=c++11)

include_directories ("${PROJECT_BINARY_DIR}")
include_directories ("${PROJECT_SOURCE_DIR}/aux/soapybrain/src")


#execute_process(COMMAND broccoli-config --libs OUTPUT_VARIABLE BROCCOLI_LIBS)
#execute_process(COMMAND broccoli-config --cflags OUTPUT_VARIABLE BROCCOLI_CFLAGS)
set (BROCCOLI_INCLUDES "/usr/local/bro/include")
set (BROCCOLI_LIBS "/usr/local/bro/lib/libbroccoli.so")
set (NCURSES_LIBS "-lncurses")
set (LINK_LIBS "${NCURSES_LIBS} ${BROCCOLI_LIBS}")
target_link_libraries (broception  ${EXTRA_LIBS})
add_definitions(-DBROCCOLI)
include_directories(${BROCCOLI_INCLUDES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${LINK_LIBS})
