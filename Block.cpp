#include "Block.h"
#include<stdlib.h>
#include<graphics.h>
#include "Tetris.h"

IMAGE* Block::imgs[7] = { NULL }; // 静态成员，需要在类外进行声明
int Block::size = 36; // 一个小方块的宽度

Block::Block()
{
	if (imgs[0] == NULL)
	{
		IMAGE imgTemp;
		loadimage(&imgTemp, "tiles.png"); // 方块图片

		// 将含有7个方块的长条切成7个小方块
		SetWorkingImage(&imgTemp);
		for (int i = 0; i < 7; i++)
		{
			imgs[i] = new IMAGE; // 给图像指针分配内存
			getimage(imgs[i], i * size, 0, size, size);
		}
		SetWorkingImage(); // 恢复工作区
	}

	// 数组装7种方块
	int blocks[7][4] = {
		1,3,5,7,
		2,4,5,7,
		3,5,4,6,
		3,5,4,7,
		2,3,5,7,
		3,5,7,6,
		2,3,4,5
	};

	// 随机生成一种俄罗斯方块
	blockType = 1 + rand() % 7;

	// 初始化mallBlock
	for(int i = 0; i < 4; i++)
	{
		int value = blocks[blockType - 1][i];
		smallBlock[i].row = value / 2;
		smallBlock[i].collom = value % 2;
	}

	img = imgs[blockType - 1];
}

void Block::drop()
{
	for (int i = 0; i < 4; i++) 
	{
		smallBlock[i].row++;
	}
}

void Block::move()
{
}

void Block::retate()
{
}

void Block::draw(int leftMargin, int topMargin)
{
	// 绘图的时候，一个一个画图，对于每一种形状，都是每次绘制一个小方块
	for (int i = 0; i < 4; i++) 
	{
		int x = leftMargin + smallBlock[i].collom * size;
		int y = topMargin + smallBlock[i].row * size;
		putimage(x, y, img);
	}
}

IMAGE** Block::getImages()
{
	// 直接回传，是因为img是Block里面的一个私有成员
	return imgs;
}

Block& Block::operator=(const Block& other)
{
	// 自己不能赋值给自己
	if (this == &other) return *this;

	this->blockType = other.blockType;

	for (int i = 0; i < 4; i++) 
	{
		this->smallBlock[i] = other.smallBlock[i];
	}
}

bool Block::blockInMap(const vector<vector<int>>& map)
{
	int maxRows = map.size();
	int maxCols = map[0].size();
	for (int i = 0; i < 4; i++) 
	{
		if (smallBlock[i].collom < 0 || smallBlock[i].collom >= maxCols ||
			smallBlock[i].row < 0 || smallBlock[i].row >= maxRows ||
			map[smallBlock[i].row][smallBlock[i].collom] != 0) { // 最后一个条件是判断当前位置有没有方块
			return false;
		}
	}
	return true;
}

void Block::solidify(vector<vector<int>>& map)
{
	for (int i = 0; i < 4; i++)
	{
		map[smallBlock[i].row][smallBlock[i].collom] = blockType;
	}
}

void Block::moveleftRigt(int offset)
{
	for (int i = 0; i < 4; i++)
	{
		smallBlock[i].collom += offset;
	}
}

void Block::rotate()
{
	Point p = smallBlock[1];  // 选定方块的第二个方块为旋转中心
	for (int i = 0; i < 4; i++)
	{
		Point temp = smallBlock[i];
		smallBlock[i].collom = p.collom - temp.row + p.row;
		smallBlock[i].row = p.row + temp.collom - p.collom;
	}
}

int Block::getBlockType()
{
	return blockType;
}



