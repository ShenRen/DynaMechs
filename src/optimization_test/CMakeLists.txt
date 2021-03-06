cmake_minimum_required(VERSION 2.8)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(MOSEK_LIB C:/Program\ Files\ \(x86\)/Mosek/6/tools/platform/win32x86/bin/mosek6_0.lib)
    SET(MOSEKNOOMP_LIB C:/Program\ Files\ \(x86\)/Mosek/6/tools/platform/win32x86/bin/moseknoomp6_0.lib)
    SET(MOSEK_INCLUDE_DIR C:/Program\ Files\ \(x86\)/Mosek/6/tools/platform/win32x86/h)
ELSE()
    SET(MOSEK_LIB ${PROJECT_SOURCE_DIR}/src/mosek/6/tools/platform/linux64x86/bin/libmosek64.so)
    SET(MOSEKNOOMP_LIB ${PROJECT_SOURCE_DIR}/src/mosek/6/tools/platform/linux64x86/bin/libmoseknoomp64.so)
    SET(MOSEK_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/mosek/6/tools/platform/linux64x86/h)
ENDIF()

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/dm  ${PROJECT_SOURCE_DIR}/src/dmu ${PROJECT_SOURCE_DIR}/src/dmgl ${PROJECT_SOURCE_DIR}/src/wxdmgl ${MOSEK_INCLUDE_DIR} )

LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib )

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # It was noticed that when using MinGW gcc it is essential that 'core' is mentioned before 'base'
    find_package(wxWidgets COMPONENTS core base gl REQUIRED)
    include( "${wxWidgets_USE_FILE}" )
ELSE()
    ##tell CMake to search first in directories listed in CMAKE_MODULE_PATH 
    #SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeScripts)
    #message("CMAKE_MODULE_PATH (search first) is ${CMAKE_MODULE_PATH}")

    #SET(wxWidgets_ROOT_DIR /usr/local/bin)
    find_package(wxWidgets 2.9 COMPONENTS core base gl REQUIRED)
    if(wxWidgets_FOUND)
        message("optimization_test: wxWidgets found successfully :) ")
		message(STATUS "wxWidgets Libraries: " ${wxWidgets_LIBRARIES})
        include( ${wxWidgets_USE_FILE} )
    else()
        message(FATAL_ERROR "wxWidgets was not found! :( ")
    endif()

    #if(wxWidgets_PFVERSION LESS 2.9)
    #  message("wxWidgets Version ${wxWidgets_PFVERSION}")
    #  #message(FATAL_ERROR "wxWidgets is not a high enough version!")
    #
    #else()
    #  message("wxWidgets Version ${wxWidgets_PFVERSION}")
    #
    #endif()

ENDIF()





SET(SRC1  src/eigen_func_test.cpp)
ADD_EXECUTABLE(func_test ${SRC1})


SET(SRCS src/QPsolver.cpp
         src/math_funcs.cpp
         src/testOpt1.cpp)
ADD_EXECUTABLE(test1 ${SRCS})
TARGET_LINK_LIBRARIES(test1 ${wxWidgets_LIBRARIES} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} dm dmu wxdmgl ${MOSEK_LIB} ${MOSEKNOOMP_LIB})


SET(SRCS src/QPsolver.cpp
         src/math_funcs.cpp
         src/PrioritizedController.cpp
         src/testOpt2.cpp)
ADD_EXECUTABLE(test2 ${SRCS})
TARGET_LINK_LIBRARIES(test2 ${wxWidgets_LIBRARIES} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} dm dmu wxdmgl ${MOSEK_LIB} ${MOSEKNOOMP_LIB})



SET(SRCS src/wxMain.cpp
         src/BasicGLPane.cpp
         src/DataLogger.cpp
         src/MainFrame.cpp
         src/SimulationThread.cpp
         src/globalVariables.cpp
         src/globalFunctions.cpp
         src/userGraphics.cpp
         src/math_funcs.cpp
         src/QPsolver.cpp
         src/PrioritizedController.cpp
         src/TaskSpaceControllerL.cpp
         src/StateMachineControllerA.cpp
         src/TraceableStateMachineControllerA.cpp
)
ADD_EXECUTABLE(opt_test ${SRCS})
TARGET_LINK_LIBRARIES(opt_test ${wxWidgets_LIBRARIES} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} dm dmu wxdmgl ${MOSEK_LIB} ${MOSEKNOOMP_LIB})






