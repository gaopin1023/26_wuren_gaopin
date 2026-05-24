#include <iostream>
#include<vector>
using namespace std;

//定义地图行列大小为10行10列
#define ROW 10 //全局
#define COL 10

//枚举8个移动方向：上下左右+四个斜向
enum Direction
{
	p_up,
	p_down,
	p_left,
	p_right,
	p_leftDown,
	p_rightDown,
	p_leftUp,
	p_rightUp,
};

//行走代价定义（整数代价写法），全局变量
#define ZXprice 10	//直线代价
#define XXprice 14	//斜线代价，近似根号下2*10

//坐标点结构体，存储网格坐标及A*算法核心代价f / g / h
struct myPoint
{
	int row, col;
	int f, g, h;

	void getH(int r, int c); //声明计算预估代价h的函数
	void getF() //计算总代价f=g+h
	{
		f = g + h;
	}
	bool operator==(const myPoint& p)  //重载==运算符，判断两个坐标点是否在同一位置
	{
		return (row == p.row && col == p.col);
	}

};

//路径树节点结构体，用来回溯最终路径
struct TreeNode
{
	myPoint				pos; //当前节点坐标信息
	TreeNode* pParent;       //父节点（上一个走过的点)
	vector<TreeNode*>	pChild;  //子节点（周围可走的点）

	TreeNode(const myPoint& p)  //构造函数：初始化节点坐标，父节点默认为空
	{
		pos = p;
		pParent = NULL;
	}
};


/*
 * @brief 判断当前坐标是否可通行
 *
 * 运作流程：
 * 1.判断坐标是否超出地图边界
 * 2.判断当前位置是否为墙体障碍物
 * 3.判断该点是否已经被走过
 * 4.全部满足则允许通行
 *
 * 变量变化：
 * 全局变量：ROW、COL 限定地图范围
 * 局部变量：pos 接收传入判断的坐标点
 *
 * 依赖关系：
 * 无内部调用其他函数
 * 被main函数寻路循环调用
 */
bool canWork(int map[ROW][COL], bool pathMap[ROW][COL], myPoint pos)
{
	//是否越界
	if (pos.row < 0 || pos.row >= ROW || pos.col < 0 || pos.col >= COL)
	{
		return false;
	}
	//是否是墙（1代表障碍物）
	else if (1 == map[pos.row][pos.col])
	{
		return false;
	}
	//是否走过
	else if (pathMap[pos.row][pos.col] == true)
	{
		return false;
	}
	return true;  //三个条件都满足，可以通行
}
/*
 * @brief A*八个方向寻路算法主入口函数
 *
 * 运作流程：
 * 1.初始化地形地图与访问标记地图
 * 2.定义寻路起点坐标与终点坐标
 * 3.创建路径树根节点，初始化开放列表
 * 4.循环遍历当前点的八个移动方向
 * 5.调用通行判断筛选合法点位，计算g/h/f代价
 * 6.每次选取开放列表内f值最小节点前进
 * 7.到达终点停止循环，回溯输出路径并打印地图
 *
 * 变量变化：
 * 全局变量：ZXprice、XXprice控制行走代价
 * 局部变量：
 * map存储整张地形，pathMap标记已走点位
 * buff为开放列表存储待选节点
 * pCurrent记录当前搜索节点，pChild临时创建邻域子节点
 * isFindEnd标记寻路结果状态
 *
 * 依赖关系：
 * 调用：canWork、getH、getF、showMap
 * 无外部函数调用本函数，程序自动从main执行
 */
int main()
{
	int map[ROW][COL] = {
		{0,0,0,0,1,1,1,0,0,0},
		{0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0},
		{0,0,0,0,1,0,0,0,0,0},
	};
	bool pathMap[ROW][COL] = { 0 };  //行走标记地图，默认全false,走过设为true
	myPoint beginPos = { 2,1 };//起点设置
	myPoint endPos = { 9,9 };//终点设置

	pathMap[beginPos.row][beginPos.col] = true;//起点标记为已走过

	//创建树的根节点（起点作为根）
	TreeNode* pRoot = new TreeNode(beginPos);  

	vector<TreeNode*> buff;  //开放列表：存储所有待筛选的可走列表（手动模拟A*开表）
	
	vector<TreeNode*>::iterator it;  //迭代器：遍历开放列表
	vector<TreeNode*>::iterator MinIt;  //存储f值最小节点的迭代器

	TreeNode* pCurrent = pRoot;  //当前正在遍历的节点，初始为起点
	TreeNode* pChild = NULL;     //临时子节点指针
	bool isFindEnd = false;      //寻路成功标记
	
	while (1)//主循环：不断寻找最优路径
	{
		//1.寻找当前点周围能走的点
		for (int i = 0; i < 8; i++)
		{
			pChild = new TreeNode(pCurrent->pos);  //先复制当前点坐标，再修改成8个方向坐标
			switch (i)  //根据方向修改坐标，累计移动代价g
			{
			case p_up:pChild->pos.row--; pChild->pos.g += ZXprice; break;
			case p_down:pChild->pos.row++; pChild->pos.g += ZXprice; break;
			case p_left:pChild->pos.col--; pChild->pos.g += ZXprice; break;
			case p_right:pChild->pos.col++; pChild->pos.g += ZXprice; break;
			case p_leftUp:pChild->pos.row--; pChild->pos.col--; pChild->pos.g += XXprice; break;
			case p_rightUp:pChild->pos.row--; pChild->pos.col++; pChild->pos.g += XXprice; break;
			case p_leftDown:pChild->pos.row++; pChild->pos.col--; pChild->pos.g += XXprice; break;
			case p_rightDown:pChild->pos.row++; pChild->pos.col++; pChild->pos.g += XXprice; break;
			}
		//2.计算f的值并入树，存储
			
			//先新坐标判断能不能走
			if (canWork(map, pathMap, pChild->pos))//能走
			{
				//计算f的值
				pChild->pos.getH(endPos.row, endPos.col);
				pChild->pos.getF();
				//加入树结构，建立父子关系
				pCurrent->pChild.push_back(pChild);
				pChild->pParent = pCurrent;
				//存入开放列表，等待筛选最优节点
				buff.push_back(pChild);
				//标记走过的路（闭表）
				pathMap[pChild->pos.row][pChild->pos.col] = true;
			}
			else//不能走，释放内存
			{
				delete pChild;
				pChild = NULL;
			}
		}

		//3.当前点周围都找完了，找出开放列表中f值最小的点，作为下次循环当前点
		MinIt = buff.begin();//假设第一个最小
		for (it = buff.begin(); it != buff.end(); it++)  //遍历寻找最小f
		{
			MinIt = (((*it)->pos.f < (*MinIt)->pos.f) ? it : MinIt);
		}

		pCurrent = *MinIt; //更新当前节点为最小f

		//4.开放列表中删除最小f节点（加入闭表）
		buff.erase(MinIt);
		//5.判断是否找到终点
		if (pCurrent->pos == endPos)
		{
			isFindEnd = true;
			break;
		}
		//如果地图找完了，没有找到终点，寻路失败
		if (buff.size() == 0)//记录数组里面的全部走完了
		{
			break;
		}
	}
	//找到终点即打印
	if (isFindEnd)
	{
		cout << "找到终点";
		//对结果进行打印
		while (pCurrent)//通过现在的终点回溯父节点，倒序输出路径
		{
			cout << "(" << pCurrent->pos.row << "," << pCurrent->pos.col << ")";
			pCurrent = pCurrent->pParent;
		}
	}
	else
	{
		cout << "找不到终点";
	}
	return 0;
}
/*
 * @brief 计算曼哈顿预估代价h值
 *
 * 运作流程：
 * 1.计算当前点与终点行列差值
 * 2.行列差值相加
 * 3.乘以直线行走代价得到h
 *
 * 变量变化：
 * 全局变量：ZXprice 固定直线代价
 * 局部变量：x横向距离、y纵向距离
 * 成员变量：h 最终赋值预估代价
 *
 * 依赖关系：
 * 无调用其他函数
 * 在main寻路循环中被调用
 */
void myPoint::getH(int r, int c)
{
	int x = ((c > col) ? (c - col) : (col - c));
	int y = ((r > row) ? (r - row) : (row - r));
	h = (x + y) * ZXprice;
}
void myPoint::getH(int r, int c)
{
	int x = ((c > col) ? (c - col) : (col - c));
	int y = ((r > row) ? (r - row) : (row - r));

	h = (x + y) * ZXprice;	//h值
}