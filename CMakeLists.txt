cmake_minimum_required(VERSION 3.3)
PROJECT(dump2dot VERSION 0.0.1 LANGUAGES CXX)

SET(DOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
SET(SDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDL2")
SET(SDL_WIDGETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SDL2-widgets")

SET(APP "dump2dot")
SET(GAPP "dump2dot-gui")

include(ExternalProject)
if (WIN32)
else ()
include(FindPkgConfig)
pkg_search_module(LIBDOT libxdot)
pkg_search_module(LIBGTK REQUIRED gtk+-3.0 libgtk-3 libgtk3 libgtk-2)
message("libgtk: ${LIBGTK_INCLUDE_DIRS}")
else ()
endif ()

file(GLOB ALL_SRC "src/*.cpp")

set(MAIN_SRC ${ALL_SRC})
list(REMOVE_ITEM MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/gui.cpp")
link_directories(${LIBDOT_LIBRARY_DIRS})
add_executable(${APP} ${MAIN_SRC})

#target_include_directories(${APP} PUBLIC
	#	${LIBDOT_INCLUDE_DIRS}
	#)

set_property(TARGET ${APP} PROPERTY CXX_STANDARD 11)

if (WIN32)
SET(EXTRA_LIBS)
else ()
SET(EXTRA_LIBS)
endif ()

target_link_libraries(${APP}
	${EXTRA_LIBS}
)

#gui
set(GUI_SRC ${ALL_SRC})
list(REMOVE_ITEM GUI_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
add_executable(${GAPP} ${GUI_SRC})
set_property(TARGET ${GAPP} PROPERTY CXX_STANDARD 11)
target_include_directories(${GAPP} PUBLIC
	${LIBGTK_INCLUDE_DIRS}
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	SET(XBITS x64)
else ()
	SET(XBITS x86)
endif ()

list(APPEND EXTRA_LIBS
	${LIBGTK_LIBRARIES})
	

target_link_libraries(${GAPP}
	${EXTRA_LIBS}
)

get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
	#    message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
