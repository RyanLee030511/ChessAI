// MainDlg.cpp: 实现文件
//

#include "pch.h"
#include "ChessAI.h"
#include "afxdialogex.h"
#include "MainDlg.h"

#include "yolov7.h"
#include "d3d9.h"
#include "http.h"
#include "Engine.h"
#include "Process.h"

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
	DDX_Control(pDX, IDC_CHESSBOARD, m_chessboard);
}


BEGIN_MESSAGE_MAP(MainDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_READDATA, &MainDlg::OnBnClickedButtonReaddata)
END_MESSAGE_MAP()


int getNumsByRowFlag(std::string rowFlag) {
	std::string rowFlags[9] = { "a","b","c","d","e","f","g","h","i" };
	for (int i = 0; i < 9; i++)
	{
		if (rowFlags[i].compare(rowFlag) == 0) {
			return i;
		}
	}
	return -1;
}

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
	MainDlg *mainDlg = (MainDlg*)lpParam;


	std::string modelPath = "best.onnx";
	if (yolo.readModel(net, modelPath, false)) {
		std::cout << "read net ok!" << std::endl;
	}
	else {
		std::cout << "read onnx model failed!";
	}

	while (true) {
		HBITMAP bitmap = Utils::WindowCapture_Front(gameHwnd);
		//显示缩略图
		mainDlg->m_chessboard.SetBitmap(Utils::stretchBitMap(bitmap,300,280));

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
		
		//给空格也计算出大概的位置
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				maps[i][j].box.x = (int)(left_top_Point[0] + margin * j);
				maps[i][j].box.y = (int)(left_top_Point[1] + margin * i);
			}
		}

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
		
		Engine_yunku engineYunku;
		//获取最佳走法
		int row_begin = 0, col_begin = 0 , row_end = 0 , col_end = 0;
		try {

			std::string runStep = engineYunku.calcStep(calcFEN(maps));
			if (runStep.size() == 4)
			{
				std::string s1 = runStep.substr(0, 1);
				std::string s2 = runStep.substr(1, 1);
				std::string s3 = runStep.substr(2, 1);
				std::string s4 = runStep.substr(3, 1);
				row_begin = getNumsByRowFlag(s1);
				col_begin = atoi(s2.c_str());
				row_end = getNumsByRowFlag(s3);
				col_end = atoi(s4.c_str());
				printf("(%d.%d)==>(%d.%d)\n", row_begin, col_begin, row_end, col_end);
			}
		}
		catch (std::exception e) {

		}



		d3d.clear();
		//绘制棋子方框
		for (int i = 0; i < result.size(); i++)
		{
			d3d.drawHollowRect(result[i].box.x, result[i].box.y, result[i].box.width, result[i].box.height, 4.0f, D3DCOLOR_XRGB(255, 0, 0));
		}
		//绘制最佳行棋路线
		d3d.drawLine(maps[9 - col_begin][row_begin].box.x + maps[9 - col_begin][row_begin].box.width/2, maps[9 - col_begin][row_begin].box.y + maps[9 - col_begin][row_begin].box.height/2, maps[9 - col_end][row_end].box.x, maps[9 - col_end][row_end].box.y, 4.0f, D3DCOLOR_XRGB(255, 0, 0));

		printf("%d.%d --> %d.%d\n", maps[9 - col_begin][row_begin].box.x, maps[9 - col_begin][row_begin].box.y, maps[9 - col_end][row_end].box.x, maps[9 - col_end][row_end].box.y);

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



void MainDlg::OnBnClickedButtonReaddata()
{
	Process process;
	process.createProcess("C:\\Users\\Administrator\\Desktop\\pikayu\\pikafish-avx2.exe");
	//process.createProcess("C:\\Users\\Administrator\\Desktop\\pikayu\\pikafish-avx2.exe isready & go");
	process.addCmdLine("isready");
	process.addCmdLine("go depth 10");
	process.execute();
	return;


	gameHwnd = FindWindowExA(FindWindowA(NULL, "天天象棋"), 0, "Intermediate D3D Window", "");


	//HBITMAP bitmap = Utils::WindowCapture_Front(gameHwnd);
	//this->m_chessboard.SetBitmap(Utils::stretchBitMap(bitmap,300,280));
	//return;



	RECT rect;
	::GetWindowRect(gameHwnd, &rect);
	d3d.initD3d(rect.right - rect.left, rect.bottom - rect.top);
	d3d.showWindow(gameHwnd);


	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)drawThread, this, 0, NULL);
	//printf("123");

}
