#pragma once

#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <initguid.h>
#include <vector>
#include <string>
#include <tchar.h>
#include <ntddser.h>

#pragma comment(lib, "setupapi.lib")

struct COMInfo {
    std::wstring deviceName;
    int com_id;
};

enum class ErrType {
    OK = 0,
    UPDATE_ERROR = 1,
    NOT_INIT = 2,
    SEND_ERROR = 3,
    RECV_ERROR = 4,
    OPEN_ERROR = 5,
    RELEASE_ERROR = 6,
};

class COMDriver {
public:
    COMDriver();
    ErrType GetDevices(std::vector<COMInfo>& devices);
    void UpdateDevices();
    ErrType OpenDevice(int deviceidx);
    ErrType ReleaseDevice(int deviceidx);
    ErrType SendData(UINT8 data);
    ErrType RecvData(UINT8* data, DWORD& bytesRead);
private:
    std::vector<COMInfo> devices;
    ErrType LastError = ErrType::NOT_INIT;
    HANDLE hdevice;
};
