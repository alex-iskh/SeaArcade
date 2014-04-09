#ifndef SEAARCADE
#define SEAARCADE

#include <windows.h>
#include <wingdi.h>  //��� ������������� ����� � ������� RGB (x, y, z)
#include <list>

#define MIN_EN 5
#define SCOREIT 20
#define TICK_TIME 20
#define XPCOEF 4
#define BONUSCOEF 400

#define FIELDWIDTH 600
#define FIELDHEIGHT 400
#define STATHEIGHT 40
#define HEROWIDTH 27
#define HEROHEIGHT 24
#define XPWIDTH 12
#define ENEMYWIDTH 44
#define ENEMYHEIGHT 30
#define LIFEBONUSWIDTH 20
#define LIFEBONUSHEIGHT 18
#define XPBONUSWIDTH 17
#define XPBONUSHEIGHT 10
#define DOUBLEENEMBONUSWIDTH 15

#define LEFT 1
#define UP 2
#define RIGHT 3
#define DOWN 4

HBITMAP hHero, hMHero, hXp, hMXp, hEnemyL, hMEnemyL, hEnemyU, hMEnemyU, hEnemyR, hMEnemyR, hEnemyD, hMEnemyD,
	hLifeBonus, hMLifeBonus, hXpBonus, hMXpBonus, hDoubleEnemBonus, hMDoubleEnemBonus, hClearField, hGameOver,
	hClearStatLine, hPause, h1Hp, h2Hp, h3Hp, h4Hp;

class Object	//������������ ����� ��� ���� ��������, ���������� ����� ������ � ������
{
protected:
	int x, y, w, h;
	unsigned char orient;
	virtual void MoveUp() = 0;
	virtual void MoveDown() = 0;
	virtual void MoveRight() = 0;
	virtual void MoveLeft() = 0;
	virtual void SetPos() = 0;
public:
	HBITMAP * curBmp;
	HBITMAP * curMask;
	int GetX () {return this->x;}
	int GetY () {return this->y;}
	int GetWidth () {return this->w;}
	int GetHeight () {return this->h;}
	virtual void MoveAndPaint(HDC & memBit, HDC & hDCBit) = 0;
	void Paint (HDC & memBit, HDC & hDCBit)
	{
		SelectObject(hDCBit, *(this->curMask));
		BitBlt(memBit, this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight(), hDCBit, 0, 0, SRCAND);
		SelectObject(hDCBit, *(this->curBmp));
		BitBlt(memBit, this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight(), hDCBit, 0, 0, SRCPAINT);
	}
	virtual void Move() = 0;
};

class Hero: public Object
{
	void SetPos() {this->x = FIELDWIDTH/2 - HEROWIDTH/2; this->y = FIELDHEIGHT/2 - HEROHEIGHT/2;}
	void MoveLeft() {if (this->x > 0) this->x -= 5; }
	void MoveUp() {if (this->y > 0) this->y -= 5; }
	void MoveRight() {if (this->x < FIELDWIDTH-HEROWIDTH) this->x += 5; }
	void MoveDown() {if (this->y < FIELDHEIGHT-HEROWIDTH) this->y += 5; }
	unsigned char ghostModeCounter;	//��.�-� Visibility ����
	bool keyUpActivated;
	bool keyDownActivated;
	bool keyLeftActivated;
	bool keyRightActivated;
public:
	bool ghostModeOn;	//��. �-� Visibility ����
	Hero ()
	{
		this->keyUpActivated = FALSE; this->keyDownActivated = FALSE; this->keyLeftActivated = FALSE; this->keyRightActivated = FALSE;
		this->ghostModeOn = FALSE; this->ghostModeCounter = 0;
		this->curBmp = &hHero; this->curMask = &hMHero;
		this->w = HEROWIDTH; this->h = HEROHEIGHT; this->orient = 0;
		SetPos();
	}
	~Hero () {this->curBmp = NULL; this->curMask = NULL;}
	void MoveAndPaint (HDC & memBit, HDC & hDCBit)
	{
		this->Move();
		if (this->Visibility() == TRUE) {this->Paint(memBit, hDCBit);}
	}
	void Move()
	{
		if(GetAsyncKeyState(VK_LEFT)&0x8000) {keyLeftActivated = TRUE;}	//� ������� �����������, ������ �� �������
		else keyLeftActivated = FALSE;
		if(GetAsyncKeyState(VK_RIGHT)&0x8000) {keyRightActivated = TRUE;}
		else keyRightActivated = FALSE;
		if(GetAsyncKeyState(VK_UP)&0x8000) {keyUpActivated = TRUE;}
		else keyUpActivated = FALSE;
		if(GetAsyncKeyState(VK_DOWN)&0x8000) {keyDownActivated = TRUE;}
		else keyDownActivated = FALSE;

		if (this->keyLeftActivated & !(this->keyRightActivated)) MoveLeft();
		if (this->keyUpActivated & !(this->keyDownActivated)) MoveUp();
		if (this->keyRightActivated & !(this->keyLeftActivated)) MoveRight();
		if (this->keyDownActivated & !(this->keyUpActivated)) MoveDown();
	}
	void GoToStart() {SetPos();}
	bool Visibility ()	//�-�, ������������, �������� �� �������� ������� (�����. ��� ���������� "�������" ����� ������������ � ������)
	{
		if (!(this->ghostModeOn)) return TRUE;	//ghostModeOn ����������� TRUE ��� ������������ � ������
		else
		{
			++ghostModeCounter;	//������� ��������� ��������� ���� ����� ������ ���. �����, � ����� �����������, ���. �� ����� ������� � ������ ������ ������ ������
			if (ghostModeCounter>80) {ghostModeOn = FALSE; ghostModeCounter = 0; return TRUE;}
			else if (int(ghostModeCounter/10)%2 == 0) return TRUE;
			else return FALSE;
		}
	}

};

class Enemy: public Object
{
	void SetOrient()	//�-� ������ ��������� ������� ���������� � �� ��� ����������� ����������� �������� HBITMAP, �����, ������
	{
		this->orient = (rand()%4)+1;
		switch(this->orient) {
		case LEFT: this->orient = LEFT; 
			this->curBmp = &hEnemyL; 
			this->curMask = &hMEnemyL;
			this->w = ENEMYWIDTH; this->h = ENEMYHEIGHT; break;
		case UP: this->orient = UP; 
			this->curBmp = &hEnemyU; 
			this->curMask = &hMEnemyU;
			this->w = ENEMYHEIGHT; this->h = ENEMYWIDTH; break;
		case RIGHT: this->orient = RIGHT; 
			this->curBmp = &hEnemyR; 
			this->curMask = &hMEnemyR;
			this->w = ENEMYWIDTH; this->h = ENEMYHEIGHT; break;
		case DOWN: this->orient = DOWN; 
			this->curBmp = &hEnemyD; 
			this->curMask = &hMEnemyD;
			this->w = ENEMYHEIGHT; this->h = ENEMYWIDTH; break; }
	}
	void SetPos()	//� ����������� �� ���������� ���� ������������ � ���� ��� ����� ���� ����
	{
		SetOrient();
		switch (this->orient)
		{
			case LEFT: this->x = FIELDWIDTH; this->y = rand()%(FIELDHEIGHT-ENEMYHEIGHT); break;
			case UP: this->x = rand()%(FIELDWIDTH-ENEMYHEIGHT); this->y = FIELDHEIGHT; break;
			case RIGHT: this->x = 0-ENEMYWIDTH; this->y = rand()%(FIELDHEIGHT-ENEMYHEIGHT); break;
			case DOWN: this->x = rand()%(FIELDWIDTH-ENEMYHEIGHT); this->y = 0-ENEMYWIDTH; break;
		}
	}
	void MoveLeft() {if (this->x > -ENEMYWIDTH) this->x -= 4; else SetPos();}	//����� ���������� ���������������� ���� ������������ ����� �������
	void MoveUp() {if (this->y > -ENEMYWIDTH) this->y -= 4; else SetPos();}
	void MoveRight() {if (this->x < FIELDWIDTH) this->x += 4; else SetPos();}
	void MoveDown() {if (this->y < FIELDHEIGHT) this->y += 4; else SetPos();}
public:
	Enemy() {SetPos();}
	~Enemy() {this->curBmp = NULL; this->curMask = NULL; }
	void MoveAndPaint(HDC & memBit, HDC & hDCBit) {this->Move(); this->Paint(memBit, hDCBit);}
	void Move()
	{
		switch(this->orient)
		{
		case LEFT: MoveLeft(); break;
		case UP: MoveUp(); break;
		case RIGHT: MoveRight(); break;
		case DOWN: MoveDown(); break;
		}
	}
};

class Xp: public Object
{
	void SetOrient() {this->orient = (rand()%4)+1;	}	//�������� ��������� ����������
	void SetPos()
	{
		SetOrient();	//� ����������� �� ���������� xp ������������ � ���� ��� ����� ���� ����
		switch (this->orient)
		{
			case LEFT: this->x = FIELDWIDTH; this->y = rand()%(FIELDHEIGHT-XPWIDTH); break;
			case UP: this->x = rand()%(FIELDWIDTH-XPWIDTH); this->y = FIELDHEIGHT; break;
			case RIGHT: this->x = 0-XPWIDTH; this->y = rand()%(FIELDHEIGHT-XPWIDTH); break;
			case DOWN: this->x = rand()%(FIELDWIDTH-XPWIDTH); this->y = 0-XPWIDTH; break;
		}
	}
	void MoveLeft() {if (this->x > -XPWIDTH) this->x -= 2; else shouldBeDestroyed = TRUE;}	//����� ���������� �������������. ���� xp ���������� ��� ����������� � ��������
	void MoveUp() {if (this->y > -XPWIDTH) this->y -= 2; else shouldBeDestroyed = TRUE;}
	void MoveRight() {if (this->x < FIELDWIDTH) this->x += 2; else shouldBeDestroyed = TRUE;}
	void MoveDown() {if (this->y < FIELDHEIGHT) this->y += 2; else shouldBeDestroyed = TRUE;}
public:
	bool shouldBeDestroyed;
	Xp()
	{
		this->w = XPWIDTH; this->h = XPWIDTH; this->curBmp = &hXp; this->curMask = &hMXp; this->shouldBeDestroyed = FALSE;
		SetPos();
	}
	~Xp() {this->curBmp = NULL; this->curMask = NULL; }
	void MoveAndPaint(HDC & memBit, HDC & hDCBit) {this->Move(); this->Paint(memBit, hDCBit);}
	void Move()
	{
		switch(this->orient)
		{
		case LEFT: MoveLeft(); break;
		case UP: MoveUp(); break;
		case RIGHT: MoveRight(); break;
		case DOWN: MoveDown(); break;
		}
	}
};

class Bonus: public Object	//������������ ����� ��� ������� ���������� ����
{
protected:
	void SetOrient() {this->orient = (rand()%4)+1;}
	void SetPos()
	{
		SetOrient();
		switch (this->orient)
		{
			case LEFT: this->x = FIELDWIDTH; this->y = rand()%(FIELDHEIGHT-this->h); break;
			case UP: this->x = rand()%(FIELDWIDTH-this->w); this->y = FIELDHEIGHT; break;
			case RIGHT: this->x = 0-this->w; this->y = rand()%(FIELDHEIGHT-this->h); break;
			case DOWN: this->x = rand()%(FIELDWIDTH-this->w); this->y = 0-this->h; break;
		}
	}
	void MoveLeft() {if (this->x > -XPWIDTH) this->x -= 2; else shouldBeDestroyed = TRUE;}
	void MoveUp() {if (this->y > -XPWIDTH) this->y -= 2; else shouldBeDestroyed = TRUE;}
	void MoveRight() {if (this->x < FIELDWIDTH) this->x += 2; else shouldBeDestroyed = TRUE;}
	void MoveDown() {if (this->y < FIELDHEIGHT) this->y += 2; else shouldBeDestroyed = TRUE;}
public:
	bool shouldBeDestroyed;
	void MoveAndPaint(HDC & memBit, HDC & hDCBit) {this->Move(); this->Paint(memBit, hDCBit);}
	void Move()
	{
		switch(this->orient)
		{
		case LEFT: MoveLeft(); break;
		case UP: MoveUp(); break;
		case RIGHT: MoveRight(); break;
		case DOWN: MoveDown(); break;
		}
	}
};

class LifeBonus: public Bonus
{
public:
	LifeBonus() {
		this->w = LIFEBONUSWIDTH; this->h = LIFEBONUSHEIGHT; this->curBmp = &hLifeBonus; this->curMask = &hMLifeBonus; this->shouldBeDestroyed = FALSE;
		SetPos();
	}
	~LifeBonus() {this->curBmp = NULL; this->curMask = NULL; }
};

class XpBonus: public Bonus
{
	public:
	XpBonus() {
		this->w = XPBONUSWIDTH; this->h = XPBONUSHEIGHT; this->curBmp = &hXpBonus; this->curMask = &hMXpBonus; this->shouldBeDestroyed = FALSE;
		SetPos();
	}
	~XpBonus() {this->curBmp = NULL; this->curMask = NULL; }
};

class DoubleEnemBonus: public Bonus
{
	public:
	DoubleEnemBonus() {
		this->w = DOUBLEENEMBONUSWIDTH; this->h = DOUBLEENEMBONUSWIDTH; this->curBmp = &hDoubleEnemBonus; this->curMask = &hMDoubleEnemBonus; this->shouldBeDestroyed = FALSE;
		SetPos();
	}
	~DoubleEnemBonus() {this->curBmp = NULL; this->curMask = NULL; }
};

bool Collision (Object * a, Hero * hero)	//�-� ��� ��������� ����������� ������� � ����������
{	//����������� �������������, ����� �������� � ������ ����� ������� ��������������� ������, ��� 0.04 ������� �������
	if (a->GetX() >= hero->GetX())
	{
		if ((a->GetX()-hero->GetX()) <= hero->GetWidth()-a->GetWidth()/5)
		{
			if (a->GetY() >= hero->GetY())
			{
				if ((a->GetY()-hero->GetY()) <= hero->GetHeight()-a->GetHeight()/5) return TRUE;
			}
			else
			{
				if ((hero->GetY()-a->GetY()) <= a->GetHeight()*0.8) return TRUE;
			}
		}
	}
	else
	{
		if ((hero->GetX()-a->GetX()) <= a->GetWidth()*0.8)
		{
			if (a->GetY() >= hero->GetY())
			{
				if ((a->GetY()-hero->GetY()) <= hero->GetHeight()-a->GetHeight()/5) return TRUE;
			}
			else
			{
				if ((hero->GetY()-a->GetY()) <= a->GetHeight()*0.8) return TRUE;
			}
		}
	}
	return FALSE;
}

HWND hWndMain = NULL;
HWND hWndField = NULL;
HWND hWndStat = NULL;	//����������� �������� ����, ���� � ������ ���������� ��������������

Hero* hero = NULL;	//������� �� ����: �����
Enemy enAr [50];	//������ ������ (�� ���� ��������� ����� ��� ������ ������ ������� �� ����)
std::list<Xp> xpList;	//������ xp, �� ����� ������������ ��������
LifeBonus* lifeBonus = NULL;	//�����, ������ ���. hp
XpBonus* xpBonus = NULL;	//�����, ������������� ����. ��������� xp
DoubleEnemBonus* doubleEnemBonus = NULL;	//�����, ������������� ����� ������ �� ����

unsigned char enCount;	//����� ������ �� ����
unsigned char bonusCoef;	//����. ��������� xp
unsigned int xpBonusEffectCounter;	//������� ����������������� �������� xpBonus
bool lifeBonusCreated;
bool xpBonusCreated;
bool doubleEnemBonusCreated;
int score;	//����
unsigned char hp;	//����� ����� ��������
bool gamePaused;

void SetGlobalVarToDefault ()	//�-�, ���������� ��� ������� ��������� � ������� � ���������� ���������
{
	hero = new Hero();
	for(int i = 0; i<50; ++i)
	{
		Enemy * newEnemy = new Enemy;
		enAr[i] = *newEnemy;
		delete newEnemy;
	}
	xpList.clear();
	enCount = MIN_EN;
	bonusCoef = 1;
	xpBonusEffectCounter = 0;
	lifeBonusCreated = FALSE;
	xpBonusCreated = FALSE;
	doubleEnemBonusCreated = FALSE;
	score = 0;
	hp = 3;
	gamePaused = FALSE;
}

void StartGame()
{
	SetGlobalVarToDefault();
	InvalidateRect (hWndStat, NULL, 0);	//����������� �������� ���� ����� ������ ���. ���������� � ������
	InvalidateRect (hWndStat, NULL, 0);
	ShowWindow (hWndField, SW_SHOW);	//��� ������ ������� �������� ���� ���������� ������ �������� ����
	ShowWindow (hWndStat, SW_SHOW);
	SetTimer(hWndField, 1, TICK_TIME, NULL);	//����������� ������, �� ������������ ������ ����� ����������� ������� �-�
}

void GameOver ();

void GameTick (HDC & memBit, HDC & hDCBit)	//������� ������� �-�
{
	if (gamePaused) return;		//����� ���� ���������� �� �����, ������� ������� ������������
	SelectObject(hDCBit, hClearField);
	BitBlt(memBit,0,0, FIELDWIDTH, FIELDHEIGHT, hDCBit, 0, 0, SRCCOPY);		//������ ������ ���� � memBit

	hero->MoveAndPaint(memBit, hDCBit);		//����������� � ������ ���������

	if (rand()%(XPCOEF*TICK_TIME/bonusCoef) == 0)	//� ������������ ����� ����������� ���������� ����� Xp
	{
		Xp * newXp = new Xp;
		xpList.push_back(*(new Xp));
		delete newXp;
	}
	for (std::list<Xp>::iterator i = xpList.begin(); i != xpList.end();)
	{
		if (Collision(&(*i), hero))		//��� ������� ������������� Xp � ������ ����������� � ���������� �������� xp �� ��������, ����������� ���� � ��������� ����������
		{
			i->shouldBeDestroyed = TRUE;
			score += SCOREIT;
			InvalidateRect(hWndStat, NULL, 0);
			if ((score%(10*SCOREIT)==0)&&(enCount<50)) ++enCount;	//��� ������ ���. ���� ����� ��������� ��� ������ ����� �� ���� (���� �� ��� �� ��������)
		}
		if (i->shouldBeDestroyed) i = xpList.erase(i);	//�������, ���� ������� �� �������� (��������� � ���������� ��� ����� �� ���� ������)
		else
		{
			i->MoveAndPaint(memBit, hDCBit);	//����� ������ ����������� � ������, � ����� ��������� � ����������
			++i;
		}
	}

	for (int i = 0; i < enCount; ++i)	//�����. ������� ����� �� ����� ���, ��� �� ����
	{
		if (Collision(&enAr[i], hero)&&(!hero->ghostModeOn)) {hero->ghostModeOn = TRUE; hero->GoToStart(); --hp; InvalidateRect(hWndStat, NULL, 0);}
		//��� �����������, ����� �������� �� � ghostMode, �� ��� ������������ � ���. ���������, ���������� ���� hp � ����������� ghostMode
		enAr[i].MoveAndPaint(memBit, hDCBit);	//������� � ��������������
	}

	if (lifeBonusCreated)
	{
		lifeBonus->MoveAndPaint(memBit, hDCBit);
		if (Collision(lifeBonus, hero))
		{
			if(hp<4) {++hp; InvalidateRect(hWndStat, NULL, 0);}
			lifeBonus->shouldBeDestroyed = TRUE;
		}
		if (lifeBonus->shouldBeDestroyed) {delete lifeBonus; lifeBonusCreated = FALSE;}
	}	//���� ������ ���� �����, �� ��� ��������, � ��� ������������ � ���������� ������������� hp; ��� ����. ���� ��� ������. � ����. ��������� �� ����� shouldBeDestroyed
	else
		if (rand()%(BONUSCOEF*TICK_TIME) == 0)		//���� �� ��� �� ������, �� ����� ���� ������ � ���. ����� �����������
		{
			lifeBonusCreated = TRUE;
			lifeBonus = new LifeBonus;
		}

	if (xpBonusCreated)
	{
		xpBonus->MoveAndPaint(memBit, hDCBit);
		if (Collision(xpBonus, hero)) {bonusCoef = 10; xpBonusEffectCounter = 1; xpBonus->shouldBeDestroyed = TRUE;}
		if (xpBonus->shouldBeDestroyed) {delete xpBonus; xpBonusCreated = FALSE;}
	}
	else
		if (rand()%(BONUSCOEF*TICK_TIME) == 0)
		{
			xpBonusCreated = TRUE;
			xpBonus = new XpBonus;
		}		//��� ��� � � ����. �������, ������ ��� ������. ����������� ����. ��������� xp � ��������� �������
	if (xpBonusEffectCounter > 0)	//��������� ����������� �������� - ����� ����. �����. ����� ������ ��������, ������� ������������
	{
		++xpBonusEffectCounter;
		if (xpBonusEffectCounter>200) {bonusCoef = 1; xpBonusEffectCounter = 0;}
	}

	if (doubleEnemBonusCreated)
	{
		doubleEnemBonus->MoveAndPaint(memBit, hDCBit);
		if (Collision(doubleEnemBonus, hero))
		{
			if ((2*enCount)<50) enCount=int(2*enCount); else enCount=50;
			doubleEnemBonus->shouldBeDestroyed = TRUE;
		}
		if (doubleEnemBonus->shouldBeDestroyed) {delete doubleEnemBonus; doubleEnemBonusCreated = FALSE;}
	}	//��� � � ���� �������, ������ ��������� ����� ������ (��� ������ ����������� ���������, ���� ��������� ����� ������)
	else
		if (rand()%(BONUSCOEF*TICK_TIME) == 0)
		{
			doubleEnemBonusCreated = TRUE;
			doubleEnemBonus = new DoubleEnemBonus;
		}

	if (hp==0)		//����� ������������� hp, �������������� ���� ����� ���� � ����������� �-� GameOver
	{
		SelectObject(hDCBit, hGameOver);
		BitBlt(memBit, 0, 0, FIELDWIDTH, FIELDHEIGHT, hDCBit, 0, 0, SRCCOPY);
		GameOver ();
	}
	InvalidateRect(hWndField, NULL, 0);	//����������� ���� �������� ����
}

void RefreshStat (HDC & memBit, HDC & hDCBit, HFONT & hFont)
{
	TCHAR buf[10];
	if (gamePaused)		//���� ���� �� �����, ������������ ������� PAUSED
		{
			SelectObject(hDCBit, hPause);
			BitBlt(memBit,0,0, FIELDWIDTH, STATHEIGHT, hDCBit, 0, 0, SRCCOPY);
		}
	else
		{
			SelectObject(hDCBit, hClearStatLine);
			BitBlt(memBit,0,0, FIELDWIDTH, STATHEIGHT, hDCBit, 0, 0, SRCCOPY);
			switch (hp)
			{
				case 1: SelectObject(hDCBit, h1Hp); BitBlt(memBit, FIELDWIDTH-148, 0, 148, 40, hDCBit, 0, 0, SRCCOPY); break;
				case 2: SelectObject(hDCBit, h2Hp); BitBlt(memBit, FIELDWIDTH-148, 0, 148, 40, hDCBit, 0, 0, SRCCOPY); break;
				case 3: SelectObject(hDCBit, h3Hp); BitBlt(memBit, FIELDWIDTH-148, 0, 148, 40, hDCBit, 0, 0, SRCCOPY); break;
				case 4: SelectObject(hDCBit, h4Hp); BitBlt(memBit, FIELDWIDTH-148, 0, 148, 40, hDCBit, 0, 0, SRCCOPY); break;
			}
			SetBkMode (memBit, TRANSPARENT);
			SelectObject (memBit, hFont);
			SetTextColor(memBit, RGB(255,255,255));
			_itot(score, buf, 10);
			TextOut(memBit, 145, -3, buf, _tcslen(buf));
		}	//����� ��������� ���� � ����� ����� ��������
}

void GameOver ()
{
	KillTimer(hWndField,1);		//��������������� ������ � ��������� ��� ����������� ��������� ������� �������
	if (hero) {delete hero; hero = NULL;}
	if (lifeBonusCreated) delete lifeBonus;
	if (xpBonusCreated) delete xpBonus;
	if (doubleEnemBonusCreated) delete doubleEnemBonus;
}

#endif