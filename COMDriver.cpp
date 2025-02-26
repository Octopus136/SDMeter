#include "pch.h"
#include "COMDriver.h"

COMDriver::COMDriver() : hdevice(INVALID_HANDLE_VALUE) {
    devices.clear();
    UpdateDevices();
}

ErrType COMDriver::GetDevices(std::vector<COMInfo>& d) {
    d = this->devices;
    return LastError;
}

void COMDriver::UpdateDevices() {

    this->devices.clear();

    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        LastError = ErrType::UPDATE_ERROR;
        return;
    }

    SP_DEVICE_INTERFACE_DATA interfaceData = { 0 };
    interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInterfaces(hDevInfo, NULL, &GUID_DEVINTERFACE_COMPORT, i, &interfaceData); ++i) {
        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, NULL, 0, &requiredSize, NULL);

        std::vector<BYTE> detailBuffer(requiredSize);
        PSP_DEVICE_INTERFACE_DETAIL_DATA pDetail = reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(detailBuffer.data());
        pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        SP_DEVINFO_DATA devInfoData = { 0 };
        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, pDetail, requiredSize, NULL, &devInfoData)) {
            continue;
        }

        WCHAR deviceName[256] = { 0 };
        if (!SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_FRIENDLYNAME, NULL,
            reinterpret_cast<PBYTE>(deviceName), sizeof(deviceName) - sizeof(WCHAR), NULL)) {
            SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, SPDRP_DEVICEDESC, NULL,
                reinterpret_cast<PBYTE>(deviceName), sizeof(deviceName) - sizeof(WCHAR), NULL);
        }

        HKEY hDeviceKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (hDeviceKey == INVALID_HANDLE_VALUE) {
            continue;
        }

        WCHAR portName[256] = { 0 };
        DWORD size = sizeof(portName);
        DWORD type = 0;
        if (RegQueryValueEx(hDeviceKey, L"PortName", NULL, &type,
            reinterpret_cast<LPBYTE>(portName), &size) == ERROR_SUCCESS && type == REG_SZ) {
            if (wcsncmp(portName, L"COM", 3) == 0) {
                int comNumber = _wtoi(portName + 3);
                if (comNumber > 0) {
                    COMInfo info;
                    info.deviceName = deviceName;
                    info.com_id = comNumber;
                    devices.push_back(info);
                }
            }
        }
        RegCloseKey(hDeviceKey);
    }

    DWORD error = GetLastError();
    if (error != ERROR_NO_MORE_ITEMS) {
        SetupDiDestroyDeviceInfoList(hDevInfo);
        LastError = ErrType::UPDATE_ERROR;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    LastError = ErrType::OK;
}

ErrType COMDriver::OpenDevice(int deviceidx) {
    if (deviceidx < 0 || deviceidx >= devices.size()) {
        LastError = ErrType::OPEN_ERROR;
        return LastError;
    }

    // 获取COM端口信息
    const COMInfo& info = devices[deviceidx];
    //std::wstring portName = L"\\\\.\\COM" + std::to_wstring(info.com_id);
    std::wstring portName = L"COM" + std::to_wstring(info.com_id);

    // 打开串口
    hdevice = CreateFileW(
        portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hdevice == INVALID_HANDLE_VALUE) {
        return LastError = ErrType::OPEN_ERROR;
    }

    // 配置串口参数
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);
    if (!GetCommState(hdevice, &dcb)) {
        CloseHandle(hdevice);
        hdevice = INVALID_HANDLE_VALUE;
        return LastError = ErrType::OPEN_ERROR;
    }

    // 配置波特率38400，8N1
    dcb.BaudRate = CBR_38400;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;

    if (!SetCommState(hdevice, &dcb)) {
        CloseHandle(hdevice);
        hdevice = INVALID_HANDLE_VALUE;
        return LastError = ErrType::OPEN_ERROR;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 200;  // 关键参数：字节间隔超时
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(hdevice, &timeouts)) {
        CloseHandle(hdevice);
        hdevice = INVALID_HANDLE_VALUE;
        return LastError = ErrType::OPEN_ERROR;
    }

    return LastError = ErrType::OK;
}

ErrType COMDriver::ReleaseDevice(int deviceidx) {
    if (deviceidx < 0 || deviceidx >= devices.size()) {
        return LastError = ErrType::RELEASE_ERROR;
    }

    if (hdevice != INVALID_HANDLE_VALUE && hdevice != NULL) {
        if (!CloseHandle(hdevice)) {
            return LastError = ErrType::RELEASE_ERROR;
        }
        return LastError = ErrType::OK;
    }
    else return LastError = ErrType::RELEASE_ERROR;
}

ErrType COMDriver::SendData(UINT8 data) {
    if (hdevice == INVALID_HANDLE_VALUE) {
        LastError = ErrType::SEND_ERROR;
        return LastError;
    }

    DWORD bytesWritten;
    if (!WriteFile(hdevice, &data, 1, &bytesWritten, nullptr)) {
        LastError = ErrType::SEND_ERROR;
        return LastError;
    }

    return LastError = (bytesWritten == 1) ? ErrType::OK : ErrType::SEND_ERROR;
}

ErrType COMDriver::RecvData(UINT8* buffer, DWORD& bytesRead) {
    bytesRead = 0;
    if (hdevice == INVALID_HANDLE_VALUE) {
        LastError = ErrType::NOT_INIT;
        return LastError;
    }

    if (!ReadFile(hdevice, buffer, 8, &bytesRead, nullptr)) {
        return LastError = ErrType::RECV_ERROR;
    }

    return LastError = ErrType::OK;
}