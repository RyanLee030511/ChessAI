#pragma once
#include<string>
#include"http.h"
#include"Utils.h"
class step
{
public:
	int beginX = -1;
	int beginY = -1;
	int endX = -1;
	int endY = -1;
public:
	boolean isValidate() {
		if (beginX!=-1 && beginY!=-1 && endX != -1 && endY != -1)
		{
			return true;
		}
		return false;
	}
};


class Engine
{
public:
	virtual std::string calcStep(std::string fen) { return ""; }
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

	}
};