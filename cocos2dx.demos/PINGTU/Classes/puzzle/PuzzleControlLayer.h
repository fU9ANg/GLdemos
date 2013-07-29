/*
 * PuzzleControlLayer.h
 *
 *  Created on: 2013-6-18
 *      Author: sxkj6
 */

#ifndef PUZZLECONTROLLAYER_H_
#define PUZZLECONTROLLAYER_H_

#include "cocos2d.h"
#include "PuzzlePicLayer.h"
#include "PuzzleScroll.h"
#include <vector>
#include "SimpleAudioEngine.h"
//#include "../tpsxTools/TpsxSocketHelper.h"

using std::vector;
using namespace cocos2d;
using namespace CocosDenshion;
enum PUZZLECONTROLSTYLE{
	PUZZLECONTROLMOVE,
	PUZZLECONTROLDRAG
};

class PuzzleControlLayer: public cocos2d::CCLayer {
public:
	PuzzleControlLayer();
	virtual ~PuzzleControlLayer();
	virtual void registerWithTouchDispatcher(void);

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchMoved(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);

	void onEnter();

	void fixControlStyle();

	void setObjPic(const char *,int i,int j);

	void throwBackToScroll();

	void updateTime(float dt);

	bool init();
	static PuzzleControlLayer* create()
	{
		PuzzleControlLayer* pPCL = new PuzzleControlLayer();
		if(pPCL && pPCL->init())
		{
			pPCL->autorelease();
		}
		else
		{
			CC_SAFE_DELETE(pPCL);
		}
		return pPCL;
	}
protected:
	//TpsxSocketHelper* m_pSocket;
	CCPoint m_nowPoint;
	CCSprite* m_nowControl;
	PuzzlePicLayer* m_pPicLayer;
	PuzzleScroll* m_pScroll;
	PUZZLECONTROLSTYLE m_controlStyle;
	CCSize m_viewSize;
	float m_movDes;
	bool m_bRepFix;

	float gameTime;
	cocos2d::CCLabelTTF* gameTimeLabel;

	int m_picCount;
	int m_nowPuzzled;
};

#endif /* PUZZLECONTROLLAYER_H_ */
