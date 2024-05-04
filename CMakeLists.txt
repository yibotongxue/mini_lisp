cmake_minimum_required(VERSION 3.24.2)
project(mini_lisp)

# 设置头文件目录
include_directories(include)

# 将源文件列表保存到变量中
set(SOURCES
    src/value.cpp
    src/main.cpp
)

# 添加可执行文件目标
add_executable(mini_lisp ${SOURCES})

# 设置 C++ 标准
set_target_properties(
  mini_lisp
  PROPERTIES CXX_STANDARD 20
             CXX_STANDARD_REQUIRED ON
)

# 设置输出目录
set_target_properties(
  mini_lisp
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin
)

# 如果是 MinGW 编译器，设置编译选项
if(MINGW)
  target_compile_options(mini_lisp PRIVATE -municode -mconsole)
endif()
