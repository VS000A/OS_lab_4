#include <windows.h>
#include <iostream>
#include <string>
#include "message.h"
using namespace std;

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc < 2) {
        cout << "Èñïîëüçîâàíèå: sender.exe <èìÿ_ôàéëà>" << endl;
        return 1;
    }

    string fileName = argv[1];

    HANDLE hReadyEvent = EventUtils::openNamedEvent(SENDER_READY_EVENT);
    HANDLE hCanWriteEvent = EventUtils::openNamedEvent(CAN_WRITE_EVENT);
    HANDLE hCanReadEvent = EventUtils::openNamedEvent(CAN_READ_EVENT);

    if (!hReadyEvent || !hCanWriteEvent || !hCanReadEvent) {
        cout << "Îøèáêà îòêðûòèÿ ñîáûòèé!" << endl;
        return static_cast<int>(ErrorCode::EVENT_OPEN_ERROR);
    }

    cout << "Ïðîöåññ Sender çàïóùåí. Ôàéë: " << fileName << endl;

    EventUtils::signalEvent(hReadyEvent);
    cout << "Ñèãíàë ãîòîâíîñòè îòïðàâëåí Receiver'ó" << endl;

    string command;
    while (true) {
        cout << "\nÂâåäèòå êîìàíäó (send - îòïðàâèòü ñîîáùåíèå, exit - âûõîä): ";
        cin >> command;

        if (command == "exit") {
            break;
        }
        else if (command == "send") {
            EventUtils::waitForEvent(hCanWriteEvent);

            string messageText;
            cout << "Ââåäèòå òåêñò ñîîáùåíèÿ (ìàêñ. 19 ñèìâîëîâ): ";
            cin.ignore(); 
            getline(cin, messageText);

            if (messageText.length() >= MAX_MESSAGE_LENGTH) {
                messageText = messageText.substr(0, MAX_MESSAGE_LENGTH - 1);
                cout << "Ñîîáùåíèå îáðåçàíî äî: " << messageText << endl;
            }

            HANDLE hFile = FileUtils::openMessageFile(fileName.c_str());

            if (hFile != INVALID_HANDLE_VALUE) {
                Message msg;

                if (FileUtils::readMessage(hFile, msg)) {
                    if (msg.isEmpty) {
                        msg.setText(messageText.c_str());
                        FileUtils::writeMessage(hFile, msg);

                        cout << "Ñîîáùåíèå îòïðàâëåíî: " << messageText << endl;

                        EventUtils::resetEvent(hCanWriteEvent);
                        EventUtils::signalEvent(hCanReadEvent);
                    }
                    else {
                        cout << "Ôàéë çàíÿò, îæèäàíèå îñâîáîæäåíèÿ..." << endl;
                    }
                }
                CloseHandle(hFile);
            }
            else {
                cout << "Îøèáêà îòêðûòèÿ ôàéëà!" << endl;
            }
        }
        else {
            cout << "Íåèçâåñòíàÿ êîìàíäà!" << endl;
        }
    }

    CloseHandle(hReadyEvent);
    CloseHandle(hCanWriteEvent);
    CloseHandle(hCanReadEvent);

    cout << "Ïðîöåññ Sender çàâåðøåí." << endl;
    return 0;
}
