#include <windows.h>
#include <iostream>
#include <string>
#include "message.h"

using namespace std;

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc < 2) {
        cout << "Использование: sender.exe <имя_файла>" << endl;
        return 1;
    }

    string fileName = argv[1];

    HANDLE hReadyEvent = EventUtils::openNamedEvent(SENDER_READY_EVENT);
    HANDLE hCanWriteEvent = EventUtils::openNamedEvent(CAN_WRITE_EVENT);
    HANDLE hCanReadEvent = EventUtils::openNamedEvent(CAN_READ_EVENT);

    if (!hReadyEvent || !hCanWriteEvent || !hCanReadEvent) {
        cout << "Ошибка открытия событий!" << endl;
        return static_cast<int>(ErrorCode::EVENT_OPEN_ERROR);
    }

    cout << "Процесс Sender запущен. Файл: " << fileName << endl;

    EventUtils::signalEvent(hReadyEvent);
    cout << "Сигнал готовности отправлен Receiver'у" << endl;

    string command;
    while (true) {
        cout << "\nВведите команду (send - отправить сообщение, exit - выход): ";
        cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "send") {
            EventUtils::waitForEvent(hCanWriteEvent);

            string messageText;
            cout << "Введите текст сообщения (макс. 19 символов): ";
            cin.ignore(); 
            getline(cin, messageText);

            if (messageText.length() >= MAX_MESSAGE_LENGTH) {
                messageText = messageText.substr(0, MAX_MESSAGE_LENGTH - 1);
                cout << "Сообщение обрезано до: " << messageText << endl;
            }

            HANDLE hFile = FileUtils::openMessageFile(fileName.c_str());

            if (hFile != INVALID_HANDLE_VALUE) {
                Message msg;

                if (FileUtils::readMessage(hFile, msg)) {
                    if (msg.isEmpty) {
                        msg.setText(messageText.c_str());
                        FileUtils::writeMessage(hFile, msg);

                        cout << "Сообщение отправлено: " << messageText << endl;

                        EventUtils::resetEvent(hCanWriteEvent);
                        EventUtils::signalEvent(hCanReadEvent);
                    }
                    else {
                        cout << "Файл занят, ожидание освобождения..." << endl;
                    }
                }
                CloseHandle(hFile);
            }
            else {
                cout << "Ошибка открытия файла!" << endl;
            }
        }
        else {
            cout << "Неизвестная команда!" << endl;
        }
    }

    CloseHandle(hReadyEvent);
    CloseHandle(hCanWriteEvent);
    CloseHandle(hCanReadEvent);

    cout << "Процесс Sender завершен." << endl;
    return 0;
}