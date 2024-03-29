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



std::string calcFEN(Output maps[10][9]) {
	std::string fen;
	boolean isRed = false;
	for (int i = 0; i < 10; i++)
	{
		int blankNums = 0;
		for (int j = 0; j < 9; j++)
		{
			if (maps[i][j].id == 4)
			{
				if (i <= 4) {
					isRed = false;
				}
				else {
					isRed = true;
				}
			}
			if (maps[i][j].id == -1) {
				blankNums++;
				if (blankNums!=0 && j == 8)
				{
					fen += std::to_string(blankNums);
					blankNums = 0;
				}
			}
			else {
				if (blankNums != 0)
				{
					fen += std::to_string(blankNums);
					blankNums = 0;
				}
				std::vector<std::string> className = { "R", "N", "B", "A", "K", "C", "P", "r", "n", "b","a", "k", "c", "p" };
				fen += className[maps[i][j].id];
			}
		}
		if (i == 9)
		{
			if (!isRed)
			{
				std::reverse(fen.begin(), fen.end());
			}
			fen +=  isRed?" w":" b";
			//这里默认跳出循环了
		}
		else {
			fen += "/";
		}
		
	}
	return fen;
}




d3d9 d3d;
Yolo yolo;

cv::dnn::Net net;
HWND gameHwnd;

// MainDlg 消息处理程序

DWORD WINAPI drawThread(LPVOID lpParam) {

	std::string modelPath = "best.onnx";
	if (yolo.readModel(net, modelPath, false)) {
		std::cout << "read net ok!" << std::endl;
	}
	else {
		std::cout << "read onnx model failed!";
	}

	while (true) {
		HBITMAP bitmap = Utils::WindowCapture_Front(gameHwnd);
		Utils::saveBitMap(L"1.png", bitmap);

		cv::Mat img = cv::imread("1.png");
		int width = img.size().width;
		int height = img.size().height;
		std::cout << width << " " << height << std::endl;

		std::vector<Output> result;
		yolo.Detect(img, net, result);

		//开始冒泡排序坐标y
		for (int i = 0; i < result.size(); i++)
		{
			for (int j = 1; j < result.size() - i; j++)
			{
				if (result[j - 1].box.y > result[j].box.y) {
					Output temp = result[j-1];
					result[j - 1] = result[j];
					result[j] = temp;
				}
			}
		}
		Output maps[10][9];
		float left_top_Point[2] = {width*208/838,height*55/624};
		float margin = width * 55 / 838;
		
		for (int i = 0; i < result.size(); i++)
		{
			int xIndex = (int)round((result[i].box.x + (result[i].box.width/2) - left_top_Point[0]) / margin);
			int yIndex = (int)round((result[i].box.y + (result[i].box.height / 2) - left_top_Point[1]) / margin);
			if (yIndex <= 9 && xIndex <= 8)
			{
				maps[yIndex][xIndex] = result[i];
			}
			else {
				//有不成立因素
			}
			
		}

		printf("fen:%s\n", calcFEN(maps).c_str());

	
		std::vector<std::string> className = { "车", "马", "相", "仕", "帅", "炮", "兵", "车", "马", "象","士", "将", "炮", "卒" };
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (maps[i][j].id == -1)
				{
					printf("  ");
				}
				else {
					printf("%s", className[maps[i][j].id].c_str());
				}
				
			}
			printf("\n");
		}
		


		d3d.clear();
		for (int i = 0; i < result.size(); i++)
		{
			d3d.drawHollowRect(result[i].box.x, result[i].box.y, result[i].box.width, result[i].box.height, 4.0f, D3DCOLOR_XRGB(255, 0, 0));
			
		
		}

		Sleep(100);
	}
	//}
}



//cv::Mat hwnd2mat(HWND hwnd) {

   // HDC hwindowDC, hwindowCompatibleDC;

   // int height, width, srcheight, srcwidth;
   // HBITMAP hbwindow;
   // cv::Mat src;
   // BITMAPINFOHEADER  bi;

   // hwindowDC = GetDC(hwnd);
   // hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
   // SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

   // RECT windowsize;    // get the height and width of the screen
   // GetClientRect(hwnd, &windowsize);

   // srcheight = windowsize.bottom;
   // srcwidth = windowsize.right;
   // height = windowsize.bottom / 2;  //change this to whatever size you want to resize to
   // width = windowsize.right / 2;

   // src.create(height, width, CV_8UC4);

   // // create a bitmap
   // hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
   // bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
   // bi.biWidth = width;
   // bi.biHeight = -height;  //this is the line that makes it draw upside down or not
   // bi.biPlanes = 1;
   // bi.biBitCount = 32;
   // bi.biCompression = BI_RGB;
   // bi.biSizeImage = 0;
   // bi.biXPelsPerMeter = 0;
   // bi.biYPelsPerMeter = 0;
   // bi.biClrUsed = 0;
   // bi.biClrImportant = 0;

   // // use the previously created device context with the bitmap
   // SelectObject(hwindowCompatibleDC, hbwindow);
   // // copy from the window device context to the bitmap device context
   // StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
   // GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

   // // avoid memory leak
   // DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

   // return src;
//}
//// create a bitmap
//HBITMAP hbDesktop = CreateCompatibleBitmap(hDesktopDC, width, height);

//cv::Mat src(height, width, CV_8UC4);
//src.data = (uchar*)hbDesktop;

//imshow("output", src);  //fails :(

//return;

#include "http.h"
void MainDlg::OnBnClickedButtonReaddata()
{

	http http;
	http.open("http://www.chessdb.cn/chessdb.php?action=queryall&board=r1bakabnr/9/1cn4c1/p1p1p1p1p/9/9/P1P1P1P1P/4C2C1/9/RNBAKABNR%20w");
	http.get();

	return;

	
	gameHwnd = FindWindowExA(FindWindowA(NULL, "天天象棋"), 0, "Intermediate D3D Window", "");
	RECT rect;
	::GetWindowRect(gameHwnd, &rect);
	d3d.initD3d(rect.right - rect.left, rect.bottom - rect.top);
	d3d.showWindow(gameHwnd);


	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)drawThread, NULL, 0, NULL);
	//printf("123");

}
