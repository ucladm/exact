###############  CMakeLists for paris build   #####################
#  Written by Quentin Riffard (riffard@lpsc.in2p3.fr)                  #
#  To create an Xcode project:                                         #
#         cmake -G Xcode .                                             #

###############             cmake version          #####################
cmake_minimum_required(VERSION 2.6)

###############              Project name          #####################
project(SoftAnalysis)



###############        Project includes        #####################
include_directories (${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


file(GLOB  APP_SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" "*.cc" )

file(GLOB sources ${CMAKE_SOURCE_DIR}/src/*.cc)

file(GLOB header ${CMAKE_SOURCE_DIR}/include/*.hh)



###############            MacOS path           #####################
if(NOT DEFINED CMAKE_MACOSX_RPATH)
  set(CMAKE_MACOSX_RPATH 0)
endif()

###############            Minimal flags           #####################
IF(${UNIX})
  #set (CMAKE_CXX_FLAGS "-std=c++11 -g3 -Wall -O0")
ENDIF(${UNIX})
IF(${APPLE})
  #set (CMAKE_CXX_FLAGS "-std=c++0x  -g3 -Wall -O0")
ENDIF(${APPLE})

#######   Check the compiler and set the compile and link flags  #######
set(CMAKE_BUILD_TYPE Debug)

###############          Output directory          #####################
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}")

###############  Include macro PATH for packge detection  #################
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

###############        Root include/lib dir        #####################
find_package(ROOT REQUIRED COMPONENTS RIO Net)
include_directories(${ROOT_INCLUDE_DIR})
link_directories(${ROOT_LIBRARY_DIR})

###############        fftw include/lib dir        #####################
find_package(FFTW REQUIRED)
include_directories( ${FFTW_INCLUDES} )
link_directories(${FFTW_LIBRARIES_DIRS})

###############        libconfig include/lib dir        #####################
find_package(Libconfig REQUIRED)
include_directories(${LIBCONFIG_INCLUDE_DIR})


###############             Root flags            #####################
if (CMAKE_SYSTEM_NAME MATCHES Darwin)
   EXEC_PROGRAM("sw_vers -productVersion | cut -d . -f 1-2" OUTPUT_VARIABLE MAC_OS_VERSION)
   MESSAGE("--- Found a Mac OS X System ${MAC_OS_VERSION}")
   if (CMAKE_COMPILER_IS_GNUCXX)
      MESSAGE("--- Found GNU compiler collection")

      STRING(COMPARE EQUAL "10.5" "${MAC_OS_VERSION}" MAC_OS_10_5)
      IF(MAC_OS_10_5)
        SET(CMAKE_CXX_FLAGS "-m64")
        SET(CMAKE_Fortran_FLAGS "-m64")
      ENDIF(MAC_OS_10_5)

      SET(CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")
      SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS} -flat_namespace -single_module -undefined dynamic_lookup")

      # Select flags.
      set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
      set(CMAKE_CXX_FLAGS_RELEASE        "-O2")
      set(CMAKE_CXX_FLAGS_DEBUG          "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
      set(CMAKE_CXX_FLAGS_DEBUGFULL      "-g3 -fno-inline -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-exceptions -fno-check-new -fno-common")
      set(CMAKE_CXX_FLAGS_PROFILE        "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
      set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -g")
      set(CMAKE_C_FLAGS_RELEASE          "-O2")
      set(CMAKE_C_FLAGS_DEBUG            "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
      set(CMAKE_C_FLAGS_DEBUGFULL        "-g3 -fno-inline -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align -Werror-implicit-function-declaration -Wchar-subscripts -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security -Wmissing-format-attribute -fno-common")
      set(CMAKE_C_FLAGS_PROFILE          "-g3 -fno-inline -ftest-coverage -fprofile-arcs")
 
      endif (CMAKE_COMPILER_IS_GNUCXX)

endif (CMAKE_SYSTEM_NAME MATCHES Darwin) 



###############             Dictionary generation            #####################
LIST(APPEND sources LibDict.cxx)
ROOT_GENERATE_DICTIONARY( "${header}"  LinkDef.h  LibDict.cxx ".;include/;${LIBCONFIG_INCLUDE_DIR}" )
add_library( LibDict SHARED ${sources} ${header} )
target_link_libraries(LibDict ${ROOT_LIBRARIES})


foreach( testsourcefile ${APP_SOURCES} )

    # I used a simple string replace, to cut off .cc.
		string( REPLACE ".cc" ".bin" testname ${testsourcefile} )
		add_executable( ${testname} ${testsourcefile}  ${sources} ${headers})	
    # Make sure YourLib is linked to each app
    target_link_libraries( ${testname} ${ROOT_LIBRARIES} ${FFTW_LIBRARIES} ${LIBCONFIG_LIBRARIES} ${LIBCONFIGPP_LIBRARY}   LibDict )
		
endforeach( testsourcefile ${APP_SOURCES} )



