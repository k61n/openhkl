file(GLOB_RECURSE sources *.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)
ADD_LIBRARY(NSXTool SHARED ${sources})
TARGET_LINK_LIBRARIES(NSXTool ${Boost_LIBRARIES} ${GSL} ${GSL_CBLAS})

##### INSTALL PART #######
# Will copy the created library and resources 
##########################
INSTALL(TARGETS NSXTool DESTINATION NSXTool)

