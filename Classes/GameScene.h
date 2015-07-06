
#ifndef  _Inkmoo_GameScene_h_
#define  _Inkmoo_GameScene_h_

#include "cocos2d.h"
#include "GameDefine.h"

USING_NS_CC;

class SpriteShape;

class GameScene : public Layer
{
public:
	GameScene();
	static Scene* createScene();
	CREATE_FUNC(GameScene);
	int m_time;
	
	void myClock( float dt );
	virtual bool init();
	virtual void onEnter();
	virtual void onEnterTransitionDidFinish();
	virtual void onExit();
	virtual void onExitTransitionDidStart();
	virtual void cleanup();

	void update( float dt );
	bool onTouchBegan(Touch *touch, Event *unused);
    void onTouchMoved(Touch *touch, Event *unused);

	void addTime();

	void menuBackCallback(Ref *pSender);
	void menuSetCallback(Ref *pSender);
	void gameOver(float dt);
	CC_SYNTHESIZE(int, score, Score);

private:

	float mapLBX,mapLBY;

	SpriteBatchNode *spriteSheet;

	SpriteShape* map[ROWS][COLS];

	bool isAction;

	bool isTouchEna;

    bool isFillSprite;

	SpriteShape* staSprite;
	SpriteShape* endSprite;

    bool fourBoom;



	void createSprite( int row , int col );

	Point positionOfItem(int row,int col);

	void initMap();

	void fillSprite();

	void checkAndRemoveSprite();

	void removeSprite();

	void getColChain(SpriteShape *spr, std::list<SpriteShape *> &chainList);
    void getRowChain(SpriteShape *spr, std::list<SpriteShape *> &chainList);
	// 
	void markRemove( SpriteShape* spr );

	void explodeSprite( SpriteShape* spr );
	void explodeSpecialH(Point point);
    void explodeSpecialV(Point point);
	//
	void actionEndCallback(Node *node);

	void swapSprite();

	SpriteShape *spriteOfPoint(Point *point);

	bool isHaveMove();
	int find( int r1 , int c1 , int r2 , int c2 , int r3 , int c3 );
};





#endif
