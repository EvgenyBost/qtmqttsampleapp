cmake_minimum_required(VERSION 3.1)

project(QTMQTTSampleApp LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Gui Qml Quick REQUIRED)

find_path(MOSQUITTO_INCLUDE_DIR mosquitto.h)
find_library(MOSQUITTO_LIBRARY mosquitto)

if(NOT MOSQUITTO_INCLUDE_DIR OR NOT MOSQUITTO_LIBRARY)
  message(FATAL_ERROR "libmosquitto not found. Install libmosquitto-dev")
endif()

add_executable(${PROJECT_NAME} "main.cpp" "qml.qrc" "mosquittopublisher.h" "mosquittopublisher.cpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${MOSQUITTO_INCLUDE_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Quick Qt5::Gui Qt5::Qml ${MOSQUITTO_LIBRARY})
