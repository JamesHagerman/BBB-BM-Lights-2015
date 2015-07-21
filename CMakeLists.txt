# CMAKE IS STUPIDLY BAD.
# This just runs the stupid Makefile I already have:
cmake_minimum_required(VERSION 2.8.4)
project(runMake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_custom_target(runMake COMMAND make -C ${runMake_SOURCE_DIR}
                         CLION_EXE_DIR=${PROJECT_BINARY_DIR})


#cmake_minimum_required(VERSION 3.1)
##set(CMAKE_LEGACY_CYGWIN_WIN32 0)
#set(CMAKE_VERBOSE_MAKEFILE 1)
#
#project("BBB-BM-Lights-2015")
#
## this "list(APPEND" stuff is similar to "set("
#list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
#
#
##find_package(Clutter REQUIRED)
#FIND_PATH(Clutter_INCLUDE_DIR clutter/clutter.h /usr/include/clutter-1.0 /usr/local/include/clutter-1.0)
#include_directories("${Clutter_INCLUDE_DIR}")
#
#find_package(Glib REQUIRED)
##FIND_PATH(glib_INCLUDE_DIR /usr/include/glib-2.0 /usr/local/include/glib-2.0)
#include_directories("${GLIB_INCLUDE_DIR}")
#include_directories("${GLIB_CONFIG_INCLUDE_DIR}")
#
#find_package(Cairo REQUIRED)
#include_directories("${CAIRO_INCLUDE_DIRS}")
#
#find_package(Cogl REQUIRED)
#include_directories("${COGL_INCLUDE_DIRS}")
#
#add_executable(clutter_window
#    main.cpp
#    animation.cpp
#    button.cpp
#    cat.cpp
#    color.cpp
#    events.cpp
#    TCLControl.cpp
#    )
#
#
#
##TARGET_LINK_LIBRARIES(clutter-1.0)


