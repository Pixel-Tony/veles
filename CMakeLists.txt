cmake_minimum_required(VERSION 3.12)
project(veles
  LANGUAGES C CXX
  VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)
set(CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()
qt_add_executable(veles-executable
  src/main.cpp
  src/palette_action.cpp
  src/palette_action.hpp
  src/palette.cpp
  src/palette.hpp
  src/state.cpp
  src/state.hpp
  src/widgets/main_window.cpp
  src/widgets/main_window.hpp
  src/widgets/palette_tab.cpp
  src/widgets/palette_tab.hpp
)
qt_add_resources(veles-executable "veles_assets"
  PREFIX "/"
  FILES
  assets/logo-128.png
  assets/palette-dos-groups.json
  assets/palette-dos.act
  assets/palette-win-groups.json
  assets/palette-win.act
  OPTIONS -no-compress
)

if(NOT DEFINED QT_DEBUG)
  qt_add_resources(veles-executable "veles_style"
    PREFIX "/"
    FILES assets/style.qss
    OPTIONS -no-compress
  )
endif()

set_target_properties(veles-executable PROPERTIES
  CXX_STANDARD 20
  OUTPUT_NAME veles
)

target_include_directories(veles-executable PRIVATE src)
target_link_libraries(veles-executable PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

target_compile_options(veles-executable
  PRIVATE $<$<STREQUAL:CMAKE_CXX_COMPILER_ID,MSVC>:"/W4 /WX /permissive-">
  PRIVATE $<$<STREQUAL:CMAKE_CXX_COMPILER_ID,GNU>:"-Wall -Wextra -Wpedantic -Werror">
)
