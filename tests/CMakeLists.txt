# CMakeLists.txt ��� ����-������

# ����� Google Test
find_package(GTest QUIET)

if(NOT GTest_FOUND)
    # ���� Google Test �� ������, ��������� ��� ����� FetchContent
    include(FetchContent)
    
    message(STATUS "Google Test not found locally, fetching from GitHub...")
    
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0  # ���������� ���������� ������
        GIT_SHALLOW    TRUE
    )
    
    # ��������� ��� Google Test
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    
    FetchContent_MakeAvailable(googletest)
    
    # ������� ������ ��� �������������
    add_library(GTest::GTest ALIAS gtest)
    add_library(GTest::Main ALIAS gtest_main)
    
    message(STATUS "Google Test fetched successfully")
else()
    message(STATUS "Using system Google Test")
endif()

# ��������� ��������� CTest
include(GoogleTest)

# ���������� �������� � ����������� � include path ��� ������
include_directories(${CMAKE_SOURCE_DIR}/src)

# �������� ������������ ����� ��� ������
add_executable(unit_tests
    test_main.cpp
    test_message.cpp
    test_file_operations.cpp
    ${CMAKE_SOURCE_DIR}/src/message.h  # �������� ������������ ����
)

# ��������� ���������� ��� ������
target_compile_features(unit_tests PRIVATE cxx_std_17)

# �������� � Google Test
if(GTest_FOUND)
    target_link_libraries(unit_tests PRIVATE 
        GTest::GTest 
        GTest::Main
    )
else()
    target_link_libraries(unit_tests PRIVATE 
        gtest 
        gtest_main
    )
endif()

# �������� � ���������� ������������ Windows
if(WIN32)
    target_link_libraries(unit_tests PRIVATE kernel32 user32)
    target_compile_definitions(unit_tests PRIVATE 
        _WIN32_WINNT=0x0601
        WINVER=0x0601
    )
endif()

# ��������� ��� MSVC
if(MSVC)
    target_compile_options(unit_tests PRIVATE /W4)
    target_compile_definitions(unit_tests PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
    )
    
    # ���������� �������������� Google Test ��� MSVC
    target_compile_definitions(unit_tests PRIVATE 
        _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
    )
endif()

# ��������� ��� MinGW/GCC
if(MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(unit_tests PRIVATE -Wall -Wextra -Wno-unused-parameter)
    target_link_options(unit_tests PRIVATE -static-libgcc -static-libstdc++)
endif()

# ����������� ������ � CTest
gtest_discover_tests(unit_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    PROPERTIES 
        TIMEOUT 60
        DISCOVERY_TIMEOUT 10
)

# ���������������� ���� ��� ������� ������
add_custom_target(run_tests
    COMMAND unit_tests --gtest_output=xml:test_results.xml
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    DEPENDS unit_tests
    COMMENT "Running unit tests"
)

# ���� ��� ������� ������ � ��������� �������
add_custom_target(run_tests_verbose
    COMMAND unit_tests --gtest_output=xml:test_results.xml --verbose
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    DEPENDS unit_tests
    COMMENT "Running unit tests with verbose output"
)

# �������� ����� ��� �������� ������
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tests/test_data)

# ����������� �������� ������ (���� ����)
add_custom_command(TARGET unit_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/tests/test_data
    COMMENT "Creating test data directory"
)

# ������� �������� ������
add_custom_target(clean_test_files
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/tests/test_data
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/tests/test_data
    COMMENT "Cleaning test files"
)

# ��������� ������ (�����������)
install(TARGETS unit_tests
    RUNTIME DESTINATION tests
    COMPONENT Testing
)

# ���������� � ������
message(STATUS "Configured test targets:")
message(STATUS "  - unit_tests (Main test executable)")
message(STATUS "  - run_tests (Run all tests)")
message(STATUS "  - run_tests_verbose (Run tests with verbose output)")

# �������� ������������� � Windows
if(NOT WIN32)
    message(WARNING "Tests are designed for Windows platform - some tests may fail on other platforms")
endif()