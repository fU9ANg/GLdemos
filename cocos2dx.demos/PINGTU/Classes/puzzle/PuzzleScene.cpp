/*
 * PuzzleScene.cpp
 *
 *  Created on: 2013-6-14
 *      Author: sxkj6
 */

#include "PuzzleScene.h"
#include "../jsoncpp/src/json/json.h"
#include <android/log.h>
#define  LOG_TAG    "tpsx"
#define  printf(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
PuzzleScene::PuzzleScene() {
	m_viewSize = CCDirector::sharedDirector()->getWinSize();

#if 0
	string picAddr;
	m_pControlLayer = NULL;
	if(MsgReceiver::serverIP == "222.186.50.76"){
		picAddr = "http://222.186.50.76:9090/static/pingtu.png";
	}
	else {
		picAddr = "http://"+ MsgReceiver::serverIP + ":8080/static/pingtu.png";
	}
//	m_picAddr = "/mnt/sdcard/pintu/target1.png";
#endif
	m_picAddr = "puzzle/pingtu.png";
	char picUrl[50];
	//strcpy(picUrl,picAddr.c_str());
	CCSprite* pBackPic = CCSprite::create("puzzle/bg.png");
	pBackPic->setPosition(ccp(m_viewSize.width/2,m_viewSize.height/2));
	addChild(pBackPic,0);

	CCSprite* pKatoon = CCSprite::create("puzzle/katongrenwu.png");
	pKatoon->setPosition(ccp(m_viewSize.width/6*5+50,m_viewSize.height/4));
	addChild(pKatoon,10);
//	getPicFromAddr(m_picAddr.c_str(),picUrl);


	// TODO Auto-generated constructor stub

	// f**king test for jsoncpp
	CCLog ("read json...............................................................");
	readJson ();
	CCLog ("write json ...................................................................");
	writeJson ();
}

void PuzzleScene::readJson() {
	using namespace std;
	std::string strValue = "{\"name\":\"json\",\"array\":[{\"cpp\":\"jsoncpp\"},{\"java\":\"jsoninjava\"},{\"php\":\"support\"}]}";

	Json::Reader reader;
	Json::Value value;

	if (reader.parse(strValue, value))
	{
		std::string out = value["name"].asString();
		//std::cout << out << std::endl;
		CCLog (out.c_str());
		const Json::Value arrayObj = value["array"];
		for (unsigned int i = 0; i < arrayObj.size(); i++)
		{
			if (!arrayObj[i].isMember("cpp"))
				continue;
			out = arrayObj[i]["cpp"].asString();
			//std::cout << out;
			CCLog (out.c_str());
			if (i != (arrayObj.size() - 1))
				std::cout << std::endl;
		}
	}
}

void PuzzleScene::writeJson() {
	using namespace std;

	Json::Value root;
	Json::Value arrayObj;
	Json::Value item;

	item["cpp"] = "jsoncpp";
	item["java"] = "jsoninjava";
	item["php"] = "support";
	arrayObj.append(item);

	root["name"] = "json";
	root["array"] = arrayObj;

	root.toStyledString();
	std::string out = root.toStyledString();
	//std::cout << out << std::endl;
	CCLog (out.c_str());
}

PuzzleScene::~PuzzleScene() {
	// TODO Auto-generated destructor stub
}
#if 0
void PuzzleScene::setSceneNumber()
{
	m_sceneNumber = PUZZLESCENE;
}

void PuzzleScene::getPicFromAddr(const char* picDir,char* url)
{
	if(access("/mnt/sdcard/pintu",F_OK) != 0)
	{
		mkdir("/mnt/sdcard/pintu",777);
	}
	printf("get pic from %s, save to %s",url,picDir);
	if(CURLE_OK!=getServerData(picDir,url))
	{
		CCMessageBox("取图失败","error");
		return;
	}
	printf("get pic success!");
}
#endif
void PuzzleScene::onEnter()
{

	CCScene::onEnter();
	printf("in onEnter");
	CCSprite* pPicIndex = CCSprite::create(m_picAddr.c_str());
	if(!pPicIndex)
	{
		CCMessageBox("create recieved pic fail","error");
		return;
	}
	printf("create pic");
	pPicIndex->setPosition(ccp(m_viewSize.width/4-54,m_viewSize.height/2+31));
	pPicIndex->setScale(0.65f);
	addChild(pPicIndex,1);

	m_pControlLayer = PuzzleControlLayer::create();
	printf("create controlLayer");
	m_pControlLayer->setPosition(ccp(0,0));
	addChild(m_pControlLayer,5);
	m_pControlLayer->setObjPic(m_picAddr.c_str(),5,4);
}


