file(GLOB_RECURSE sources *.cpp)

if(WIN32)
    # Awful hack to make it work on windows where CGAL gave me some troubles to build.
    list(REMOVE_ITEM sources ${PROJECT_SOURCE_DIR}/src/geometry/CrystalHull.cpp)
endif()
    
include_directories(${PROJECT_SOURCE_DIR}/include/
                                ${PROJECT_SOURCE_DIR}/include/data
                                ${PROJECT_SOURCE_DIR}/include/geometry
                                ${PROJECT_SOURCE_DIR}/include/maths
                                ${PROJECT_SOURCE_DIR}/include/utils)
                                
ADD_LIBRARY(NSXTool SHARED ${sources})
TARGET_LINK_LIBRARIES(NSXTool ${Boost_LIBRARIES} ${GSL} ${GSL_CBLAS} ${GSL_LIBRARIES} ${GMP_LIBRARIES} ${MPFR_LIBRARIES} ${CGAL_LIBRARIES})
#TARGET_LINK_LIBRARIES(NSXTool ${Boost_LIBRARIES} ${GSL_LIBRARIES})

##### INSTALL PART #######
# Will copy the created library and resources 
##########################
INSTALL(TARGETS NSXTool DESTINATION NSXTool)

