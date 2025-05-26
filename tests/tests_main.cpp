#include <gtest/gtest.h>
#include <windows.h>
#include <iostream>

// Настройка консоли для корректного отображения русского текста в тестах
class TestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        // Настройка кодировки консоли для Windows
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);

        std::cout << "=== Запуск юнит-тестов для лабораторной работы №4 ===" << std::endl;
        std::cout << "Тема: Синхронизация процессов" << std::endl;
        std::cout << "Платформа: Windows (WinAPI)" << std::endl;
        std::cout << "========================================================" << std::endl;
    }

    void TearDown() override {
        std::cout << "========================================================" << std::endl;
        std::cout << "=== Завершение юнит-тестов ===" << std::endl;
    }
};

// Глобальная функция для очистки тестовых файлов
void cleanupTestFiles() {
    // Удаляем тестовые файлы
    DeleteFileA("test_message.dat");
    DeleteFileA("test_sync.dat");
    DeleteFileA("test_empty.dat");
    DeleteFileA("test_operations.dat");
}

// Класс для базовых тестов с общей функциональностью
class BaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Очистка перед каждым тестом
        cleanupTestFiles();
    }

    void TearDown() override {
        // Очистка после каждого теста
        cleanupTestFiles();
    }

    // Вспомогательная функция для создания временного файла
    std::string createTempFileName(const std::string& prefix = "test") {
        return prefix + "_" + std::to_string(GetTickCount()) + ".dat";
    }

    // Проверка существования файла
    bool fileExists(const std::string& fileName) {
        DWORD attributes = GetFileAttributesA(fileName.c_str());
        return (attributes != INVALID_FILE_ATTRIBUTES &&
            !(attributes & FILE_ATTRIBUTE_DIRECTORY));
    }
};

int main(int argc, char** argv) {
    // Инициализация Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // Добавление глобального окружения для тестов
    ::testing::AddGlobalTestEnvironment(new TestEnvironment);

    // Настройка фильтров тестов (если нужно)
    if (argc > 1) {
        std::cout << "Запуск с аргументами командной строки..." << std::endl;
        for (int i = 1; i < argc; ++i) {
            std::cout << "  Аргумент " << i << ": " << argv[i] << std::endl;
        }
    }

    // Запуск всех тестов
    int result = RUN_ALL_TESTS();

    // Финальная очистка
    cleanupTestFiles();

    return result;
}