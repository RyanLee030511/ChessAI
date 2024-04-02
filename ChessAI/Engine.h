#pragma once
#include<string>
#include"http.h"
#include"Utils.h"
#include"Process.h"
class stepIdx
{
public:
	int beginX = -1;
	int beginY = -1;
	int endX = -1;
	int endY = -1;
public:
	stepIdx() {

	}
	stepIdx(int beginX,int beginY,int endX,int endY) {
		this->beginX = beginX;
		this->beginY = beginY;
		this->endX = endX;
		this->endY = endY;
	}
	boolean isValidate() {
		if (beginX!=-1 && beginY!=-1 && endX != -1 && endY != -1)
		{
			return true;
		}
		return false;
	}
	void print() {
		printf("(%d.%d)==>(%d.%d)\n", beginX, beginY, endX, endY);
	}
	void setBeginX(int beginX) {
		this->beginX = beginX;
	}
	void setBeginY(int beginY) {
		this->beginY = beginY;
	}
	void setEndX(int endX) {
		this->endX = endX;
	}
	void setEndY(int endY) {
		this->endY = endY;
	}
	void set(int beginX, int beginY, int endX, int endY) {
		this->beginX = beginX;
		this->beginY = beginY;
		this->endX = endX;
		this->endY = endY;
	}
};


class Engine
{
public:
	virtual std::string calcStep(std::string fen) { return ""; }

	static int getNumsByRowFlag(std::string rowFlag) {
		std::string rowFlags[9] = { "a","b","c","d","e","f","g","h","i" };
		for (int i = 0; i < 9; i++)
		{
			if (rowFlags[i].compare(rowFlag) == 0) {
				return i;
			}
		}
		return -1;
	}


	static stepIdx getStepIdx(std::string step, std::string fen) {
		stepIdx stepIdx;

		boolean isRed;
		if (fen.find("w") != std::string::npos)
		{
			//红
			isRed = true;
		}
		else {
			isRed = false;
		}

		int row_begin = 0, col_begin = 0, row_end = 0, col_end = 0;
		try {

			
			if (step.size() == 4)
			{
				std::string s1 = step.substr(0, 1);
				std::string s2 = step.substr(1, 1);
				std::string s3 = step.substr(2, 1);
				std::string s4 = step.substr(3, 1);
				row_begin = getNumsByRowFlag(s1);
				col_begin = atoi(s2.c_str());
				row_end = getNumsByRowFlag(s3);
				col_end = atoi(s4.c_str());
				if (isRed)
				{
					stepIdx.set(9 - col_begin, row_begin, 9- col_end , row_end);
				}
				else {
					stepIdx.set(col_begin, 8 - row_begin, col_end, 8 - row_end);
				}
				
				printf("(%d.%d)==>(%d.%d)\n", row_begin, col_begin, row_end, col_end);
				stepIdx.print();
			}
		}
		catch (std::exception e) {

		}
		return stepIdx;
	}

	
};

class Engine_yunku : public Engine
{
public :
	virtual std::string calcStep(std::string fen) {
		http http;
		http.open("http://www.chessdb.cn/chessdb.php?action=queryall&showall=1&board=" + Utils::ReplaceAllText(fen," ","%20"));
		std::string body = http.get();

		printf("body: %s\n", body.c_str());

		int idx = body.find("move:") + 5;
		std::string runStep = body.substr(idx, 4);

		return runStep;
	}
};

class Engine_pikafish : public Engine	
{
public:
	virtual std::string calcStep(std::string fen) {
		Process process;
		process.createProcess("C:\\Users\\Administrator\\Desktop\\pikayu\\pikafish-avx2.exe");
		process.addCmdLine("position fen "+ fen);
		process.addCmdLine("go depth 20");
		


		std::string runStep;
		try {
			std::string result = process.execute("bestmove");
			int idx1 = result.find("bestmove");
			int idx2 = result.find("ponder");
			if (idx1 != std::string::npos && idx2 == std::string::npos)
			{
				//最后一步绝杀
				runStep = result.substr(idx1 + 9, 4);
			}
			else {
				runStep = result.substr(idx1 + 9, idx2 - 1 - (idx1 + 9));
			}
		}
		catch (std::exception e) {

		}
		return runStep;
	}
};
