/*
 * PuzzleScroll.h
 *
 *  Created on: 2013-6-18
 *      Author: sxkj6
 */

#ifndef PUZZLESCROLL_H_
#define PUZZLESCROLL_H_

#include "cocos2d.h"
#include "cocos-ext.h"
#include <vector>

using std::vector;
using namespace cocos2d;
using namespace cocos2d::extension;

class PuzzleScroll: public cocos2d::extension::CCScrollView {
public:
	PuzzleScroll();
	bool init();
	virtual void registerWithTouchDispatcher();
	virtual ~PuzzleScroll();

	bool addChildrens(vector<CCSprite*>*);

	CCSprite* getNowSprite(CCPoint& cp);

	void reArrange(CCSprite*);

	static PuzzleScroll* create(){
		PuzzleScroll* pPS = new PuzzleScroll();
		if(pPS && pPS->init())
		{
			pPS->autorelease();
		}
		else
		{
			CC_SAFE_DELETE(pPS);
		}

		return pPS;
	}

	CCPoint m_spriteOldLocation;

	CCLayer* getChildrenLayer(){
		return m_pChildrenLayer;
	}
protected:
	CCLayer* m_pChildrenLayer;
	CCSize m_viewSize;
	CCSize m_ChildrenSize;
};

#endif /* PUZZLESCROLL_H_ */
