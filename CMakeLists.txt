cmake_minimum_required(VERSION 3.16)

# Название проекта
project(InterProcessCommunication 
    VERSION 1.0.0
    DESCRIPTION "Lab 4: Process Synchronization"
    LANGUAGES CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Конфигурация для Windows
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    add_definitions(-DUNICODE -D_UNICODE)
endif()

# Конфигурация для отладки
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# Опции компиляции
if(MSVC)
    add_compile_options(/W4)
    add_compile_options(/MP)  # Многопоточная компиляция
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Проверка на Windows (проект использует WinAPI)
if(NOT WIN32)
    message(FATAL_ERROR "This project requires Windows platform (uses WinAPI)")
endif()

# Включение CTest для тестирования
enable_testing()

# Добавление поддиректорий
add_subdirectory(src)
add_subdirectory(tests)

# Пользовательские цели
add_custom_target(run_receiver
    COMMAND receiver.exe
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src
    DEPENDS receiver
    COMMENT "Running receiver process"
)

add_custom_target(clean_data_files
    COMMAND ${CMAKE_COMMAND} -E remove -f *.dat *.bin
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/src
    COMMENT "Cleaning data files"
)

# Установка переменных для тестов
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# Информация о проекте
message(STATUS "=== ${PROJECT_NAME} ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}")

# Конфигурация для разных типов сборки
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug build - enabling additional debugging features")
    add_definitions(-DDEBUG_MODE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Release build - optimizations enabled")
    add_definitions(-DRELEASE_MODE)
endif()