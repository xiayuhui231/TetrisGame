#pragma once
#include <graphics.h>
#include <cstring>
#include <vector>

using namespace std;

struct Point {
	int row;
	int collom;
};

class Block
{
public:
	Block();

	void drop();
	void move();
	void retate();
	void draw(int leftMargin, int topMargin);
	static IMAGE** getImages(); // 私有成员在public里面，需要通过函数来间接访问和修改
	Block& operator=(const Block& other); // 赋值运算符重载，将一个对象拷贝给另一个对象，函数返回新返回的对象
	bool blockInMap(const vector<vector<int>>& map);
	
	void solidify(vector<vector<int>>& map); // 将方块固定在游戏界面固定的位置

	void moveleftRigt(int offset);
	void rotate(); //旋转
	int getBlockType();

private:
	int blockType;  // 方块类型
	Point smallBlock[4]; // 每个俄罗斯方块由四个小方块相对位置改变而来

	// 小方块要以怎么样的图像的形式显示出来
	IMAGE* img; // 保存图像的IMAGE对象指针，IMAGE是graphics里面的函数，可以当成类来使用，这里img表示一种类型的俄罗斯方块
	static IMAGE* imgs[7];
	static int size;
};

