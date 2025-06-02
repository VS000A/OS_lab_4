#include <windows.h>
#include <iostream>
#include <string>
#include "message.h"
using namespace std;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    string fileName;
    cout << "Введите имя сообщения файла: ";
    cin >> fileName;
    
    HANDLE hFile = FileUtils::createMessageFile(fileName.c_str());
    if (hFile == INVALID_HANDLE_VALUE) {
        cout << "Ошибка создания файла!" << endl;
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
        cout << "Ошибка создания событий!" << endl;
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
        cout << "Ошибка запуска процесса Sender!" << endl;
        return 1;
    }
    
    cout << "Ожидаем готовности процесса Sender..." << endl;
    WaitForSingleObject(hReadyEvent, INFINITE);
    cout << "Процесс Sender готов к работе!" << endl;
    
    string command;
    while (true) {
        cout << "\nВведите команду (read - чтение сообщения, exit - выход): ";
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
                        cout << "Получено сообщение: " << msg.text << endl;
                        msg.clear();
                        FileUtils::writeMessage(hFile, msg);
                        EventUtils::signalEvent(hCanWriteEvent);
                        EventUtils::resetEvent(hCanReadEvent);
                    }
                    else {
                        cout << "Нет сообщений для чтения. Ожидание..." << endl;
                    }
                }
                CloseHandle(hFile);
            }
        }
        else {
            cout << "Неизвестная команда!" << endl;
        }
    }
    
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hReadyEvent);
    CloseHandle(hCanWriteEvent);
    CloseHandle(hCanReadEvent);
    cout << "Процесс Receiver завершен." << endl;
    
    return 0;
}
