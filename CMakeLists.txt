cmake_minimum_required(VERSION 3.24.2)
project(mini_lisp)

# 设置头文件目录
include_directories(include)

# 将源文件列表保存到变量中
set(SOURCES
    src/token.cpp
    src/tokenizer.cpp
    src/value.cpp
    src/parser.cpp
    src/main.cpp
)

# 添加可执行文件目标
add_executable(mini_lisp ${SOURCES})

# 设置 C++ 标准
set_target_properties(
  mini_lisp
  PROPERTIES CXX_STANDARD 17
             CXX_STANDARD_REQUIRED ON
)


if(MINGW)
  target_compile_options(mini_lisp PRIVATE -fexceptions)
endif()

if(WIN32)
    target_link_libraries(mini_lisp ws2_32)
endif()