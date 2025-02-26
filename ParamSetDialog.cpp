// ParamSetDialog.cpp: 实现文件
//

#include "pch.h"
#include "SDMeter.h"
#include "afxdialogex.h"
#include "ParamSetDialog.h"


// ParamSetDialog 对话框

IMPLEMENT_DYNAMIC(ParamSetDialog, CDialogEx)

ParamSetDialog::ParamSetDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PARAMSET, pParent)
{

}

ParamSetDialog::ParamSetDialog(double _d, CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_PARAMSET, pParent), d(_d)
{

}

ParamSetDialog::~ParamSetDialog()
{
}

void ParamSetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ParamSetDialog, CDialogEx)
    ON_BN_CLICKED(IDOK_P, &ParamSetDialog::OnBnClickedOK)
    ON_BN_CLICKED(IDCANCEL_P, &ParamSetDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// ParamSetDialog 消息处理程序


BOOL ParamSetDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

    // 各种控件的字体设置
    for (int i = 0; i < 7; i++) {
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

    std::wostringstream wos;
    wos << std::fixed << std::setprecision(3) << d;
    std::wstring formatted_d = wos.str();
    GetDlgItem(IDC_EDIT_P1)->SetWindowTextW(formatted_d.c_str());

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

bool isNumber(const std::string& str) {
    std::regex pattern(R"(([-+]?\d*\.\d+)|([-+]?\d+))");
    return std::regex_match(str, pattern);
}


void ParamSetDialog::OnBnClickedOK()
{
    CString value;
    GetDlgItem(IDC_EDIT_P1)->GetWindowTextW(value);
    std::string str = CT2A(value);
    if (!isNumber(str)) {
        MessageBox(_T("请输入一个数字。"), _T("烟雾光学密度计"), MB_ICONWARNING | MB_OK);
    }
    else {
        d = std::stod(str);
        OnOK();
    }
}

void ParamSetDialog::OnBnClickedCancel()
{
    OnCancel();
}


