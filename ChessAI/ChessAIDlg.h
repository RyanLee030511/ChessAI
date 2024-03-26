
// ChessAIDlg.h: 头文件
//

#pragma once

#include "MainDlg.h"
#include "ScreenShotDlg.h"
#include "TrainDlg.h"
#include "SettingDlg.h"
#include "RenameDlg.h"


// CChessAIDlg 对话框
class CChessAIDlg : public CDialogEx
{
// 构造
public:
	CChessAIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHESSAI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_tab;
	ScreenShotDlg m_ScreenShotDlg;
	MainDlg m_MainDlg;
	TrainDlg m_TrainDlg;
	SettingDlg m_SettingDlg;
	RenameDlg m_RenameDlg;
	int cursel;
	afx_msg void OnTcnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTcnSelchangingTab(NMHDR* pNMHDR, LRESULT* pResult);
};
