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
////// Schedule 結構
////struct Schedule {
////    int hour;
////    int minute;
////    int second;
////    int action; // 1: 開啟, 0: 關閉
////};
//struct Schedule {
//    int hour;
//    int minute;
//    int second;
//    int action; // 1: 開啟, 0: 關閉
//    std::wstring planStartDate; // 添加字段以存儲開始日期時間
//    std::wstring planCloseDate; // 添加字段以存儲結束日期時間
//};
//
//
//// 全域宣告星期文字陣列，避免在 switch-case 中初始化被略過
//const wchar_t* daysOfWeek[] = { L"Sunday", L"Monday", L"Tuesday", L"Wednesday", L"Thursday", L"Friday", L"Saturday" };
//wchar_t previousTimeBuffer[64] = L""; // 上次顯示的時間
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
//    L"當前狀態", L"當前狀態", L"漏電電流", L"A相溫度", L"B相溫度", L"C相溫度", L"N相溫度",
//    L"A相電壓", L"B相電壓", L"C相電壓", L"A相電流", L"B相電流", L"C相電流",
//    L"A相功率因數", L"B相功率因數", L"C相功率因數", L"A相有功功率",
//    L"B相有功功率", L"C相有功功率", L"A相無功功率", L"B相無功功率",
//    L"C相無功功率", L"合閘次數", L"電能高字節", L"電能低字節", L"開關狀態",
//    L"A相視在功率", L"B相視在功率", L"C相視在功率", L"總視在功率",
//    L"總有功功率", L"總無功功率", L"合相功率因數", L"線頻率", L"設備類型",
//    L"歷史漏電值", L"歷史A相電流", L"歷史B相電流", L"歷史C相電流", L"線路顏色標記"
//};
//
//// 初始化串口
//BOOL InitSerialPort() {
//    hSerial = CreateFile(L"COM6", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
//    if (hSerial == INVALID_HANDLE_VALUE) {
//        MessageBox(NULL, L"串口開啟失敗", L"錯誤", MB_OK | MB_ICONERROR);
//        return FALSE;
//    }
//    DCB dcbSerialParams = { 0 };
//    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
//
//    if (!GetCommState(hSerial, &dcbSerialParams)) {
//        MessageBox(NULL, L"無法獲取串口狀態", L"錯誤", MB_OK | MB_ICONERROR);
//        return FALSE;
//    }
//    dcbSerialParams.BaudRate = CBR_9600;
//    dcbSerialParams.ByteSize = 8;
//    dcbSerialParams.StopBits = ONESTOPBIT;
//    dcbSerialParams.Parity = NOPARITY;
//
//    if (!SetCommState(hSerial, &dcbSerialParams)) {
//        MessageBox(NULL, L"無法設定串口參數", L"錯誤", MB_OK | MB_ICONERROR);
//        return FALSE;
//    }
//
//    return TRUE;
//}
//
////
////// 修改後的時間解析程式碼
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
////                            // 使用正則表達式檢查和提取時間
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
//    // 定義 URI 和客戶端
//    web::uri api_uri(U("http://CNDGNMESIMQ001.delta.corp:10101/schedule/queryScheduleMo"));
//    web::http::client::http_client client(api_uri); // 確保使用命名空間
//
//    // 創建 HTTP POST 請求
//    web::http::http_request request(web::http::methods::POST);
//    request.headers().add(U("Content-Type"), U("application/json"));
//    request.headers().add(U("tokenID"), U("107574C7FAAA0EA5E0630ECA940AF1FE"));
//
//    // 設置請求主體（以 JSON 格式）
//    web::json::value body;
//    body[U("orgInfo")][U("plant")] = web::json::value::string(U("WJ3"));
//    body[U("clientInfo")][U("user")] = web::json::value::string(U("55319938"));
//    body[U("clientInfo")][U("pcName")] = web::json::value::string(U("CNWJxxxx"));
//    body[U("clientInfo")][U("program")] = web::json::value::string(U("test"));
//    body[U("clientInfo")][U("programVer")] = web::json::value::string(U("1.2"));
//    body[U("parameters")][U("prodArea")] = web::json::value::string(U("WJ3_SMT"));
//    body[U("parameters")][U("lineName")] = web::json::value::string(U("SMT-21"));
//
//    // 設置請求主體
//    request.set_body(body);
//
//    // 發送請求並處理回應
//    client.request(request).then([](web::http::http_response response) {
//        if (response.status_code() == web::http::status_codes::OK) {
//            response.extract_string().then([](utility::string_t responseString) {
//                web::json::value jsonResponse = web::json::value::parse(responseString);
//
//                // 處理 responseData (請確保 responseData 的欄位存在)
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
//// 開啟/關閉空開
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
//// 排程執行緒
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
//// 處理 Modbus 回應數據並顯示
//void ProcessModbusResponse(const char* response, DWORD length) {
//    int startIndex = 3;
//    int endIndex = length - 2;
//
//    for (int i = 0, index = startIndex; i < 40 && index < endIndex; i++, index += 2) {
//        int value = (response[index] << 8) | (response[index + 1] & 0xFF);
//        if (i == 7 || i == 8 || i == 9) { // 特定參數需要調整數值顯示
//            value /= 10;
//        }
//        std::wstring valueText = std::to_wstring(value);
//        SetWindowText(hwndParameters[i], valueText.c_str());
//    }
//}
//
////// 顯示生產排程窗口
////void ShowScheduleWindow(HINSTANCE hInstance) {
////    if (!hScheduleWindow) {
////        const wchar_t SCHEDULE_CLASS_NAME[] = L"ScheduleWindowClass";
////        WNDCLASS wc = {};
////        wc.lpfnWndProc = DefWindowProc;
////        wc.hInstance = hInstance;
////        wc.lpszClassName = SCHEDULE_CLASS_NAME;
////        RegisterClass(&wc);
////
////        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"生產排程", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
////            100, 100, 400, 600, NULL, NULL, hInstance, NULL);
////
////        hScheduleList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY,
////            10, 10, 360, 540, hScheduleWindow, NULL, hInstance, NULL);
////
////        SendMessage(hScheduleList, LB_RESETCONTENT, 0, 0);
////
////        for (const auto& schedule : scheduleList) {
////            std::wstring item = std::to_wstring(schedule.hour) + L":" + std::to_wstring(schedule.minute) + L":" +
////                std::to_wstring(schedule.second) + L" - " + (schedule.action == 1 ? L"開啟" : L"關閉");
////            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
////        }
////    }
////    else {
////        ShowWindow(hScheduleWindow, SW_SHOW); // 如果窗口已存在，顯示它
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
////        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"生產排程", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
////            100, 100, 500, 600, NULL, NULL, hInstance, NULL); // 將寬度擴大為500，確保顯示完整時間
////
////        hScheduleList = CreateWindowW(L"LISTBOX", NULL, WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY,
////            10, 10, 460, 540, hScheduleWindow, NULL, hInstance, NULL); // 調整寬度為460
////
////        SendMessage(hScheduleList, LB_RESETCONTENT, 0, 0);
////
////        // 星期文字陣列
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
////            // 格式化顯示完整時間，包括年月日、星期和時分秒
////            std::wstring item = std::to_wstring(year) + L"-" +
////                std::to_wstring(month) + L"-" +
////                std::to_wstring(day) + L" (" +
////                daysOfWeek[dayOfWeekIndex] + L") " +
////                std::to_wstring(schedule.hour) + L":" +
////                std::to_wstring(schedule.minute) + L":" +
////                std::to_wstring(schedule.second) +
////                (schedule.action == 1 ? L" - 開啟" : L" - 關閉");
////
////            SendMessage(hScheduleList, LB_ADDSTRING, 0, (LPARAM)item.c_str());
////        }
////    }
////    else {
////        ShowWindow(hScheduleWindow, SW_SHOW); // 如果窗口已存在，顯示它
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
////        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"生產排程", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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
////            // 解析並格式化 planStartDate
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
////            // 解析並格式化 planCloseDate
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
//        hScheduleWindow = CreateWindowEx(0, SCHEDULE_CLASS_NAME, L"生產排程", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
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
//            // 解析並格式化 planStartDate
//            if (std::regex_search(schedule.planStartDate, match, timeRegex) && match.size() == 7) {
//                int year = std::stoi(match[1].str());
//                int month = std::stoi(match[2].str());
//                int day = std::stoi(match[3].str());
//                int hour = std::stoi(match[4].str());
//                int minute = std::stoi(match[5].str());
//                int second = std::stoi(match[6].str());
//
//                std::tm timeinfo = {};
//                timeinfo.tm_year = year - 1900;  // tm_year是從1900年開始的年數
//                timeinfo.tm_mon = month - 1;     // tm_mon範圍是0-11
//                timeinfo.tm_mday = day;
//                timeinfo.tm_hour = hour;
//                timeinfo.tm_min = minute;
//                timeinfo.tm_sec = second;
//                mktime(&timeinfo);  // 更新tm_wday
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
//            // 解析並格式化 planCloseDate
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
//                mktime(&timeinfo);  // 更新tm_wday
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
//// 發送 Modbus 請求，並讀取回應
//void SendModbusRequestOnce(HWND hwnd) {
//    DWORD bytesWritten, bytesRead;
//    char modbusRequest[] = { 0x06, 0x03, 0x00, 0x00, 0x00, 0x30, 0x44, 0x69 };
//    char modbusResponse[256] = { 0 };
//
//    if (!WriteFile(hSerial, modbusRequest, sizeof(modbusRequest), &bytesWritten, NULL)) {
//        MessageBox(hwnd, L"無法發送 Modbus 請求", L"錯誤", MB_OK);
//        return;
//    }
//
//    if (ReadFile(hSerial, modbusResponse, sizeof(modbusResponse), &bytesRead, NULL) && bytesRead > 0) {
//        ProcessModbusResponse(modbusResponse, bytesRead);
//    }
//}
//
//// 繪製主視窗和控制元件
//LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//    switch (uMsg) {
//    case WM_CREATE:
//        for (int i = 0; i < 40; i++) {
//            int col = i / 20;  // 分兩列顯示，前20個在左列，後20個在右列
//            int row = i % 20;
//            int xOffset = col * 400;  // 左列從0開始，右列從400開始
//            int yOffset = 50 + row * 30;  // 每行向下偏移30像素
//
//            // 參數名稱
//            HWND label = CreateWindow(L"STATIC", parameterNames[i], WS_VISIBLE | WS_CHILD | SS_LEFT,
//                10 + xOffset, yOffset, 200, 20, hwnd, (HMENU)(i * 2), NULL, NULL);
//
//            // 參數值
//            hwndParameters[i] = CreateWindow(L"STATIC", L"0", WS_VISIBLE | WS_CHILD | SS_LEFT,
//                220 + xOffset, yOffset, 150, 20, hwnd, (HMENU)(i * 2 + 1), NULL, NULL);
//        }
//
//        // 創建按鈕和時間顯示
//        hOpenButton = CreateWindowW(L"BUTTON", L"開啟空開", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 10, 150, 30, hwnd, (HMENU)1, NULL, NULL);
//        hCloseButton = CreateWindowW(L"BUTTON", L"關閉空開", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 170, 10, 150, 30, hwnd, (HMENU)2, NULL, NULL);
//        hScheduleButton = CreateWindowW(L"BUTTON", L"顯示生產排程", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 330, 10, 150, 30, hwnd, (HMENU)3, NULL, NULL);
//        hAutoScheduleButton = CreateWindowW(L"BUTTON", L"依照生產排程", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 500, 10, 150, 30, hwnd, (HMENU)4, NULL, NULL);
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
//        else if (LOWORD(wParam) == 3) { // 點擊顯示生產排程按鈕
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
//        // 使用星期文字表示
//        wchar_t timeBuffer[64];
//        swprintf_s(timeBuffer, L"%04d-%02d-%02d (%s) %02d:%02d:%02d",
//            st.wYear, st.wMonth, st.wDay, daysOfWeek[st.wDayOfWeek], st.wHour, st.wMinute, st.wSecond);
//
//        // 只有當時間變更時才更新顯示內容
//        if (wcscmp(timeBuffer, previousTimeBuffer) != 0) {
//            SetWindowText(hTimeDisplay, timeBuffer);
//            wcscpy_s(previousTimeBuffer, timeBuffer); // 更新上次顯示的時間
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
//    // 啟動排程 API 的定時更新執行緒
//    std::thread updateThread([]() {
//        while (true) {
//            callScheduleAPI();  // 更新生產排程
//            std::wcout << L"生產排程已更新" << std::endl;  // 更新完成後通知
//            std::this_thread::sleep_for(std::chrono::seconds(10));  // 每 10 秒更新一次
//        }
//        });
//
//    // 啟動生產排程的執行緒
//    callScheduleAPI();
//    std::thread scheduleThread(RunSchedule);
//
//    // 分離執行緒
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
