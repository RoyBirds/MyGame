
#include "GameScene.h"
#include "SpriteShape.h"
#include "MainScene.h"
#include "GameOverScene.h"

using namespace CocosDenshion;

GameScene::GameScene()
: spriteSheet(NULL)
, m_time(60)
, score(0)
, isFillSprite(false)
, isAction(true)//start with drop animation
, isTouchEna(true)
, staSprite(NULL)
, endSprite(NULL)
, fourBoom(true)//drop animation is vertical
{

}

Scene* GameScene::createScene( )
{
	auto scene = Scene::create();
	auto layer = GameScene::create();
	scene -> addChild( layer );
	return scene;
}

bool GameScene::init( )
{
	if( !Layer::init() ){
		return false;
	}


	auto sprite = Sprite::create("scene_bg.png");
	sprite->setPosition(Point(GAME_SCREEN_WIDTH/2,GAME_SCREEN_HEIGHT/2));
    this->addChild(sprite,-1);


	auto backItem = MenuItemImage::create(
                                           "btn_back01.png",
                                           "btn_back02.png",
										   CC_CALLBACK_1(GameScene::menuBackCallback, this));
	backItem->setPosition(Vec2(GAME_SCREEN_WIDTH-backItem->getContentSize().width/2,backItem->getContentSize().height/2));

	
	
	auto menu = Menu::create(backItem, NULL);
    menu->setPosition(Vec2::ZERO);
	this -> addChild( menu );

	if(!userDefault->getIntegerForKey("Int")){
		 userDefault->setIntegerForKey("Int",0);
	}

	TTFConfig config("haibaoti.ttf",30);

	auto labelHScore = Label::createWithTTF(config, "Highest: 0");
	labelHScore -> setPosition(Vec2( GAME_SCREEN_WIDTH - labelHScore->getContentSize().width , GAME_SCREEN_HEIGHT - labelHScore->getContentSize().height ));
	labelHScore -> setString( StringUtils::format("Highest: %d ",userDefault->getIntegerForKey("Int")));
	this->addChild(labelHScore);


	auto labelScore = Label::createWithTTF(config,"Score: 0");
	labelScore -> setPosition(Vec2( GAME_SCREEN_WIDTH/2 , GAME_SCREEN_HEIGHT - labelScore->getContentSize().height*2.6 ));
	labelScore -> setTag(10);
	this->addChild(labelScore);


	auto labelTime = Label::createWithTTF(config,"Time: 60");
	labelTime -> setPosition(Vec2( labelTime->getContentSize().width , GAME_SCREEN_HEIGHT - labelTime->getContentSize().height ));
	labelTime -> setTag(11);
	this ->addChild(labelTime);
	


	mapLBX = (GAME_SCREEN_WIDTH - SPRITE_WIDTH * COLS - (COLS - 1) * BOADER_WIDTH) / 2;
	mapLBY = (GAME_SCREEN_HEIGHT - SPRITE_WIDTH * ROWS - (ROWS - 1) * BOADER_WIDTH) / 2;


    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("icon.plist");
    spriteSheet = SpriteBatchNode::create("icon.png");
    addChild(spriteSheet);

	initMap();
	scheduleUpdate();
	schedule(schedule_selector(GameScene::myClock),1.0f );


    auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameScene::onTouchMoved, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

	return true;
}

void GameScene::createSprite( int row , int col )
{

	SpriteShape *spr = SpriteShape::create(row, col);
    

    Point endPosition = positionOfItem(row, col);
	Point startPosition = Point(endPosition.x, endPosition.y + GAME_SCREEN_HEIGHT / 2);
    spr->setPosition(startPosition);
	float speed = startPosition.y / (1.5 * GAME_SCREEN_HEIGHT );
    spr->runAction(MoveTo::create(speed, endPosition));

    spriteSheet->addChild(spr);

    map[row][col] = spr;
	
}


Point GameScene::positionOfItem(int row, int col)
{
	float x = mapLBX + (SPRITE_WIDTH + BOADER_WIDTH) * col + SPRITE_WIDTH / 2;
    float y = mapLBY + (SPRITE_WIDTH + BOADER_WIDTH) * row + SPRITE_WIDTH / 2;
    return Point(x, y);
}


void GameScene::initMap( )
{
	for( int r = 0 ; r < ROWS ; ++r ){
		for( int c = 0 ; c < COLS ; ++c ){
			createSprite(r,c);
		}
	}
}


void GameScene::update( float t )
{

	if ( isAction ) {

        isAction = false;
		for( int r = 0 ; r < ROWS ; ++r )	{
			for( int c = 0  ; c < COLS ; ++c )	{
				SpriteShape* spr = map[r][c];
				if (spr && spr->getNumberOfRunningActions() > 0) {
					isAction = true;
					break;
				}
			}
		}
	}
    

	isTouchEna = !isAction;
    
    if (!isAction) {
		if ( isFillSprite ) {

            fillSprite();
            isFillSprite = false;


			if( isHaveMove() == false )
			{
				for( int r = 0 ; r < ROWS ; ++r )
					for( int c = 0 ; c < COLS ; ++c )
						map[r][c]->setIsNeedRemove(true);
				removeSprite();
			}
        } else {
            checkAndRemoveSprite();
        }
    }


	Label *labelScore = (Label *)this -> getChildByTag(10);
	labelScore -> setString( StringUtils::format("Score: %d ",this->getScore()));

}

void GameScene::fillSprite() {
	־
	fourBoom = true;
	isAction = true;
	int sum = 0;

    int *colEmptyInfo = (int *)malloc(sizeof(int) * COLS);
    memset((void *)colEmptyInfo, 0, sizeof(int) * COLS);
    

	SpriteShape *spr = NULL;
    for (int c = 0; c < COLS; c++) {
        int removedSpriteOfCol = 0;

        for (int r = 0; r < ROWS; r++ ) {
            spr = map[r][c];
            if ( spr == NULL ) {
                ++removedSpriteOfCol;
            } else {
                if ( removedSpriteOfCol > 0) {
                    // evey item have its own drop distance
                    int newRow = r - removedSpriteOfCol;
                    // switch in matrix
                    map[newRow][c] = spr;
                    map[r][c] = NULL;
                    // move to new position
                    Point startPosition = spr->getPosition();
                    Point endPosition = positionOfItem(newRow, c);
					float speed = (startPosition.y - endPosition.y) / GAME_SCREEN_HEIGHT*3;
                    spr->stopAllActions();// must stop pre drop action
                    spr->runAction(CCMoveTo::create(speed, endPosition));
                    // set the new row to item
                    spr->setRow(newRow);
                }
            }
        }
        

        colEmptyInfo[c] = removedSpriteOfCol;
		sum+=removedSpriteOfCol;
    }
    

    for (int c = 0; c < COLS; ++c ) {
        for (int r = ROWS - colEmptyInfo[c]; r < ROWS ; ++r ) {
            createSprite(r,c);
        }
    }
    
	setScore(getScore()+sum*30);
    free(colEmptyInfo);
}

void GameScene::checkAndRemoveSprite()
{
	SpriteShape *spr;

	for( int r = 0 ; r < ROWS ; ++r )	{
		for( int c = 0 ; c < COLS ; ++c )	{
			spr = map[r][c];
			if( !spr )	{
				continue;
			}
			spr -> setIgnoreCheck(false);
		}
	}

	for( int r = 0 ; r < ROWS ; ++r )	{
		for( int c = 0 ; c < COLS ; ++c )	{
			spr = map[r][c];
			if( !spr )	{
				continue;
			}

			if( spr -> getIsNeedRemove() )	{
				continue;
			}

			if ( spr -> getIgnoreCheck() ) {
				continue;
			}
			std::list< SpriteShape *> colChainList;
			getColChain( spr , colChainList );

			std::list< SpriteShape *> rowChainList;
			getRowChain( spr , rowChainList );

			std::list< SpriteShape *> &longerList = colChainList.size() > rowChainList.size() ? colChainList : rowChainList;
			if( longerList.size() < 3 )	{
				continue;
			}

			std::list<SpriteShape *>::iterator itList;
			bool isSetedIgnoreCheck = false;

			for( itList = longerList.begin() ; itList != longerList.end() ; ++itList ) {
				spr = ( SpriteShape * )* itList;
				if( !spr )	{
					continue;
				}

				if( longerList.size() > 3 )	{
					if ( spr == staSprite || spr == endSprite ) {
						isSetedIgnoreCheck = true;
						spr->setIgnoreCheck(true);
						spr->setIsNeedRemove(false);
						spr->setDisplayMode( fourBoom ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
						continue;
					}
				}
				markRemove( spr );
			}   

			if (!isSetedIgnoreCheck && longerList.size() > 3) {
				spr -> setIgnoreCheck(true);
				spr -> setIsNeedRemove(false);
				spr -> setDisplayMode( fourBoom ? DISPLAY_MODE_VERTICAL : DISPLAY_MODE_HORIZONTAL);
			}
		}
	}


	removeSprite();
}


void GameScene::removeSprite()
{

    isAction = true;
    
	for( int r = 0 ; r < ROWS ; ++r )	{
		for( int c = 0 ; c < COLS ; ++c )	{
			SpriteShape* spr = map[r][c];
			if( !spr )	{
				continue;
			}

			if( spr -> getIsNeedRemove() )	{
				isFillSprite = true;

				if( spr -> getDisplayMode() == DISPLAY_MODE_HORIZONTAL)
				{
					this->m_time += 3;
					addTime();
					explodeSpecialH( spr -> getPosition() );
				}
				else if ( spr -> getDisplayMode() == DISPLAY_MODE_VERTICAL)
				{
					this->m_time += 3;
					addTime();
					explodeSpecialV( spr -> getPosition() );
				}
				explodeSprite( spr );
			}
		}
	}
}

void GameScene::markRemove( SpriteShape* spr )
{
	if ( spr -> getIsNeedRemove()) {
        return;
    }
    if ( spr -> getIgnoreCheck() ) {
        return;
    }
    
    // mark self
    spr -> setIsNeedRemove(true);
    // check for type and mark for certical neighbour
    if ( spr ->getDisplayMode() == DISPLAY_MODE_VERTICAL) {
        for ( int r = 0; r < ROWS ; ++r ) {
			SpriteShape* tmp = map[r][spr->getCol()];
            if ( !tmp || tmp == spr ) {
                continue;
            }
            
            if (tmp->getDisplayMode() == DISPLAY_MODE_NORMAL) {
                tmp->setIsNeedRemove(true);
            } else {
                markRemove(tmp);
            }
        }
    // check for type and mark for horizontal neighbour
    } else if ( spr -> getDisplayMode() == DISPLAY_MODE_HORIZONTAL) {
        for (int c = 0; c < COLS ; ++c ) {
            SpriteShape *tmp = map[ spr -> getRow()][c];
            if (!tmp || tmp == spr) {
                continue;
            }
            
            if (tmp->getDisplayMode() == DISPLAY_MODE_NORMAL) {
                tmp->setIsNeedRemove(true);
            } else {
                markRemove(tmp);
            }
        }
    }
}

void GameScene::explodeSprite( SpriteShape* spr )
{
	if (UserDefault::getInstance()->getBoolForKey(SOUND_KEY)) {                      
        SimpleAudioEngine::getInstance()->playEffect("music_explode.wav",false);  
    }

	float time = 0.2;
    

    spr->runAction(Sequence::create(
                                      ScaleTo::create(time, 0.0),
									  CallFuncN::create(CC_CALLBACK_1(GameScene::actionEndCallback, this)),
                                      NULL));
    

    auto circleSprite = Sprite::create("circle.png");
	addChild(circleSprite, 10);
	circleSprite->setPosition(spr->getPosition());
	circleSprite->setScale(0);// start size
    circleSprite->runAction(Sequence::create(ScaleTo::create(time, 1.0),
                                             CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, circleSprite)),
                                             NULL));

	
}

void GameScene::explodeSpecialH(Point point)
{
	if (UserDefault::getInstance()->getBoolForKey(SOUND_KEY)) {                      
        SimpleAudioEngine::getInstance()->playEffect("music_explodeOne.wav",false);  
    }

    float scaleX = 4 ;
    float scaleY = 0.7 ;
    float time = 0.3;
    Point startPosition = point;
    float speed = 0.6f;
    
    auto colorSpriteRight = Sprite::create("colorHRight.png");
	addChild(colorSpriteRight, 10);
	Point endPosition1 = Point(point.x - GAME_SCREEN_WIDTH, point.y);
    colorSpriteRight->setPosition(startPosition);
    colorSpriteRight->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
                                             MoveTo::create(speed, endPosition1),
                                             CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteRight)),
                                             NULL));
    
    auto colorSpriteLeft = Sprite::create("colorHLeft.png");
	addChild(colorSpriteLeft, 10);
    Point endPosition2 = Point(point.x + GAME_SCREEN_WIDTH, point.y);
    colorSpriteLeft->setPosition(startPosition);
    colorSpriteLeft->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
                                             MoveTo::create(speed, endPosition2),
                                             CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteLeft)),
                                             NULL));
    
	
}

void GameScene::explodeSpecialV(Point point)
{
	if (UserDefault::getInstance()->getBoolForKey(SOUND_KEY)) {                      
        SimpleAudioEngine::getInstance()->playEffect("music_explodeOne.wav",false);  
    }

    float scaleY = 4 ;
    float scaleX = 0.7 ;
    float time = 0.3;
    Point startPosition = point;
    float speed = 0.6f;

    auto colorSpriteDown = Sprite::create("colorVDown.png");
	addChild(colorSpriteDown, 10);
	Point endPosition1 = Point(point.x , point.y - GAME_SCREEN_HEIGHT);
    colorSpriteDown->setPosition(startPosition);
    colorSpriteDown->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
                                             MoveTo::create(speed, endPosition1),
                                             CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteDown)),
                                             NULL));
    
    auto colorSpriteUp = Sprite::create("colorVUp.png");
	addChild(colorSpriteUp, 10);
    Point endPosition2 = Point(point.x , point.y + GAME_SCREEN_HEIGHT);
    colorSpriteUp->setPosition(startPosition);
    colorSpriteUp->runAction(Sequence::create(ScaleTo::create(time, scaleX, scaleY),
                                             MoveTo::create(speed, endPosition2),
                                             CallFunc::create(CC_CALLBACK_0(Sprite::removeFromParent, colorSpriteUp)),
                                             NULL));

	
}

void GameScene::actionEndCallback(Node *node)
{
	SpriteShape *spr = (SpriteShape *)node;
	map[spr->getRow()][spr->getCol()] = NULL;
	spr -> removeFromParent();
}

void GameScene::swapSprite()
{

    isAction = true;
    isTouchEna = false;

	if (!staSprite || !endSprite) {
        fourBoom = true;
        return;
    }
    
	Point posOfSrc = staSprite->getPosition();
	Point posOfDest = endSprite->getPosition();

    float time = 0.2;
    

	map[ staSprite -> getRow() ][staSprite -> getCol() ] = endSprite;
	map[ endSprite -> getRow() ][endSprite -> getCol() ] = staSprite;

    int tmpRow = staSprite->getRow();
    int tmpCol = staSprite->getCol();
    staSprite->setRow(endSprite->getRow());
    staSprite->setCol(endSprite->getCol());
    endSprite->setRow(tmpRow);
    endSprite->setCol(tmpCol);
    

	std::list<SpriteShape *> colChainListOfFirst;
    getColChain(staSprite, colChainListOfFirst);
    
    std::list<SpriteShape *> rowChainListOfFirst;
    getRowChain(staSprite, rowChainListOfFirst);
    
    std::list<SpriteShape *> colChainListOfSecond;
    getColChain(endSprite, colChainListOfSecond);
    
    std::list<SpriteShape *> rowChainListOfSecond;
    getRowChain(endSprite, rowChainListOfSecond);
    
    if (colChainListOfFirst.size() >= 3
        || rowChainListOfFirst.size() >= 3
        || colChainListOfSecond.size() >= 3
        || rowChainListOfSecond.size() >= 3) {

        staSprite->runAction(MoveTo::create(time, posOfDest));
        endSprite->runAction(MoveTo::create(time, posOfSrc));
        return;
    }

	map[ staSprite -> getRow()][staSprite -> getCol() ] = endSprite;
	map[ endSprite -> getRow()][endSprite -> getCol() ] = staSprite;

    tmpRow = staSprite->getRow();
    tmpCol = staSprite->getCol();
    staSprite->setRow(endSprite->getRow());
    staSprite->setCol(endSprite->getCol());
    endSprite->setRow(tmpRow);
    endSprite->setCol(tmpCol);
    
    staSprite->runAction(Sequence::create(
                                      MoveTo::create(time, posOfDest),
                                      MoveTo::create(time, posOfSrc),
                                      NULL));
    endSprite->runAction(Sequence::create(
                                      MoveTo::create(time, posOfSrc),
                                      MoveTo::create(time, posOfDest),
                                      NULL));
}

SpriteShape *GameScene::spriteOfPoint(Point *point)
{
    SpriteShape *spr = NULL;
    Rect rect = Rect(0, 0, 0, 0);
	Size sz;
	sz.height=SPRITE_WIDTH;
	sz.width=SPRITE_WIDTH;

	for( int r = 0 ; r < ROWS ; ++r )	{
		for( int c = 0 ; c < COLS ; ++c )	{
			spr = map[r][c];
			if( spr )	{
				rect.origin.x = spr->getPositionX() - ( SPRITE_WIDTH / 2);
				rect.origin.y = spr->getPositionY() - ( SPRITE_WIDTH / 2);

				rect.size = sz;
				if (rect.containsPoint(*point)) {
					return spr;
				}
			}
		}
	}
    
    return NULL;
}

bool GameScene::onTouchBegan(Touch *touch, Event *unused)
{
	staSprite = NULL;
	endSprite = NULL;

	if ( isTouchEna ) {
        auto location = touch->getLocation();
		staSprite = spriteOfPoint(&location);
    }
	return isTouchEna;
}

void GameScene::onTouchMoved(Touch *touch, Event *unused)
{
	if (!staSprite || !isTouchEna) {
        return;
    }
    
    int row = staSprite->getRow();
    int col = staSprite->getCol();
    
    auto location = touch->getLocation();
    auto halfSpriteWidth = SPRITE_WIDTH / 2;
    auto halfSpriteHeight = SPRITE_WIDTH / 2;
    
    auto  upRect = Rect(staSprite->getPositionX() - halfSpriteWidth,
                        staSprite->getPositionY() + halfSpriteHeight,
                        SPRITE_WIDTH,
                        SPRITE_WIDTH);
    
    if (upRect.containsPoint(location)) {
        ++row;
        if ( row < ROWS ) {
            endSprite = map[row][col];
        }
        fourBoom = true;
        swapSprite();
        return;
    }
    
    auto  downRect = Rect(staSprite->getPositionX() - halfSpriteWidth,
                        staSprite->getPositionY() - (halfSpriteHeight * 3),
                        SPRITE_WIDTH,
                        SPRITE_WIDTH);
    
    if (downRect.containsPoint(location)) {
        --row;
        if ( row >= 0 ) {
            endSprite = map[row][col];
        }
        fourBoom = true;
        swapSprite();
        return;
    }
    
    auto  leftRect = Rect(staSprite->getPositionX() - (halfSpriteWidth * 3),
                          staSprite->getPositionY() - halfSpriteHeight,
                          SPRITE_WIDTH,
						  SPRITE_WIDTH);
    
    if (leftRect.containsPoint(location)) {
        --col;
        if ( col >= 0 ) {
            endSprite = map[row][col];
        }
        fourBoom = true;
        swapSprite();
        return;
    }
    
    auto  rightRect = Rect(staSprite->getPositionX() + halfSpriteWidth,
                          staSprite->getPositionY() - halfSpriteHeight,
                          SPRITE_WIDTH,
                          SPRITE_WIDTH);
    
    if (rightRect.containsPoint(location)) {
        ++col;
        if ( col < COLS ) {
			endSprite = map[row][col];
        }
        fourBoom = true;
        swapSprite();
        return;
    }
    

}

void GameScene::getColChain(SpriteShape *spr, std::list<SpriteShape *> &chainList)
{

    chainList.push_back(spr);
    

    int neighborCol = spr->getCol() - 1;
    while (neighborCol >= 0) {
        SpriteShape *neighborSprite = map[spr->getRow()][neighborCol];
        if (neighborSprite
            && (neighborSprite->getImgIndex() == spr->getImgIndex())
            && !neighborSprite->getIsNeedRemove()
            && !neighborSprite->getIgnoreCheck()) {
            chainList.push_back(neighborSprite);
            neighborCol--;
        } else {
            break;
        }
    }
    

    neighborCol = spr->getCol() + 1;
    while (neighborCol < COLS) {
        SpriteShape *neighborSprite = map[spr->getRow()][neighborCol];
		if (neighborSprite
            && (neighborSprite->getImgIndex() == spr->getImgIndex())
            && !neighborSprite->getIsNeedRemove()
            && !neighborSprite->getIgnoreCheck()) {
            chainList.push_back(neighborSprite);
            neighborCol++;
        } else {
            break;
        }
    }
}

void GameScene::getRowChain(SpriteShape *spr, std::list<SpriteShape *> &chainList)
{
	chainList.push_back(spr);// add first sushi
    
    int neighborRow = spr->getRow() - 1;
    while (neighborRow >= 0) {
        SpriteShape *neighborSprite = map[neighborRow][spr->getCol()];
        if (neighborSprite
            && (neighborSprite->getImgIndex() == spr->getImgIndex())
            && !neighborSprite->getIsNeedRemove()
            && !neighborSprite->getIgnoreCheck()) {
            chainList.push_back(neighborSprite);
            neighborRow--;
        } else {
            break;
        }
    }
    
    neighborRow = spr->getRow() + 1;
    while (neighborRow < ROWS) {
        SpriteShape *neighborSprite = map[neighborRow][spr->getCol()];
        if (neighborSprite
            && (neighborSprite->getImgIndex() == spr->getImgIndex())
            && !neighborSprite->getIsNeedRemove()
            && !neighborSprite->getIgnoreCheck()) {
            chainList.push_back(neighborSprite);
            neighborRow++;
        } else {
            break;
        }
    }
}

void GameScene::menuBackCallback( Ref *pSender )
{
	auto scene = MainScene::createScene();
	CCTransitionScene* reScene = CCTransitionMoveInR::create(1.0f, scene);
    CCDirector::sharedDirector()->replaceScene(reScene);
}

void GameScene::myClock( float dt )
{

	--m_time;
	if( m_time == 0 )
	{
		SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic(); 
		if (UserDefault::getInstance()->getBoolForKey(SOUND_KEY)) {                      
			SimpleAudioEngine::getInstance()->playEffect("music_gameOver.mp3",false);  
		}

		Label *labelTime = (Label *)this -> getChildByTag(11);
		labelTime->setScale(0);


		auto gmov = Sprite::create("pic_gameover.png");
		gmov->setPosition(Point(GAME_SCREEN_WIDTH/2,GAME_SCREEN_HEIGHT*1.5));
		this->addChild(gmov);

		auto action = MoveTo::create(3.0f, Point(GAME_SCREEN_WIDTH/2, GAME_SCREEN_HEIGHT/2));  

		gmov -> runAction( action );

		scheduleOnce(schedule_selector(GameScene::gameOver),3.5f);
		return;
	}
	if( m_time > 0 )	{
		Label *labelTime = (Label *)this -> getChildByTag(11);
		labelTime -> setString( StringUtils::format("Time: %d ",m_time));
	}
}

bool GameScene::isHaveMove()
{
	for( int r = 0 ; r < ROWS ; ++r )	{
		for( int c = 0 ; c < COLS ; ++c )	{
			if( map[r][c]->getImgIndex() == find(r,c-1,r-1,c+1,r+1,c+1) )
				return true;
			if( map[r][c]->getImgIndex() == find(r,c+1,r-1,c-1,r+1,c-1) )
				return true;
			if( map[r][c]->getImgIndex() == find(r-1,c,r+1,c-1,r+1,c+1) )
				return true;
			if( map[r][c]->getImgIndex() == find(r+1,c,r-1,c-1,r-1,c+1) )
				return true;
		}
	}
	return false;
}

int GameScene::find( int r1 , int c1 , int r2 , int c2 , int r3 , int c3 )
{
	bool isr1,isr2,isr3;
	isr1 = false;
	isr2 = false;
	isr3 = false;

	if( r1 >= 0 && r1 < ROWS && c1 >= 0 && c1 < COLS )	isr1 = true;
	if( r2 >= 0 && r2 < ROWS && c2 >= 0 && c2 < COLS )  isr2 = true;
	if( r3 >= 0 && r3 < ROWS && c3 >= 0 && c3 < COLS )	isr3 = true;

	if( isr1 )	{
		if( isr2 )
			if( map[r1][c1]->getImgIndex() == map[r2][c2]->getImgIndex() )
				return map[r1][c1]->getImgIndex();
		
		if( isr3 )
			if( map[r1][c1]->getImgIndex() == map[r3][c3]->getImgIndex() )
				return map[r1][c1]->getImgIndex();
	}

	if( isr2 && isr3 )
		if( map[r2][c2]->getImgIndex() == map[r3][c3]->getImgIndex() )
			return map[r2][c2]->getImgIndex();

	return -1;
}

void GameScene::gameOver(float dt)
{
	auto scene = Scene::create();
	auto layer = GameOver::create();
	layer -> setScore( getScore() );
	scene -> addChild( layer );

	CCTransitionScene* reScene= CCTransitionFadeUp::create(1.0f, scene);
	CCDirector::sharedDirector()->replaceScene(reScene);
}

void GameScene::addTime( )
{
	Label *labelTime = (Label *)this -> getChildByTag(11);

	auto plus = Sprite::create("sprite_plus3.png");
	plus->setPosition(Vec2( labelTime->getContentSize().width + plus->getContentSize().width*1.3 , GAME_SCREEN_HEIGHT - labelTime->getContentSize().height*0.4 ));
	plus->setScale(1.6);
	this->addChild(plus);

	auto action = ScaleTo::create(0.4f ,0);
	plus -> runAction( action );  

}

void GameScene::onEnter()
{
	Layer::onEnter();
}

void GameScene::onEnterTransitionDidFinish()
{
	Layer::onEnterTransitionDidFinish();  
	if (UserDefault::getInstance()->getBoolForKey(MUSIC_KEY)) {                      
        SimpleAudioEngine::getInstance()->playBackgroundMusic("music_bg.mp3", true);  
    } 
}

void GameScene::onExit()
{
	Layer::onExit();
}

void GameScene::onExitTransitionDidStart()
{
	Layer::onExitTransitionDidStart();
}

void GameScene::cleanup()
{
	Layer::cleanup();
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("music_bg.mp3");
}
