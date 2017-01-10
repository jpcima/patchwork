cmake_minimum_required(VERSION 3.0)

project(patchwork VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

include(FindPkgConfig)

find_package(Qt5Widgets REQUIRED)
pkg_check_modules(Graphviz libgvc REQUIRED)

set(CMAKE_AUTOMOC ON)

qt5_add_resources(patchwork_QRC_SOURCES
  resources/patchwork.qrc)
add_library(patchwork STATIC
  sources/patchwork/pwarea.cc
  sources/patchwork/pwareascene.cc
  sources/patchwork/pwareascene_gvlayout.cc
  sources/patchwork/pwboxitem.cc
  sources/patchwork/pwboxtitle.cc
  sources/patchwork/pwnode.cc
  sources/patchwork/pwendpoint.cc
  sources/patchwork/pwconnector.cc
  sources/patchwork/widgets/pwdial.cc
  sources/patchwork/widgets/pwslider.cc
  sources/patchwork/private/qutil/qutil.cc
  ${patchwork_QRC_SOURCES})
target_link_libraries(patchwork
  Qt5::Widgets ${Graphviz_LIBRARIES})
target_include_directories(patchwork
  PRIVATE ${Graphviz_INCLUDE_DIRS}
  PUBLIC sources
  PRIVATE sources/patchwork/private)

add_executable(demo tests/demo.cc)
target_link_libraries(demo patchwork)
