cmake_minimum_required(VERSION 2.8.9)
project(dds-robot-arm C)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(MOTION_NAME dds_motion)
SET(ROBOT_NAME  dds_robot)
SET(ROBOT2_NAME dds_robot2)

# libdds location
SET(DDS_LIB_DIR /opt/PrismTech/Vortex_v2/Device/VortexLite/2.0.4/lib/linux_gcc_x86/libdds.so)

# dds include files
SET(DDS_HEADER_INCLUDE_DIR /opt/PrismTech/Vortex_v2/Device/VortexLite/2.0.4/include)

# rs232 include files
SET(RS232_HEADER_INCLUDE_DIR /usr/include/librs232)

# source codes
SET(MOTION_SOURCES dds_motion.c motion.c motion.h ${DDS_HEADER_INCLUDE_DIR}/dds.h)

SET(ROBOT_SOURCES  dds_robot.c  motion.c motion.h ${DDS_HEADER_INCLUDE_DIR}/dds.h ${RS232_HEADER_INCLUDE_DIR}/rs232.h)
SET(ROBOT2_SOURCES dds_robot2.c motion.c motion.h ${DDS_HEADER_INCLUDE_DIR}/dds.h ${RS232_HEADER_INCLUDE_DIR}/rs232.h)

# cflag
SET(EXT_C_FLAGS "-m64 -pipe -Wall -fno-strict-aliasing -O3 -std=c99 -Wstrict-prototypes -Wmissing-prototypes -Dos_linux_gcc_x86 -DLITE=1 -DNDEBUG -D_GNU_SOURCE")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXT_C_FLAGS}")

INCLUDE_DIRECTORIES(${DDS_HEADER_INCLUDE_DIR} ${RS232_HEADER_INCLUDE_DIR})

# motion
ADD_EXECUTABLE(${MOTION_NAME} ${MOTION_SOURCES})
TARGET_LINK_LIBRARIES(${MOTION_NAME} LINK_PUBLIC ${DDS_LIB_DIR} rt pthread zmq czmq)

# robot
ADD_EXECUTABLE(${ROBOT_NAME} ${ROBOT_SOURCES})
TARGET_LINK_LIBRARIES(${ROBOT_NAME} LINK_PUBLIC ${DDS_LIB_DIR} rt pthread rs232)

# robot2
ADD_EXECUTABLE(${ROBOT2_NAME} ${ROBOT2_SOURCES})
TARGET_LINK_LIBRARIES(${ROBOT2_NAME} LINK_PUBLIC ${DDS_LIB_DIR} rt pthread rs232)

# install binary
install(TARGETS ${MOTION_NAME} DESTINATION /usr/bin COMPONENT binaries)
install(TARGETS ${ROBOT_NAME} DESTINATION /usr/bin COMPONENT binaries)
install(TARGETS ${ROBOT2_NAME} DESTINATION /usr/bin COMPONENT binaries)