/*
 * PuzzleScene.h
 *
 *  Created on: 2013-6-14
 *      Author: sxkj6
 */

#ifndef PUZZLESCENE_H_
#define PUZZLESCENE_H_

//#include "../BaseScene.h"
//#include "../AllScene.h"
//#include "../Jigsaw/SendGetFromServerData.h"
#include <string>
#include "PuzzleControlLayer.h"
using namespace std;
using namespace cocos2d;

class PuzzleScene: public CCScene {
public:
	PuzzleScene();
	virtual ~PuzzleScene();
	//virtual void setSceneNumber();
	//void getPicFromAddr(const char*,char*);
	void onEnter();
	virtual void readJson();
	virtual void writeJson();
protected:
	PuzzleControlLayer* m_pControlLayer;
	string m_picAddr;
	CCSize m_viewSize;
};

#endif /* PUZZLESCENE_H_ */
