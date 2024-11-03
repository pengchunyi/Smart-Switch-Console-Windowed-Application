//
//
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
//#include <cpprest/http_client.h>
//#include <cpprest/json.h>
//#include <regex>
//
//using namespace web;
//using namespace web::http;
//using namespace web::http::client;
//
//// Schedule ���c
//struct Schedule {
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
//
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

//// �ե� API �ñN�}�����ɶ��K�[��Ƶ{
//void callScheduleAPI() {
//    uri api_uri(U("http://CNDGNMESIMQ001.delta.corp:10101/schedule/queryScheduleMo"));
//    http_client client(api_uri);
//
//    http_request request(methods::POST);
//    request.headers().add(U("Content-Type"), U("application/json"));
//    request.headers().add(U("tokenID"), U("107574C7FAAA0EA5E0630ECA940AF1FE"));
//
//    json::value body;
//    body[U("orgInfo")][U("plant")] = json::value::string(U("WJ3"));
//    body[U("clientInfo")][U("user")] = json::value::string(U("55319938"));
//    body[U("clientInfo")][U("pcName")] = json::value::string(U("CNWJxxxx"));
//    body[U("clientInfo")][U("program")] = json::value::string(U("test"));
//    body[U("clientInfo")][U("programVer")] = json::value::string(U("1.2"));
//    body[U("parameters")][U("prodArea")] = json::value::string(U("WJ3_SMT"));
//    body[U("parameters")][U("lineName")] = json::value::string(U("SMT-21"));
//
//    request.set_body(body);
//
//    client.request(request).then([](http_response response) {
//        if (response.status_code() == status_codes::OK) {
//            response.extract_string().then([](utility::string_t responseString) {
//                json::value jsonResponse = json::value::parse(responseString);
//
//                if (jsonResponse.has_array_field(U("responseData"))) {
//                    auto data = jsonResponse[U("responseData")].as_array();
//                    for (const auto& item : data) {
//                        if (item.has_field(U("planStartDate")) && item.has_field(U("planCloseDate"))) {
//                            auto startDateStr = item.at(U("planStartDate")).as_string();
//                            auto closeDateStr = item.at(U("planCloseDate")).as_string();
//
//                            // �ѪR�ɶ��å[�J�Ƶ{�C��
//                            int startHour, startMin, startSec, closeHour, closeMin, closeSec;
//                            swscanf_s(startDateStr.c_str(), L"%d-%d-%dT%d:%d:%d", &startHour, &startMin, &startSec);
//                            swscanf_s(closeDateStr.c_str(), L"%d-%d-%dT%d:%d:%d", &closeHour, &closeMin, &closeSec);
//
//                            scheduleList.push_back({ startHour, startMin, startSec, 1 });
//                            scheduleList.push_back({ closeHour, closeMin, closeSec, 0 });
//                        }
//                    }
//                }
//                }).wait();
//        }
//        }).wait();
//}
//
//// �ק�᪺�ɶ��ѪR�{���X
//void callScheduleAPI() {
//    uri api_uri(U("http://CNDGNMESIMQ001.delta.corp:10101/schedule/queryScheduleMo"));
//    http_client client(api_uri);
//
//    http_request request(methods::POST);
//    request.headers().add(U("Content-Type"), U("application/json"));
//    request.headers().add(U("tokenID"), U("107574C7FAAA0EA5E0630ECA940AF1FE"));
//
//    json::value body;
//    body[U("orgInfo")][U("plant")] = json::value::string(U("WJ3"));
//    body[U("clientInfo")][U("user")] = json::value::string(U("55319938"));
//    body[U("clientInfo")][U("pcName")] = json::value::string(U("CNWJxxxx"));
//    body[U("clientInfo")][U("program")] = json::value::string(U("test"));
//    body[U("clientInfo")][U("programVer")] = json::value::string(U("1.2"));
//    body[U("parameters")][U("prodArea")] = json::value::string(U("WJ3_SMT"));
//    body[U("parameters")][U("lineName")] = json::value::string(U("SMT-21"));
//
//    request.set_body(body);
//
//    client.request(request).then([](http_response response) {
//        if (response.status_code() == status_codes::OK) {
//            response.extract_string().then([](utility::string_t responseString) {
//                json::value jsonResponse = json::value::parse(responseString);
//
//                if (jsonResponse.has_array_field(U("responseData"))) {
//                    auto data = jsonResponse[U("responseData")].as_array();
//                    std::wregex timeRegex(L"(\\d+)-(\\d+)-(\\d+)T(\\d+):(\\d+):(\\d+)");
//
//                    for (const auto& item : data) {
//                        if (item.has_field(U("planStartDate")) && item.has_field(U("planCloseDate"))) {
//                            std::wstring startDateStr = item.at(U("planStartDate")).as_string();
//                            std::wstring closeDateStr = item.at(U("planCloseDate")).as_string();
//
//                            std::wsmatch match;
//                            int startHour = 0, startMin = 0, startSec = 0;
//                            int closeHour = 0, closeMin = 0, closeSec = 0;
//
//                            // �ϥΥ��h��F���ˬd�M�����ɶ�
//                            if (std::regex_search(startDateStr, match, timeRegex) && match.size() == 7) {
//                                startHour = std::stoi(match[4].str());
//                                startMin = std::stoi(match[5].str());
//                                startSec = std::stoi(match[6].str());
//                                scheduleList.push_back({ startHour, startMin, startSec, 1 });
//                            }
//                            else {
//                                std::wcerr << L"Invalid planStartDate format: " << startDateStr << std::endl;
//                            }
//
//                            if (std::regex_search(closeDateStr, match, timeRegex) && match.size() == 7) {
//                                closeHour = std::stoi(match[4].str());
//                                closeMin = std::stoi(match[5].str());
//                                closeSec = std::stoi(match[6].str());
//                                scheduleList.push_back({ closeHour, closeMin, closeSec, 0 });
//                            }
//                            else {
//                                std::wcerr << L"Invalid planCloseDate format: " << closeDateStr << std::endl;
//                            }
//                        }
//                    }
//                }
//                }).wait();
//        }
//        }).wait();
//}
//
//// �}��/�����Ŷ}
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
//// �Ƶ{�����
//void RunSchedule() {
//    while (true) {
//        if (isScheduleRunning) {
//            time_t now = time(0);
//            tm localTime;
//            localtime_s(&localTime, &now);
//
//            for (const auto& schedule : scheduleList) {
//                if (localTime.tm_hour == schedule.hour &&
//                    localTime.tm_min == schedule.minute &&
//                    localTime.tm_sec == schedule.second) {
//                    if (schedule.action == 1) {
//                        OpenBreaker();
//                    }
//                    else {
//                        CloseBreaker();
//                    }
//                    std::this_thread::sleep_for(std::chrono::seconds(1));
//                    break;
//                }
//            }
//        }
//        std::this_thread::sleep_for(std::chrono::seconds(1));
//    }
//}
//
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//    switch (uMsg) {
//    case WM_CREATE:
//        for (int i = 0; i < 40; i++) {
//            HWND label = CreateWindow(L"STATIC", parameterNames[i], WS_VISIBLE | WS_CHILD | SS_LEFT, 10, 30 + i * 20, 200, 20, hwnd, (HMENU)(i * 2), NULL, NULL);
//            hwndParameters[i] = CreateWindow(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | SS_LEFT, 220, 30 + i * 20, 150, 20, hwnd, (HMENU)(i * 2 + 1), NULL, NULL);
//        }
//
//        hOpenButton = CreateWindowW(L"BUTTON", L"�}�ҪŶ}", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 150, 30, hwnd, (HMENU)1, NULL, NULL);
//        hCloseButton = CreateWindowW(L"BUTTON", L"�����Ŷ}", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 170, 10, 150, 30, hwnd, (HMENU)2, NULL, NULL);
//        hScheduleButton = CreateWindowW(L"BUTTON", L"��ܥͲ��Ƶ{", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 330, 10, 150, 30, hwnd, (HMENU)3, NULL, NULL);
//        hAutoScheduleButton = CreateWindowW(L"BUTTON", L"�̷ӥͲ��Ƶ{", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 500, 10, 150, 30, hwnd, (HMENU)4, NULL, NULL);
//        hTimeDisplay = CreateWindowW(L"STATIC", L"00:00:00", WS_VISIBLE | WS_CHILD | SS_LEFT, 670, 10, 100, 30, hwnd, NULL, NULL, NULL);
//        SetTimer(hwnd, 1, 1000, NULL);
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
//        else if (LOWORD(wParam) == 4) {
//            isScheduleRunning = !isScheduleRunning;
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
//    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Modbus Control", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);
//    if (!hwnd) return 0;
//
//    ShowWindow(hwnd, SW_MAXIMIZE);
//
//    // �ե� API ����}���ɶ��ñN��[�J�Ƶ{
//    callScheduleAPI();
//
//    std::thread scheduleThread(RunSchedule);
//    scheduleThread.detach();
//
//    MSG msg = {};
//    while (GetMessage(&msg, NULL, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    return 0;
//}
