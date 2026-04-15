cmake_minimum_required(VERSION 3.20)
project(blWidgets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(glfw3 CONFIG QUIET)
if(NOT glfw3_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
  )
  FetchContent_MakeAvailable(glfw)
endif()

find_package(blend2d CONFIG QUIET)
if(NOT blend2d_FOUND)
  set(_blend2d_candidates
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/blend2d"
    "${CMAKE_CURRENT_SOURCE_DIR}/../blend2d"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../blend2d"
  )
  foreach(_cand IN LISTS _blend2d_candidates)
    if(EXISTS "${_cand}/CMakeLists.txt")
      add_subdirectory("${_cand}" "${CMAKE_BINARY_DIR}/blend2d")
      set(blend2d_FOUND ON)
      break()
    endif()
  endforeach()
endif()
if(NOT blend2d_FOUND)
  message(FATAL_ERROR "Blend2D not found. Place it in third_party/blend2d or provide a package config.")
endif()

find_package(OpenGL REQUIRED)

add_library(bl_core STATIC
  core/bl_app.cpp
  ui/bl_event_dispatcher.cpp
)
target_include_directories(bl_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bl_core PUBLIC bl_platform bl_rendering bl_ui)

add_library(bl_platform STATIC
  platform/bl_window.cpp
)
target_include_directories(bl_platform PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bl_platform PUBLIC glfw)

add_library(bl_rendering STATIC
  rendering/bl_render_context.cpp
)
target_include_directories(bl_rendering PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(TARGET blend2d::blend2d)
  set(BLEND2D_LINK_TARGET blend2d::blend2d)
elseif(TARGET blend2d)
  set(BLEND2D_LINK_TARGET blend2d)
else()
  message(FATAL_ERROR "Blend2D target not found after dependency resolution.")
endif()
target_link_libraries(bl_rendering PUBLIC ${BLEND2D_LINK_TARGET} OpenGL::GL)

add_library(bl_ui STATIC
  ui/bl_box_sizer.cpp
  ui/bl_button.cpp
  ui/bl_stylesheet.cpp
  ui/bl_label.cpp
  ui/bl_checkbox.cpp
  ui/bl_panel.cpp
  ui/bl_slider.cpp
  ui/bl_ui_root.cpp
  ui/bl_textfield.cpp
  ui/bl_dropdown.cpp
  ui/bl_radio_button.cpp
  ui/bl_scroll_container.cpp
  ui/bl_progress_bar.cpp
)
target_include_directories(bl_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(bl_ui PUBLIC ${BLEND2D_LINK_TARGET})

add_executable(bl_widgets
  app/main.cpp
)
target_link_libraries(bl_widgets PRIVATE bl_core)
