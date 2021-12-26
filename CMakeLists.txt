cmake_minimum_required(VERSION 3.14)

project(Pelipper VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  message(STATUS "Found ccache")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

# libetpan library
find_library(LIBETPAN_LIBRARY NAMES etpan REQUIRED)
if(NOT LIBETPAN_LIBRARY)
  message(FATAL_ERROR "ERROR: Could not find libetpan")
else()
  message(STATUS "Found libetpan: ${LIBETPAN_LIBRARY}")
endif()

find_package(Qt5 COMPONENTS Core Widgets Quick WebView Concurrent Sql REQUIRED)
find_package(KF5Kirigami2)
find_package(KF5I18n)


set(PROJECT_SOURCES
    src/session.h src/session.cpp
    src/account.h src/account.cpp
    src/imap.h src/imap.cpp
    src/folderlistmodel.h src/folderlistmodel.cpp
    src/messagelistmodel.h src/messagelistmodel.cpp
    src/message.h src/message.cpp
    src/messageitem.h src/messageitem.cpp
    src/folder.h src/folder.cpp
    src/folderitem.h src/folderitem.cpp
    src/accountitem.h src/accountitem.cpp
    src/contact.h src/contact.cpp
    src/settings.h src/settings.cpp
    src/basefolderitem.h src/basefolderitem.cpp
    src/imapcache.h src/imapcache.cpp
    src/sortmodel.h src/sortmodel.cpp
    src/application.h src/application.cpp
    src/roles.h
    src/main.cpp
    src/qml.qrc
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME}  ${LIBETPAN_LIBRARY})
target_link_libraries(${PROJECT_NAME}  Qt5::Core Qt5::Widgets Qt5::WebView Qt5::Quick Qt5::Concurrent Qt5::Sql)
target_link_libraries(${PROJECT_NAME}  KF5::Kirigami2)
target_link_libraries(${PROJECT_NAME}  KF5::I18n)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER org.kumar
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
