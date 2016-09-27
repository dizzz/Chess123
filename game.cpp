#include<graphics.h>
#include<ctime>
#include<vector>
#include<stack>
#include<list>
#include<map>
using namespace std;
typedef unsigned int uint;
typedef void(*func)();
bool game_end;

class Chess
{
private:
	int x, y;//x,y分别棋子在棋盘上的横坐标和纵坐标，左上为(1,1),右下为(8，8)
	int last_x, last_y, last_s;
	int status;//status表示棋子的状态,0表示死亡，1表示属于甲方，2表示属于乙方
	char pos;//pos表示棋子的身份，用大写字母的K,Q,B,N,R,P表示
	int psb;
public:
	Chess(int x = 0, int y = 0, int s = 0, char c = 0) //构造函数
	{
		this->x = x;
		this->y = y;
		this->status = s;
		this->pos = c;
		this->psb = 0;
	}
	//获取数据的函数
	int getX()const { return x; }
	int getY()const { return y; }
	int getLastX() { return last_x; }
	int getLastY() { return last_y; }
	int getLastS() { return last_s; }
	char getPos() const { return pos; }
	int getStatus() { return status; }
	int getPsb() { return psb; }
	//设置信息的函数
	void setLastXY(int x, int y) { last_x = x, last_y = y; }
	void setLastXYS() { last_x = x, last_y = y, last_s = status; }
	void setLastS(int s) { last_s = s; }
	void setPsb(int p) { psb = p; }
	void setStatus(int s) { status = s; }
	void setXY(int x, int y) {
		this->x = x;
		this->y = y;
	}
	void setXYS(int x, int y, int s)
	{
		this->x = x;
		this->y = y;
		this->status = s;

	}
	//draw函数用于在棋盘上打印出棋子的身份，用EGE库实现
	void draw() {
		if (!status)
			return;
		if (status == 1)
			setcolor(EGERGB(0x0, 0xff, 0x0));
		else
			setcolor(EGERGB(0xff, 0x0, 0x0));
		setfont(37, 30, "Consolas");
		outtextxy(y * 40 + 125, x * 40 + 41, pos);
		setcolor(EGERGB(0xff, 0xff, 0xff));
	}
	//safe函数判断移动的目标区域是否超出棋盘范围
	bool safe(int mx, int my);
	//highlight函数用于用白色框表示出所选中的棋子
	void highlight(int mx, int my);
	//虚函数movoto，用于后台棋子的移动

	virtual void moveto(int mx, int my);
	//moveto函数的重载
	void moveto(Chess* cp);
	//check函数
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
	int check_state;
	bool ai;
public:
	Game() {
		int check_state = 0;
	}
	void wel_init();
	void array_init();
	void chess_init();
	void map_init();
	void extra_init();
	void game_init() {
		array_init();
		chess_init();
		map_init();
		extra_init();
	}
	void chess_clear();
	void extra_clear();
	Chess* player_move(int st);
	void draw(int tx, int ty, char c) {
		setfont(37, 30, "Consolas");
		outtextxy(ty * 40 + 125, tx * 40 + 41, c);
	}
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
	Chess* naive_ai_move(int status);
	Chess* simple_ai_move(int status);
	Chess* normal_ai_move(int st);
	void good_game(Chess*);
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
	void moveto(int mx, int my);
	void Promotion();
};
//各个棋类的静态成员，用于储存各棋的走法。
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
bool Knight::check(bool bo) {
	int tx, ty;
	int tpsb = 0;
	bool flag = 0;
	v.clear();
	for (int i = 0; i < 8; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		if (moveable(tx, ty))
		{
			tpsb++;
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			if (bo)
				v.push_back(make_pair(tx, ty));
		}
	}
	setPsb(tpsb);
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
	int tpsb = 0;
	v.clear();
	for (int i = 0; i < 8; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		if (moveable(tx, ty))
		{
			tpsb++;
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
		}
	}
	setPsb(tpsb);
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
	int tpsb = 0;
	v.clear();
	for (int i = 0; i < 8; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		while (moveable(tx, ty))
		{
			tpsb++;
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			tx += nextx[i];
			ty += nexty[i];
		}
	}
	setPsb(tpsb);
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
	int tpsb = 0;
	v.clear();
	for (int i = 0; i < 4; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		while (moveable(tx, ty))
		{
			tpsb++;
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			tx += nextx[i];
			ty += nexty[i];
		}
	}
	setPsb(tpsb);
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
	int tpsb = 0;
	v.clear();
	for (int i = 0; i < 4; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		while (moveable(tx, ty))
		{
			tpsb++;
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			tx += nextx[i];
			ty += nexty[i];
		};
	}
	setPsb(tpsb);
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
		//xyprintf(0, 150, "%d",cmap[mx][my]->getStatus());
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
	int tmp = 0;
	bool flag = 0;
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
				Chess* newc;
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
	int tpsb = 0;
	for (int i = 0; i < 6; i++)
	{
		tx = Chess::getX() + nextx[i];
		ty = Chess::getY() + nexty[i];
		while (moveable(tx, ty))
		{
			tpsb++;
			if (bo)
				v.push_back(make_pair(tx, ty));
			if (cmap[tx][ty]->getPos() == 'K')
				flag = 1;
			tx += nextx[i];
			ty += nexty[i];
		}
	}
	setPsb(tpsb);
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
void Chess::moveto(Chess* cp)
{
	if (cp->getStatus() != 0)
	{
		player_chess[cp->getStatus() - 1].remove(cp);
	}
	swap(cp, cmap[x][y]);
	cmap[x][y]->setStatus(0);
	x = cp->getX();
	y = cp->getY();
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
		for (int j = 280; j <= 440; j++)
			mmap[i][j] = j / 40 - 6;

}
void Game::chess_init()
{
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
	rook[0].setXYS(1, 1, 1);
	rook[1].setXYS(1, 8, 1);
	rook[2].setXYS(8, 1, 2);
	rook[3].setXYS(8, 8, 2);
	knight[0].setXYS(1, 2, 1);
	knight[1].setXYS(1, 7, 1);
	knight[2].setXYS(8, 2, 2);
	knight[3].setXYS(8, 7, 2);
	bishop[0].setXYS(1, 3, 1);
	bishop[1].setXYS(1, 6, 1);
	bishop[2].setXYS(8, 3, 2);
	bishop[3].setXYS(8, 6, 2);
	king[0].setXYS(1, 4, 1);
	king[1].setXYS(8, 4, 2);
	queen[0].setXYS(1, 5, 1);
	queen[1].setXYS(8, 5, 2);

	for (int i = 0; i < 8; i++)
	{
		pawn[i].setXYS(2, i + 1, 1);
		pawn[i].setLastXYS();
		pawn[i + 8].setXYS(7, i + 1, 2);
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
		rook[i + 2].setLastXYS();
		knight[i].setLastXYS();
		knight[i + 2].setLastXYS();
		bishop[i].setLastXYS();
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
}
void Game::extra_clear()
{

}
void Game::select(int tx, int ty)
{

	setfillcolor(EGERGB(0xff, 0xff, 0xff));
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 124, tx * 40 + 79);
	bar(ty * 40 + 157, tx * 40 + 42, ty * 40 + 159, tx * 40 + 79);
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 159, tx * 40 + 44);
	bar(ty * 40 + 122, tx * 40 + 77, ty * 40 + 159, tx * 40 + 79);
	setfillcolor(EGERGB(0x0, 0x0, 0x0));

}
void Game::disselect(int tx, int ty)
{
	setfillcolor(EGERGB(0x0, 0x0, 0x0));
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 124, tx * 40 + 79);
	bar(ty * 40 + 157, tx * 40 + 42, ty * 40 + 159, tx * 40 + 79);
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 159, tx * 40 + 44);
	bar(ty * 40 + 122, tx * 40 + 77, ty * 40 + 159, tx * 40 + 79);
	setfillcolor(EGERGB(0x0, 0x0, 0x0));
}
void Game::highlight(int tx, int ty)
{
	setfillcolor(EGERGB(255, 215, 0));
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 124, tx * 40 + 79);
	bar(ty * 40 + 157, tx * 40 + 42, ty * 40 + 159, tx * 40 + 79);
	bar(ty * 40 + 122, tx * 40 + 42, ty * 40 + 159, tx * 40 + 44);
	bar(ty * 40 + 122, tx * 40 + 77, ty * 40 + 159, tx * 40 + 79);
	setfillcolor(EGERGB(0x0, 0x0, 0x0));
}
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
void Game::move_chess(int x1, int y1, int x2, int  y2)
{
	cmap[x1][y1]->moveto(x2, y2);
	cover(x1, y1);
	cmap[x2][y2]->draw();
}
void Game::undo_chess(int tx, int ty)
{
	int lx = cmap[tx][ty]->getLastX();
	int ly = cmap[tx][ty]->getLastY();
	int ls = cmap[tx][ty]->getLastS();
	cmap[tx][ty]->setXYS(lx, ly, ls);
	cmap[lx][ly]->setXY(tx, ty);
	swap(cmap[tx][ty], cmap[lx][ly]);

}
Chess* Game::player_move(int st)
{
	setfont(10, 8, "Consolas");
	int t1 = 0, t2 = 0, t3 = 0, t4 = 0, tmp = 0, flag = 0;
	int t = 20;
	mouse_msg mouse = { 0 };
	for (;; delay_jfps(60))
	{
		while (mousemsg())
		{
			mouse = getmouse();
			tmp = mmap[mouse.x][mouse.y];
			if (tmp < 0)
				continue;
			//xyprintf(0, 400, "%d %d %d", mouse.x, mouse.y, flag);
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
				bar(0, 0, 30, 400);
				t += 10;
				if (!flag)
				{
					disselect(t1, t2);
					t1 = tmp / 10;
					t2 = tmp % 10;
					if (cmap[t1][t2]->getStatus() == st)
					{
						Game::select(t1, t2);
						for (uint i = 0; i < v.size(); i++)
							disselect(v[i].first, v[i].second);
						v.clear();
						cmap[t1][t2]->check(true);
						for (uint i = 0; i < v.size(); i++)
							highlight(v[i].first, v[i].second);
						//xyprintf(0, 50, "first: %c at %d %d", cmap[t1][t2]->getPos(), t1, t2);
						flag++;
						continue;
					}
				}
				if (flag == 1)
				{
					t3 = tmp / 10;
					t4 = tmp % 10;
					//xyprintf(0, 100, "second: %d %d %d", t3, t4,cmap[t3][t4]->getStatus());
					flushmouse();
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
	if (cmap[t3][t4]->getPos() == 'K')
	{
		game_end = 1;
		return cmap[t3][t4];
	}
	move_chess(t1, t2, t3, t4);
	//cmap[t1][t2]->moveto(t3, t4);
	//cover(t1, t2);
	//cmap[t3][t4]->draw();
	return cmap[t3][t4];
}
Chess* Game::naive_ai_move(int status)
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
	return cmap[v[t2].first][v[t2].second];
}
Chess* Game::simple_ai_move(int status)
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
	return cmap[v[maxt].first][v[maxt].second];
}
int max_score = 0;
Chess* maxc;
int maxi;

bool Game::dfs(int st, int dep, int score)
{

	int t1, t2, sc;
	bool flag = 0, li;
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
				if (dfs(st, dep + 1, sc))
				{
					maxc = *it;
					xyprintf(0, t++* 10, "%c", (*it)->getPos());
					maxi = i;
				}
				if (flag)
					dead_chess.pop_back();
				undo_chess(t1, t2);

			}


		}
		return false;
	}
	if (dep == 1)
	{
		for (list <Chess*>::iterator it = player_chess[2 - st].begin(); it != player_chess[2 - st].end(); it++)
		{
			for (uint i = 0; i < dead_chess.size(); i++)
			{
				if ((*it)->getX() == dead_chess[i]->getX() && (*it)->getY() == dead_chess[i]->getY())
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
				li = dfs(st, dep + 1, sc);
				if (flag)
					dead_chess.pop_back();
				undo_chess(t1, t2);
				return li;
			}
		}
	}
	else
	{
		for (list <Chess*>::iterator it = player_chess[2 - st].begin(); it != player_chess[2 - st].end(); it++)
		{
			for (uint i = 0; i < dead_chess.size(); i++)
			{
				if ((*it)->getX() == dead_chess[i]->getX() && (*it)->getY() == dead_chess[i]->getY())
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
					sc = score + importance[cmap[v[i].first][v[i].second]->getPos()];
				}
				if (sc >= max_score)
				{
					max_score = sc;
					return true;
				}
				return false;
			}
		}
	}

}
Chess* Game::normal_ai_move(int st)
{
	dfs(st, 0, 0);
	maxc->check(1);
	move_chess(maxc->getX(), maxc->getY(), v[maxi].first, v[maxi].second);
	return cmap[v[maxi].first][v[maxi].second];
}
int Game::welcome()
{
	wel_init();
	setfont(70, 50, "Microsoft YaHei UI Light");
	xyprintf(80, 100, "Chess123");
	//rectangle(200, 280, 440, 440);
	for (int i = 0; i <= 240; i++)
	{
		putpixel(200 + i, 280, EGERGB(0xff, 0xff, 0xff));
		putpixel(200 + i, 440, EGERGB(0xff, 0xff, 0xff));
		putpixel(200, 280 + i * 2 / 3, EGERGB(0xff, 0xff, 0xff));
		putpixel(440, 280 + i * 2 / 3, EGERGB(0xff, 0xff, 0xff));
		//Sleep(1);
	}
	for (int i = 0; i < 240; i++)
	{
		putpixel(200 + i, 320, EGERGB(0xff, 0xff, 0xff));
		putpixel(440 - i, 360, EGERGB(0xff, 0xff, 0xff));
		putpixel(200 + i, 400, EGERGB(0xff, 0xff, 0xff));
		//Sleep(1);
	}
	setfont(20, 15, "Consolas");
	xyprintf(300, 288, "1v1");
	xyprintf(250, 328, "1vC(naive)");
	xyprintf(250, 368, "1vC(simple)");
	xyprintf(290, 408, "Exit");
	mouse_msg mouse;
	int tmp;
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
void Game::good_game(Chess*tc)
{
	int i;
	int tx = tc->getX(), ty = tc->getY();
	for (i = 0; i <= 640; i++)
	{
		fillellipse(ty * 40 + 140, tx * 40 + 60, i, i);
		Sleep(5);
	}
}
void Game::play(int mode)
{
	Chess* (Game::*func)(int);
	if (mode == 1)
		func = &Game::player_move;
	else if (mode == 2)
		func = &Game::naive_ai_move;
	else if (mode == 3)
		func = &Game::simple_ai_move;
	else if (mode == 4)
		func = &Game::normal_ai_move;
	cleardevice();
	game_init();
	Chess* tc;
	while (1)
	{
		setfont(10, 8, "Consolas");
		xyprintf(500, 450, "Player1");
		tc = player_move(2);
		Sleep(300);
		check_check();
		if (game_end)
			break;
		bar(500, 450, 600, 460);
		setfont(10, 8, "Consolas");
		xyprintf(500, 50, "Player2");
		tc = normal_ai_move(1);
		//tc = naive_ai_move(1);
		Sleep(300);
		check_check();
		if (game_end)
			break;
		bar(500, 50, 600, 60);
		setfont(10, 8, "Consolas");
	}
	good_game(tc);
	//while (1);
}
void Game::test()
{
	game_init();
	good_game(cmap[1][1]);
}
int main()
{
	Game newgame;
	delay(60);
	initgraph(640, 480);
	setfont(37, 30, "Consolas");
	//newgame.play(newgame.welcome());
	newgame.play(1);
	closegraph();
}
