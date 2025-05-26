# CMakeLists.txt для исходного кода

# Создание исполняемого файла receiver
add_executable(receiver 
    receiver.cpp
    message.h
)

# Создание исполняемого файла sender
add_executable(sender 
    sender.cpp
    message.h
)

# Настройки компиляции для receiver
target_compile_features(receiver PRIVATE cxx_std_17)
target_compile_definitions(receiver PRIVATE 
    _WIN32_WINNT=0x0601  # Windows 7 и выше
    WINVER=0x0601
)

# Настройки компиляции для sender
target_compile_features(sender PRIVATE cxx_std_17)
target_compile_definitions(sender PRIVATE 
    _WIN32_WINNT=0x0601  # Windows 7 и выше
    WINVER=0x0601
)

# Линковка с системными библиотеками Windows
if(WIN32)
    target_link_libraries(receiver PRIVATE kernel32 user32)
    target_link_libraries(sender PRIVATE kernel32 user32)
endif()

# Настройки для MSVC
if(MSVC)
    target_compile_options(receiver PRIVATE /W4 /WX-)
    target_compile_options(sender PRIVATE /W4 /WX-)
    
    # Отключение предупреждений для MSVC
    target_compile_definitions(receiver PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
    )
    target_compile_definitions(sender PRIVATE 
        _CRT_SECURE_NO_WARNINGS
        _CRT_NONSTDC_NO_DEPRECATE
    )
endif()

# Настройки для MinGW/GCC
if(MINGW OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(receiver PRIVATE -Wall -Wextra)
    target_compile_options(sender PRIVATE -Wall -Wextra)
    
    # Статическая линковка для MinGW
    target_link_options(receiver PRIVATE -static-libgcc -static-libstdc++)
    target_link_options(sender PRIVATE -static-libgcc -static-libstdc++)
endif()

# Копирование исполняемых файлов в корень build директории (опционально)
add_custom_command(TARGET receiver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:receiver> ${CMAKE_BINARY_DIR}
    COMMENT "Copying receiver.exe to build directory"
)

add_custom_command(TARGET sender POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sender> ${CMAKE_BINARY_DIR}
    COMMENT "Copying sender.exe to build directory"
)

# Установка файлов (для make install)
install(TARGETS receiver sender
    RUNTIME DESTINATION bin
    COMPONENT Runtime
)

# Установка заголовочных файлов
install(FILES message.h
    DESTINATION include
    COMPONENT Development
)

# Создание зависимости между receiver и sender
add_dependencies(receiver sender)

# Информация о целях
message(STATUS "Configured targets:")
message(STATUS "  - receiver.exe (Process Receiver)")
message(STATUS "  - sender.exe (Process Sender)")