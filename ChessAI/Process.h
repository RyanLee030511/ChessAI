#pragma once
#include<string>
#include<vector>
#include<atlconv.h>
#include"Utils.h"
class Process
{
private:
	HANDLE hOutRead, hOutWrite;
	HANDLE hInRead, hInWrite;

	std::vector<std::string> cmdLines;

public:
	void createProcess(std::string process);
	void addCmdLine(std::string cmdLine);
	std::string execute(std::string endFlag);
};

