
// ChessAIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ChessAI.h"
#include "ChessAIDlg.h"
#include "afxdialogex.h"
#include <locale>



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChessAIDlg 对话框



CChessAIDlg::CChessAIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHESSAI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChessAIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_tab);
}

BEGIN_MESSAGE_MAP(CChessAIDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB, &CChessAIDlg::OnTcnSelchangeTab)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB, &CChessAIDlg::OnTcnSelchangingTab)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CChessAIDlg 消息处理程序

BOOL CChessAIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	//设置控制台支持中文
	setlocale(LC_ALL, "Chinese-simplified");


	m_tab.InsertItem(0, _T("主页"));
	m_tab.InsertItem(1, _T("截图"));
	m_tab.InsertItem(2, _T("训练"));
	m_tab.InsertItem(3, _T("设置"));
	m_tab.InsertItem(4, _T("批量改名"));

	m_ScreenShotDlg.Create(IDD_DIALOG_SCREENSHOT);
	m_MainDlg.Create(IDD_DIALOG_MAIN);
	m_TrainDlg.Create(IDD_DIALOG_TRAIN);
	m_SettingDlg.Create(IDD_DIALOG_SETTING);
	m_RenameDlg.Create(IDD_DIALOG_RENAME);

	CRect rect;
	m_tab.GetClientRect(&rect);
	rect.top += 30;
	rect.bottom -= 5;
	rect.left += 5;
	rect.right -= 5;
	m_MainDlg.MoveWindow(&rect);
	m_MainDlg.SetParent(&m_tab);
	m_MainDlg.ShowWindow(FALSE);
	m_ScreenShotDlg.MoveWindow(&rect);
	m_ScreenShotDlg.SetParent(&m_tab);
	m_ScreenShotDlg.ShowWindow(TRUE);
	m_TrainDlg.MoveWindow(&rect);
	m_TrainDlg.SetParent(&m_tab);
	m_TrainDlg.ShowWindow(FALSE);
	m_SettingDlg.MoveWindow(&rect);
	m_SettingDlg.SetParent(&m_tab);
	m_SettingDlg.ShowWindow(FALSE);
	m_RenameDlg.MoveWindow(&rect);
	m_RenameDlg.SetParent(&m_tab);
	m_RenameDlg.ShowWindow(FALSE);
	m_tab.SetCurSel(1);


	AllocConsole();//控制台调试窗口开启  
	freopen("CONOUT$", "w", stdout);//开启中文控制台输出支持  

	//设置默认目录


	WCHAR desktopPath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);

	CEditView* CEditView_SaveDir = (CEditView*)m_ScreenShotDlg.GetDlgItem(IDC_EDIT_SAVEDIR);
	CEditView_SaveDir->SetWindowTextW(desktopPath);
	CEditView* CEditView_TrainOutDir = (CEditView*)m_TrainDlg.GetDlgItem(IDC_EDIT_TRAINOUTDIR);
	CEditView_TrainOutDir->SetWindowTextW(desktopPath);





	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChessAIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChessAIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CChessAIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CChessAIDlg::OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	cursel = m_tab.GetCurSel();
	switch (cursel)
	{

	case 0:
		m_MainDlg.ShowWindow(TRUE);
		m_ScreenShotDlg.ShowWindow(FALSE);
		m_TrainDlg.ShowWindow(FALSE);
		m_SettingDlg.ShowWindow(FALSE);
		m_RenameDlg.ShowWindow(FALSE);
		break;
	case 1:
		m_MainDlg.ShowWindow(FALSE);
		m_ScreenShotDlg.ShowWindow(TRUE);
		m_TrainDlg.ShowWindow(FALSE);
		m_SettingDlg.ShowWindow(FALSE);
		m_RenameDlg.ShowWindow(FALSE);
		break;
	case 2:
		m_MainDlg.ShowWindow(FALSE);
		m_ScreenShotDlg.ShowWindow(FALSE);
		m_TrainDlg.ShowWindow(TRUE);
		m_SettingDlg.ShowWindow(FALSE);
		m_RenameDlg.ShowWindow(FALSE);
		break;
	case 3:
		m_MainDlg.ShowWindow(FALSE);
		m_ScreenShotDlg.ShowWindow(FALSE);
		m_TrainDlg.ShowWindow(FALSE);
		m_SettingDlg.ShowWindow(TRUE);
		m_RenameDlg.ShowWindow(FALSE);
		break;
	case 4:
		m_MainDlg.ShowWindow(FALSE);
		m_ScreenShotDlg.ShowWindow(FALSE);
		m_TrainDlg.ShowWindow(FALSE);
		m_SettingDlg.ShowWindow(FALSE);
		m_RenameDlg.ShowWindow(TRUE);
		break;
	default:
		break;
	}
	*pResult = 0;
}


void CChessAIDlg::OnTcnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	cursel = m_tab.GetCurSel();
	*pResult = 0;
}

