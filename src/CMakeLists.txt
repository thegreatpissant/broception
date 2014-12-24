
#   Define our dependence on the soapybrain lib
set (EXTRA_LIBS ${EXTRA_LIBS} libsoapybrain)

#   Other dependencies
set (CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)
#find_package(X11 REQUIRED)
#find_package(Threads REQUIRED)
set (EXTRA_LIBS ${EXTRA_LIBS} ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} ${GLUT_LIBRARY} ${CURSES_LIBRARIES})


#   Use the C++11 std
add_definitions(-std=c++11)

#execute_process(COMMAND broccoli-config --libs OUTPUT_VARIABLE BROCCOLI_LIBS)
#execute_process(COMMAND broccoli-config --cflags OUTPUT_VARIABLE BROCCOLI_CFLAGS)
set (BROCCOLI_INCLUDES "/usr/local/bro/include" CACHE FILEPATH "/usr/local/bro/include" )
set (BROCCOLI_LIBS "/usr/local/bro/lib/libbroccoli.so" CACHE FILEPATH "/usr/local/bro/lib/libbroccoli.so")

set (EXTRA_LIBS ${EXTRA_LIBS} ${BROCCOLI_LIBS})

set (SRC_LIST main.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})

add_definitions(-DBROCCOLI)
include_directories(${BROCCOLI_INCLUDES} ${SOAPYBRAIN_INCLUDES} ${CURSES_INCLUDE_DIR})

target_link_libraries (${PROJECT_NAME} ${EXTRA_LIBS})


