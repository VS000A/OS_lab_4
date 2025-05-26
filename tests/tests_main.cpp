#include <gtest/gtest.h>
#include <windows.h>
#include <iostream>

// ��������� ������� ��� ����������� ����������� �������� ������ � ������
class TestEnvironment : public ::testing::Environment {
public:
    void SetUp() override {
        // ��������� ��������� ������� ��� Windows
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);

        std::cout << "=== ������ ����-������ ��� ������������ ������ �4 ===" << std::endl;
        std::cout << "����: ������������� ���������" << std::endl;
        std::cout << "���������: Windows (WinAPI)" << std::endl;
        std::cout << "========================================================" << std::endl;
    }

    void TearDown() override {
        std::cout << "========================================================" << std::endl;
        std::cout << "=== ���������� ����-������ ===" << std::endl;
    }
};

// ���������� ������� ��� ������� �������� ������
void cleanupTestFiles() {
    // ������� �������� �����
    DeleteFileA("test_message.dat");
    DeleteFileA("test_sync.dat");
    DeleteFileA("test_empty.dat");
    DeleteFileA("test_operations.dat");
}

// ����� ��� ������� ������ � ����� �����������������
class BaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        // ������� ����� ������ ������
        cleanupTestFiles();
    }

    void TearDown() override {
        // ������� ����� ������� �����
        cleanupTestFiles();
    }

    // ��������������� ������� ��� �������� ���������� �����
    std::string createTempFileName(const std::string& prefix = "test") {
        return prefix + "_" + std::to_string(GetTickCount()) + ".dat";
    }

    // �������� ������������� �����
    bool fileExists(const std::string& fileName) {
        DWORD attributes = GetFileAttributesA(fileName.c_str());
        return (attributes != INVALID_FILE_ATTRIBUTES &&
            !(attributes & FILE_ATTRIBUTE_DIRECTORY));
    }
};

int main(int argc, char** argv) {
    // ������������� Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // ���������� ����������� ��������� ��� ������
    ::testing::AddGlobalTestEnvironment(new TestEnvironment);

    // ��������� �������� ������ (���� �����)
    if (argc > 1) {
        std::cout << "������ � ����������� ��������� ������..." << std::endl;
        for (int i = 1; i < argc; ++i) {
            std::cout << "  �������� " << i << ": " << argv[i] << std::endl;
        }
    }

    // ������ ���� ������
    int result = RUN_ALL_TESTS();

    // ��������� �������
    cleanupTestFiles();

    return result;
}