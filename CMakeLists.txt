project(broception)
cmake_minimum_required(VERSION 2.6)
aux_source_directory(./src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

#  Add in the bro scripts
file (GLOB RES_FILES ./src/*.bro)

#execute_process(COMMAND broccoli-config --libs OUTPUT_VARIABLE BROCCOLI_LIBS)
#execute_process(COMMAND broccoli-config --cflags OUTPUT_VARIABLE BROCCOLI_CFLAGS)
set (BROCCOLI_INCLUDES "/usr/local/bro/include")
set (BROCCOLI_LIBS "/usr/local/bro/lib/libbroccoli.so")
set (NCURSES_LIBS "-lncurses")
set (LINK_LIBS "${NCURSES_LIBS} ${BROCCOLI_LIBS}")
add_definitions(-DBROCCOLI)
include_directories(${BROCCOLI_INCLUDES})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_FLAGS ${LINK_LIBS})
