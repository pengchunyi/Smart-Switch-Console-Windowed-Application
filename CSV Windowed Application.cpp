#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <atomic>
#include <fstream>
#include <sstream>

struct Schedule {
    //int year;
    //int month;
    //int day;
    int hour;
    int minute;
    int second;
    int action; // 1: �}��, 0: ����
};

std::vector<Schedule> scheduleList;
HANDLE hSerial;
HWND hwndParameters[40];
HWND hOpenButton, hCloseButton, hScheduleButton, hAutoScheduleButton, hTimeDisplay;
HWND hScheduleList, hScheduleWindow;
HWND hManualModeButton;
std::atomic<bool> isScheduleRunning{ false };
std::atomic<bool> shouldStopManualControl{ false };

const wchar_t* parameterNames[] = {
    L"��e���A", L"��e���A", L"�|�q�q�y", L"A�۷ū�", L"B�۷ū�", L"C�۷ū�", L"N�۷ū�",
    L"A�۹q��", L"B�۹q��", L"C�۹q��", L"A�۹q�y", L"B�۹q�y", L"C�۹q�y",
    L"A�ۥ\�v�]��", L"B�ۥ\�v�]��", L"C�ۥ\�v�]��", L"A�ۦ��\�\�v",
    L"B�ۦ��\�\�v", L"C�ۦ��\�\�v", L"A�۵L�\�\�v", L"B�۵L�\�\�v",
    L"C�۵L�\�\�v", L"�X�h����", L"�q�ప�r�`", L"�q��C�r�`", L"�}�����A",
    L"A�۵��b�\�v", L"B�۵��b�\�v", L"C�۵��b�\�v", L"�`���b�\�v",
    L"�`���\�\�v", L"�`�L�\�\�v", L"�X�ۥ\�v�]��", L"�u�W�v", L"�]������",
    L"���v�|�q��", L"���vA�۹q�y", L"���vB�۹q�y", L"���vC�۹q�y", L"�u���C��аO"
};

// ��l�Ʀ�f
BOOL InitSerialPort() {
    hSerial = CreateFile(L"COM6", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        MessageBox(NULL, L"��f�}�ҥ���", L"���~", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        MessageBox(NULL, L"�L�k�����f���A", L"���~", MB_OK | MB_ICONERROR);
        return FALSE;
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        MessageBox(NULL, L"�L�k�]�w��f�Ѽ�", L"���~", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    return TRUE;
}


// �B�z Modbus �^���ƾڨ����
void ProcessModbusResponse(const char* response, DWORD length) {
    int startIndex = 3;
    int endIndex = length - 2;

    for (int i = 0, index = startIndex; i < 40 && index < endIndex; i++, index += 2) {
        int value = (response[index] << 8) | (response[index + 1] & 0xFF);
        if (i == 7 || i == 8 || i == 9) { // �S�w�Ѽƻݭn�վ�ƭ����
            value /= 10;
        }
        std::wstring valueText = std::to_wstring(value);
        SetWindowText(hwndParameters[i], valueText.c_str());
    }
}

// �o�e Modbus �ШD�A��Ū���^��
void SendModbusRequestOnce(HWND hwnd) {
    DWORD bytesWritten, bytesRead;
    char modbusRequest[] = { 0x06, 0x03, 0x00, 0x00, 0x00, 0x30, 0x44, 0x69 };
    char modbusResponse[256] = { 0 };

    if (!WriteFile(hSerial, modbusRequest, sizeof(modbusRequest), &bytesWritten, NULL)) {
        MessageBox(hwnd, L"�L�k�o�e Modbus �ШD", L"���~", MB_OK);
        return;
    }

    if (ReadFile(hSerial, modbusResponse, sizeof(modbusResponse), &bytesRead, NULL) && bytesRead > 0) {
        ProcessModbusResponse(modbusResponse, bytesRead);
    }
}



// Ū�� CSV �Ƶ{
void ReadSchedule(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string timeStr, actionStr;
        std::getline(ss, timeStr, ',');
        std::getline(ss, actionStr);

        int hour, minute, second, action;
        if (!timeStr.empty() && !actionStr.empty()) {
            sscanf_s(timeStr.c_str(), "%d:%d:%d", &hour, &minute, &second);
            action = std::stoi(actionStr);
            scheduleList.push_back({ hour, minute, second, action });
        }
    }
}

// ��ܱƵ{���u�X����
void ShowScheduleWindow(HINSTANCE hInstance) {
    if (!hScheduleWindow) {
        const wchar_t SCHEDULE_CLASS_NAME[] = L"ScheduleWindowClass";
        WNDCLASS wc = {};
        wc.lpfnWndProc = DefWindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = SCHEDULE_CLASS_NAME;
        RegisterClass(&wc);

        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"�Ƶ{����", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            100, 100, 400, 600, NULL, NULL, hInstance, NULL);

        hScheduleList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY,
            10, 10, 360, 540, hScheduleWindow, NULL, hInstance, NULL);

        SendMessage(hScheduleList, LB_RESETCONTENT, 0, 0);

        for (const auto& schedule : scheduleList) {
            std::wstring item = std::to_wstring(schedule.hour) + L":" + std::to_wstring(schedule.minute) + L":" +
                std::to_wstring(schedule.second) + L" - " + (schedule.action == 1 ? L"�}��" : L"����");
            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
        }
    }
}

void OpenBreaker() {
    char openCommand[] = { 0x06, 0x06, 0x00, 0x31, 0x55, 0x88, 0xE6, 0x84 };
    DWORD bytesWritten;
    WriteFile(hSerial, openCommand, sizeof(openCommand), &bytesWritten, NULL);
}

void CloseBreaker() {
    char closeCommand[] = { 0x06, 0x06, 0x00, 0x31, 0x55, 0x66, 0x66, 0xC8 };
    DWORD bytesWritten;
    WriteFile(hSerial, closeCommand, sizeof(closeCommand), &bytesWritten, NULL);
}

// �Ƶ{�����
void RunSchedule() {
    while (true) {
        if (isScheduleRunning) {
            time_t now = time(0);
            tm localTime;
            localtime_s(&localTime, &now);

            for (const auto& schedule : scheduleList) {
                if (
                    localTime.tm_hour == schedule.hour &&
                    localTime.tm_min == schedule.minute &&
                    localTime.tm_sec == schedule.second) {
                    if (schedule.action == 1) {
                        OpenBreaker();
                    }
                    else {
                        CloseBreaker();
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    break;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

// �վ�Ѽ���ܦ�m�A�N�ɶ���ܩ�b�̥���
void AdjustControlPositions(HWND hwnd) {
    RECT rect;
    GetClientRect(hwnd, &rect);

    int labelWidth = 200;
    int valueWidth = 150;
    int rowHeight = 25;
    int xOffset = 20;
    int yOffset = 60;

    // �N�ɶ���ܩ�b�̥���
    MoveWindow(hTimeDisplay, xOffset, 20, 100, 30, TRUE);

    // �}�l�\���L���s
    int xPos = xOffset + 120; // �վ� xPos �_�l�ȡA�N�ɶ��d�X�����Ŷ�
    MoveWindow(hOpenButton, xPos, 20, 150, 30, TRUE);
    xPos += 160;
    MoveWindow(hCloseButton, xPos, 20, 150, 30, TRUE);
    xPos += 160;
    MoveWindow(hScheduleButton, xPos, 20, 150, 30, TRUE);
    xPos += 160;
    MoveWindow(hAutoScheduleButton, xPos, 20, 150, 30, TRUE);
    xPos += 160;
    MoveWindow(hManualModeButton, xPos, 20, 150, 30, TRUE);

    // �\��ѼƼ��ҩM��
    int xBaseOffset = xOffset;
    int yBaseOffset = yOffset + 40;
    for (int i = 0; i < 40; i++) {
        int col = i / 20;
        int row = i % 20;
        xPos = xBaseOffset + col * (labelWidth + valueWidth + 40);
        int yPos = yBaseOffset + row * rowHeight;

        MoveWindow(GetDlgItem(hwnd, i * 2), xPos, yPos, labelWidth, rowHeight, TRUE);
        MoveWindow(GetDlgItem(hwnd, i * 2 + 1), xPos + labelWidth + 10, yPos, valueWidth, rowHeight, TRUE);
    }
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        for (int i = 0; i < 40; i++) {
            HWND label = CreateWindow(L"STATIC", parameterNames[i],
                WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 30 + i * 20, 200, 20, hwnd, (HMENU)(i * 2), NULL, NULL);
            hwndParameters[i] = CreateWindow(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | SS_LEFT, 220, 30 + i * 20, 150, 20, hwnd, (HMENU)(i * 2 + 1), NULL, NULL);
        }

        hOpenButton = CreateWindowW(L"BUTTON", L"�}�ҪŶ}", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 150, 30, hwnd, (HMENU)1, NULL, NULL);
        hCloseButton = CreateWindowW(L"BUTTON", L"�����Ŷ}", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 170, 10, 150, 30, hwnd, (HMENU)2, NULL, NULL);
        hScheduleButton = CreateWindowW(L"BUTTON", L"��ܥͲ��Ƶ{", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 330, 10, 150, 30, hwnd, (HMENU)3, NULL, NULL);
        hAutoScheduleButton = CreateWindowW(L"BUTTON", L"�̷ӥͲ��Ƶ{", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 500, 10, 150, 30, hwnd, (HMENU)4, NULL, NULL);
        hTimeDisplay = CreateWindowW(L"STATIC", L"00:00:00", WS_VISIBLE | WS_CHILD | SS_LEFT, 800, 0, 100, 30, hwnd, NULL, NULL, NULL);
        hManualModeButton= CreateWindowW(L"BUTTON", L"��ʼҦ�", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 670, 10, 150, 30, hwnd, (HMENU)5, NULL, NULL);
        SetTimer(hwnd, 1, 1000, NULL);
        break;

    case WM_SIZE:
        AdjustControlPositions(hwnd);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            isScheduleRunning = false;
            OpenBreaker();
        }
        else if (LOWORD(wParam) == 2) {
            isScheduleRunning = false;
            CloseBreaker();
        }
        else if (LOWORD(wParam) == 3) {
            ShowScheduleWindow(GetModuleHandle(NULL));
        }
        else if (LOWORD(wParam) == 4) {
            isScheduleRunning = !isScheduleRunning;
        }
        else if (LOWORD(wParam) == 5) {
            isScheduleRunning = false;
            MessageBox(hwnd, L"�w�������ʡA�Ƶ{�N����B��", L"��ʼҦ�", MB_OK | MB_ICONINFORMATION);
        }
        break;

    case WM_TIMER:
        SendModbusRequestOnce(hwnd);

        SYSTEMTIME st;
        GetLocalTime(&st);
        wchar_t timeBuffer[9];
        swprintf_s(timeBuffer, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
        SetWindowText(hTimeDisplay, timeBuffer);
        break;

    case WM_DESTROY:
        CloseHandle(hSerial);
        PostQuitMessage(0);
        break;



    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    if (!InitSerialPort()) return 1;

    const wchar_t CLASS_NAME[] = L"ModbusViewer";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"pengchunyi", WS_OVERLAPPEDWINDOW, 0, 0, 1200, 800, NULL, NULL, hInstance, NULL);
    if (!hwnd) return 0;

    ShowWindow(hwnd, SW_MAXIMIZE);

    ReadSchedule("D:\\PRODUCTION_SCHEDULE_SAMPLE\\time_series.csv");

    std::thread scheduleThread(RunSchedule);
    scheduleThread.detach();

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


//
//#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
//#define _SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS
//#include <windows.h>
//#include <iostream>
//#include <string>
//#include <thread>
//#include <chrono>
//#include <vector>
//#include <atomic>
//#include <fstream>
//#include <sstream>
//
//struct Schedule {
//    int year;
//    int month;
//    int day;
//    int hour;
//    int minute;
//    int second;
//    int action; // 1: �}��, 0: ����
//};
//
//std::vector<Schedule> scheduleList;
//HANDLE hSerial;
//HWND hwndParameters[40];
//HWND hOpenButton, hCloseButton, hScheduleButton, hAutoScheduleButton, hTimeDisplay;
//HWND hScheduleList, hScheduleWindow;
//HWND hManualModeButton;
//std::atomic<bool> isScheduleRunning{ false };
//std::atomic<bool> shouldStopManualControl{ false };
//
//const wchar_t* parameterNames[] = {
//    L"��e���A", L"��e���A", L"�|�q�q�y", L"A�۷ū�", L"B�۷ū�", L"C�۷ū�", L"N�۷ū�",
//    L"A�۹q��", L"B�۹q��", L"C�۹q��", L"A�۹q�y", L"B�۹q�y", L"C�۹q�y",
//    L"A�ۥ\�v�]��", L"B�ۥ\�v�]��", L"C�ۥ\�v�]��", L"A�ۦ��\�\�v",
//    L"B�ۦ��\�\�v", L"C�ۦ��\�\�v", L"A�۵L�\�\�v", L"B�۵L�\�\�v",
//    L"C�۵L�\�\�v", L"�X�h����", L"�q�ప�r�`", L"�q��C�r�`", L"�}�����A",
//    L"A�۵��b�\�v", L"B�۵��b�\�v", L"C�۵��b�\�v", L"�`���b�\�v",
//    L"�`���\�\�v", L"�`�L�\�\�v", L"�X�ۥ\�v�]��", L"�u�W�v", L"�]������",
//    L"���v�|�q��", L"���vA�۹q�y", L"���vB�۹q�y", L"���vC�۹q�y", L"�u���C��аO"
//};
//// �B�z Modbus �^���ƾڨ����
//void ProcessModbusResponse(const char* response, DWORD length) {
//    int startIndex = 3;
//    int endIndex = length - 2;
//
//    for (int i = 0, index = startIndex; i < 40 && index < endIndex; i++, index += 2) {
//        int value = (response[index] << 8) | (response[index + 1] & 0xFF);
//        if (i == 7 || i == 8 || i == 9) { // �S�w�Ѽƻݭn�վ�ƭ����
//            value /= 10;
//        }
//        std::wstring valueText = std::to_wstring(value);
//        SetWindowText(hwndParameters[i], valueText.c_str());
//    }
//}
//// ��l�Ʀ�f
//BOOL InitSerialPort() {
//    hSerial = CreateFile(L"COM6", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
//    if (hSerial == INVALID_HANDLE_VALUE) {
//        MessageBox(NULL, L"��f�}�ҥ���", L"���~", MB_OK | MB_ICONERROR);
//        return FALSE;
//    }
//    DCB dcbSerialParams = { 0 };
//    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
//
//    if (!GetCommState(hSerial, &dcbSerialParams)) {
//        MessageBox(NULL, L"�L�k�����f���A", L"���~", MB_OK | MB_ICONERROR);
//        return FALSE;
//    }
//    dcbSerialParams.BaudRate = CBR_9600;
//    dcbSerialParams.ByteSize = 8;
//    dcbSerialParams.StopBits = ONESTOPBIT;
//    dcbSerialParams.Parity = NOPARITY;
//
//    if (!SetCommState(hSerial, &dcbSerialParams)) {
//        MessageBox(NULL, L"�L�k�]�w��f�Ѽ�", L"���~", MB_OK | MB_ICONERROR);
//        return FALSE;
//    }
//
//    return TRUE;
//}
//
//// Ū�� CSV �Ƶ{
//void ReadSchedule(const std::string& filename) {
//    std::ifstream file(filename);
//    std::string line;
//    while (std::getline(file, line)) {
//        std::istringstream ss(line);
//        std::string dateTimeStr, actionStr;
//        std::getline(ss, dateTimeStr, ',');
//        std::getline(ss, actionStr);
//
//        int year, month, day, hour, minute, second, action;
//        if (!dateTimeStr.empty() && !actionStr.empty()) {
//            sscanf_s(dateTimeStr.c_str(), "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
//            action = std::stoi(actionStr);
//            scheduleList.push_back({ year, month, day, hour, minute, second, action });
//        }
//    }
//}
//
//void OpenBreaker() {
//    char openCommand[] = { 0x06, 0x06, 0x00, 0x31, 0x55, 0x88, 0xE6, 0x84 };
//    DWORD bytesWritten;
//    WriteFile(hSerial, openCommand, sizeof(openCommand), &bytesWritten, NULL);
//}
//
//void CloseBreaker() {
//    char closeCommand[] = { 0x06, 0x06, 0x00, 0x31, 0x55, 0x66, 0x66, 0xC8 };
//    DWORD bytesWritten;
//    WriteFile(hSerial, closeCommand, sizeof(closeCommand), &bytesWritten, NULL);
//}
//
//// �Ƶ{������A�ھڮɶ��i��}���ާ@
//void RunSchedule() {
//    while (true) {
//        if (isScheduleRunning) {
//            time_t now = time(0);
//            tm localTime;
//            localtime_s(&localTime, &now);
//
//            for (const auto& schedule : scheduleList) {
//                if (
//                    localTime.tm_year + 1900 == schedule.year && // �~��
//                    localTime.tm_mon + 1 == schedule.month &&    // ���
//                    localTime.tm_mday == schedule.day &&         // ���
//                    localTime.tm_hour == schedule.hour &&        // �p��
//                    localTime.tm_min == schedule.minute &&       // ����
//                    localTime.tm_sec == schedule.second) {       // ����
//                    if (schedule.action == 1) {
//                        OpenBreaker();
//                    }
//                    else {
//                        CloseBreaker();
//                    }
//                    std::this_thread::sleep_for(std::chrono::seconds(1)); // ����ư���
//                    break;
//                }
//            }
//        }
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//    }
//}
////
////// ��ܱƵ{���u�X����
////void ShowScheduleWindow(HINSTANCE hInstance) {
////    if (!hScheduleWindow) {
////        const wchar_t SCHEDULE_CLASS_NAME[] = L"ScheduleWindowClass";
////        WNDCLASS wc = {};
////        wc.lpfnWndProc = DefWindowProc;
////        wc.hInstance = hInstance;
////        wc.lpszClassName = SCHEDULE_CLASS_NAME;
////        RegisterClass(&wc);
////
////        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"�Ƶ{����", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
////            100, 100, 400, 600, NULL, NULL, hInstance, NULL);
////
////        hScheduleList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY,
////            10, 10, 360, 540, hScheduleWindow, NULL, hInstance, NULL);
////
////        SendMessage(hScheduleList, LB_RESETCONTENT, 0, 0);
////
////        for (const auto& schedule : scheduleList) {
////            std::wstring item = std::to_wstring(schedule.year) + L"-" + std::to_wstring(schedule.month) + L"-" +
////                std::to_wstring(schedule.day) + L" " + std::to_wstring(schedule.hour) + L":" +
////                std::to_wstring(schedule.minute) + L":" + std::to_wstring(schedule.second) + L" - " +
////                (schedule.action == 1 ? L"�}��" : L"����");
////            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
////        }
////    }
////}
//
//void ShowScheduleWindow(HINSTANCE hInstance) {
//    if (!hScheduleWindow) {
//        const wchar_t SCHEDULE_CLASS_NAME[] = L"ScheduleWindowClass";
//        WNDCLASS wc = {};
//        wc.lpfnWndProc = DefWindowProc;
//        wc.hInstance = hInstance;
//        wc.lpszClassName = SCHEDULE_CLASS_NAME;
//        RegisterClass(&wc);
//
//        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"�Ͳ��Ƶ{", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
//            100, 100, 800, 600, NULL, NULL, hInstance, NULL);
//
//        hScheduleList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY,
//            10, 10, 780, 540, hScheduleWindow, NULL, hInstance, NULL);
//
//        SendMessage(hScheduleList, LB_RESETCONTENT, 0, 0);
//
//        const wchar_t* daysOfWeek[] = { L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday" };
//
//        for (const auto& schedule : scheduleList) {
//            std::wstring formattedDateTime;
//            int dayOfWeekIndex;
//
//            // �榡�Ƥ���M�ɶ�
//            std::tm timeinfo = {};
//            timeinfo.tm_year = schedule.year - 1900;  // tm_year�q1900�~�}�l�p��
//            timeinfo.tm_mon = schedule.month - 1;     // tm_mon�d��O0-11
//            timeinfo.tm_mday = schedule.day;
//            timeinfo.tm_hour = schedule.hour;
//            timeinfo.tm_min = schedule.minute;
//            timeinfo.tm_sec = schedule.second;
//            mktime(&timeinfo);  // ��stm_wday
//
//            dayOfWeekIndex = timeinfo.tm_wday;
//            formattedDateTime = std::to_wstring(schedule.year) + L"-" +
//                std::to_wstring(schedule.month) + L"-" +
//                std::to_wstring(schedule.day) + L" (" +
//                daysOfWeek[dayOfWeekIndex] + L") " +
//                std::to_wstring(schedule.hour) + L":" +
//                std::to_wstring(schedule.minute) + L":" +
//                std::to_wstring(schedule.second);
//
//            // �ھ� action �]�w��ܤ��e
//            std::wstring actionText = (schedule.action == 1) ? L"�}��" : L"����";
//            std::wstring item = formattedDateTime + L" - " + actionText;
//
//            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
//        }
//    }
//    else {
//        ShowWindow(hScheduleWindow, SW_SHOW);
//    }
//}
//
//
//// �վ�Ѽ���ܦ�m�A�N�ɶ���ܩ�b�̥���
//void AdjustControlPositions(HWND hwnd) {
//    RECT rect;
//    GetClientRect(hwnd, &rect);
//
//    int labelWidth = 200;
//    int valueWidth = 150;
//    int rowHeight = 25;
//    int xOffset = 20;
//    int yOffset = 60;
//
//    MoveWindow(hTimeDisplay, xOffset, 20, 100, 30, TRUE);
//
//    int xPos = xOffset + 120;
//    MoveWindow(hOpenButton, xPos, 20, 150, 30, TRUE);
//    xPos += 160;
//    MoveWindow(hCloseButton, xPos, 20, 150, 30, TRUE);
//    xPos += 160;
//    MoveWindow(hScheduleButton, xPos, 20, 150, 30, TRUE);
//    xPos += 160;
//    MoveWindow(hAutoScheduleButton, xPos, 20, 150, 30, TRUE);
//    xPos += 160;
//    MoveWindow(hManualModeButton, xPos, 20, 150, 30, TRUE);
//
//    // �\��ѼƼ��ҩM��
//    int xBaseOffset = xOffset;
//    int yBaseOffset = yOffset + 40;
//    for (int i = 0; i < 40; i++) {
//        int col = i / 20;
//        int row = i % 20;
//        xPos = xBaseOffset + col * (labelWidth + valueWidth + 40);
//        int yPos = yBaseOffset + row * rowHeight;
//
//        MoveWindow(GetDlgItem(hwnd, i * 2), xPos, yPos, labelWidth, rowHeight, TRUE);
//        MoveWindow(GetDlgItem(hwnd, i * 2 + 1), xPos + labelWidth + 10, yPos, valueWidth, rowHeight, TRUE);
//    }
//}
//
//
//// �o�e Modbus �ШD�A��Ū���^��
//void SendModbusRequestOnce(HWND hwnd) {
//    DWORD bytesWritten, bytesRead;
//    char modbusRequest[] = { 0x06, 0x03, 0x00, 0x00, 0x00, 0x30, 0x44, 0x69 };
//    char modbusResponse[256] = { 0 };
//
//    if (!WriteFile(hSerial, modbusRequest, sizeof(modbusRequest), &bytesWritten, NULL)) {
//        MessageBox(hwnd, L"�L�k�o�e Modbus �ШD", L"���~", MB_OK);
//        return;
//    }
//
//    if (ReadFile(hSerial, modbusResponse, sizeof(modbusResponse), &bytesRead, NULL) && bytesRead > 0) {
//        ProcessModbusResponse(modbusResponse, bytesRead);
//    }
//}
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//    switch (uMsg) {
//    case WM_CREATE:
//        for (int i = 0; i < 40; i++) {
//            HWND label = CreateWindow(L"STATIC", parameterNames[i],
//                WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 30 + i * 20, 200, 20, hwnd, (HMENU)(i * 2), NULL, NULL);
//            hwndParameters[i] = CreateWindow(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | SS_LEFT, 220, 30 + i * 20, 150, 20, hwnd, (HMENU)(i * 2 + 1), NULL, NULL);
//        }
//
//        hOpenButton = CreateWindowW(L"BUTTON", L"�}�ҪŶ}", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 150, 30, hwnd, (HMENU)1, NULL, NULL);
//        hCloseButton = CreateWindowW(L"BUTTON", L"�����Ŷ}", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 170, 10, 150, 30, hwnd, (HMENU)2, NULL, NULL);
//        hScheduleButton = CreateWindowW(L"BUTTON", L"��ܥͲ��Ƶ{", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 330, 10, 150, 30, hwnd, (HMENU)3, NULL, NULL);
//        hAutoScheduleButton = CreateWindowW(L"BUTTON", L"�̷ӥͲ��Ƶ{", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 500, 10, 150, 30, hwnd, (HMENU)4, NULL, NULL);
//        hTimeDisplay = CreateWindowW(L"STATIC", L"00:00:00", WS_VISIBLE | WS_CHILD | SS_LEFT, 800, 0, 100, 30, hwnd, NULL, NULL, NULL);
//        hManualModeButton = CreateWindowW(L"BUTTON", L"��ʼҦ�", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 670, 10, 150, 30, hwnd, (HMENU)5, NULL, NULL);
//        SetTimer(hwnd, 1, 1000, NULL);
//        break;
//
//    case WM_SIZE:
//        AdjustControlPositions(hwnd);
//        break;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == 1) {
//            isScheduleRunning = false;
//            OpenBreaker();
//        }
//        else if (LOWORD(wParam) == 2) {
//            isScheduleRunning = false;
//            CloseBreaker();
//        }
//        else if (LOWORD(wParam) == 3) {
//            ShowScheduleWindow(GetModuleHandle(NULL));
//        }
//        else if (LOWORD(wParam) == 4) {
//            isScheduleRunning = !isScheduleRunning;
//        }
//        else if (LOWORD(wParam) == 5) {
//            isScheduleRunning = false;
//            MessageBox(hwnd, L"�w�������ʡA�Ƶ{�N����B��", L"��ʼҦ�", MB_OK | MB_ICONINFORMATION);
//        }
//        break;
//
//    case WM_TIMER:
//        SendModbusRequestOnce(hwnd);
//
//        SYSTEMTIME st;
//        GetLocalTime(&st);
//        wchar_t timeBuffer[9];
//        swprintf_s(timeBuffer, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
//        SetWindowText(hTimeDisplay, timeBuffer);
//        break;
//
//    case WM_DESTROY:
//        CloseHandle(hSerial);
//        PostQuitMessage(0);
//        break;
//
//    default:
//        return DefWindowProc(hwnd, uMsg, wParam, lParam);
//    }
//    return 0;
//}
//
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
//    if (!InitSerialPort()) return 1;
//
//    const wchar_t CLASS_NAME[] = L"ModbusViewer";
//    WNDCLASS wc = {};
//    wc.lpfnWndProc = WindowProc;
//    wc.hInstance = hInstance;
//    wc.lpszClassName = CLASS_NAME;
//    RegisterClass(&wc);
//
//    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Modbus�������", WS_OVERLAPPEDWINDOW, 0, 0, 1200, 800, NULL, NULL, hInstance, NULL);
//    if (!hwnd) return 0;
//
//    ShowWindow(hwnd, SW_MAXIMIZE);
//
//    ReadSchedule("D:\\PRODUCTION_SCHEDULE_SAMPLE\\updated_time_series_alternate_v3.csv");
//
//    std::thread scheduleThread(RunSchedule);
//    scheduleThread.detach();
//
//    ShowWindow(hwnd, nCmdShow);
//
//    MSG msg = {};
//    while (GetMessage(&msg, NULL, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    return 0;
//}
