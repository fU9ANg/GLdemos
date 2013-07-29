/*
 * PuzzleControlLayer.cpp
 *
 *  Created on: 2013-6-18
 *      Author: sxkj6
 */

#include "PuzzleControlLayer.h"
#include <android/log.h>
#define  LOG_TAG    "tpsx"
#define  printf(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
PuzzleControlLayer::PuzzleControlLayer() {
	CCLOG("PuzzleControlLayer::PuzzleControlLayer();========================================\n");
#if 0
	m_pSocket = new TpsxSocketHelper();
	if(!m_pSocket)
	{
		CCMessageBox("create Socket Helper fail","内存交出来！");
	}
#endif
	m_picCount = 10;
	m_nowPuzzled =0;
	m_bRepFix = false;
	m_pPicLayer = NULL;
	m_pScroll = NULL;
	m_controlStyle = PUZZLECONTROLMOVE;
	m_viewSize = CCDirector::sharedDirector()->getWinSize();
	m_nowControl = NULL;
	// TODO Auto-generated constructor stub

}

PuzzleControlLayer::~PuzzleControlLayer() {
#if 0
	if(m_pSocket)
	{
		delete m_pSocket;
	}
#endif
	CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
	// TODO Auto-generated destructor stub
}

bool PuzzleControlLayer::init()
{
	bool bInit = CCLayer::init();
	setTouchEnabled(true);
	m_pPicLayer = PuzzlePicLayer::create();
	if(NULL ==m_pPicLayer )
	{
		CCMessageBox("init PuzzlePicLayer error","err");
		return false;
	}
	m_pPicLayer->setPosition(ccp(m_viewSize.width/2-40-25,m_viewSize.height/4+40+35));
	addChild(m_pPicLayer);

	m_pScroll = PuzzleScroll::create();
	if(m_pScroll == NULL)
	{
		CCMessageBox("init PuzzleScroll error","err");
		return false;
	}
	m_pScroll->setPosition(ccp(m_viewSize.width/10,m_viewSize.height/10-20));
	addChild(m_pScroll);


	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	gameTime =0.0f;
	gameTimeLabel = CCLabelTTF::create("00:00","Arial",56);
	gameTimeLabel->setColor(ccGREEN);
	gameTimeLabel->setPosition(ccp(winSize.width*0.20f,winSize.height*0.83f));
	gameTimeLabel->setAnchorPoint(CCPointZero);
	addChild(gameTimeLabel);
	schedule(schedule_selector(PuzzleControlLayer::updateTime));

	return bInit;
}

void  PuzzleControlLayer::setObjPic(const char * picAddr,int i,int j)
{
	vector<CCSprite*>* pv = NULL;
	if(m_pPicLayer && m_pScroll)
	{
		m_picCount = i*j;
		m_pScroll->addChildrens(m_pPicLayer->getSpritesWithPic(picAddr,i,j));
//		m_pPicLayer->testFunc();
	}
	else
	{
		printf("error use");
	}
}

void PuzzleControlLayer::updateTime(float dt)
{
	gameTime += dt;
	int minute  = (int)gameTime/60;
	int second = (int)gameTime%60;
	char timeString[60] = {'\0'};
	if (minute<10&&second<10)
	{
	    sprintf(timeString,"0%d:0%d",minute,second);
	}
	else if(minute<10&&second>=10)
	{
		sprintf(timeString,"0%d:%d",minute,second);
	}
	else if(minute>=10&&second<10)
	{
	    sprintf(timeString,"%d:0%d",minute,second);

    }
	else if(minute>=10&&second>=10)
	{
	    sprintf(timeString,"%d:%d",minute,second);

	}
	gameTimeLabel->setString(timeString);
}

void PuzzleControlLayer::onEnter()
{
	CCLayer::onEnter();


}

void PuzzleControlLayer::registerWithTouchDispatcher()
{
	printf("reg touch");
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this,-100,true);
}

bool PuzzleControlLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	CCLOG("bool PuzzleControlLayer::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);==========================================\n");
	if(m_nowControl)
	{
		return false;
	}
	m_nowPoint = pTouch->getLocation();
	m_movDes = 0.0f;
	m_pScroll->ccTouchBegan(pTouch,pEvent);
	if(!m_bRepFix)
	{
		m_bRepFix = true;
		scheduleOnce(schedule_selector(PuzzleControlLayer::fixControlStyle),0.15f);
	}
	else
	{
		m_bRepFix = false;
		unschedule(schedule_selector(PuzzleControlLayer::fixControlStyle));
	}
	return true;
}

void PuzzleControlLayer::ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent)
{
	if(m_controlStyle == PUZZLECONTROLMOVE)
	{
		m_nowPoint = pTouch->getLocation();
		m_movDes += fabsf(pTouch->getDelta().x)+fabsf(pTouch->getDelta().y/2);
		m_pScroll->ccTouchMoved(pTouch,pEvent);
	}
	if(m_controlStyle == PUZZLECONTROLDRAG)
	{
		m_nowPoint = pTouch->getLocation();
		if(m_nowControl)
		{
			m_nowControl->setPosition(pTouch->getLocation());
		}
	}
}

void PuzzleControlLayer::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	if(m_controlStyle == PUZZLECONTROLDRAG)
	{
		if(m_nowControl)
		{

			if(m_pPicLayer->tryToRecieveSprite(m_nowControl,pTouch->getLocation()))
			{
#if 0
				sPuzzleUpdatePic sPUP;
				sPUP.pic_idx = m_nowControl->getTag();
				m_pSocket->TpsxFastSend(CT_Puzzle_UpdatePic, &sPUP, sizeof(sPuzzleUpdatePic));
#endif
				m_nowPuzzled++;
				m_pScroll->reArrange(m_nowControl);
				if(m_picCount == m_nowPuzzled)
				{
					unschedule(schedule_selector(PuzzleControlLayer::updateTime));
					SimpleAudioEngine::sharedEngine()->playEffect("sounds/p_finish.WAV");
				}
				else {
					SimpleAudioEngine::sharedEngine()->playEffect("sounds/p_right.WAV");
				}
				m_nowControl = NULL;
			}
			else {
				SimpleAudioEngine::sharedEngine()->playEffect("sounds/p_wrong.WAV");
				CCActionInterval* pMoveTo = CCSpawn::createWithTwoActions(CCMoveTo::create(0.1f,m_pScroll->getChildrenLayer()->convertToWorldSpace(m_pScroll->m_spriteOldLocation)),CCScaleTo::create(0.1f,1.0f));
				CCActionInterval* paction = CCSequence::create(pMoveTo,CCCallFunc::create(this,callfunc_selector(PuzzleControlLayer::throwBackToScroll)),NULL);
				m_nowControl->runAction(paction);
			}
		}
	}
	m_controlStyle = PUZZLECONTROLMOVE;
	m_nowPoint = ccp(0,0);
	m_pScroll->ccTouchEnded(pTouch,pEvent);
}

void PuzzleControlLayer::throwBackToScroll()
{
	m_nowControl->removeFromParent();
	m_nowControl->setPosition(m_pScroll->m_spriteOldLocation);
	m_pScroll->getChildrenLayer()->addChild(m_nowControl);
	m_nowControl = NULL;

}

void PuzzleControlLayer::fixControlStyle()
{
	if(m_movDes > 20)
	{
		m_controlStyle = PUZZLECONTROLMOVE;
	}
	else
	{
		printf("try to get sprite");
		m_nowControl = m_pScroll->getNowSprite(m_nowPoint);
		if(m_nowControl)
		{
			SimpleAudioEngine::sharedEngine()->playEffect("sounds/p_pick.WAV");
			m_nowControl->removeFromParent();
			m_nowControl->runAction(CCScaleTo::create(0.1f,1.2f));
			addChild(m_nowControl);
			m_nowControl->setPosition(m_nowPoint);
			m_controlStyle = PUZZLECONTROLDRAG;
		}
	}
	m_bRepFix = false;
}
