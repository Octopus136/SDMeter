#pragma once
#include "afxdialogex.h"
#include <sstream>
#include <iomanip>
#include "regex"


// GetCOMDialog 对话框

class GetCOMDialog : public CDialogEx
{
	DECLARE_DYNAMIC(GetCOMDialog)

public:
	int com;
	GetCOMDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~GetCOMDialog();

	struct StaticFontInfo {
		int nID;
		CString fontName;
		int fontSize;
		bool fontBold;
	};

	StaticFontInfo fontinfo[6] = {
		{IDC_STATIC_C1, _T("宋体"), 16, false}, // 外框
		{IDC_STATIC_C2, _T("宋体"), 16, false}, // COM
		{IDC_STATIC_C4, _T("宋体"), 16, false}, // 说明
		{IDC_EDIT_C1, _T("宋体"), 16, false}, // 输入框
		{IDOK_C, _T("宋体"), 16, false}, // OK
		{IDCANCEL_C, _T("宋体"), 16, false} // 取消
	};

	CFont mFont[6];

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GETOTHERCOM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedCancel();
	DECLARE_MESSAGE_MAP()
};
