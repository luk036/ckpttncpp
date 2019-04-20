# Distributed under the MIT License (See accompanying file /LICENSE )

# CMake build : global project

cmake_minimum_required (VERSION 3.3)

project (CkPttn)

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIBS ${LIBS} "-L$ENV{CONDA_PREFIX}/lib")

set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

# set (Boost_DIR $ENV{CONDA_PREFIX}/include)
# find_package (Boost REQUIRED COMPONENTS coroutine)
# if (Boost_FOUND)
#     message(STATUS "Found boost: ${Boost_INCLUDE_DIRS}")
#     set(LIBS ${LIBS} ${Boost_LIBRARIES})
# endif (Boost_FOUND)

set (Boost_LIBRARIES "-lboost_coroutine -lboost_context")
message(STATUS "boost library: ${Boost_LIBRARIES}")

add_definitions ( -std=c++1z )
# add_subdirectory (third_party EXCLUDE_FROM_ALL)

# add_executable (Main profit_main.cpp)
# add_executable (Test exp1d.cpp)
# target_link_libraries ( Main Threads::Threads ${LIBS} -llapack -lblas -lfmt)
# target_link_libraries ( Test Threads::Threads ${LIBS} -llapack -lblas -lfmt)
set (LIBRARY_INCLUDE_PATH ${LIBRARY_INCLUDE_PATH} "$ENV{CONDA_PREFIX}/include")

add_subdirectory (lib)
add_subdirectory (app)

enable_testing ()
