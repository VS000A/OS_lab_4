#include <windows.h>
#include <iostream>
#include <string>
#include "message.h"

using namespace std;

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc < 2) {
        cout << "�������������: sender.exe <���_�����>" << endl;
        return 1;
    }

    string fileName = argv[1];

    HANDLE hReadyEvent = EventUtils::openNamedEvent(SENDER_READY_EVENT);
    HANDLE hCanWriteEvent = EventUtils::openNamedEvent(CAN_WRITE_EVENT);
    HANDLE hCanReadEvent = EventUtils::openNamedEvent(CAN_READ_EVENT);

    if (!hReadyEvent || !hCanWriteEvent || !hCanReadEvent) {
        cout << "������ �������� �������!" << endl;
        return static_cast<int>(ErrorCode::EVENT_OPEN_ERROR);
    }

    cout << "������� Sender �������. ����: " << fileName << endl;

    EventUtils::signalEvent(hReadyEvent);
    cout << "������ ���������� ��������� Receiver'�" << endl;

    string command;
    while (true) {
        cout << "\n������� ������� (send - ��������� ���������, exit - �����): ";
        cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "send") {
            EventUtils::waitForEvent(hCanWriteEvent);

            string messageText;
            cout << "������� ����� ��������� (����. 19 ��������): ";
            cin.ignore(); 
            getline(cin, messageText);

            if (messageText.length() >= MAX_MESSAGE_LENGTH) {
                messageText = messageText.substr(0, MAX_MESSAGE_LENGTH - 1);
                cout << "��������� �������� ��: " << messageText << endl;
            }

            HANDLE hFile = FileUtils::openMessageFile(fileName.c_str());

            if (hFile != INVALID_HANDLE_VALUE) {
                Message msg;

                if (FileUtils::readMessage(hFile, msg)) {
                    if (msg.isEmpty) {
                        msg.setText(messageText.c_str());
                        FileUtils::writeMessage(hFile, msg);

                        cout << "��������� ����������: " << messageText << endl;

                        EventUtils::resetEvent(hCanWriteEvent);
                        EventUtils::signalEvent(hCanReadEvent);
                    }
                    else {
                        cout << "���� �����, �������� ������������..." << endl;
                    }
                }
                CloseHandle(hFile);
            }
            else {
                cout << "������ �������� �����!" << endl;
            }
        }
        else {
            cout << "����������� �������!" << endl;
        }
    }

    CloseHandle(hReadyEvent);
    CloseHandle(hCanWriteEvent);
    CloseHandle(hCanReadEvent);

    cout << "������� Sender ��������." << endl;
    return 0;
}