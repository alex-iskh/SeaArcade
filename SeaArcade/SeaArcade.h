#ifndef SEAARCADE
#define SEAARCADE

#include <windows.h>
#include <wingdi.h>  //для представления цвета в формате RGB (x, y, z)
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

class Object	//родительский класс для всех объектов, содержащий общие методы и данные
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
	unsigned char ghostModeCounter;	//см.ф-ю Visibility ниже
	bool keyUpActivated;
	bool keyDownActivated;
	bool keyLeftActivated;
	bool keyRightActivated;
public:
	bool ghostModeOn;	//см. ф-ю Visibility ниже
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
		if(GetAsyncKeyState(VK_LEFT)&0x8000) {keyLeftActivated = TRUE;}	//в условии проверяется, нажата ли клавиша
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
	bool Visibility ()	//ф-я, определяющая, является ли персонаж видимым (необх. для реализации "мигания" после столкновения с врагом)
	{
		if (!(this->ghostModeOn)) return TRUE;	//ghostModeOn объявляется TRUE при столкновении с врагом
		else
		{
			++ghostModeCounter;	//счетчик позволяет выключить этот режим спустя опр. время, а также определения, явл. ли герой видимым в каждый момент работы режима
			if (ghostModeCounter>80) {ghostModeOn = FALSE; ghostModeCounter = 0; return TRUE;}
			else if (int(ghostModeCounter/10)%2 == 0) return TRUE;
			else return FALSE;
		}
	}

};

class Enemy: public Object
{
	void SetOrient()	//ф-я задает случайным образом ориентацию и по ней присваивает необходимые значения HBITMAP, длины, ширины
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
	void SetPos()	//в зависимости от ориентации враг генерируется с того или иного края поля
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
	void MoveLeft() {if (this->x > -ENEMYWIDTH) this->x -= 4; else SetPos();}	//после достижения противоположного края генерируется новая позиция
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
	void SetOrient() {this->orient = (rand()%4)+1;	}	//задается случайная ориентация
	void SetPos()
	{
		SetOrient();	//в зависимости от ориентации xp генерируется с того или иного края поля
		switch (this->orient)
		{
			case LEFT: this->x = FIELDWIDTH; this->y = rand()%(FIELDHEIGHT-XPWIDTH); break;
			case UP: this->x = rand()%(FIELDWIDTH-XPWIDTH); this->y = FIELDHEIGHT; break;
			case RIGHT: this->x = 0-XPWIDTH; this->y = rand()%(FIELDHEIGHT-XPWIDTH); break;
			case DOWN: this->x = rand()%(FIELDWIDTH-XPWIDTH); this->y = 0-XPWIDTH; break;
		}
	}
	void MoveLeft() {if (this->x > -XPWIDTH) this->x -= 2; else shouldBeDestroyed = TRUE;}	//после достижения противоположн. края xp помечается как необходимое к удалению
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

class Bonus: public Object	//родительский класс для бонусов различного типа
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

bool Collision (Object * a, Hero * hero)	//ф-я для обработки пересечения объекта с персонажем
{	//пересечение засчитывается, когда персонаж и объект имеют площадь соприкосновения больше, чем 0.04 площади объекта
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
HWND hWndStat = NULL;	//дескрипторы главного окна, поля и строка статистики соответственно

Hero* hero = NULL;	//объекты на поле: герой
Enemy enAr [50];	//массив врагов (по мере набирания очков все больше врагов выходит на поле)
std::list<Xp> xpList;	//список xp, их число генерируется случайно
LifeBonus* lifeBonus = NULL;	//бонус, дающий доп. hp
XpBonus* xpBonus = NULL;	//бонус, увеличивающий коэф. генерации xp
DoubleEnemBonus* doubleEnemBonus = NULL;	//бонус, увеличивающий число врагов на поле

unsigned char enCount;	//число врагов на поле
unsigned char bonusCoef;	//коэф. генерации xp
unsigned int xpBonusEffectCounter;	//счетчик продолжительности действия xpBonus
bool lifeBonusCreated;
bool xpBonusCreated;
bool doubleEnemBonusCreated;
int score;	//счет
unsigned char hp;	//число очков здоровья
bool gamePaused;

void SetGlobalVarToDefault ()	//ф-я, приводящая все игровые параметры и объекты к начальному состоянию
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
	InvalidateRect (hWndStat, NULL, 0);	//перерисовка дочерних окон после сброса игр. параметров к началу
	InvalidateRect (hWndStat, NULL, 0);
	ShowWindow (hWndField, SW_SHOW);	//при первом запуске дочерние окна появляются поверх главного окна
	ShowWindow (hWndStat, SW_SHOW);
	SetTimer(hWndField, 1, TICK_TIME, NULL);	//запускается таймер, по срабатыванию корого будет запускаться игровая ф-я
}

void GameOver ();

void GameTick (HDC & memBit, HDC & hDCBit)	//главная игровая ф-я
{
	if (gamePaused) return;		//когда игра поставлена на паузу, игровой процесс прекращается
	SelectObject(hDCBit, hClearField);
	BitBlt(memBit,0,0, FIELDWIDTH, FIELDHEIGHT, hDCBit, 0, 0, SRCCOPY);		//рисуем чистое поле в memBit

	hero->MoveAndPaint(memBit, hDCBit);		//передвигаем и рисуем персонажа

	if (rand()%(XPCOEF*TICK_TIME/bonusCoef) == 0)	//с определенной долей вероятности генерируем новый Xp
	{
		Xp * newXp = new Xp;
		xpList.push_back(*(new Xp));
		delete newXp;
	}
	for (std::list<Xp>::iterator i = xpList.begin(); i != xpList.end();)
	{
		if (Collision(&(*i), hero))		//для каждого существующего Xp в случае пересечения с персонажем помечаем xp на удаление, увеличиваем счет и обновляем статистику
		{
			i->shouldBeDestroyed = TRUE;
			score += SCOREIT;
			InvalidateRect(hWndStat, NULL, 0);
			if ((score%(10*SCOREIT)==0)&&(enCount<50)) ++enCount;	//при наборе опр. чила очков добавляем еще одного врага на поле (если их еще не максимум)
		}
		if (i->shouldBeDestroyed) i = xpList.erase(i);	//удаляем, если помечен на удаление (пересекся с персонажем или дошел до края экрана)
		else
		{
			i->MoveAndPaint(memBit, hDCBit);	//иначе просто передвигаем и рисуем, а потом переходим к следующему
			++i;
		}
	}

	for (int i = 0; i < enCount; ++i)	//рассм. каждого врага из числа тех, что на поле
	{
		if (Collision(&enAr[i], hero)&&(!hero->ghostModeOn)) {hero->ghostModeOn = TRUE; hero->GoToStart(); --hp; InvalidateRect(hWndStat, NULL, 0);}
		//при столновении, когда персонаж не в ghostMode, то его перекидывает в нач. состояние, отнимается одно hp и запускается ghostMode
		enAr[i].MoveAndPaint(memBit, hDCBit);	//двигаем и перерисовываем
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
	}	//если создан этот бонус, то они движется, а при столкновении с персонажем увеличивается hp; при дост. края или столкн. с перс. удаляется по флагу shouldBeDestroyed
	else
		if (rand()%(BONUSCOEF*TICK_TIME) == 0)		//если он еще не создан, то может быть создан с опр. долей вероятности
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
		}		//все как и с пред. бонусом, только при столкн. увеличивает коэф. генерации xp и запускает счетчик
	if (xpBonusEffectCounter > 0)	//обработка запущенного счетчика - после дост. опред. числа эффект исчезает, счетчик сбрасывается
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
	}	//как и с пред бонусом, только удваивает число врагов (или делает максимально возможным, если удвоенное число больше)
	else
		if (rand()%(BONUSCOEF*TICK_TIME) == 0)
		{
			doubleEnemBonusCreated = TRUE;
			doubleEnemBonus = new DoubleEnemBonus;
		}

	if (hp==0)		//когда заканчиваются hp, отрисовывается окно конца игры и запускается ф-я GameOver
	{
		SelectObject(hDCBit, hGameOver);
		BitBlt(memBit, 0, 0, FIELDWIDTH, FIELDHEIGHT, hDCBit, 0, 0, SRCCOPY);
		GameOver ();
	}
	InvalidateRect(hWndField, NULL, 0);	//перерисовка окна игрового поля
}

void RefreshStat (HDC & memBit, HDC & hDCBit, HFONT & hFont)
{
	TCHAR buf[10];
	if (gamePaused)		//если игра на паузе, отображается надпись PAUSED
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
		}	//иначе выводится счет и число очков здоровья
}

void GameOver ()
{
	KillTimer(hWndField,1);		//останавливается таймер и удаляются все динамически созданные игровые объекты
	if (hero) {delete hero; hero = NULL;}
	if (lifeBonusCreated) delete lifeBonus;
	if (xpBonusCreated) delete xpBonus;
	if (doubleEnemBonusCreated) delete doubleEnemBonus;
}

#endif