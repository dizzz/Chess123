#include<graphics.h>
/*EGE是windows下的简易图形库，介绍一下程序中使用的主要函数。
*initgraph：初始化绘图环境，设置绘图环境大小
*closegraph:关闭绘图环境
*cleardevice：清屏，用背景色填充绘图环境
*setbkcolor:设置背景色
*setcolor:设置绘图颜色（线条和文字）
*setfillcolor:设置填充颜色
*setfont:设置字体大小和字体样式
*line：绘制直线
*bar:绘制填充矩形
*rectangle:绘制空心矩形
*putpixel:绘制像素点
*xyprintf：绘图界面的输出函数
*outtextxy：输出字符
*/
#include<ctime>
#include<vector>
#include<stack>
#include<list>
#include<map>
#include<cmath>
using namespace std;
typedef unsigned int uint;
typedef void(*func)();

class Chess//抽象类
{
private:
	int x, y;//x,y分别棋子在棋盘上的横坐标和纵坐标，左上为(1,1),右下为(8，8)
	int last_x, last_y, last_s;//
	int status;//status表示棋子的状态,0表示死亡，1表示属于甲方，2表示属于乙方
	char pos;//pos表示棋子的身份，用大写字母的K,Q,B,N,R,P表示
	bool player;
public:
	Chess(int x = 0, int y = 0, int s = 0, char c = 0) //构造函数
	{
		this->x = x;
		this->y = y;
		this->status = s;
		this->pos = c;
	}
	//获取数据的函数
	int getX()const { return x; }
	int getY()const { return y; }
	int getLastX() { return last_x; }
	int getLastY() { return last_y; }
	int getLastS() { return last_s; }
	char getPos() const { return pos; }
	int getStatus() { return status; }
	bool getPlayer() { return player; }
	//设置信息的函数
	void setPlayer(bool p) { player = p; }
	void setLastXY(int x, int y) { last_x = x, last_y = y; }
	void setLastXYS() { last_x = x, last_y = y, last_s = status; }
	void setLastS(int s) { last_s = s; }
	void setStatus(int s) { status = s; }
	void setXY(int tx, int ty) {
		x = tx,y = ty;
	}
	void setXYS(int tx, int ty, int ts)	{
		x = tx,y = ty,status = ts;
	}
	void setXYSP(int tx, int ty, int ts, bool tp) {
		x = tx, y = ty, status = ts, player = tp;
	}
	//draw函数用于在棋盘上打印出棋子的身份，用EGE库实现
	void draw() {
		if (!status)
			return;
		if (status == 1)
			setcolor(EGERGB(0x0, 0x0, 0x0));
		else
			setcolor(EGERGB(0xff, 0xff, 0xff));
		setfont(37, 30, "Consolas");
		outtextxy(y * 40 + 125, x * 40 + 41, pos);
		setcolor(EGERGB(0xff, 0xff, 0xff));
	}
	//safe函数判断移动的目标区域是否超出棋盘范围或是否有己方棋子
	bool safe(int mx, int my);
	//highlight函数用于用白色框表示出所选中的棋子
	void highlight(int mx, int my);
	//虚函数movoto，用于后台棋子的移动
	virtual void moveto(int mx, int my);
	//check函数,bo为1时把棋子所有可能走到的位置保存到一个vector内
	virtual bool check(bool bo) = 0;
	//moveable函数用于判断目标位置是否为空，或是否有对方棋子，是否符合该棋子走法
	virtual bool moveable(int mx, int my) = 0;
};
//基类指针，cmap[i][j]代表棋盘(i,j)处的棋子。
Chess* cmap[9][9];
//vector类用于保存需要标示出来的位置
vector <pair<int, int>> v;
//chees_v用于保存棋盘上所有棋子继承对象数组的头指针，用于析构时的delete
vector <Chess*> chess_v;
//play_chess分别储存了双方存活的棋子
list <Chess*> player_chess[2];
//importance映射，存储对各棋子的估值
vector<Chess*> dead_chess;
map <char, int> importance;
//flex映射，储存各种棋子没多一种走法加上的估值
map <char, int> flex;
//game类用于控制游戏的进行,各函数的所用在之后的定义中详细说明。
class Game
{
private:
	bool check_state;
	bool game_end;
	bool ai;
public:
	Game() {
		int check_state = 0;
	}
	void wel_init();
	void array_init();
	void chess_init(int mode);
	void map_init();
	void extra_init();
	void game_init(int mode) {
		array_init();
		chess_init(mode);
		map_init();
		extra_init();
	}
	void chess_clear();
	void player_move(int st);
	//绘制棋子的图标
	void draw(int tx, int ty, char c) {
		setfont(37, 30, "Consolas");
		outtextxy(ty * 40 + 125, tx * 40 + 41, c);
	}
	//用背景色覆盖棋子的图标
	void cover(int tx, int ty) {
		bar(ty * 40 + 121, tx * 40 + 41, ty * 40 + 160, tx * 40 + 80);
	}
	bool check_check();
	void print_check();
	void clear_check();
	void select(int tx, int ty);
	void highlight(int tx, int ty);
	void disselect(int tx, int ty);
	int get_checkstate() { return check_state; }
	void set_checkstate(int ck) { check_state = ck; }
	void move_chess(int x1, int y1, int x2, int  y2);
	void undo_chess(int tx, int ty);
	bool dfs(int dep, int st, int score);
	void naive_ai_move(int status);
	void simple_ai_move(int status);
	void normal_ai_move(int st);
	bool good_game();
	void play(int mode);
	int welcome();
	void test();
	~Game()
	{
		chess_clear();
	}
};
//"骑士"类
class Knight :public Chess
{
private:
	static const int nextx[8];
	static const int nexty[8];
public:
	Knight(int x = 0, int y = 0, int s = 0) :Chess(x, y, s, 'N') {}
	bool moveable(int mx, int my);
	bool check(bool bo);
};
//"国王"类
class King :public Chess
{
private:
	static const int nextx[8];
	static const int nexty[8];
public:
	King(int x = 0, int y = 0, int s = 0) :Chess(x, y, s, 'K') {}
	bool moveable(int mx, int my);
	bool check(bool bo);
};
//“王后”类
class Queen :public Chess
{
private:
	static const int nextx[8];
	static const int nexty[8];
public:
	Queen(int x = 0, int y = 0, int s = 0) :Chess(x, y, s, 'Q') {}
	bool moveable(int mx, int my);
	bool check(bool bo);
};
//“城堡”类
class Rook :public Chess
{
private:
	static const int nextx[4];
	static const int nexty[4];
public:
	Rook(int x = 0, int y = 0, int s = 0) :Chess(x, y, s, 'R') {}
	bool moveable(int mx, int my);
	bool check(bool bo);
};
//“主教"类
class Bishop :public Chess
{
private:
	static const int nextx[4];
	static const int nexty[4];
public:
	Bishop(int x = 0, int y = 0, int s = 0) :Chess(x, y, s, 'B') {}
	bool moveable(int mx, int my);
	bool check(bool bo);
};
//“士兵”类
class Pawn :public Chess
{
private:
	static const int nextx[6];
	static const int nexty[6];
public:
	Pawn(int x = 0, int y = 0, int s = 0) :Chess(x, y, s, 'P') {}
	bool moveable(int mx, int my);
	void set(int x, int y, int s) {
		Chess::setStatus(s);
		Chess::setXY(x, y);
	}
	bool check(bool bo);
	//考虑到士兵走到最后一行时会有“升变”，所以重载了moveto函数
	void moveto(int mx, int my);
	//用于升变的函数
	void Promotion();
};
//各个棋类的静态成员，用于储存各棋的走法
const int Rook::nextx[4] = { 1,-1,0,0 };
const int Rook::nexty[4] = { 0,0,1,-1 };
const int Pawn::nextx[6] = { 1,1,1,-1,-1,-1 };
const int Pawn::nexty[6] = { 1,-1,0,0,1,-1 };
const int King::nextx[8] = { 1,1,1,-1,-1,-1,0,0 };
const int King::nexty[8] = { -1,0,1,-1,0,1,1,-1 };
const int Queen::nextx[8] = { 1,1,1,-1,-1,-1,0,0 };
const int Queen::nexty[8] = { -1,0,1,-1,0,1,1,-1 };
const int Bishop::nextx[4] = { 1,1,-1,-1 };
const int Bishop::nexty[4] = { -1,1,-1,1 };
const int Knight::nextx[8] = { 1,1,-1,-1,2,2,-2,-2 };
const int Knight::nexty[8] = { 2,-2,2,-2,1,-1,1,-1 };
//对游戏区域的映射，用于对鼠标选中区域的判断
int mmap[640][480];
bool Knight::moveable(int mx, int my)
{
	int tx = Chess::getX();
	int ty = Chess::getY();
	if (!Chess::safe(mx, my))
		return false;
	if ((abs(mx - tx) == 2 && abs(my - ty) == 1) || (abs(mx - tx) == 1 && abs(my - ty) == 2))
		return true;
	return false;
}
//检查是否能将军，在bo值为true时把所有可行位置保存在v内
bool Knight::check(bool bo) {
	int tx, ty;
	bool flag = 0;
	v.clear();
	for (int i = 0; i < 8; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		if (moveable(tx, ty))
		{
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			if (bo)
				v.push_back(make_pair(tx, ty));
		}
	}
	return flag;
}
bool King::moveable(int mx, int my)
{
	int tx = Chess::getX();
	int ty = Chess::getY();
	if (!Chess::safe(mx, my))
		return false;
	if (tx == mx&&ty == my)
		return false;
	if (abs(mx - tx) <= 1 && abs(my - ty) <= 1)
		return true;
	return false;
}
bool King::check(bool bo)
{
	int tx, ty;
	bool flag = 0;
	v.clear();
	for (int i = 0; i < 8; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		if (moveable(tx, ty))
		{
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
		}
	}
	return flag;
}
bool Queen::moveable(int mx, int my)
{
	int tx = Chess::getX();
	int ty = Chess::getY();
	if (!Chess::safe(mx, my))
		return false;
	if (tx == mx&&ty == my)
		return false;
	int maxx = max(tx, mx);
	int minx = min(tx, mx);
	int maxy = max(ty, my);
	int miny = min(ty, my);
	if (ty == my)
	{
		for (int i = minx + 1; i < maxx; i++)
			if (cmap[i][ty]->getStatus() != 0)
				return false;
		return true;
	}
	if (tx == mx)
	{

		for (int i = miny + 1; i < maxy; i++)
			if (cmap[tx][i]->getStatus() != 0)
				return false;
		return true;
	}
	if (maxx - minx != maxy - miny)
		return false;
	if ((mx - tx)*(my - ty) > 0)
	{
		for (int i = minx + 1; i < maxx; i++)
			if (cmap[i][i - minx + miny]->getStatus() != 0)
				return false;
		return true;
	}
	else
	{

		for (int i = 1; i < maxx - minx; i++)
			if (cmap[maxx - i][miny + i]->getStatus() != 0)
				return false;
		return true;
	}
	return false;
}
bool Queen::check(bool bo)
{
	int tx, ty;
	bool flag = 0;
	v.clear();
	for (int i = 0; i < 8; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		while (moveable(tx, ty))
		{
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			tx += nextx[i];
			ty += nexty[i];
		}
	}
	return flag;
}
bool Rook::moveable(int mx, int my)
{
	int tx = Chess::getX();
	int ty = Chess::getY();
	if (!Chess::safe(mx, my))
		return false;
	if (tx == mx&&ty == my)
		return false;
	int maxx = max(tx, mx);
	int minx = min(tx, mx);
	int maxy = max(ty, my);
	int miny = min(ty, my);
	if (ty == my)
	{
		for (int i = minx + 1; i < maxx; i++)
			if (cmap[i][ty]->getStatus() != 0)
				return false;
		return true;
	}
	if (tx == mx)
	{
		for (int i = miny + 1; i < maxy; i++)
			if (cmap[tx][i]->getStatus() != 0)
				return false;
		return true;
	}
	return false;
}
bool Rook::check(bool bo)
{
	int tx, ty;
	bool flag = 0;
	v.clear();
	for (int i = 0; i < 4; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		while (moveable(tx, ty))
		{
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			tx += nextx[i];
			ty += nexty[i];
		}
	}
	return flag;
}
bool Bishop::moveable(int mx, int my)
{
	int tx = Chess::getX();
	int ty = Chess::getY();
	if (!Chess::safe(mx, my))
		return false;
	if (tx == mx&&ty == my)
		return false;
	int maxx = max(tx, mx);
	int minx = min(tx, mx);
	int maxy = max(ty, my);
	int miny = min(ty, my);
	if (maxx - minx != maxy - miny)
		return false;
	if ((mx - tx)*(my - ty) > 0)
	{
		for (int i = minx + 1; i < maxx; i++)
		{
			//xyprintf
			if (cmap[i][i - minx + miny]->getStatus() != 0)
				return false;
		}
		return true;
	}
	else
	{
		for (int i = 1; i < maxx - minx; i++)
			if (cmap[maxx - i][miny + i]->getStatus() != 0)
				return false;
		return true;
	}
}
bool Bishop::check(bool bo)
{
	int tx, ty;
	bool flag = 0;
	v.clear();
	for (int i = 0; i < 4; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		while (moveable(tx, ty))
		{
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			tx += nextx[i];
			ty += nexty[i];
		};
	}
	return flag;
}
bool Pawn::moveable(int mx, int my)
{
	int tx = getX();
	int ty = getY();
	if (!safe(mx, my))
		return false;
	if (getStatus() == 0)
		return false;
	if (getStatus() == 1)
	{
		if (tx == 2)
		{
			if (mx == tx + 2 && my == ty&&cmap[mx][my]->getStatus() == 0)
				return true;
		}
		if (mx == tx + 1 && my == ty&&cmap[mx][my]->getStatus() == 0)
			return true;
		if (mx == tx + 1 && abs(my - ty) == 1 && cmap[mx][my]->getStatus() == 2)
			return true;
		return false;
	}
	else if (getStatus() == 2)
	{
		if (tx == 7)
		{
			if (mx == tx - 2 && my == ty&&cmap[mx][my]->getStatus() == 0)
				return true;
		}
		if (mx == tx - 1 && my == ty&&cmap[mx][my]->getStatus() == 0)
		{
			return true;
		}

		if (mx == tx - 1 && abs(my - ty) == 1 && cmap[mx][my]->getStatus() == 1)
			return true;
		return false;
	}
	return false;
}
void Pawn::moveto(int mx, int my)
{
	int tx = getX();
	int ty = getY();
	cmap[mx][my]->setLastXYS();
	cmap[mx][my]->setXY(tx, ty);
	if (cmap[mx][my]->getStatus() != 0)
		player_chess[cmap[mx][my]->getStatus() - 1].remove(cmap[mx][my]);
	swap(cmap[mx][my], cmap[tx][ty]);
	cmap[tx][ty]->setStatus(0);
	setXY(mx, my);
	if (mx == 1 || mx == 8)
		Promotion();
}
void Pawn::Promotion()
{
	int tx = getX(), ty = getY();
	int tmp = 0;
	bool flag = 0;
	Chess* newc;
	//一般考虑王后的价值更大，所以AI控制的士兵升变时直接选择王后
	if (this->getPlayer() == 1)
	{
		newc = new Queen;
		newc->setXYS(tx, ty, getStatus());
		cmap[tx][ty] = newc;
		chess_v.push_back(newc);
		player_chess[getStatus() - 1].push_back(newc);
		return;
	}
	setcolor(EGERGB(0xff, 0xff, 0xff));
	setfont(15, 12, "Consolas");
	xyprintf(510, 130, "Promorion");
	setfont(37, 30, "Consolas");
	rectangle(540, 160, 580, 320);
	outtextxy(544, 161, 'Q');
	outtextxy(544, 201, 'R');
	outtextxy(544, 241, 'N');
	outtextxy(544, 281, 'B');
	line(540, 200, 580, 200);
	line(540, 240, 580, 240);
	line(540, 280, 580, 280);
	mouse_msg mouse = { 0 };
	for (;; delay_jfps(60))
	{
		while (mousemsg())
		{
			mouse = getmouse();
			tmp = mmap[mouse.x][mouse.y];
			if (mouse.is_up() && mouse.is_left())
			{
				setfont(10, 8, "Consolas");
				if (tmp >= -1)
					continue;
				if (tmp == -2)
					newc = new Queen;
				else if (tmp == -3)
					newc = new Rook;
				else if (tmp == -4)
					newc = new Knight;
				else if (tmp == -5)
					newc = new Bishop;
				newc->setXYS(tx, ty, getStatus());
				cmap[tx][ty] = newc;
				chess_v.push_back(newc);
				player_chess[getStatus() - 1].push_back(newc);
				flag = 1;
				break;
			}
		}
		if (flag)
			break;
	}
	bar(510, 130, 630, 330);

}
bool Pawn::check(bool bo)
{
	int tx, ty;
	bool flag = 0;
	v.clear();
	for (int i = 0; i < 6; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		while (moveable(tx, ty))
		{
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			tx += nextx[i];
			ty += nexty[i];
		}
	}
	return flag;
}
bool Chess::safe(int mx, int my)
{
	if (mx == x&&my == y)
		return false;
	if (mx < 1 || mx>8 || my < 1 || my>8)
		return false;
	if (cmap[mx][my]->getStatus() != this->getStatus())
		return true;
	return false;

}
void Chess::moveto(int mx, int my)
{
	cmap[mx][my]->setLastXYS();
	cmap[mx][my]->setXY(x, y);
	if (cmap[mx][my]->getStatus() != 0)
		player_chess[cmap[mx][my]->getStatus() - 1].remove(cmap[mx][my]);
	swap(cmap[mx][my], cmap[x][y]);
	cmap[x][y]->setStatus(0);
	x = mx;
	y = my;
}
void Chess::highlight(int mx, int my)
{
	setfillcolor(EGERGB(255, 215, 0));
	bar(my * 40 + 122, mx * 40 + 42, my * 40 + 124, mx * 40 + 79);
	bar(my * 40 + 157, mx * 40 + 42, my * 40 + 159, mx * 40 + 79);
	bar(my * 40 + 122, mx * 40 + 42, my * 40 + 159, mx * 40 + 44);
	bar(my * 40 + 122, mx * 40 + 77, my * 40 + 159, mx * 40 + 79);
	setfillcolor(EGERGB(0x0, 0x0, 0x0));
}
void Game::print_check()
{
	setcolor(EGERGB(0xff, 0xff, 0xff));
	setfont(60, 40, "Consolas");
	xyprintf(200, 0, "Check!");
	setfont(10, 8, "Consolas");
}
void Game::clear_check()
{
	bar(130, 0, 500, 80);
}
void Game::wel_init()
{

	memset(mmap, -1, sizeof(mmap));
	for (int i = 200; i <= 440; i++)
		for (int j = 240; j <= 440; j++)
			mmap[i][j] = j / 40 - 5;

}
void Game::chess_init(int mode)
{
	bool p1, p2;
	switch (mode)
	{
		case 1:
			p1 = 1, p2 = 1;
			break;
		case 2:
			p1 = 0, p2 = 0;
			break;
		default:
			p1 = 0, p2 = 1;
	}
	Rook* rook = new Rook[4];
	Knight* knight = new Knight[4];
	Bishop* bishop = new Bishop[4];
	Queen* queen = new Queen[2];
	King *king = new King[2];
	Pawn* pawn = new Pawn[16];
	Pawn* empt = new Pawn[32];
	chess_v.push_back(rook);
	chess_v.push_back(bishop);
	chess_v.push_back(queen);
	chess_v.push_back(king);
	chess_v.push_back(knight);
	chess_v.push_back(pawn);
	chess_v.push_back(empt);
	rook[0].setXYSP(1, 1, 1,p1);
	rook[1].setXYSP(1, 8, 1,p1);
	rook[2].setXYSP(8, 1, 2,p2);
	rook[3].setXYSP(8, 8, 2,p2);
	knight[0].setXYSP(1, 2, 1,p1);
	knight[1].setXYSP(1, 7, 1,p1);
	knight[2].setXYSP(8, 2, 2,p2);
	knight[3].setXYSP(8, 7, 2,p2);
	bishop[0].setXYSP(1, 3, 1,p1);
	bishop[1].setXYSP(1, 6, 1,p1);
	bishop[2].setXYSP(8, 3, 2,p2);
	bishop[3].setXYSP(8, 6, 2,p2);
	king[0].setXYSP(1, 4, 1,p1);
	king[1].setXYSP(8, 4, 2,p2);
	queen[0].setXYSP(1, 5, 1,p1);
	queen[1].setXYSP(8, 5, 2,p2);

	for (int i = 0; i < 8; i++)
	{
		pawn[i].setXYSP(2, i + 1, 1,p1);
		pawn[i].setLastXYS();
		pawn[i + 8].setXYSP(7, i + 1, 2,p2);
		pawn[i + 8].setLastXYS();
		cmap[2][i + 1] = &pawn[i];
		cmap[7][i + 1] = &pawn[i + 8];
		player_chess[0].push_back(&pawn[i]);
		player_chess[1].push_back(&pawn[i + 8]);
	}
	for (int i = 0; i < 2; i++)
	{
		king[i].setLastXYS();
		queen[i].setLastXYS();
		rook[i].setLastXYS();
		knight[i].setLastXYS();
		bishop[i].setLastXYS();
		rook[i + 2].setLastXYS();
		knight[i + 2].setLastXYS();
		bishop[i + 2].setLastXYS();
		player_chess[0].push_back(&rook[i]);
		player_chess[0].push_back(&knight[i]);
		player_chess[0].push_back(&bishop[i]);
		player_chess[1].push_back(&rook[i + 2]);
		player_chess[1].push_back(&knight[i + 2]);
		player_chess[1].push_back(&bishop[i + 2]);
	}
	player_chess[0].push_back(&king[0]);
	player_chess[0].push_back(&queen[0]);
	player_chess[1].push_back(&king[1]);
	player_chess[1].push_back(&queen[1]);
	cmap[1][1] = &rook[0];
	cmap[1][8] = &rook[1];
	cmap[8][1] = &rook[2];
	cmap[8][8] = &rook[3];
	cmap[1][2] = &knight[0];
	cmap[1][7] = &knight[1];
	cmap[8][2] = &knight[2];
	cmap[8][7] = &knight[3];
	cmap[1][3] = &bishop[0];
	cmap[1][6] = &bishop[1];
	cmap[8][3] = &bishop[2];
	cmap[8][6] = &bishop[3];
	cmap[1][4] = &king[0];
	cmap[8][4] = &king[1];
	cmap[1][5] = &queen[0];
	cmap[8][5] = &queen[1];
	for (int i = 3; i <= 6; i++)
		for (int j = 1; j <= 8; j++)
			empt[i * 8 + j - 25].setXYS(i, j, 0);
	for (int i = 3; i <= 6; i++)
		for (int j = 1; j <= 8; j++)
			cmap[i][j] = &empt[i * 8 + j - 25];
}
void Game::array_init()
{
	memset(mmap, -1, sizeof(mmap));
	for (int i = 160; i <= 480; i++)
		for (int j = 80; j <= 400; j++)
			mmap[i][j] = (j / 40 - 1) * 10 + i / 40 - 3;
	for (int i = 540; i <= 580; i++)
		for (int j = 160; j <= 320; j++)
			mmap[i][j] = -j / 40 + 2;
}
void Game::map_init()
{
	cleardevice();
	setfillcolor(0xBEBEBE);
	setbkcolor(0xBEBEBE);
	setcolor(BLACK);
	for (int i = 160; i <= 480; i += 40)
		line(i, 80, i, 400);
	for (int i = 80; i <= 400; i += 40)
		line(160, i, 480, i);
	for (int i = 1; i <= 2; i++)
		for (int j = 1; j <= 8; j++)
		{
			cmap[i][j]->draw();
			cmap[i + 6][j]->draw();
		}
	setcolor(EGERGB(0xff, 0xff, 0xff));
}

void Game::extra_init()
{
	importance['K'] = 1000000;
	importance['Q'] = 6000;
	importance['R'] = 3000;
	importance['N'] = 2800;
	importance['B'] = 2800;
	importance['P'] = 2000;
	flex['K'] = 0;
	flex['Q'] = 2;
	flex['R'] = 4;
	flex['B'] = 3;
	flex['N'] = 5;
	flex['P'] = 2;
}
void Game::chess_clear()
{
	for (uint i = 0; i < chess_v.size(); i++)
		delete chess_v[i];
	chess_v.clear();
	player_chess[0].clear();
	player_chess[1].clear();
	dead_chess.clear();
}
//选中所选择的棋子
void Game::select(int tx, int ty)
{

	setfillcolor(EGERGB(108, 108, 108));
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 124, tx * 40 + 79);
	bar(ty * 40 + 157, tx * 40 + 42, ty * 40 + 159, tx * 40 + 79);
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 159, tx * 40 + 44);
	bar(ty * 40 + 122, tx * 40 + 77, ty * 40 + 159, tx * 40 + 79);
	setfillcolor(EGERGB(0x0, 0x0, 0x0));

}
//用背景色覆盖选框，作用是“取消选中”所选择的棋子
void Game::disselect(int tx, int ty)
{
	setfillcolor(0xBEBEBE);
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 124, tx * 40 + 79);
	bar(ty * 40 + 157, tx * 40 + 42, ty * 40 + 159, tx * 40 + 79);
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 159, tx * 40 + 44);
	bar(ty * 40 + 122, tx * 40 + 77, ty * 40 + 159, tx * 40 + 79);
}
//高亮表示坐标为(tx,ty)的棋子
void Game::highlight(int tx, int ty)
{
	setfillcolor(EGERGB(30, 30, 30));
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 124, tx * 40 + 79);
	bar(ty * 40 + 157, tx * 40 + 42, ty * 40 + 159, tx * 40 + 79);
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 159, tx * 40 + 44);
	bar(ty * 40 + 122, tx * 40 + 77, ty * 40 + 159, tx * 40 + 79);
	setfillcolor(EGERGB(0x0, 0x0, 0x0));
}
//考虑到当自己走棋时，可能将对方的军，也可能让自己被将军，所以直接检查场面上所有棋子，其实是偷懒的写法.. 
bool Game::check_check()
{
	bool flag = 0;
	int side;
	for (list<Chess*>::iterator it = player_chess[0].begin(); it != player_chess[0].end(); it++)
	{
		flag |= (*it)->check(0);
		if (flag)
			side = 2;
	}
	for (list<Chess*>::iterator it = player_chess[1].begin(); it != player_chess[1].end(); it++)
	{
		flag |= (*it)->check(0);
		if (flag)
			side = 1;
	}
	if (flag)
	{
		set_checkstate(side);
		print_check();
		return true;
	}
	clear_check();
	return false;
}
//把位于(x1,y1)的棋子移动到(x2,y2)
void Game::move_chess(int x1, int y1, int x2, int  y2)
{
	//后台移动
	cmap[x1][y1]->moveto(x2, y2);
	//“擦除”原先所在位置的图像
	cover(x1, y1);
	//在新的位置上输出图像
	cmap[x2][y2]->draw();
}
//normalAI深搜回溯时回到初始状态时用到
void Game::undo_chess(int tx, int ty)
{
	int lx = cmap[tx][ty]->getLastX();
	int ly = cmap[tx][ty]->getLastY();
	int ls = cmap[tx][ty]->getLastS();
	cmap[tx][ty]->setXYS(lx, ly, ls);
	cmap[lx][ly]->setXY(tx, ty);
	swap(cmap[tx][ty], cmap[lx][ly]);

}
//玩家落子时的函数
void Game::player_move(int st)
{
	setfont(10, 8, "Consolas");
	int t1 = 0, t2 = 0, t3 = 0, t4 = 0, tmp = 0, flag = 0;
	int t = 20;
	//获得鼠标信息
	mouse_msg mouse = { 0 };
	for (;; delay_jfps(60))
	{
		while (mousemsg())
		{
			mouse = getmouse();
			tmp = mmap[mouse.x][mouse.y];
			if (tmp < 0)
				continue;
			//鼠标右击时，取消选择原来选中的棋子
			if (mouse.is_right() && mouse.is_up())
			{
				for (uint i = 0; i < v.size(); i++)
					disselect(v[i].first, v[i].second);
				disselect(t1, t2);
				flag = 0;
				continue;
			}
			if (mouse.is_up() && mouse.is_left())
			{
				t += 10;
				//flag为0时，选中被移动的棋子
				if (!flag)
				{
					t1 = tmp / 10;
					t2 = tmp % 10;
					if (cmap[t1][t2]->getStatus() == st)
					{
						Game::select(t1, t2);
						cmap[t1][t2]->check(true);
						for (uint i = 0; i < v.size(); i++)
							highlight(v[i].first, v[i].second);
						flag++;
						continue;
					}
				}
				//flag为1时，选中目标位置，选中合法位置后跳出循环
				if (flag == 1)
				{
					t3 = tmp / 10;
					t4 = tmp % 10;
					//flushmouse();
					if (cmap[t1][t2]->moveable(t3, t4))
					{
						for (unsigned int i = 0; i < v.size(); i++)
							disselect(v[i].first, v[i].second);
						flag++;
						break;
					}
				}
			}
		}
		if (flag == 2)
			break;
	}
	//如果吃掉了对方国王，游戏结束
	if (cmap[t3][t4]->getPos() == 'K')
	{
		game_end = 1;
		return;
	}
	move_chess(t1, t2, t3, t4);
}
//navieAI,随机选择移动的棋子和移动位置.. 
void Game::naive_ai_move(int status)
{
	int t1, t2;
	vector<Chess*>pits;
	for (list <Chess*>::iterator it = player_chess[status - 1].begin(); it != player_chess[status - 1].end(); it++)
	{
		(*it)->check(1);
		if (v.size() != 0)
			pits.push_back(*it);
	}
	srand(time(0));
	t1 = rand() % pits.size();
	pits[t1]->check(1);
	t2 = rand() % v.size();
	if (cmap[v[t2].first][v[t2].second]->getPos() == 'K')
	{
		game_end = 1;
	}
	move_chess(pits[t1]->getX(), pits[t1]->getY(), v[t2].first, v[t2].second);
}
//simpleAI,贪心地选出所有走法中价值最高的。吃掉对方棋子或有更多的走法能够增加价值。
void Game::simple_ai_move(int status)
{
	Chess* maxf, *temp;
	int maxt;
	int score, maxs = 0;
	int t = 0;
	for (list <Chess*>::iterator it = player_chess[status - 1].begin(); it != player_chess[status - 1].end(); it++)
	{
		(*it)->check(1);
		for (uint i = 0; i < v.size(); i++)
		{
			score = v.size()*flex[(*it)->getPos()];
			temp = cmap[v[i].first][v[i].second];
			if (temp->getStatus())
				score += importance[temp->getPos()];
			if (score > maxs)
			{
				maxs = score;
				maxt = i;
				maxf = *it;
				//xyprintf(0, 10 * t++, "%d %c", score,maxf->getPos());
			}
		}
		v.clear();
	}
	maxf->check(1);
	if (cmap[v[maxt].first][v[maxt].second]->getPos() == 'K')
	{
		game_end = 1;
	}
	cover(maxf->getX(), maxf->getY());
	maxf->moveto(v[maxt].first, v[maxt].second);
	cmap[v[maxt].first][v[maxt].second]->draw();
	setcolor(EGERGB(0xff, 0xff, 0xff));
}
//normalAI，回溯时会出现问题..还在Debug...
int max_score = 0;
Chess* maxc;
int maxi;
int num;
bool Game::dfs(int st, int dep, int score)
{

	int t1, t2, sc;
	bool flag = 0, ret;
	if (dep == 0)
	{
		int t = 0;
		for (list <Chess*>::iterator it = player_chess[st - 1].begin(); it != player_chess[st - 1].end(); it++)
		{
			(*it)->check(1);
			t1 = (*it)->getX();
			t2 = (*it)->getY();
			for (uint i = 0; i < v.size(); i++)
			{
				sc = score;
				flag = 0;
				if (cmap[v[i].first][v[i].second]->getStatus())
				{
					flag = 1;
					dead_chess.push_back(cmap[v[i].first][v[i].second]);
					sc = score + importance[cmap[v[i].first][v[i].second]->getPos()];
				}
				cmap[t1][t2]->moveto(v[i].first, v[i].second);
				ret = dfs(st, dep + 1, sc);
				if (ret)
				{
					maxc = *it;
					maxi = i;
				}
				if (flag)
					dead_chess.pop_back();
				undo_chess(t1, t2);
			}
		}
	}
	if (dep == 1)
	{
		ret = 0;
		for (list <Chess*>::iterator it = player_chess[2 - st].begin(); it != player_chess[2 - st].end(); it++)
		{
			for (uint i = 0; i < dead_chess.size(); i++)
			{
				if ((*it) == dead_chess[i])
					continue;
			}
			(*it)->check(1);
			t1 = (*it)->getX();
			t2 = (*it)->getY();
			for (uint i = 0; i < v.size(); i++)
			{
				sc = score;
				flag = 0;
				if (cmap[v[i].first][v[i].second]->getStatus())
				{
					flag = 1;
					dead_chess.push_back(cmap[v[i].first][v[i].second]);
					sc = score - importance[cmap[v[i].first][v[i].second]->getPos()]*2;
				}
				cmap[t1][t2]->moveto(v[i].first, v[i].second);
				ret |= dfs(st, dep + 1, sc);
				if (flag)
					dead_chess.pop_back();
				undo_chess(t1, t2);
			}
		}
		return ret;
	}
	else
	{
		ret = 0;
		num++;
		for (list <Chess*>::iterator it = player_chess[2 - st].begin(); it != player_chess[2 - st].end(); it++)
		{
			for (int i = 0; i < dead_chess.size(); i++)
			{
				if ((*it) == dead_chess[i])
					continue;
			}
			(*it)->check(1);
			t1 = (*it)->getX();
			t2 = (*it)->getY();
			for (int i = 0; i < v.size(); i++)
			{
				sc = score;
				flag = 0;
				if (cmap[v[i].first][v[i].second]->getStatus())
				{
					flag = 1;
					sc = score + importance[cmap[v[i].first][v[i].second]->getPos()];
				}
				if (sc >= max_score)
				{
					ret = 1;
					max_score = sc;
				}
			}
		}
		return ret;
	}
}
void Game::normal_ai_move(int st)
{
	num = 0;
	dfs(st, 0, 0);
	xyprintf(100, 0, "%d", num);
	maxc->check(1);
	move_chess(maxc->getX(), maxc->getY(), v[maxi].first, v[maxi].second);
	//return cmap[v[maxi].first][v[maxi].second];
}
//最初的菜单界面
int Game::welcome()
{
	wel_init();
	setfillcolor(0xBEBEBE);
	setbkcolor(0xBEBEBE);
	setcolor(BLACK);
	setfont(70, 50, "Microsoft YaHei UI Light");
	xyprintf(100, 100, "Chess123");
	//实现线框的动画效果
	for (int i = 0; i <= 240; i++)
	{
		putpixel(200 + i, 240, BLACK);
		putpixel(200 + i, 440, BLACK);
		putpixel(200, 240 + i * 5 / 6, BLACK);
		putpixel(440, 240 + i * 5 / 6, BLACK);
		Sleep(1);
	}
	for (int i = 0; i < 240; i++)
	{
		putpixel(440 - i, 280, BLACK);
		putpixel(200 + i, 320, BLACK);
		putpixel(440 - i, 360, BLACK);
		putpixel(200 + i, 400, BLACK);
		Sleep(1);
	}
	setfont(20, 15, "Consolas");
	xyprintf(295, 248, "demo");
	xyprintf(300, 288, "1v1");
	xyprintf(250, 328, "1vNaiveCom");
	xyprintf(240, 368, "1vSimpleCom");
	xyprintf(240, 408, "1vNormalCom");
	setfont(10, 8, "Consolas");
	xyprintf(280, 428, "(unavailable)");
	mouse_msg mouse;
	int tmp;
	//同样鼠标点击区域进行了映射
	for (;; delay_jfps(60))
	{
		while (mousemsg())
		{
			mouse = getmouse();
			tmp = mmap[mouse.x][mouse.y];
			//xyprintf(10, 10, "%d", tmp);
			if (mouse.is_up() && mouse.is_left())
			{
				if (tmp < 0)
					continue;
				return tmp;
			}
		}
	}
}
//游戏结束画面
bool Game::good_game()
{
	srand(time(0));
	int p = 0;
	setcolor(0xBEBEBE);
	//擦除棋盘
	for (int i = 160; i<=640; i++) 
	{
		line(i, 0, i, 480);
		Sleep(1);
	}
	setcolor(WHITE);
	setfont(60, 40, "Consolas");
	xyprintf(140, 200, "Checkmate!");
	//实现线框的动画效果
	for (int i = 0; i <= 200; i++)
	{
		putpixel(540 + i, 400, EGERGB(0xff, 0xff, 0xff));
		putpixel(540 + i, 440, EGERGB(0xff, 0xff, 0xff));
		putpixel(540 + i, 478, EGERGB(0xff, 0xff, 0xff));
		putpixel(540, 400 +  i * 2 / 5, EGERGB(0xff, 0xff, 0xff));
		putpixel(638, 400 + i * 2 / 5, EGERGB(0xff, 0xff, 0xff));
		Sleep(1);
	}
	
	setcolor(BLACK);
	setfont(20, 16, "Consolas");
	xyprintf(550, 410, "Retry");
	xyprintf(560, 450, "Exit");
	mouse_msg mouse;
	//菜单选项较少所以直接用if语句判断..
	for (;; delay_jfps(60))
	{
		while (mousemsg())
		{
			mouse = getmouse();
			if (mouse.is_up() && mouse.is_left())
			{
				if (540 < mouse.x&&mouse.x < 640 && 400 < mouse.y&&mouse.y < 440)
					return true;
				if (540 < mouse.x&&mouse.x < 640 && 440 < mouse.y&&mouse.y < 480)
					return false;
			}
		}
	}
}
//控制游戏进行的函数
void Game::play(int mode)
{
	//用了函数指针，方便不同模式下函数的调用
	void (Game::*move1)(int);
	void (Game::*move2)(int);
	game_init(mode);
	switch (mode) {
	case 1:
		move1 = &Game::naive_ai_move;
		move2 = &Game::simple_ai_move;
		break;
	case 2:
		move1 = &Game::player_move;
		move2 = &Game::player_move;
		break;
	case 3:
		move1 = &Game::player_move;
		move2 = &Game::naive_ai_move;
		break;
	case 4:
		move1 = &Game::player_move;
		move2 = &Game::simple_ai_move;
		break;
	case 5:
		move1 = &Game::player_move;
		move2 = &Game::normal_ai_move;
		break;
	default:
		return;
	}
	while (1)
	{
		while (1)
		{
			setfont(15, 12, "Consolas");
			xyprintf(500, 450, "Player1");
			if (mode == 1) Sleep(300);
			(this->*move1)(2);
			check_check();
			if (game_end)
				break;
			bar(500, 450, 600, 480);
			setfont(15, 12, "Consolas");
			xyprintf(500, 50, "Player2");
			if (mode == 1) Sleep(300);
			(this->*move2)(1);
			check_check();
			if (game_end)
				break;
			bar(500, 50, 600, 80);
			setfont(10, 8, "Consolas");
		}
		//选中"retry"后释放内存，重新申请对象，重置指针
		if (good_game())
		{
			chess_clear();
			game_init(mode);
			game_end = 0;
		}
		else
			break;
	}
}
//Debug用..
void Game::test()
{
	chess_init(2);
	good_game();
	while (1);
}

int main()
{
	Game newgame;
	delay(60);
	initgraph(640, 480);
	newgame.play(newgame.welcome());
	closegraph();
}
