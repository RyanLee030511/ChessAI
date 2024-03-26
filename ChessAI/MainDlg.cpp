// MainDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "MainDlg.h"

#include "yolov7.h"
#include "d3d9.h"
// MainDlg 对话框

IMPLEMENT_DYNAMIC(MainDlg, CDialogEx)

MainDlg::MainDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MAIN, pParent)
{

}

MainDlg::~MainDlg()
{
}

void MainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MainDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_READDATA, &MainDlg::OnBnClickedButtonReaddata)
END_MESSAGE_MAP()


// MainDlg 消息处理程序


void MainDlg::OnBnClickedButtonReaddata()
{


	
	HBITMAP bitmap = Utils::WindowCapture_Front((HWND)0x00172636);
	Utils::saveBitMap(L"1.png", bitmap);
	cv::Mat img = cv::imread("1.png");


	//cv::Mat img;
	//Utils::HBitmap2Mat(bitmap, img);

	

	std::cout << img.size().width << " " << img.size().height << std::endl;

	std::string modelPath = "best.onnx";
	Yolo yolo;
	cv::dnn::Net net;
	if (yolo.readModel(net, modelPath, false)) {
		std::cout << "read net ok!" << std::endl;
	}
	else {
		std::cout << "read onnx model failed!";
	}
	std::vector<Output> result;
	yolo.Detect(img, net, result);

	d3d9 d3d;
	for (int i = 0; i < result.size(); i++)
	{
		d3d.drawHollowRect(result[i].box.x, result[i].box.y, result[i].box.width, result[i].box.height, 4.0f, D3DCOLOR_XRGB(255, 0, 0));
	}
	d3d.initD3d();
	d3d.showWindow();

	return;
	// TODO: 在此添加控件通知处理程序代码
	/*std::string imgPath = "./1008.png";
	std::string modelPath = "best.onnx";

	Yolo yolo;
	cv::dnn::Net net;
	if (yolo.readModel(net, modelPath, false)) {
		std::cout << "read net ok!" << std::endl;
	}
	else {
		std::cout << "read onnx model failed!";
	}

	cv::Mat img = cv::imread(imgPath);

	std::vector<Output> result;
	yolo.Detect(img, net, result);*/

	printf("123");

}
