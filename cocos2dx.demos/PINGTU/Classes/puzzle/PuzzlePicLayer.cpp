/*
 * PuzzlePicLayer.cpp
 *
 *  Created on: 2013-6-18
 *      Author: sxkj6
 */

#include "PuzzlePicLayer.h"
#include <android/log.h>
#define  LOG_TAG    "tpsx"
#define  printf(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
PuzzlePicLayer::PuzzlePicLayer() {
	m_emitter = CCParticleFlower::create();
	addChild(m_emitter,100);
	m_emitter->setTexture(CCTextureCache::sharedTextureCache()->addImage("puzzle/stars.png"));
	m_emitter->setLifeVar(0);
	m_emitter->setLife(4);
	m_emitter->setSpeed(100);
	m_emitter->setSpeedVar(0);
	m_emitter->setEmissionRate(10000);
	m_emitter->stopSystem();
	// TODO Auto-generated constructor stub

}

PuzzlePicLayer::~PuzzlePicLayer() {
	// TODO Auto-generated destructor stub
}

vector<CCSprite*>* PuzzlePicLayer::getSpritesWithPic(const char* picAddr,int horizontal ,int vertical)
{
//	if(m_pSprites.empty())
//	{
//
//	}
	CCSprite* pSprite = CCSprite::create(picAddr);
	if(NULL == pSprite)
	{
		CCMessageBox("create img fail","err");
		return NULL;
	}
	m_picWidth = pSprite->getContentSize().width;
	m_picHight = pSprite->getContentSize().height;
	m_picCount = horizontal * vertical;
	m_picHorizontal = horizontal;
	m_picVertical = vertical;
	CCTexture2D* pTexture = pSprite->getTexture();

	CCRect rect;
	float cellWidth = m_picWidth/horizontal;
	float cellHight = m_picHight/vertical;
	rect.size = CCSizeMake(cellWidth,cellHight);
	for(int i = 0 ; i < vertical ; i++)
	{
		for(int j = 0 ; j < horizontal ; j++)
		{
			rect.origin = ccp(j*cellWidth,i*cellHight);
			CCSprite* psp = CCSprite::create(picAddr,rect);
			psp->setTag(i*horizontal+j);
			psp->retain();
			m_pSprites.push_back(psp);
			printf("create sprite with part:bp:%f,%f, rect:%f,%f",rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
		}
	}
	testFunc();
	return &m_pSprites;
}

bool PuzzlePicLayer::tryToRecieveSprite(CCSprite* pSprite,CCPoint hp)
{
	CCPoint localLocation = convertToNodeSpace(hp);
	int spriteTag = pSprite->getTag();
	printf("picTag = %d",spriteTag);
	if(spriteTag < 0 || spriteTag >= m_picCount)
	{
		CCMessageBox("error tag","err");
		return false;
	}
	CCRect rect;
	rect.origin = ccp(spriteTag%m_picHorizontal*m_picWidth/m_picHorizontal,(m_picVertical-spriteTag/m_picHorizontal-1)*m_picHight/m_picVertical);
	rect.size = CCSizeMake(m_picWidth/m_picHorizontal,m_picHight/m_picVertical);
	printf("obj rect : x:%f,y:%f,   w:%f,h:%f",rect.origin.x,rect.origin.y,rect.size.width,rect.size.height);
	if(rect.containsPoint(localLocation))
	{
		pSprite->stopAllActions();
		pSprite->removeFromParent();
		addChild(pSprite,5);
		pSprite->setPosition(localLocation);
		m_emitter->setPosition(ccp(spriteTag%m_picHorizontal*m_picWidth/m_picHorizontal+m_picWidth/m_picHorizontal/2,
				(m_picVertical-spriteTag/m_picHorizontal-1)*m_picHight/m_picVertical+m_picHight/m_picVertical/2));
		m_emitter->resetSystem();
		CCActionInterval* pMoveAndScale = CCSpawn::createWithTwoActions(CCMoveTo::create(0.1f,ccp(spriteTag%m_picHorizontal*m_picWidth/m_picHorizontal+m_picWidth/m_picHorizontal/2,
				(m_picVertical-spriteTag/m_picHorizontal-1)*m_picHight/m_picVertical+m_picHight/m_picVertical/2)),CCScaleTo::create(0.1f,1.0f));
		CCActionInterval* pAction = CCSequence::createWithTwoActions(pMoveAndScale,CCCallFunc::create(this,callfunc_selector(PuzzlePicLayer::puzzleRight)));
		pSprite->runAction(pAction);

		return true;
	}
	else
	{
		return false;
	}
	return false;
}

void PuzzlePicLayer::puzzleRight()
{
	m_emitter->stopSystem();
}

void PuzzlePicLayer::testFunc()
{
	printf("in test func");
	for(vector<CCSprite*>::iterator i = m_pSprites.begin(); i != m_pSprites.end(); i++)
	{
		int tag = (*i)->getTag();
		CCSprite* pSprite = CCSprite::create("puzzle/parts00.png");
//		pSprite->setScale(0.5f);
		//pSprite->setScaleX(5/8);
		//pSprite->setScaleY(4/6);
		pSprite->setPosition(ccp(tag%m_picHorizontal*m_picWidth/m_picHorizontal+m_picWidth/m_picHorizontal/2,
				m_picHight-tag/m_picHorizontal*m_picHight/m_picVertical-m_picHight/m_picVertical/2));
		addChild(pSprite,2);
	}
}
