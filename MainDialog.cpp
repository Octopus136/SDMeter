// MainDialog.cpp: 实现文件
//

#include "pch.h"
#include "SDMeter.h"
#include "afxdialogex.h"
#include "MainDialog.h"
#include <afxwin.h>
#include <dbt.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <fstream>
#include <sstream>
#include <iomanip>

// MainDialog 对话框

IMPLEMENT_DYNAMIC(MainDialog, CDialogEx)

MainDialog::MainDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MAINDIALOG, pParent)
{

}

MainDialog::~MainDialog()
{
}

void MainDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MainDialog, CDialogEx)
    ON_WM_ERASEBKGND()
    ON_WM_CTLCOLOR()
    ON_EN_SETFOCUS(IDC_EDIT1, &MainDialog::OnEnSetfocusEdit)
    ON_CBN_DROPDOWN(IDC_COMBO2, &MainDialog::OnCbnDropdownCombo)
    ON_WM_DEVICECHANGE()
    ON_BN_CLICKED(IDC_BUTTON6, &MainDialog::OnBnClickedButton6)
    ON_BN_CLICKED(IDC_BUTTON5, &MainDialog::OnBnClickedButton5)
    ON_BN_CLICKED(IDC_BUTTON7, &MainDialog::OnBnClickedButton7)
    ON_BN_CLICKED(IDC_BUTTON2, &MainDialog::OnBnClickedButton2)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON4, &MainDialog::OnBnClickedButton4)
    ON_BN_CLICKED(IDC_BUTTON1, &MainDialog::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON3, &MainDialog::OnBnClickedButton3)
    // 菜单命令
    ON_COMMAND(ID_BEGIN_EXECUTE, &MainDialog::OnBnClickedButton6)
    ON_COMMAND(ID_END_EXECUTE, &MainDialog::OnBnClickedButton5)
    ON_COMMAND(ID_ABSORB_SET, &MainDialog::OnBnClickedButton2)
    ON_COMMAND(ID_ABSORB_CLEAR, &MainDialog::OnBnClickedButton1)
    ON_COMMAND(ID_REFLECT_SET, &MainDialog::OnBnClickedButton4)
    ON_COMMAND(ID_REFLECT_CLEAR, &MainDialog::OnBnClickedButton3)
    ON_COMMAND(ID_SET_DISTANCE, &MainDialog::OnBnClickedButton7)
    ON_COMMAND(ID_EXIT, &MainDialog::ExitFunction)
    ON_COMMAND(ID_ABOUT, &MainDialog::AboutFunction)
    // TODO
    ON_COMMAND(ID_READ_FILE, &MainDialog::TODOFunction)
    ON_COMMAND(ID_RECORD_FILE, &MainDialog::TODOFunction)
    ON_COMMAND(ID_BEGIN_RECORD, &MainDialog::TODOFunction)
    ON_COMMAND(ID_CLEAR_BUFFER, &MainDialog::TODOFunction)
END_MESSAGE_MAP()


// MainDialog 消息处理程序

void MainDialog::SetStatusText(LPCTSTR text) {
    SetDlgItemTextW(IDC_STATIC8, text);
}

BOOL MainDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 各种控件的字体设置
    for (int i = 0; i < fontSetNum; i++) {
        int nID = fontinfo[i].nID;
        CString fontName = fontinfo[i].fontName;
        int fontSize = fontinfo[i].fontSize;
        LOGFONT lf = { 0 };
        lf.lfHeight = fontSize;
        wcscpy_s(lf.lfFaceName, fontName);
        if (fontinfo[i].fontBold) {
            lf.lfWeight = FW_BOLD;
        }
        mFont[i].CreateFontIndirect(&lf);
        CWnd* pWnd = GetDlgItem(nID);
        pWnd->SetFont(&mFont[i]);
    }

    // 参数相关
    std::vector<COMInfo> devices;
    if (cdriver.GetDevices(devices) != ErrType::OK) {
        SetStatusText(_T("读取USB设备失败"));
    }
    
    pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO2);
    pComboBox->ResetContent();

    for (const auto& item : devices) {
        std::wstring display_name = item.deviceName;
        pComboBox->AddString(display_name.c_str());
    }

    // 设置COM设备监控
    DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
    ZeroMemory(&NotificationFilter, sizeof(NotificationFilter));
    NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_COMPORT; // COM设备

    m_hDeviceNotify = RegisterDeviceNotification(m_hWnd, &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);
    // 读取ini
    std::ifstream infile("SDMeter.ini");
    if (infile.is_open()) {
        std::string line;
        while (std::getline(infile, line)) {
            std::istringstream iss(line);
            std::string param, value;
            if (std::getline(iss, param, '=') && std::getline(iss, value)) {
                size_t start = param.find_first_not_of(" \t");
                size_t end = param.find_last_not_of(" \t");
                param = (start != std::string::npos && start <= end) ? param.substr(start, end - start + 1) : "";

                start = value.find_first_not_of(" \t");
                end = value.find_last_not_of(" \t");
                value = (start != std::string::npos && start <= end) ? value.substr(start, end - start + 1) : "";

                config[param] = value;
            }
        }
        infile.close();
    }
    
    // 配置ini
    try {
        W0 = std::stoi(config["W0"]);
    }
    catch (...) {
        W0 = 0;
    }

    try {
        W1 = std::stoi(config["W1"]);
    }
    catch (...) {
        W1 = W1_th;
    }

    try {
        d = std::stod(config["d"]);
    }
    catch (...) {
        d = 0.5;
    }

    try {
        TEMPFACTOR = std::stod(config["TEMPFACTOR"]);
    }
    catch (...) {
        TEMPFACTOR = 0;
    }

    std::wostringstream wos;
    wos << L"d=" << std::fixed << std::setprecision(3) << d << L"m";
    std::wstring formatted_d = wos.str();
    GetDlgItem(IDC_BUTTON7)->SetWindowTextW(formatted_d.c_str());

    // 菜单句柄
    pMenu = GetMenu();

    // 关于字符串拼接
    AboutInformation = About;
    for (auto& clog : Changelog) {
        AboutInformation += clog;
    }

    // 设置图标
    HICON hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    SetIcon(hIcon, TRUE);
    SetIcon(hIcon, FALSE);

	return TRUE;
}

// 绘制背景
BOOL MainDialog::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    pDC->FillSolidRect(rect, RGB(240, 240, 240));
    CRect DarkRect(0, 0, 626, 141);
    pDC->FillSolidRect(DarkRect, RGB(220, 220, 220));
    return TRUE;
}

HBRUSH MainDialog::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    int nowID = pWnd->GetDlgCtrlID();
    // 需要重绘背景的深色区控件
    int IDList[] = {IDC_STATIC9 , IDC_STATIC10 , IDC_STATIC11 , IDC_STATIC12 , IDC_STATIC13 , IDC_STATIC14};
    for (auto& ID : IDList) {
        if (ID == nowID) {
            if (nowID == IDC_STATIC12) { // 减光率，红色
                pDC->SetTextColor(RGB(255, 0, 0));
            }
            else if (nowID == IDC_STATIC13) { // 减光系数，绿色
                pDC->SetTextColor(RGB(0, 255, 0));
            }
            else if (nowID == IDC_STATIC14) { // 读数，蓝色
                pDC->SetTextColor(RGB(0, 0, 255));
            }
            pDC->SetBkColor(RGB(220, 220, 220));
            static CBrush brush(RGB(220, 220, 220));
            return brush;
        }
    }
    return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}

// 获取文本框焦点时隐藏光标
void MainDialog::OnEnSetfocusEdit()
{
    ::HideCaret(GetDlgItem(IDC_EDIT1)->GetSafeHwnd());
}


void MainDialog::OnCbnDropdownCombo()
{
    CComboBox* pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO2);

    // 获取下拉框中最长项的宽度
    int nMaxWidth = 0;
    int nCount = pComboBox->GetCount();
    for (int i = 0; i < nCount; ++i)
    {
        CString strItem;
        pComboBox->GetLBText(i, strItem);

        // 获取每项的文本宽度
        CClientDC dc(pComboBox);
        CSize size = dc.GetTextExtent(strItem);
        nMaxWidth = max(nMaxWidth, size.cx);
    }

    // 设置下拉框的宽度，确保所有文本都能显示
    pComboBox->SetDroppedWidth(nMaxWidth + 20);  // 添加一些额外空间
}

// USB设备改变
BOOL MainDialog::OnDeviceChange(UINT nType, DWORD_PTR dwData) {
    std::vector<COMInfo> devices;
    cdriver.UpdateDevices();
    if (cdriver.GetDevices(devices) != ErrType::OK) {
        SetStatusText(_T("读取USB设备失败"));
    }

    pComboBox->ResetContent();

    for (const auto& item : devices) {
        std::wstring display_name = item.deviceName;
        pComboBox->AddString(display_name.c_str());
    }

    pComboBox->SetCurSel(0);
    return TRUE;
}

void MainDialog::ShowAdjustText(const std::wstring& str) {
    if (m_timerId != 1)
    {
        KillTimer(m_timerId);
    }

    GetDlgItem(IDC_STATIC_ADJ)->SetWindowTextW(str.c_str());

    m_timerId = SetTimer(1, 3000, NULL); // 3000毫秒 = 3秒
}

inline void MainDialog::Button_CTRL(int op) {
    GetDlgItem(IDC_BUTTON6)->EnableWindow(FALSE ^ op);
    GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE ^ op);
    GetDlgItem(IDC_BUTTON4)->EnableWindow(TRUE ^ op);
    GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE ^ op);
    GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE ^ op);
    GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE ^ op);
    GetDlgItem(IDC_BUTTON7)->EnableWindow(FALSE ^ op);
    GetDlgItem(IDC_COMBO2)->EnableWindow(FALSE ^ op);
    pMenu->EnableMenuItem(ID_BEGIN_EXECUTE, TRUE ^ op);
    pMenu->EnableMenuItem(ID_END_EXECUTE, FALSE ^ op);
    pMenu->EnableMenuItem(ID_ABSORB_SET, FALSE ^ op);
    pMenu->EnableMenuItem(ID_ABSORB_CLEAR, FALSE ^ op);
    pMenu->EnableMenuItem(ID_REFLECT_SET, FALSE ^ op);
    pMenu->EnableMenuItem(ID_REFLECT_CLEAR, FALSE ^ op);
    pMenu->EnableMenuItem(ID_SET_DISTANCE, TRUE ^ op);
}

inline void MainDialog::BEGIN() {
    Button_CTRL(0);
}

inline void MainDialog::STOP() {
    Button_CTRL(1);
}

enum class AdjustSignal {
    IDLE = 0,
    FINISHED = 1,
    NEED_W0 = 2,
    NEED_W1 = 3,
    ADJUSTING = 4,
};

bool LoopInterrupt = false;
int AdjustW0 = -1, AdjustW1 = -1, AdjustWt = -1;
int begin_t = -1;
AdjustSignal signal = AdjustSignal::IDLE;

UINT MainLoop(LPVOID pParam) {
    MainDialog* pWnd = (MainDialog*)pParam;
    UINT8* data = new UINT8[8];
    int timestamp = 0;
    std::vector<double> vV(4, 0.0), vm(4, 0.0), vU(4, 0.0), vWt(4, 0.0), vD(4, 0.0);
    double meanV = 0.0, meanm = 0.0, meanU = 0.0, meanWt = 0.0, meanD = 0.0;
    bool canoutput = false;
    bool needw0 = false, needw1 = false;
    while (!LoopInterrupt) {
        DWORD bytesread = 0;
        pWnd->cdriver.RecvData(data, bytesread);
        if (bytesread != 8) continue;
        else {
            if (data[0] != 0xAA || data[7] != 0x55) continue;
            int base = data[1] << 8 | data[2];
            int current = data[3] << 8 | data[4];
            int temp = data[5] << 8 | data[6]; // 温度
            int real = current - base; // 实际值
            // double t = 1.0 / (1.0 / 3950.0 * std::log((3.0 / 5.0 * temp) / (1024.0 - temp)) + 1.0 / 298.15) - 273.15; // 实际温度
            double t = -1.0;
            int nTempAdj = (int)((pWnd->Wt - t) * pWnd->TEMPFACTOR + 0.5); // 温度补偿
            double W = 1.0 * pWnd->W1_th / (pWnd->W1 - pWnd->W0) * (real + nTempAdj - pWnd->W0); // 读数
            double V = (1.0 - W / (1.0 * pWnd->W1_th)) * 100; // 减光率，单位%
            double m = 0.0; // 减光系数，单位dB/m
            if (real + nTempAdj - pWnd->W0 <= 0)
                m = std::log10(1.0 * (pWnd->W1 - pWnd->W0) / (1.0 / (pWnd->W0 - real - nTempAdj + 100.0))) * 10 / pWnd->d; // W0 很大的时候
            else 
                m = std::log10(1.0 * (pWnd->W1 - pWnd->W0) / (real + nTempAdj - pWnd->W0)) * 10 / pWnd->d; // 正常的时候
            double U = W * 2048 / 32768; // 毫伏数，单位mV
            meanV += 0.25 * (V - vV[timestamp % 4]);
            meanm += 0.25 * (m - vm[timestamp % 4]);
            meanU += 0.25 * (U - vU[timestamp % 4]);
            meanWt += 0.25 * (t - vWt[timestamp % 4]);
            meanD += 0.25 * (real - vD[timestamp % 4]);
            vV[timestamp % 4] = V;
            vm[timestamp % 4] = m;
            vWt[timestamp % 4] = t;
            vU[timestamp % 4] = U;
            vD[timestamp % 4] = real;
            timestamp++;
            if (signal == AdjustSignal::ADJUSTING && timestamp - begin_t >= 3) {
                signal = AdjustSignal::FINISHED;
                begin_t = -1;
                if (needw0) AdjustW0 = meanD;
                else AdjustW1 = meanD;
                needw0 = false;
                needw1 = false;
            }
            if (signal == AdjustSignal::NEED_W0 || signal == AdjustSignal::NEED_W1) { // 需要校准
                if (signal == AdjustSignal::NEED_W0) needw0 = true;
                else needw1 = true;
                signal = AdjustSignal::ADJUSTING;
                begin_t = timestamp;
            }
            if (timestamp == 4) canoutput = 1;
            if (canoutput) {
                std::wostringstream Vwos, mwos, twos, Uwos, Dwos, Wwos;
                Vwos << std::fixed << std::setprecision(2) << meanV;
                mwos << std::fixed << std::setprecision(3) << meanm;
                twos << std::fixed << std::setprecision(1) << meanWt << L"℃";
                Uwos << std::fixed << std::setprecision(1) << meanU << L"mV";
                Dwos << (int)meanD;
                Wwos << (int)(1.0 * pWnd->W1_th / (pWnd->W1 - pWnd->W0) * (meanD + nTempAdj - pWnd->W0));
                std::wstring formatted_V = Vwos.str(), formatted_m = mwos.str(), formatted_t = twos.str(), formatted_U = Uwos.str(), formatted_D = Dwos.str(), formatted_W = Wwos.str();
                pWnd->GetDlgItem(IDC_STATIC12)->SetWindowTextW(formatted_V.c_str()); // 减光率
                pWnd->GetDlgItem(IDC_STATIC13)->SetWindowTextW(formatted_m.c_str()); // 减光系数
                pWnd->GetDlgItem(IDC_STATIC15)->SetWindowTextW(formatted_t.c_str()); // 温度
                pWnd->GetDlgItem(IDC_STATIC16)->SetWindowTextW(formatted_U.c_str()); // 毫伏
                pWnd->GetDlgItem(IDC_STATIC17)->SetWindowTextW(formatted_D.c_str()); // 原始值
                pWnd->GetDlgItem(IDC_STATIC14)->SetWindowTextW(formatted_W.c_str()); // 读数
            }
        }
    }
    delete[] data;
    return 0;
}

void AdjustButton(MainDialog* pWnd, bool op) {
    pWnd->GetDlgItem(IDC_BUTTON1)->EnableWindow(op);
    pWnd->GetDlgItem(IDC_BUTTON2)->EnableWindow(op);
    pWnd->GetDlgItem(IDC_BUTTON3)->EnableWindow(op);
    pWnd->GetDlgItem(IDC_BUTTON4)->EnableWindow(op);
}

UINT AdjustW0Thread(LPVOID pParam) {
    MainDialog* pWnd = (MainDialog*)pParam;
    AdjustButton(pWnd, FALSE);
    signal = AdjustSignal::NEED_W0;
    while (signal != AdjustSignal::FINISHED);
    pWnd->W0 = AdjustW0;
    AdjustW0 = -1;
    signal = AdjustSignal::IDLE;
    AdjustButton(pWnd, TRUE);
    pWnd->ShowAdjustText(L"烟密计全吸收校准成功");
    return TRUE;
}

UINT AdjustW1Thread(LPVOID pParam) {
    MainDialog* pWnd = (MainDialog*)pParam;
    AdjustButton(pWnd, FALSE);
    signal = AdjustSignal::NEED_W1;
    while (signal != AdjustSignal::FINISHED);
    pWnd->W1 = AdjustW1;
    AdjustW1 = -1;
    signal = AdjustSignal::IDLE;
    AdjustButton(pWnd, TRUE);
    pWnd->ShowAdjustText(L"烟密计全反射校准成功");
    return TRUE;
}

UINT ClearW0Thread(LPVOID pParam) {
    MainDialog* pWnd = (MainDialog*)pParam;
    AdjustButton(pWnd, FALSE);
    Sleep(3000); // 显得好看
    pWnd->W0 = 0;
    AdjustButton(pWnd, TRUE);
    pWnd->ShowAdjustText(L"烟密计全吸收清除成功");
    return TRUE;
}

UINT ClearW1Thread(LPVOID pParam) {
    MainDialog* pWnd = (MainDialog*)pParam;
    AdjustButton(pWnd, FALSE);
    Sleep(3000); // 显得好看
    pWnd->W1 = pWnd->W1_th;
    AdjustButton(pWnd, TRUE);
    pWnd->ShowAdjustText(L"烟密计全反射清除成功");
    return TRUE;
}

void MainDialog::OnBnClickedButton6()
{
    int idx = pComboBox->GetCurSel();
    idx = pComboBox->GetCount() - idx - 1;
    if (cdriver.OpenDevice(idx) != ErrType::OK) {
        MessageBox(_T("打开指定串行通信口失败！"), _T("烟雾光学密度计"), MB_ICONWARNING | MB_OK);
    }
    else if (cdriver.SendData(0xAA) != ErrType::OK) {
        MessageBox(_T("打开指定串行通信口失败！"), _T("烟雾光学密度计"), MB_ICONWARNING | MB_OK);
    }
    else {
        LoopInterrupt = false;
        BEGIN();
        CWinThread* m_pMainLoopThread;
        m_pMainLoopThread = AfxBeginThread(MainLoop, this);
    }
}

void MainDialog::OnBnClickedButton5()
{
    int idx = pComboBox->GetCurSel();
    if (cdriver.ReleaseDevice(idx) != ErrType::OK) {
        MessageBox(_T("关闭指定串行通信口失败！"), _T("烟雾光学密度计"), MB_ICONWARNING | MB_OK);
    }
    else {
        LoopInterrupt = true;
        AdjustW0 = -1, AdjustW1 = -1, AdjustWt = -1;
        begin_t = -1;
        signal = AdjustSignal::IDLE;
        STOP();
    }
}


BOOL MainDialog::DestroyWindow()
{
    std::ostringstream oss;
    oss << "d=" << std::fixed << std::setprecision(3) << this->d << std::endl;
    oss << "W0=" << std::fixed << std::setprecision(3) << this->W0 << std::endl;
    oss << "W1=" << std::fixed << std::setprecision(3) << this->W1 << std::endl; 
    oss << "TEMPFACTOR=" << std::fixed << std::setprecision(3) << this->TEMPFACTOR << std::endl;
    std::string content = oss.str();

    std::ofstream outFile("SDMeter.ini");
    if (outFile.is_open()) {
        outFile << content << std::endl;
        outFile.close();
    }

    return CDialogEx::DestroyWindow();
}


void MainDialog::OnBnClickedButton7()
{
    ParamSetDialog dialogPS(this->d, this);
    if (dialogPS.DoModal() == IDOK) {
        this->d = dialogPS.d;

        std::wostringstream wos;
        wos << L"d=" << std::fixed << std::setprecision(3) << d << L"m";
        std::wstring formatted_d = wos.str();
        GetDlgItem(IDC_BUTTON7)->SetWindowTextW(formatted_d.c_str());
    }
}

void MainDialog::OnTimer(UINT_PTR nIDEvent)
{
    GetDlgItem(IDC_STATIC_ADJ)->SetWindowTextW(_T(""));
    KillTimer(m_timerId);
    m_timerId = 0;
    CDialogEx::OnTimer(nIDEvent);
}

void MainDialog::OnBnClickedButton2() // W0校准
{
    ShowAdjustText(L"烟密计开始全吸收校准");
    CWinThread* m_pAdjustW0Thread;
    m_pAdjustW0Thread = AfxBeginThread(AdjustW0Thread, this);
}

void MainDialog::OnBnClickedButton4() // W1校准
{
    ShowAdjustText(L"烟密计开始全反射校准");
    CWinThread* m_pAdjustW1Thread;
    m_pAdjustW1Thread = AfxBeginThread(AdjustW1Thread, this);
}


void MainDialog::OnBnClickedButton1() // W0清除
{
    ShowAdjustText(L"烟密计开始全吸收清除");
    CWinThread* m_pClearW0Thread;
    m_pClearW0Thread = AfxBeginThread(ClearW0Thread, this);
}


void MainDialog::OnBnClickedButton3() // W1清除
{
    ShowAdjustText(L"烟密计开始全反射清除");
    CWinThread* m_pClearW1Thread;
    m_pClearW1Thread = AfxBeginThread(ClearW1Thread, this);
}

void MainDialog::TODOFunction()
{
    MessageBox(_T("本功能锐意制作中，敬请期待！"), _T("烟雾光学密度计"), MB_ICONWARNING | MB_OK);
}

void MainDialog::ExitFunction()
{
    EndDialog(0);
}

void MainDialog::AboutFunction()
{
    MessageBox(this->AboutInformation.c_str(), _T("关于 烟雾光学密度计"), MB_ICONINFORMATION | MB_OK);
}