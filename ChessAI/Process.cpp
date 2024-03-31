#include "pch.h"
#include "Process.h"
#include<iostream>
using namespace std;
void Process::createProcess(std::string process)
{
	USES_CONVERSION;

	//设置管道安全属性
	SECURITY_ATTRIBUTES sa = SECURITY_ATTRIBUTES();
	sa.bInheritHandle = TRUE;
	//sa.nLength = sizeof(sa);
	//sa.lpSecurityDescriptor = NULL;
	//创建匿名管道
	CreatePipe(&hOutRead, &hOutWrite, &sa, 0);
	CreatePipe(&hInRead, &hInWrite, &sa, 0);


	SetHandleInformation(hOutRead, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(hInWrite, HANDLE_FLAG_INHERIT, 0);

	//设置新进程参数
	PROCESS_INFORMATION pi = PROCESS_INFORMATION();

	STARTUPINFOA si = STARTUPINFOA();
	si.cb = sizeof(si);
	//si.wShowWindow = SW_NORMAL;
	si.dwFlags = /*STARTF_USESHOWWINDOW |*/ STARTF_USESTDHANDLES;
	si.hStdInput = hInRead;
	si.hStdOutput = hOutWrite;
	//si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	std::string currentDir;
	int idx = process.find_last_of("\\");
	currentDir = process.substr(0, idx);



	//process += " \r\n isready \r\n go";
	CreateProcessA(NULL, const_cast<char*>(process.c_str()), NULL, NULL, TRUE, CREATE_NEW_PROCESS_GROUP, NULL, currentDir.c_str(), &si, &pi);

	// not needed
	CloseHandle(hOutWrite);
	CloseHandle(hInRead);


	//::WaitForSingleObject(pi.hThread, 2000);
	//::WaitForSingleObject(pi.hProcess, 2000);
}


void Process::addCmdLine(std::string cmdLine)
{
	cmdLines.push_back(cmdLine);
}

std::string Process::execute()
{
	std::string cmd;
	for (int i = 0; i < cmdLines.size(); i++)
	{
		cmd += cmdLines[i] + "\r\n";
	}
	WriteFile(hInWrite, cmd.c_str(), cmd.length(), NULL, NULL);

	//std::string buf = "isready\r\ngo\r\n";
	//bool ret = WriteFile(hInWrite, buf.c_str(), buf.length(), NULL, NULL);
	//if (!ret)
	//{
	//	cout << "写入管道数据失败。" << endl;
	//}
	//else
	//{
	//	cout << "写入管道数据成功。" << endl;
	//}


	std::string strRet;
	char resultbuffer[10000]{ 0 };
	DWORD dwRead = 0;
	while (true)
	{
		memset(resultbuffer, 0, 10000);
		if (!ReadFile(hOutRead, resultbuffer, sizeof(resultbuffer), &dwRead, NULL))
		{
			break;
		}
		strRet += std::string(resultbuffer, dwRead);

		printf("%s", resultbuffer);

		if (strRet.find("bestmove") != std::string::npos)
		{
			break;
		}

	}
	printf("\nall:%s", strRet.c_str());


	return std::string();
}
