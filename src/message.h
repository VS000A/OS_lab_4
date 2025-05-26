#ifndef MESSAGE_H
#define MESSAGE_H

#include <windows.h>
#include <cstring>

// ���������
const int MAX_MESSAGE_LENGTH = 20;
const char* const SENDER_READY_EVENT = "SenderReadyEvent";
const char* const CAN_WRITE_EVENT = "CanWriteEvent";
const char* const CAN_READ_EVENT = "CanReadEvent";

// ��������� ���������
struct Message {
    char text[MAX_MESSAGE_LENGTH];  // ����� ���������
    bool isEmpty;                   // ���� ������� ���������

    // ����������� �� ���������
    Message() : isEmpty(true) {
        memset(text, 0, MAX_MESSAGE_LENGTH);
    }

    // ����������� � �������
    explicit Message(const char* messageText) : isEmpty(false) {
        memset(text, 0, MAX_MESSAGE_LENGTH);
        if (messageText) {
            strncpy_s(text, MAX_MESSAGE_LENGTH, messageText, MAX_MESSAGE_LENGTH - 1);
        }
    }

    // ����� ��� ������� ���������
    void clear() {
        memset(text, 0, MAX_MESSAGE_LENGTH);
        isEmpty = true;
    }

    // ����� ��� ��������� ������
    void setText(const char* messageText) {
        memset(text, 0, MAX_MESSAGE_LENGTH);
        if (messageText) {
            strncpy_s(text, MAX_MESSAGE_LENGTH, messageText, MAX_MESSAGE_LENGTH - 1);
            isEmpty = false;
        }
        else {
            isEmpty = true;
        }
    }

    // �������� ���������� ���������
    bool isValid() const {
        return !isEmpty && text[0] != '\0';
    }

    // ��������� ����� ���������
    size_t length() const {
        return isEmpty ? 0 : strlen(text);
    }
};

// ��������������� ������� ��� ������ � �������
namespace FileUtils {

    // �������� ����� ��� ���������
    inline HANDLE createMessageFile(const char* fileName) {
        return CreateFileA(fileName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    }

    // �������� ������������� �����
    inline HANDLE openMessageFile(const char* fileName) {
        return CreateFileA(fileName,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
    }

    // ������ ��������� �� �����
    inline bool readMessage(HANDLE hFile, Message& msg) {
        DWORD bytesRead;
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        return ReadFile(hFile, &msg, sizeof(Message), &bytesRead, NULL) &&
            bytesRead == sizeof(Message);
    }

    // ������ ��������� � ����
    inline bool writeMessage(HANDLE hFile, const Message& msg) {
        DWORD bytesWritten;
        SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
        return WriteFile(hFile, &msg, sizeof(Message), &bytesWritten, NULL) &&
            bytesWritten == sizeof(Message);
    }
}

// ��������������� ������� ��� ������ � ���������
namespace EventUtils {

    // �������� ������������ �������
    inline HANDLE createNamedEvent(const char* eventName, bool initialState = false) {
        return CreateEventA(NULL, TRUE, initialState, eventName);
    }

    // �������� ������������� �������
    inline HANDLE openNamedEvent(const char* eventName) {
        return OpenEventA(EVENT_ALL_ACCESS, FALSE, eventName);
    }

    // �������� ������� � ���������
    inline bool waitForEvent(HANDLE hEvent, DWORD timeout = INFINITE) {
        return WaitForSingleObject(hEvent, timeout) == WAIT_OBJECT_0;
    }

    // ��������� �������
    inline bool signalEvent(HANDLE hEvent) {
        return SetEvent(hEvent) != 0;
    }

    // ����� �������
    inline bool resetEvent(HANDLE hEvent) {
        return ResetEvent(hEvent) != 0;
    }
}

// ���� ������
enum class ErrorCode {
    SUCCESS = 0,
    FILE_CREATE_ERROR = 1,
    FILE_OPEN_ERROR = 2,
    FILE_READ_ERROR = 3,
    FILE_WRITE_ERROR = 4,
    EVENT_CREATE_ERROR = 5,
    EVENT_OPEN_ERROR = 6,
    PROCESS_CREATE_ERROR = 7,
    INVALID_ARGUMENTS = 8,
    TIMEOUT_ERROR = 9
};

// ������� ��� ��������� �������� ������
inline const char* getErrorDescription(ErrorCode code) {
    switch (code) {
    case ErrorCode::SUCCESS: return "�������� ��������� �������";
    case ErrorCode::FILE_CREATE_ERROR: return "������ �������� �����";
    case ErrorCode::FILE_OPEN_ERROR: return "������ �������� �����";
    case ErrorCode::FILE_READ_ERROR: return "������ ������ �����";
    case ErrorCode::FILE_WRITE_ERROR: return "������ ������ �����";
    case ErrorCode::EVENT_CREATE_ERROR: return "������ �������� �������";
    case ErrorCode::EVENT_OPEN_ERROR: return "������ �������� �������";
    case ErrorCode::PROCESS_CREATE_ERROR: return "������ �������� ��������";
    case ErrorCode::INVALID_ARGUMENTS: return "�������� ���������";
    case ErrorCode::TIMEOUT_ERROR: return "��������� ����� ��������";
    default: return "����������� ������";
    }
}

#endif // MESSAGE_H