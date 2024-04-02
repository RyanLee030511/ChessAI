
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
	m_MainDlg.ShowWindow(TRUE);
	m_ScreenShotDlg.MoveWindow(&rect);
	m_ScreenShotDlg.SetParent(&m_tab);
	m_ScreenShotDlg.ShowWindow(FALSE);
	m_TrainDlg.MoveWindow(&rect);
	m_TrainDlg.SetParent(&m_tab);
	m_TrainDlg.ShowWindow(FALSE);
	m_SettingDlg.MoveWindow(&rect);
	m_SettingDlg.SetParent(&m_tab);
	m_SettingDlg.ShowWindow(FALSE);
	m_RenameDlg.MoveWindow(&rect);
	m_RenameDlg.SetParent(&m_tab);
	m_RenameDlg.ShowWindow(FALSE);
	m_tab.SetCurSel(0);


	AllocConsole();//控制台调试窗口开启  
	freopen("CONOUT$", "w", stdout);//开启中文控制台输出支持  

	//设置默认目录


	WCHAR desktopPath[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, 0, desktopPath);

	CEditView* CEditView_SaveDir = (CEditView*)m_ScreenShotDlg.GetDlgItem(IDC_EDIT_SAVEDIR);
	CEditView_SaveDir->SetWindowTextW(desktopPath);
	CEditView* CEditView_TrainOutDir = (CEditView*)m_TrainDlg.GetDlgItem(IDC_EDIT_TRAINOUTDIR);
	CEditView_TrainOutDir->SetWindowTextW(desktopPath);


	CMenu menu;
	menu.LoadMenuW(IDR_MENU1);
	SetMenu(&menu);
	//menu.Detach();//防止窗口创建后menu2被释放，再按选项


	//设置状态栏
	m_Statusbar.Create(this);                  //创造状态栏
	//m_Statusbar.SetIndicators(indicators, 3);   //设置状态栏项目栏数

	////伸缩窗格以填满空间，第一栏显示IDS_STRING_MESSAGE内容，即“状态栏” ，0表示默认长度 
	//m_Statusbar.SetPaneInfo(0, IDS_STRING_MESSAGE, SBPS_STRETCH, 0);
	////窗格突出显示，第二栏长度为320单位，显示IDS_STRING_VERSION字符串  
	//m_Statusbar.SetPaneInfo(1, IDS_STRING_VERSION, SBPS_POPOUT, 320);
	//m_Statusbar.SetPaneInfo(2, IDS_STRING_TIME, SBPS_POPOUT, 120); //第三栏长度120单位
	//RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, IDS_STRING_TIME);//指定状态栏在显示窗口位置

	UINT id[] = { 1000, 1001 };//一个id 是1000  一个是1001创建两个状态栏
	m_Statusbar.SetIndicators(id, 2);//这个2好像是你id数组的大小也就是下面有几个状态栏，这里是两个
	m_Statusbar.SetPaneInfo(0, 1000, SBPS_NORMAL, 100);//这里是设置状态栏 第一个参数0代表第一个状态栏 1的话是第二个，第二个参数是状态栏id，第三个是风格，第四个是宽度
	m_Statusbar.SetPaneInfo(1, 1001, SBPS_STRETCH, 10);//同上这个风格是自动增加的，最后一个参数写不写没什么意思
	//下面代码是取得本窗口矩形区域...把状态栏放在底部
	RECT clientRect;
	GetClientRect(&clientRect);
	m_Statusbar.MoveWindow(0, clientRect.bottom - 30, clientRect.right, 30, TRUE);//这里设置了状态栏高度
	m_Statusbar.SetPaneText(0, _T("ChessAI V1.0"), TRUE);//第一个0代表第一个状态栏1代表第二个依次... 第二个是要设置的文本，第三个不清粗
	m_Statusbar.SetPaneText(1, _T("这里是窗口初始化自带状态文本"), TRUE);//第一个0代表第一个状态栏1代表第二个依次... 第二个是要设置的文本，第三个不清粗



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

