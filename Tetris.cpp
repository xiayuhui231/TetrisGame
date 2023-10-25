#include "Tetris.h"
#include<stdlib.h>
#include<time.h>
#include<graphics.h>
#include"Block.h"
#include<conio.h>
#include<iostream>
#include<string>
#include<fstream>
#include<mmsystem.h>
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "WINMM.LIB")

using namespace std;

#define MAX_LEVLE 5
#define RECORD_FILE "record_file.txt"
const int SPEED_QUCKY = 50;
const int SPEED_NORMAL[MAX_LEVLE] = { 500, 300, 150, 100, 80 };

Tetris::Tetris(int rows, int cols, int left, int top, int blockSize)
{
	// 构造函数对成员函数进行赋值
	this->rows = rows; // 使用this是表示当前构造函数里面的参数，因为存在同名了
	this->cols = cols;
	this->leftMargin = left;
	this->topMargin = top;
	this->blockSize = blockSize;

	// 下面循环的目的是给map里面的每个元素赋初值0，思路是先给每一个一维的vector赋值0，再把一维的vector循环放到二维vector里面
	for (int i = 0; i < rows; i++)
	{
		vector<int> mapRow;
		for (int j = 0; j < cols; j++)
		{
			mapRow.push_back(0);
		}
		map.push_back(mapRow);
	};
}

void Tetris::init()
{
	// 播放背景音乐
	mciSendString("play bg.mp3 repeat", 0, 0, 0);  
	delay = SPEED_NORMAL[level - 1];

	//配置随机数种子
	srand(time(NULL));

	initgraph(938, 896);

	// 加载背景图片
	loadimage(&imgBg, "bg2.png");
	loadimage(&imgWin, "win.png");
	loadimage(&imgOver, "over.png");
	// 初始化游戏界面
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			map[i][j] = 0;
		}
	}

	score = 0;
	level = 1; // 当前关卡
	lineCount = 0; // 当前已经消除多少行

	// 从已有的文件里面去读取之前的最高分
	ifstream file(RECORD_FILE);
	if (!file.is_open()) {
		cout << RECORD_FILE << "打开失败" << endl;
		highestScore = 0;
	}
	else {
		file >> highestScore; // 从文件输出这个分数
	}
	file.close();

	gameOver = false;
}

void Tetris::play()
{
	init();
	
	nextBlock = new Block;
	curBlock = nextBlock;
	nextBlock = new Block;

	int timer = 0;
	// 所有的游戏都是一个死循环
	while (1)
	{
		BeginBatchDraw();
		//接收用户的输入
		keyEvent();

		timer += getDelay();
		if (timer > delay)
		{
			timer = 0;
			drop();
			update = true;
		}

		if (update)
		{
			// 更新游戏画面
			updateWindow();
			// 更新数据
			clearLine();
		}

		if (gameOver) {
			// 保存分数
			saveScore();
			// 更新游戏结束界面
			displayOver();
			system("pause");
			// 重新开局
			init(); 
		}
		EndBatchDraw();
	}
}

void Tetris::keyEvent()
{
	unsigned char ch;
	bool rotateFlag = false;
	int dx = 0;

	// 判断当前有没有按键输入，如果没有，则正常执行
	if (_kbhit()) {
		ch = _getch();
		if (ch == 224) {
			ch = _getch();

			// 按下字母，返回的是字母，按下方向键 ： 上 先后返回：224 72 ，下： 224 80， 左： 224 75， 右： 224 77
			switch (ch)
			{
			case 72:
				rotateFlag = true;
				break;
			case 80:
				delay = SPEED_QUCKY;
				break;
			case 75:
				dx = -1;
				break;
			case 77:
				dx = 1;
				break;
			default:
				break;
			}
		}
	}

	if (rotateFlag)
	{
		rotate();
		update = true;
		rotateFlag = false;
	}

	if (dx != 0) // 左右移动
	{
		moveleftRight(dx);
		update = true;
	}
}

void Tetris::updateWindow()
{
	BeginBatchDraw(); // 落到底部的方块的渲染

	putimage(0, 0, &imgBg); // 在(x, y) = (0, 0) 的位置绘制背景图片

	IMAGE** imgs = Block::getImages();
	// 真正绘制之前，加这个函数，使得图片进行一次绘制结束之后，再更新界面，不会出现频闪状态

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (map[i][j] == 0)continue; // 判断有没有落到画面底部的方块， 0表示当前位置是空的

			// 如果存在方块，计算当前的方块再背景矩阵中的位置
			int x = j * blockSize + leftMargin;
			int y = i * blockSize + topMargin;
			putimage(x, y, imgs[map[i][j] - 1]);
		}
	}
	
	// 初始化方块，当前方块和下一块方块
	curBlock->draw(leftMargin, topMargin);
	nextBlock->draw(689, 150);

	drawScore(); // 绘制分数
	EndBatchDraw();
}

int Tetris::getDelay()
{
	static unsigned long long lastTime = 0;
	unsigned long long currentTime = GetTickCount();
	if (lastTime == 0) {
		lastTime = currentTime;
		return 0;
	}
	else {
		int ret = currentTime - lastTime;
		lastTime = currentTime;
		return ret;
	}
}

void Tetris::drop()
{
	backBlock = *curBlock;
	curBlock->drop(); // 也就是说，curBlock最后调用的还是Block本身的drop

	// 判断方块是不是已经降落到背景底部之外了
	if (!curBlock->blockInMap(map)) { // 方块位置不合法
		// 如果方块降落到最底部，使得方块不动，应该使得当前方块的备用方块不动，当前方块的位置已经不合法了

		backBlock.solidify(map);
		delete curBlock;
		curBlock = nextBlock;
		nextBlock = new Block;

		checkOver();
	}

	delay = SPEED_NORMAL[level - 1];
}

void Tetris::clearLine()
{
	int lines = 0; // 记录满行是几行，用于后面的得分计算
	int k = rows - 1;
	for (int i = rows - 1; i >= 0; i--) {
		// 检查是否满行
		int count = 0;
		for (int j = 0; j < cols; j++) {
			if (map[i][j]) {
				count++;
			}
			map[k][j] = map[i][j]; // 一边扫描一边存储
		}

		if (count < cols) // 不满行，说明该行保留， 否则k不动， 那么后面就会被覆盖掉
		{
			k--;
		}
		else
		{
			lines++;
		}
	}

	if (lines > 0) { // 播放音效
		int addScore[4] = { 10, 30, 60, 80 };
		score += addScore[lines - 1];
		mciSendString("play xiaochu1.MP3", 0, 0, 0);  // 播放音效

		update = true;

		// 每100分，为一关，以此类推
		level = (score + 99) / 100;
		if (level > MAX_LEVLE) {
			gameOver = true;
		}
		lineCount += lines;
	}
}

void Tetris::moveleftRight(int offset)
{
	backBlock = *curBlock;
	curBlock->moveleftRigt(offset);

	if (!curBlock->blockInMap(map)) { // 判断是否合法
		*curBlock = backBlock; // 左右移动， 超出边界的话，就还原上一步操作，也就是移动到边界了
	}
}

void Tetris::rotate()
{
	if (curBlock->getBlockType() == 7) return; // 田子格不用旋转

	backBlock = *curBlock;
	curBlock->rotate();
	if (!curBlock->blockInMap(map)) { 
		*curBlock = backBlock;
	}
}

void Tetris::drawScore()
{
	char strScore[32];
	sprintf_s(strScore, sizeof(strScore), "%d", score);

	setcolor(RGB(180, 180, 180));

	LOGFONT f;
	gettextstyle(&f); // 获取当前字体
	f.lfHeight = 60;
	f.lfWeight = 30;
	f.lfQuality = ANTIALIASED_QUALITY; // 字体效果
	strcpy_s((char*)f.lfFaceName, sizeof(f.lfFaceName), "Segoe UI Black");
	settextstyle(&f);
	setbkmode(TRANSPARENT); // 字体的背景设置为透明

	outtextxy(670, 727, strScore);

	// 消除的行数绘制
	sprintf_s(strScore, sizeof(strScore), "%d", lineCount);
	settextstyle(&f);
	int xPosi = 224 - f.lfWeight * strlen(strScore);
	outtextxy(xPosi, 817, strScore);

	// 关卡数绘制
	sprintf_s(strScore, sizeof(strScore), "%d", level);
	outtextxy(224 - 30, 727, strScore);

	// 最高分数绘制
	sprintf_s(strScore, sizeof(strScore), "%d", highestScore);
	outtextxy(670, 817, strScore);
}

void Tetris::checkOver()
{
	if (curBlock->blockInMap(map) == false) {
		gameOver = true;
	}
}

void Tetris::saveScore()
{
	if (score > highestScore) {
		highestScore = score;

		ofstream file(RECORD_FILE);
		file << highestScore;
		file.close();
	}
}

void Tetris::displayOver()
{
	mciSendString("stop bg.MP3", 0, 0, 0);     
	if (level <= MAX_LEVLE) {
		putimage(262, 361, &imgOver);
		mciSendString("play over.MP3", 0, 0, 0);   // 失败音效
	}
	else {
		putimage(262, 361, &imgWin);
		mciSendString("play win.MP3", 0, 0, 0);  // 成功音效
	}
}