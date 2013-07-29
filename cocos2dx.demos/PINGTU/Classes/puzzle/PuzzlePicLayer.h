/*
 * PuzzlePicLayer.h
 *
 *  Created on: 2013-6-18
 *      Author: sxkj6
 */

#ifndef PUZZLEPICLAYER_H_
#define PUZZLEPICLAYER_H_

#include "cocos2d.h"
#include <vector>
#include "SimpleAudioEngine.h"
#include <map>
using std::map;
using namespace cocos2d;
using std::vector;
using namespace CocosDenshion;
class PuzzlePicLayer: public cocos2d::CCLayer {
public:
	PuzzlePicLayer();
	virtual ~PuzzlePicLayer();
	vector<CCSprite*>* getSpritesWithPic(const char*,int row,int col);
	bool tryToRecieveSprite(CCSprite*,CCPoint handPoint);

	void testFunc();

	static PuzzlePicLayer* create()
	{
		PuzzlePicLayer* pPPL = new PuzzlePicLayer();
		if(pPPL && pPPL->init())
		{
			pPPL->autorelease();
		}
		else
		{
			CC_SAFE_DELETE(pPPL);
		}
		return pPPL;
	}

	void puzzleRight();

protected:
//	CCNode* m_pSprites;
	vector<CCSprite*> m_pSprites;
	float m_picWidth;
	float m_picHight;
	int m_picCount;
//	int m_nowPuzzled;
	int m_picHorizontal;
	int m_picVertical;

	CCParticleSystem* m_emitter;
};

#endif /* PUZZLEPICLAYER_H_ */
