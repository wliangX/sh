#****************************************************************************
# set minimum version
#****************************************************************************
cmake_minimum_required(VERSION 3.0)

#****************************************************************************
# get current folder name
#****************************************************************************
string(REGEX REPLACE ".*/(.*)" "\\1" CURRENT_FOLDER_NAME ${CMAKE_CURRENT_SOURCE_DIR})

#****************************************************************************
# set project name
#****************************************************************************
project(${CURRENT_FOLDER_NAME})

#****************************************************************************
# set output name
#****************************************************************************
set(TARGET_NAME ${CURRENT_FOLDER_NAME})

#****************************************************************************
# set current path
#****************************************************************************
string(LENGTH ${ICC_ROOT} ICC_ROOT_LEN)
string(REGEX REPLACE "(.*)/.*" "\\1" PARENT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
string(SUBSTRING ${PARENT_PATH} ${ICC_ROOT_LEN} -1 TARGET_DIR)
string(SUBSTRING ${TARGET_DIR} 1 -1 TARGET_DIR)
string(CONCAT TARGET_DIR ${TARGET_DIR} "/PersonInfoData")

#****************************************************************************
# auto search code file
#****************************************************************************
file(GLOB_RECURSE SRC_LIST . "*.h" "*.cpp" "*.hpp" "*.c")

#****************************************************************************
# Include paths
#****************************************************************************
include_directories(
	${ICC_ROOT}/Interface/
	${ICC_ROOT}/ThirdParty/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PLATFORM}/boost/
	)
	
#****************************************************************************
# Include paths
#****************************************************************************
link_directories(
	${ICC_ROOT}/ThirdParty/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_BUILD_TYPE}/${PLATFORM}/boost/stage/lib/
	)	

#****************************************************************************
# set compiler options
#****************************************************************************
if (${UNIX_OS})
	add_compile_options(
		-fPIC
		)
		
	add_definitions(
		-std=c++11
		-W
		-O2
		-Wall
		-fno-pie
		)
		
	if(PLATFORM STREQUAL "arm")
		add_definitions(-D _GLIBCXX_USE_CXX11_ABI=0)
    endif()	
	
	if (CMAKE_BUILD_TYPE STREQUAL "Release")
		add_definitions(-O3)
	else()
		add_definitions(-g)
	endif()
elseif (${WIN_OS})
	add_definitions(
		-D_WIN32_WINNT
		-D_WIN32_WINDOWS
		-D_WINSOCK_DEPRECATED_NO_WARNINGS
		-D_WIN32_WINNT=0x0501)
		
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /MDd /Zi /Ob0 /Od /RTC1") 	
	else()
		set(CMAKE_C_FLAGS_RELEASE_INIT "/MD /O2 /Ob2 /D NDEBUG") 	
	endif()
endif()

#****************************************************************************
# if this is an shared library, add ICC_DYN_LINK definition.
#****************************************************************************
string(TOUPPER ${ICC_LIBRARY_TYPE} ICC_LIBRARY_TYPE)
if (${ICC_LIBRARY_TYPE} STREQUAL "shared")
	add_definitions(-DICC_DYN_LINK)
endif ()

#****************************************************************************
# set target properties
# do not generate manifests
#****************************************************************************
if (WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO" )
endif ()

#****************************************************************************
# set output file suffix name
#****************************************************************************
if (WIN32)
	set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ".plg")
endif ()

#****************************************************************************
# set project grouping name
#****************************************************************************
if (WIN32)
	set_property(GLOBAL PROPERTY USE_FOLDERS ON)  
	set_property(TARGET ${TARGET_NAME} PROPERTY FOLDER ${TARGET_DIR})
endif ()

#****************************************************************************
# generate library file
#****************************************************************************
add_library(${TARGET_NAME} ${ICC_LIBRARY_TYPE} ${SRC_LIST})

#****************************************************************************
# generate link library file & path
#****************************************************************************
if (WIN32)
	if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
	target_link_libraries(${TARGET_NAME}	

		)	
	else()
	target_link_libraries(${TARGET_NAME}	

		)
	endif()
else ()
	target_link_libraries(${TARGET_NAME} 
		boost_system
		boost_filesystem	
		boost_thread
		boost_date_time
		boost_chrono
		boost_regex
		boost_atomic
		)
endif ()

#****************************************************************************
# output destination
#****************************************************************************
string(TOUPPER ${CMAKE_BUILD_TYPE} OUTPUT_DIR)
set_target_properties( ${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${OUTPUT_DIR} ${ICC_ROOT}/Build/${CMAKE_BUILD_TYPE}/${TARGET_DIR})
set_target_properties( ${TARGET_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_${OUTPUT_DIR} ${ICC_ROOT}/Build/${CMAKE_BUILD_TYPE}/${TARGET_DIR})
set_target_properties( ${TARGET_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_${OUTPUT_DIR} ${ICC_ROOT}/Build/${CMAKE_BUILD_TYPE}/${TARGET_DIR})
