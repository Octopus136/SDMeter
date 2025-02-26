#pragma once
#include "afxdialogex.h"
#include <sstream>
#include <iomanip>
#include "regex"


// ParamSetDialog 对话框

class ParamSetDialog : public CDialogEx
{
	DECLARE_DYNAMIC(ParamSetDialog)

public:
	double d;
	ParamSetDialog(CWnd* pParent = nullptr);   // 标准构造函数
	ParamSetDialog(double _d, CWnd* pParent = nullptr);   // 传参构造函数
	virtual ~ParamSetDialog();

	struct StaticFontInfo {
		int nID;
		CString fontName;
		int fontSize;
		bool fontBold;
	};

	StaticFontInfo fontinfo[7] = {
		{IDC_STATIC_P1, _T("宋体"), 16, false}, // 外框
		{IDC_STATIC_P2, _T("宋体"), 16, false}, // d=
		{IDC_STATIC_P3, _T("宋体"), 16, false}, // 米
		{IDC_STATIC_P4, _T("宋体"), 16, false}, // 说明
		{IDC_EDIT_P1, _T("宋体"), 16, false}, // 输入框
		{IDOK_P, _T("宋体"), 16, false}, // OK
		{IDCANCEL_P, _T("宋体"), 16, false} // 取消
	};

	CFont mFont[7];

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARAMSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOK();
	afx_msg void OnBnClickedCancel();
};
