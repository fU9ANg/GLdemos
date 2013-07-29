/*
 * PuzzleScroll.cpp
 *
 *  Created on: 2013-6-18
 *      Author: sxkj6
 */

#include "PuzzleScroll.h"
#include <android/log.h>
#define  LOG_TAG    "tpsx"
#define  printf(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
PuzzleScroll::PuzzleScroll() {
	m_viewSize = CCDirector::sharedDirector()->getWinSize();
	// TODO Auto-generated constructor stub

}

PuzzleScroll::~PuzzleScroll() {
	// TODO Auto-generated destructor stub
}

bool PuzzleScroll::init()
{
	bool bInit = CCScrollView::init();

	setDirection(kCCScrollViewDirectionHorizontal);
	setViewSize(CCSizeMake(m_viewSize.width/5*4,m_viewSize.height/5));
	m_pChildrenLayer = CCLayer::create();
	addChild(m_pChildrenLayer);
	return bInit;
}

bool PuzzleScroll::addChildrens(vector<CCSprite*>* spriteVector)
{
	printf("sprite count:%d",spriteVector->size());
	bool returnValue = true;
	if(!spriteVector->empty())
	{
		m_ChildrenSize = (*spriteVector->begin())->getContentSize();
	}
	else
	{
		CCMessageBox("childrens is empty","err");
		return false;
	}

	float childrenHeight = getViewSize().height/2;
	int childrenCount = 0;
	setContentSize(CCSizeMake(spriteVector->size()*(m_ChildrenSize.width+10)+100,getViewSize().height));


	vector<CCSprite*> ranSpriteVector;

	printf("spriteVector->size() = %d",spriteVector->size());
	int spriteVectorSize = spriteVector->size();
	for(int i = 0; i != spriteVectorSize; i++)//for for random;
	{
		int ttttttt = random()%spriteVector->size();
		printf("ttttttt = %d",ttttttt);
		ranSpriteVector.push_back(spriteVector->at(ttttttt));
		vector<CCSprite*>::iterator* removeObjIter = NULL;
		for(vector<CCSprite*>::iterator i = spriteVector->begin(); i != spriteVector->end(); i++)
		{
			if((*i) == spriteVector->at(ttttttt))
			{
				printf("find sprite");
				spriteVector->erase(i);
				break;
			}
		}
	}

	for(vector<CCSprite*>::iterator i = ranSpriteVector.begin(); i != ranSpriteVector.end(); i++)
	{
		m_pChildrenLayer->addChild(*i);
		(*i)->setPosition(ccp(childrenCount*(m_ChildrenSize.width+10)+m_ChildrenSize.width/2,childrenHeight));
		childrenCount++;
//		printf("addChildren:%d in positon:%f,%f",childrenCount,childrenCount*(m_ChildrenSize.width+10)+m_ChildrenSize.width/2,childrenHeight);
	}
	return returnValue;
}

void PuzzleScroll::registerWithTouchDispatcher()
{

}

CCSprite* PuzzleScroll::getNowSprite(CCPoint& cp)
{
	CCPoint localcp = m_pChildrenLayer->convertToNodeSpace(cp);
	printf("now local point :%f,%f",localcp.x,localcp.y);
	for(int i = 0; i < m_pChildrenLayer->getChildrenCount() ; i++)
	{
		CCSprite* psp = dynamic_cast<CCSprite*>(m_pChildrenLayer->getChildren()->objectAtIndex(i));
//		printf("sprite boundingBox:%f,%f,  %f,%f",psp->boundingBox().origin.x,psp->boundingBox().origin.y,psp->boundingBox().size.height,psp->boundingBox().size.width);
		if(!psp)
		{
			CCMessageBox("convent fail...","err");
			return NULL;
		}
		if(psp->boundingBox().containsPoint(localcp))
		{
			printf("sprite boundingBox:%f,%f,  %f,%f",psp->boundingBox().origin.x,psp->boundingBox().origin.y,psp->boundingBox().size.height,psp->boundingBox().size.width);
			m_spriteOldLocation = psp->getPosition();
			return psp;
		}
	}
	return NULL;
}

void PuzzleScroll::reArrange(CCSprite* pMovedSprite)
{
	CCSprite* pSprite;
	int childrenCount = m_pChildrenLayer->getChildren()->count();
	if(!childrenCount)
	{
		return;
	}
//	printf("childrenCount = %d, localLocation x:%d",childrenCount,m_spriteOldLocation.x);
	for(int i = 0; i < childrenCount ; i++)
	{
		pSprite = dynamic_cast<CCSprite*>(m_pChildrenLayer->getChildren()->objectAtIndex(i));
		if(!pSprite)
		{
			printf("dynamic_fail");
		}
		if(pSprite->getPosition().x > m_spriteOldLocation.x)
		{
			pSprite->runAction(CCMoveBy::create(0.1f,ccp(-pSprite->getContentSize().width-10,0)));
		}
	}
	setContentSize(CCSizeMake(getContentSize().width-pSprite->getContentSize().width-10,getContentSize().height));
}
