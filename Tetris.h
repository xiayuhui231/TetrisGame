#pragma once
#include<vector>
#include<graphics.h>
#include "Block.h"
using namespace std;

class Tetris
{
public:
	// 构造函数，用于初始化参数
	Tetris(int rows, int cols, int left, int top, int blockSize);

	void init(); // 初始化

	void play(); // 开始游戏

	IMAGE imgOver;
	IMAGE imgWin;

private:
	void keyEvent(); // 内部用

	void updateWindow(); // 渲染界面
	int getDelay(); // 返回距离上一次调用该函数的时间，隔了多少秒
	void drop(); // 下落
	void clearLine(); // 消除满行
	void moveleftRight(int offset);
	void rotate(); // 旋转
	void drawScore();
	void checkOver();
	void saveScore();
	void displayOver();

private:
	int delay;
	bool update;
	vector<vector<int>> map; // 创建一个二维数字，存放的背景图片矩阵

	// 背景图像参数
	int rows;
	int cols;
	int leftMargin;
	int topMargin;
	int blockSize;
	bool gameOver;
	IMAGE imgBg; // 背景图片

	Block* curBlock;
	Block* nextBlock;
	Block  backBlock; // 当前方块降落过程，用来备份上一个位置， 不用指针是因为不希望备用的方块和当前方块指向同一个内存，一个动，另一个也动了

	int score; // 当前分数
	int level; // 当前关卡
	int lineCount; // 当前已经消除多少行
	int highestScore; // 最高分
};

