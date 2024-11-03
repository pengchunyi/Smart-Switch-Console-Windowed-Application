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
////// Schedule ���c
////struct Schedule {
////    int hour;
////    int minute;
////    int second;
////    int action; // 1: �}��, 0: ����
////};
//struct Schedule {
//    int hour;
//    int minute;
//    int second;
//    int action; // 1: �}��, 0: ����
//    std::wstring planStartDate; // �K�[�r�q�H�s�x�}�l����ɶ�
//    std::wstring planCloseDate; // �K�[�r�q�H�s�x��������ɶ�
//};
//
//
//// ����ŧi�P����r�}�C�A�קK�b switch-case ����l�ƳQ���L
//const wchar_t* daysOfWeek[] = { L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday" };
//wchar_t previousTimeBuffer[64] = L""; // �W����ܪ��ɶ�
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
////
////// �ק�᪺�ɶ��ѪR�{���X
////void callScheduleAPI() {
////    uri api_uri(U("http://CNDGNMESIMQ001.delta.corp:10101/schedule/queryScheduleMo"));
////    http_client client(api_uri);
////
////    http_request request(methods::POST);
////    request.headers().add(U("Content-Type"), U("application/json"));
////    request.headers().add(U("tokenID"), U("107574C7FAAA0EA5E0630ECA940AF1FE"));
////
////    json::value body;
////    body[U("orgInfo")][U("plant")] = json::value::string(U("WJ3"));
////    body[U("clientInfo")][U("user")] = json::value::string(U("55319938"));
////    body[U("clientInfo")][U("pcName")] = json::value::string(U("CNWJxxxx"));
////    body[U("clientInfo")][U("program")] = json::value::string(U("test"));
////    body[U("clientInfo")][U("programVer")] = json::value::string(U("1.2"));
////    body[U("parameters")][U("prodArea")] = json::value::string(U("WJ3_SMT"));
////    body[U("parameters")][U("lineName")] = json::value::string(U("SMT-21"));
////
////    request.set_body(body);
////
////    client.request(request).then([](http_response response) {
////        if (response.status_code() == status_codes::OK) {
////            response.extract_string().then([](utility::string_t responseString) {
////                json::value jsonResponse = json::value::parse(responseString);
////
////                if (jsonResponse.has_array_field(U("responseData"))) {
////                    auto data = jsonResponse[U("responseData")].as_array();
////                    std::wregex timeRegex(L"(\\d+)-(\\d+)-(\\d+)T(\\d+):(\\d+):(\\d+)");
////
////                    for (const auto& item : data) {
////                        if (item.has_field(U("planStartDate")) && item.has_field(U("planCloseDate"))) {
////                            std::wstring startDateStr = item.at(U("planStartDate")).as_string();
////                            std::wstring closeDateStr = item.at(U("planCloseDate")).as_string();
////
////                            std::wsmatch match;
////                            int startHour = 0, startMin = 0, startSec = 0;
////                            int closeHour = 0, closeMin = 0, closeSec = 0;
////
////                            // �ϥΥ��h��F���ˬd�M�����ɶ�
////                            if (std::regex_search(startDateStr, match, timeRegex) && match.size() == 7) {
////                                startHour = std::stoi(match[4].str());
////                                startMin = std::stoi(match[5].str());
////                                startSec = std::stoi(match[6].str());
////                                scheduleList.push_back({ startHour, startMin, startSec, 1 });
////                            }
////
////                            if (std::regex_search(closeDateStr, match, timeRegex) && match.size() == 7) {
////                                closeHour = std::stoi(match[4].str());
////                                closeMin = std::stoi(match[5].str());
////                                closeSec = std::stoi(match[6].str());
////                                scheduleList.push_back({ closeHour, closeMin, closeSec, 0 });
////                            }
////                        }
////                    }
////                }
////                }).wait();
////        }
////        }).wait();
////}
//
//void callScheduleAPI() {
//    // �w�q URI �M�Ȥ��
//    web::uri api_uri(U("http://CNDGNMESIMQ001.delta.corp:10101/schedule/queryScheduleMo"));
//    web::http::client::http_client client(api_uri); // �T�O�ϥΩR�W�Ŷ�
//
//    // �Ы� HTTP POST �ШD
//    web::http::http_request request(web::http::methods::POST);
//    request.headers().add(U("Content-Type"), U("application/json"));
//    request.headers().add(U("tokenID"), U("107574C7FAAA0EA5E0630ECA940AF1FE"));
//
//    // �]�m�ШD�D��]�H JSON �榡�^
//    web::json::value body;
//    body[U("orgInfo")][U("plant")] = web::json::value::string(U("WJ3"));
//    body[U("clientInfo")][U("user")] = web::json::value::string(U("55319938"));
//    body[U("clientInfo")][U("pcName")] = web::json::value::string(U("CNWJxxxx"));
//    body[U("clientInfo")][U("program")] = web::json::value::string(U("test"));
//    body[U("clientInfo")][U("programVer")] = web::json::value::string(U("1.2"));
//    body[U("parameters")][U("prodArea")] = web::json::value::string(U("WJ3_SMT"));
//    body[U("parameters")][U("lineName")] = web::json::value::string(U("SMT-21"));
//
//    // �]�m�ШD�D��
//    request.set_body(body);
//
//    // �o�e�ШD�óB�z�^��
//    client.request(request).then([](web::http::http_response response) {
//        if (response.status_code() == web::http::status_codes::OK) {
//            response.extract_string().then([](utility::string_t responseString) {
//                web::json::value jsonResponse = web::json::value::parse(responseString);
//
//                // �B�z responseData (�нT�O responseData �����s�b)
//                if (jsonResponse.has_array_field(U("responseData"))) {
//                    auto data = jsonResponse[U("responseData")].as_array();
//                    for (const auto& item : data) {
//                        if (item.has_field(U("planStartDate")) && item.has_field(U("planCloseDate"))) {
//                            Schedule schedule;
//                            schedule.planStartDate = item.at(U("planStartDate")).as_string();
//                            schedule.planCloseDate = item.at(U("planCloseDate")).as_string();
//                            scheduleList.push_back(schedule);
//                        }
//                    }
//                }
//                }).wait();
//        }
//        }).wait();
//}
//
//
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
//
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
////// ��ܥͲ��Ƶ{���f
////void ShowScheduleWindow(HINSTANCE hInstance) {
////    if (!hScheduleWindow) {
////        const wchar_t SCHEDULE_CLASS_NAME[] = L"ScheduleWindowClass";
////        WNDCLASS wc = {};
////        wc.lpfnWndProc = DefWindowProc;
////        wc.hInstance = hInstance;
////        wc.lpszClassName = SCHEDULE_CLASS_NAME;
////        RegisterClass(&wc);
////
////        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"�Ͳ��Ƶ{", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
////            100, 100, 400, 600, NULL, NULL, hInstance, NULL);
////
////        hScheduleList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY,
////            10, 10, 360, 540, hScheduleWindow, NULL, hInstance, NULL);
////
////        SendMessage(hScheduleList, LB_RESETCONTENT, 0, 0);
////
////        for (const auto& schedule : scheduleList) {
////            std::wstring item = std::to_wstring(schedule.hour) + L":" + std::to_wstring(schedule.minute) + L":" +
////                std::to_wstring(schedule.second) + L" - " + (schedule.action == 1 ? L"�}��" : L"����");
////            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
////        }
////    }
////    else {
////        ShowWindow(hScheduleWindow, SW_SHOW); // �p�G���f�w�s�b�A��ܥ�
////    }
////}
//
////void ShowScheduleWindow(HINSTANCE hInstance) {
////    if (!hScheduleWindow) {
////        const wchar_t SCHEDULE_CLASS_NAME[] = L"ScheduleWindowClass";
////        WNDCLASS wc = {};
////        wc.lpfnWndProc = DefWindowProc;
////        wc.hInstance = hInstance;
////        wc.lpszClassName = SCHEDULE_CLASS_NAME;
////        RegisterClass(&wc);
////
////        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"�Ͳ��Ƶ{", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
////            100, 100, 500, 600, NULL, NULL, hInstance, NULL); // �N�e���X�j��500�A�T�O��ܧ���ɶ�
////
////        hScheduleList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY,
////            10, 10, 460, 540, hScheduleWindow, NULL, hInstance, NULL); // �վ�e�׬�460
////
////        SendMessage(hScheduleList, LB_RESETCONTENT, 0, 0);
////
////        // �P����r�}�C
////        const wchar_t* daysOfWeek[] = { L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday" };
////
////        for (const auto& schedule : scheduleList) {
////            SYSTEMTIME time = {};
////            GetLocalTime(&time);
////            int year = time.wYear;
////            int month = time.wMonth;
////            int day = time.wDay;
////            int dayOfWeekIndex = time.wDayOfWeek;
////
////            // �榡����ܧ���ɶ��A�]�A�~���B�P���M�ɤ���
////            std::wstring item = std::to_wstring(year) + L"-" +
////                std::to_wstring(month) + L"-" +
////                std::to_wstring(day) + L" (" +
////                daysOfWeek[dayOfWeekIndex] + L") " +
////                std::to_wstring(schedule.hour) + L":" +
////                std::to_wstring(schedule.minute) + L":" +
////                std::to_wstring(schedule.second) +
////                (schedule.action == 1 ? L" - �}��" : L" - ����");
////
////            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
////        }
////    }
////    else {
////        ShowWindow(hScheduleWindow, SW_SHOW); // �p�G���f�w�s�b�A��ܥ�
////    }
////}
////void ShowScheduleWindow(HINSTANCE hInstance) {
////    if (!hScheduleWindow) {
////        const wchar_t SCHEDULE_CLASS_NAME[] = L"ScheduleWindowClass";
////        WNDCLASS wc = {};
////        wc.lpfnWndProc = DefWindowProc;
////        wc.hInstance = hInstance;
////        wc.lpszClassName = SCHEDULE_CLASS_NAME;
////        RegisterClass(&wc);
////
////        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"�Ͳ��Ƶ{", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
////            100, 100, 500, 600, NULL, NULL, hInstance, NULL);
////
////        hScheduleList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY,
////            10, 10, 460, 540, hScheduleWindow, NULL, hInstance, NULL);
////
////        SendMessage(hScheduleList, LB_RESETCONTENT, 0, 0);
////
////        const wchar_t* daysOfWeek[] = { L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday" };
////        std::wregex timeRegex(L"(\\d+)-(\\d+)-(\\d+)T(\\d+):(\\d+):(\\d+)");
////
////        for (const auto& schedule : scheduleList) {
////            std::wstring formattedStart, formattedClose;
////            std::wsmatch match;
////
////            // �ѪR�î榡�� planStartDate
////            if (std::regex_search(schedule.planStartDate, match, timeRegex) && match.size() == 7) {
////                int year = std::stoi(match[1].str());
////                int month = std::stoi(match[2].str());
////                int day = std::stoi(match[3].str());
////                int hour = std::stoi(match[4].str());
////                int minute = std::stoi(match[5].str());
////                int second = std::stoi(match[6].str());
////
////                SYSTEMTIME st = { year, month, day, 0, hour, minute, second, 0 };
////                int dayOfWeekIndex = st.wDayOfWeek;
////                formattedStart = std::to_wstring(year) + L"-" +
////                    std::to_wstring(month) + L"-" +
////                    std::to_wstring(day) + L" (" +
////                    daysOfWeek[dayOfWeekIndex] + L") " +
////                    std::to_wstring(hour) + L":" +
////                    std::to_wstring(minute) + L":" +
////                    std::to_wstring(second);
////            }
////
////            // �ѪR�î榡�� planCloseDate
////            if (std::regex_search(schedule.planCloseDate, match, timeRegex) && match.size() == 7) {
////                int year = std::stoi(match[1].str());
////                int month = std::stoi(match[2].str());
////                int day = std::stoi(match[3].str());
////                int hour = std::stoi(match[4].str());
////                int minute = std::stoi(match[5].str());
////                int second = std::stoi(match[6].str());
////
////                SYSTEMTIME st = { year, month, day, 0, hour, minute, second, 0 };
////                int dayOfWeekIndex = st.wDayOfWeek;
////                formattedClose = std::to_wstring(year) + L"-" +
////                    std::to_wstring(month) + L"-" +
////                    std::to_wstring(day) + L" (" +
////                    daysOfWeek[dayOfWeekIndex] + L") " +
////                    std::to_wstring(hour) + L":" +
////                    std::to_wstring(minute) + L":" +
////                    std::to_wstring(second);
////            }
////
////            std::wstring item = formattedStart + L" - " + formattedClose;
////            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
////        }
////    }
////    else {
////        ShowWindow(hScheduleWindow, SW_SHOW);
////    }
////}
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
//        std::wregex timeRegex(L"(\\d+)-(\\d+)-(\\d+)T(\\d+):(\\d+):(\\d+)");
//
//        for (const auto& schedule : scheduleList) {
//            std::wstring formattedStart, formattedClose;
//            std::wsmatch match;
//            int dayOfWeekIndex;
//
//            // �ѪR�î榡�� planStartDate
//            if (std::regex_search(schedule.planStartDate, match, timeRegex) && match.size() == 7) {
//                int year = std::stoi(match[1].str());
//                int month = std::stoi(match[2].str());
//                int day = std::stoi(match[3].str());
//                int hour = std::stoi(match[4].str());
//                int minute = std::stoi(match[5].str());
//                int second = std::stoi(match[6].str());
//
//                std::tm timeinfo = {};
//                timeinfo.tm_year = year - 1900;  // tm_year�O�q1900�~�}�l���~��
//                timeinfo.tm_mon = month - 1;     // tm_mon�d��O0-11
//                timeinfo.tm_mday = day;
//                timeinfo.tm_hour = hour;
//                timeinfo.tm_min = minute;
//                timeinfo.tm_sec = second;
//                mktime(&timeinfo);  // ��stm_wday
//
//                dayOfWeekIndex = timeinfo.tm_wday;
//                formattedStart = std::to_wstring(year) + L"-" +
//                    std::to_wstring(month) + L"-" +
//                    std::to_wstring(day) + L" (" +
//                    daysOfWeek[dayOfWeekIndex] + L") " +
//                    std::to_wstring(hour) + L":" +
//                    std::to_wstring(minute) + L":" +
//                    std::to_wstring(second);
//            }
//
//            // �ѪR�î榡�� planCloseDate
//            if (std::regex_search(schedule.planCloseDate, match, timeRegex) && match.size() == 7) {
//                int year = std::stoi(match[1].str());
//                int month = std::stoi(match[2].str());
//                int day = std::stoi(match[3].str());
//                int hour = std::stoi(match[4].str());
//                int minute = std::stoi(match[5].str());
//                int second = std::stoi(match[6].str());
//
//                std::tm timeinfo = {};
//                timeinfo.tm_year = year - 1900;
//                timeinfo.tm_mon = month - 1;
//                timeinfo.tm_mday = day;
//                timeinfo.tm_hour = hour;
//                timeinfo.tm_min = minute;
//                timeinfo.tm_sec = second;
//                mktime(&timeinfo);  // ��stm_wday
//
//                dayOfWeekIndex = timeinfo.tm_wday;
//                formattedClose = std::to_wstring(year) + L"-" +
//                    std::to_wstring(month) + L"-" +
//                    std::to_wstring(day) + L" (" +
//                    daysOfWeek[dayOfWeekIndex] + L") " +
//                    std::to_wstring(hour) + L":" +
//                    std::to_wstring(minute) + L":" +
//                    std::to_wstring(second);
//            }
//
//            std::wstring item = formattedStart + L" START--- " + formattedClose + L" CLOSE ";
//            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
//        }
//    }
//    else {
//        ShowWindow(hScheduleWindow, SW_SHOW);
//    }
//}
//
//
//
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
//// ø�s�D�����M�����
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//    switch (uMsg) {
//    case WM_CREATE:
//        for (int i = 0; i < 40; i++) {
//            int col = i / 20;  // ����C��ܡA�e20�Ӧb���C�A��20�Ӧb�k�C
//            int row = i % 20;
//            int xOffset = col * 400;  // ���C�q0�}�l�A�k�C�q400�}�l
//            int yOffset = 50 + row * 30;  // �C��V�U����30����
//
//            // �ѼƦW��
//            HWND label = CreateWindow(L"STATIC", parameterNames[i], WS_VISIBLE | WS_CHILD | SS_LEFT,
//                10 + xOffset, yOffset, 200, 20, hwnd, (HMENU)(i * 2), NULL, NULL);
//
//            // �Ѽƭ�
//            hwndParameters[i] = CreateWindow(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | SS_LEFT,
//                220 + xOffset, yOffset, 150, 20, hwnd, (HMENU)(i * 2 + 1), NULL, NULL);
//        }
//
//        // �Ыث��s�M�ɶ����
//        hOpenButton = CreateWindowW(L"BUTTON", L"�}�ҪŶ}", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 150, 30, hwnd, (HMENU)1, NULL, NULL);
//        hCloseButton = CreateWindowW(L"BUTTON", L"�����Ŷ}", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 170, 10, 150, 30, hwnd, (HMENU)2, NULL, NULL);
//        hScheduleButton = CreateWindowW(L"BUTTON", L"��ܥͲ��Ƶ{", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 330, 10, 150, 30, hwnd, (HMENU)3, NULL, NULL);
//        hAutoScheduleButton = CreateWindowW(L"BUTTON", L"�̷ӥͲ��Ƶ{", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 500, 10, 150, 30, hwnd, (HMENU)4, NULL, NULL);
//        hTimeDisplay = CreateWindowW(L"STATIC", L"00:00:00", WS_VISIBLE | WS_CHILD | SS_LEFT, 670, 10, 200, 20, hwnd, NULL, NULL, NULL);
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
//        else if (LOWORD(wParam) == 3) { // �I����ܥͲ��Ƶ{���s
//            ShowScheduleWindow(GetModuleHandle(NULL));
//        }
//        else if (LOWORD(wParam) == 4) {
//            isScheduleRunning = !isScheduleRunning;
//        }
//        break;
//
//        /*case WM_TIMER:
//            SendModbusRequestOnce(hwnd);
//            SYSTEMTIME st;
//            GetLocalTime(&st);
//            wchar_t timeBuffer[9];
//            swprintf_s(timeBuffer, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
//            SetWindowText(hTimeDisplay, timeBuffer);
//            break;*/
//
//    case WM_TIMER:
//        SendModbusRequestOnce(hwnd);
//        SYSTEMTIME st;
//        GetLocalTime(&st);
//
//        // �ϥάP����r���
//        wchar_t timeBuffer[64];
//        swprintf_s(timeBuffer, L"%04d-%02d-%02d (%s) %02d:%02d:%02d",
//            st.wYear, st.wMonth, st.wDay, daysOfWeek[st.wDayOfWeek], st.wHour, st.wMinute, st.wSecond);
//
//        // �u����ɶ��ܧ�ɤ~��s��ܤ��e
//        if (wcscmp(timeBuffer, previousTimeBuffer) != 0) {
//            SetWindowText(hTimeDisplay, timeBuffer);
//            wcscpy_s(previousTimeBuffer, timeBuffer); // ��s�W����ܪ��ɶ�
//        }
//        break;
//
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
////int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
////    if (!InitSerialPort()) return 1;
////
////    const wchar_t CLASS_NAME[] = L"ModbusViewer";
////    WNDCLASS wc = {};
////    wc.lpfnWndProc = WindowProc;
////    wc.hInstance = hInstance;
////    wc.lpszClassName = CLASS_NAME;
////    RegisterClass(&wc);
////
////    HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Modbus Control", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, hInstance, NULL);
////    if (!hwnd) return 0;
////
////    ShowWindow(hwnd, SW_MAXIMIZE);
////
////
////
////
////    callScheduleAPI();
////    std::thread scheduleThread(RunSchedule);
////    scheduleThread.detach();
////
////    MSG msg = {};
////    while (GetMessage(&msg, NULL, 0, 0)) {
////        TranslateMessage(&msg);
////        DispatchMessage(&msg);
////    }
////
////    return 0;
////}
//
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
//    // �ҰʱƵ{ API ���w�ɧ�s�����
//    std::thread updateThread([]() {
//        while (true) {
//            callScheduleAPI();  // ��s�Ͳ��Ƶ{
//            std::wcout << L"�Ͳ��Ƶ{�w��s" << std::endl;  // ��s������q��
//            std::this_thread::sleep_for(std::chrono::seconds(10));  // �C 10 ���s�@��
//        }
//        });
//
//    // �ҰʥͲ��Ƶ{�������
//    callScheduleAPI();
//    std::thread scheduleThread(RunSchedule);
//
//    // ���������
//    scheduleThread.detach();
//    updateThread.detach();
//
//    MSG msg = {};
//    while (GetMessage(&msg, NULL, 0, 0)) {
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//    }
//
//    return 0;
//}
