cmake_minimum_required(VERSION 2.8)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/dm  ${PROJECT_SOURCE_DIR}/src/dmu ${PROJECT_SOURCE_DIR}/src/dmgl)
LINK_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

SET(KURMET_SRC  kurmet.cpp )
ADD_EXECUTABLE(kurmet ${KURMET_SRC})

SET(FUNC_SRC functions.cpp)
ADD_LIBRARY(func SHARED ${FUNC_SRC})
SET_TARGET_PROPERTIES(func PROPERTIES VERSION 5.0 SOVERSION 1)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

TARGET_LINK_LIBRARIES(kurmet ${GLUT_LIBRARY} ${OPENGL_LIBRARY} dm dmu dmgl func)



# set(DEMOS simple start pendulum tree carts wire ring )
# set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} dm dmu dmgl)

# foreach(demo ${DEMOS}
#   add_executable(${demo} ${demo}.cpp)
#   target_link_libraries(${demo} ${CORELIBS})
# endforeach(demo)



