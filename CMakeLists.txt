cmake_minimum_required(VERSION 3.26)

project(
  Manor
  VERSION 0.1
  LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wconversion -Wshadow -Wpedantic")

add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
add_link_options(-fsanitize=address,undefined)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql PrintSupport)

qt_add_executable(
  Manor
  MANUAL_FINALIZATION
  main.cpp
  manor.cpp
  manor.h
  database.h
  manor.ui
  shared.h
  tenantdialog.h
  tenantdialog.cpp
  tenantdialog.ui
  leasewizard.h
  leasewizard.cpp)

qt_add_resources(Manor "images" PREFIX "/" FILES images/napoleon1.png)

target_link_libraries(Manor PRIVATE Qt6::Widgets Qt6::Sql Qt6::PrintSupport)

set_target_properties(
  Manor
  PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
             MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
             MACOSX_BUNDLE_SHORT_VERSION_STRING
             ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
             MACOSX_BUNDLE TRUE
             WIN32_EXECUTABLE TRUE)

include(GNUInstallDirs)
install(
  TARGETS Manor
  BUNDLE DESTINATION .
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

qt_finalize_executable(Manor)
