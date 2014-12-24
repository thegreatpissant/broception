project(broception)

cmake_minimum_required(VERSION 2.6)


set (SOAPYBRAIN_DIRECTORY "${PROJECT_SOURCE_DIR}/aux/soapybrain")

add_subdirectory ("aux/soapybrain")
add_subdirectory ("src")
