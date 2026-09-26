cmake_minimum_required(VERSION 3.12)
project(veles
  LANGUAGES C CXX
  VERSION 1.0.0)

set(CMAKE_AUTOMOC ON)
set(CXX_INCLUDE_WHAT_YOU_USE "include-what-you-use")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets)

qt_standard_project_setup()
qt_add_executable(veles-executable
  src/main.cpp
  src/palette_action.cpp
  src/palette_action.hpp
  src/palette.cpp
  src/palette.hpp
  src/state.cpp
  src/state.hpp
  src/tools/color_picker.cpp
  src/tools/color_picker.hpp
  src/tools/fill.cpp
  src/tools/fill.hpp
  src/tools/pen.cpp
  src/tools/pen.hpp
  src/tools/tool.cpp
  src/tools/tool.hpp
  src/tools/view.cpp
  src/tools/view.hpp
  src/widgets/canvas.cpp
  src/widgets/canvas.hpp
  src/widgets/convert_palette_dialog.cpp
  src/widgets/convert_palette_dialog.hpp
  src/widgets/create_file_dialog.cpp
  src/widgets/create_file_dialog.hpp
  src/widgets/dialogs.cpp
  src/widgets/dialogs.hpp
  src/widgets/editor.cpp
  src/widgets/editor.hpp
  src/widgets/image.cpp
  src/widgets/image.hpp
  src/widgets/main_window.cpp
  src/widgets/main_window.hpp
  src/widgets/palette_tab.cpp
  src/widgets/palette_tab.hpp
  src/widgets/resize_dialog.cpp
  src/widgets/resize_dialog.hpp
  src/widgets/unsaved_file_dialog.cpp
  src/widgets/unsaved_file_dialog.hpp
)
qt_add_resources(veles-executable "veles_assets"
  PREFIX "/"
  FILES
  assets/icons/color_picker.png
  assets/icons/fill.png
  assets/icons/pen.png
  assets/icons/view.png
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
target_link_libraries(veles-executable PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::OpenGLWidgets)

target_compile_options(veles-executable
  PRIVATE $<$<STREQUAL:CMAKE_CXX_COMPILER_ID,MSVC>:"/W4 /WX /permissive-">
  PRIVATE $<$<STREQUAL:CMAKE_CXX_COMPILER_ID,GNU>:"-Wall -Wextra -Wpedantic -Werror">
)
