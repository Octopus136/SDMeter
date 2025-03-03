// GetCOMDialog.cpp: 实现文件
//

#include "pch.h"
#include "SDMeter.h"
#include "afxdialogex.h"
#include "GetCOMDialog.h"


// GetCOMDialog 对话框

IMPLEMENT_DYNAMIC(GetCOMDialog, CDialogEx)

GetCOMDialog::GetCOMDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GETOTHERCOM, pParent)
{

}

GetCOMDialog::~GetCOMDialog()
{
}

void GetCOMDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GetCOMDialog, CDialogEx)
	ON_BN_CLICKED(IDOK_C, &GetCOMDialog::OnBnClickedOK)
	ON_BN_CLICKED(IDCANCEL_C, &GetCOMDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// GetCOMDialog 消息处理程序
BOOL GetCOMDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化

    // 各种控件的字体设置
    for (int i = 0; i < 6; i++) {
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

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void GetCOMDialog::OnBnClickedOK()
{
    CString value;
    GetDlgItem(IDC_EDIT_C1)->GetWindowTextW(value);
    std::string str = CT2A(value);
    if (str.length() >= 5) {
        MessageBox(_T("请输入一个1-256的数字。"), _T("烟雾光学密度计"), MB_ICONWARNING | MB_OK);
    }
    else {
        com = std::stoi(str);
        if (com < 1 || com > 256) {
            MessageBox(_T("请输入一个1-256的数字。"), _T("烟雾光学密度计"), MB_ICONWARNING | MB_OK);
        }
        else {
            OnOK();
        }
    }
}

void GetCOMDialog::OnBnClickedCancel()
{
    OnCancel();
}
