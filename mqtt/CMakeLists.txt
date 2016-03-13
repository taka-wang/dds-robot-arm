cmake_minimum_required(VERSION 2.8.9)
project(mqtt-robot-arm C)

SET(CMAKE_VERBOSE_MAKEFILE ON)

SET(MOTION_NAME mqtt_motion)
SET(ROBOT_NAME  mqtt_robot)

# rs232 include files
SET(RS232_HEADER_INCLUDE_DIR /usr/include/librs232)

# source codes
SET(MOTION_SOURCES mqtt_motion.c)
SET(ROBOT_SOURCES  mqtt_robot.c ${RS232_HEADER_INCLUDE_DIR}/rs232.h)

# cflag
SET(EXT_C_FLAGS "-m64 -pipe -Wall -fno-strict-aliasing -O3 -std=c99 -Wstrict-prototypes -Wmissing-prototypes -D_GNU_SOURCE")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXT_C_FLAGS}")

INCLUDE_DIRECTORIES(${RS232_HEADER_INCLUDE_DIR})

# motion
ADD_EXECUTABLE(${MOTION_NAME} ${MOTION_SOURCES})
TARGET_LINK_LIBRARIES(${MOTION_NAME} LINK_PUBLIC rt pthread zmq czmq mosquitto)

# robot
ADD_EXECUTABLE(${ROBOT_NAME} ${ROBOT_SOURCES})
TARGET_LINK_LIBRARIES(${ROBOT_NAME} LINK_PUBLIC rt pthread rs232 mosquitto)

# install binary
install(TARGETS ${MOTION_NAME} DESTINATION /usr/bin COMPONENT binaries)
install(TARGETS ${ROBOT_NAME} DESTINATION /usr/bin COMPONENT binaries)
