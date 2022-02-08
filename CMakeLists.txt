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
    src/Widget/attachmentmodel.h src/Widget/attachmentmodel.cpp
    src/Widget/documenthandler.h src/Widget/documenthandler.cpp
    src/messageview/messageitem.h src/messageview/messageitem.cpp
    src/messagelistview/sortmodel.h src/messagelistview/sortmodel.cpp
    src/messagelistview/roles.h
    src/messagelistview/messagelistmodel.h src/messagelistview/messagelistmodel.cpp
    src/folderview/folderitem.h src/folderview/folderitem.cpp
    src/folderview/accountitem.h src/folderview/accountitem.cpp
    src/folderview/basefolderitem.h src/folderview/basefolderitem.cpp
    src/folderview/folderlistmodel.h src/folderview/folderlistmodel.cpp
    src/backend/contact.h src/backend/contact.cpp
    src/backend/flags.h src/backend/flags.cpp
    src/backend/message.h src/backend/message.cpp
    src/backend/folder.h src/backend/folder.cpp
    src/backend/cachemanager.h src/backend/cachemanager.cpp
    src/backend/idlemanager.h src/backend/idlemanager.cpp
    src/backend/smtpservice.h src/backend/smtpservice.cpp
    src/backend/imapcache.h src/backend/imapcache.cpp
    src/backend/imapservice.h src/backend/imapservice.cpp
    src/backend/account.h src/backend/account.cpp
    src/backend/session.h src/backend/session.cpp
    src/backend/settings.h src/backend/settings.cpp
    src/application.h src/application.cpp
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
