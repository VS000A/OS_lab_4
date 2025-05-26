#include <windows.h>
#include <iostream>
#include <string>
#include "message.h"

using namespace std;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    string fileName;
    cout << "������� ��� ��������� �����: ";
    cin >> fileName;

    HANDLE hFile = FileUtils::createMessageFile(fileName.c_str());

    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "������ �������� �����!" << endl;
        return static_cast<int>(ErrorCode::FILE_CREATE_ERROR);
    }

    Message emptyMsg;

    DWORD bytesWritten;
    WriteFile(hFile, &emptyMsg, sizeof(Message), &bytesWritten, NULL);
    CloseHandle(hFile);

    HANDLE hReadyEvent = EventUtils::createNamedEvent(SENDER_READY_EVENT, false);
    HANDLE hCanWriteEvent = EventUtils::createNamedEvent(CAN_WRITE_EVENT, true);
    HANDLE hCanReadEvent = EventUtils::createNamedEvent(CAN_READ_EVENT, false);

    if (!hReadyEvent || !hCanWriteEvent || !hCanReadEvent) {
        cout << "������ �������� �������!" << endl;
        return static_cast<int>(ErrorCode::EVENT_CREATE_ERROR);
    }

    string commandLine = "sender.exe " + fileName;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(NULL, (LPSTR)commandLine.c_str(), NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        cout << "������ ������� �������� Sender!" << endl;
        return 1;
    }

    cout << "������� ���������� �������� Sender..." << endl;

    WaitForSingleObject(hReadyEvent, INFINITE);
    cout << "������� Sender ����� � ������!" << endl;

    string command;
    while (true) {
        cout << "\n������� ������� (read - ������ ���������, exit - �����): ";
        cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "read") {
            EventUtils::waitForEvent(hCanReadEvent);

            hFile = FileUtils::openMessageFile(fileName.c_str());

            if (hFile != INVALID_HANDLE_VALUE) {
                Message msg;

                if (FileUtils::readMessage(hFile, msg)) {
                    if (msg.isValid()) {
                        cout << "�������� ���������: " << msg.text << endl;

                        msg.clear();
                        FileUtils::writeMessage(hFile, msg);

                        EventUtils::signalEvent(hCanWriteEvent);
                        EventUtils::resetEvent(hCanReadEvent);
                    }
                    else {
                        cout << "��� ��������� ��� ������. ��������..." << endl;
                    }
                }
                CloseHandle(hFile);
            }
        }
        else {
            cout << "����������� �������!" << endl;
        }
    }

    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    CloseHandle(hReadyEvent);
    CloseHandle(hCanWriteEvent);
    CloseHandle(hCanReadEvent);

    cout << "������� Receiver ��������." << endl;
    return 0;
}