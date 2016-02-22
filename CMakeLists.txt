cmake_minimum_required(VERSION 3.3)
PROJECT(dump2dot VERSION 0.0.1 LANGUAGES CXX)

SET(DOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external")
SET(APP "dump2dot")

include(ExternalProject)
if (WIN32)
else ()
include(FindPkgConfig)
pkg_search_module(LIBDOT REQUIRED libxdot)
else ()
endif ()

file(GLOB MAIN_SRC "src/*.cpp")

link_directories(${LIBDOT_LIBRARY_DIRS})
add_executable(${APP} ${MAIN_SRC})

target_include_directories(${APP} PUBLIC
	${LIBDOT_INCLUDE_DIRS}
)

set_property(TARGET ${APP} PROPERTY CXX_STANDARD 11)

if (WIN32)
SET(EXTRA_LIBS)
else ()
SET(EXTRA_LIBS)
endif ()

target_link_libraries(${APP}
	${EXTRA_LIBS}
)
