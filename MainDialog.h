#pragma once
#include "afxdialogex.h"
#include "COMDriver.h"
#include <map>
#include "ParamSetDialog.h"
#include <cmath>
#include <chrono>

// MainDialog 对话框

class MainDialog : public CDialogEx
{
	DECLARE_DYNAMIC(MainDialog)

public:
	MainDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~MainDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAINDIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnDeviceChange(UINT nType, DWORD_PTR dwData);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	// 字体相关
	#define fontSetNum 27

	struct StaticFontInfo {
		int nID;
		CString fontName;
		int fontSize;
		bool fontBold;
	};

	StaticFontInfo fontinfo[fontSetNum] = {
		{IDC_STATIC1, _T("宋体"), 16, false}, // 温度
		{IDC_STATIC2, _T("宋体"), 16, false}, // mV
		{IDC_STATIC3, _T("宋体"), 16, false}, // 原始值
		{IDC_STATIC4, _T("宋体"), 16, false}, // 全吸收
		{IDC_STATIC5, _T("宋体"), 16, false}, // 全反射
		{IDC_STATIC6, _T("宋体"), 16, false}, // 参数设置
		{IDC_STATIC7, _T("宋体"), 16, false}, // 控制
		{IDC_BUTTON1, _T("宋体"), 16, false}, // 全吸收清除
		{IDC_BUTTON2, _T("宋体"), 16, false}, // 全吸收校准
		{IDC_BUTTON3, _T("宋体"), 16, false}, // 全反射清除
		{IDC_BUTTON4, _T("宋体"), 16, false}, // 全反射校准
		{IDC_BUTTON5, _T("宋体"), 16, false}, // 停止
		{IDC_BUTTON6, _T("宋体"), 16, false}, // 开始
		{IDC_STATIC8, _T("微软雅黑"), 17, false}, // 状态栏
		{IDC_STATIC9, _T("宋体"), 25, true}, // 减光率
		{IDC_STATIC10, _T("宋体"), 25, true}, // 减光系数
		{IDC_STATIC11, _T("宋体"), 25, true}, // 读数
		{IDC_STATIC12, _T("宋体"), 54, true}, // 减光率
		{IDC_STATIC13, _T("宋体"), 54, true}, // 减光系数
		{IDC_STATIC14, _T("宋体"), 54, true}, // 读数
		{IDC_STATIC15, _T("宋体"), 16, false}, // 温度值
		{IDC_STATIC16, _T("宋体"), 16, false}, // 毫伏值
		{IDC_STATIC17, _T("宋体"), 16, false}, // 原始值
		{IDC_EDIT1, _T("宋体"), 14, true}, // 文本框
		{IDC_COMBO2, _T("宋体"), 16, false}, // 选择框
		{IDC_BUTTON7, _T("宋体"), 16, false}, // 光程
		{IDC_STATIC_ADJ, _T("宋体"), 16, false}, // 校准字样
	};

	CFont mFont[fontSetNum];

	// 顶部深色绘制
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnEnSetfocusEdit();
	void OnTimer(UINT_PTR nIDEvent);

	// 参数设置相关
	CComboBox* pComboBox;
	double d = 0.5;
	int W0 = 0, W1 = 2000;
	const int W1_th = 2000;
	double Wt = 21.0;
	double TEMPFACTOR = -0.848;
	std::map<std::string, std::string> config;
	afx_msg void OnCbnDropdownCombo();

	// 设备通知句柄
	HDEVNOTIFY m_hDeviceNotify;
	// 定时器
	UINT_PTR m_timerId;
	// 串口驱动
	COMDriver cdriver;
	// 菜单句柄
	CMenu* pMenu;
	void SetStatusText(LPCTSTR text);
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton5();

	inline void BEGIN();
	inline void STOP();
	inline void Button_CTRL(int op);
	inline void ShowAdjustText(const std::wstring& str);
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();

	// 供菜单调用
	afx_msg void ExitFunction();
	afx_msg void AboutFunction();
	afx_msg void ClearText();

	// TODO
	afx_msg void TODOFunction();

	std::wstring About = L"本产品由华中科技大学电信学院战翔宇开发。\n本产品遵循 Apache License 协议开源。\n部分功能仿照武汉华大安信有限公司开发的烟雾光写密度计-HI20110411型。\n";
	std::vector<std::wstring> Changelog = {
		L"\nVersion 1.0\n1. 完成了本产品的主要逻辑，部分附加功能仍在开发中。\n"
		L"\nVersion 1.1\n1. 左边的缓冲区支持写入信息和清除了。\n2. 温度计算和毫伏计算目前是正确的结果了。\n3. 更新了信号传递机制，校准不会被错误阻塞了。"
	};
	std::wstring AboutInformation = L"";

	// 左侧追加文本
	void AppendText(CString newText);
	CEdit* pEdit; // 左侧的那个栏
};
