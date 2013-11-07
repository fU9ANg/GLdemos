#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#include <iostream>
#include <string>

#include "message/proto/login.pb.h"

using namespace cocos2d;
using namespace CocosDenshion;
using namespace std;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback) );
    pCloseItem->setPosition( ccp(CCDirector::sharedDirector()->getWinSize().width - 20, 20) );

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition( CCPointZero );
    this->addChild(pMenu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    CCLabelTTF* pLabel = CCLabelTTF::create("Hello World", "Thonburi", 34);

    // ask director the window size
    CCSize size = CCDirector::sharedDirector()->getWinSize();

    // position the label on the center of the screen
    pLabel->setPosition( ccp(size.width / 2, size.height - 20) );

    // add the label as a child to this layer
    this->addChild(pLabel, 1);

    // add "HelloWorld" splash screen"
    CCSprite* pSprite = CCSprite::create("HelloWorld.png");

    // position the sprite on the center of the screen
    pSprite->setPosition( ccp(size.width/2, size.height/2) );

    // add the sprite as a child to this layer
    this->addChild(pSprite, 0);
    
    CCLog ("---------------------");
    run_protobuf ();
    CCLog ("=====================");
    return true;
}

void HelloWorld::run_protobuf (void)
{
	#define MAXLINE	2048

    int    n, sockfd;
    char   recvbuf[MAXLINE];
    struct sockaddr_in servaddr;

    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("socket");
        exit (1);
    }

    (void) memset (&servaddr, 0x00, sizeof (servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons (1300);
    if (::inet_pton (AF_INET, "172.16.10.110", &servaddr.sin_addr.s_addr) < 0)
    {
        CCLog ("[Error]: inet_pton");
        exit (1);
    }

    if ((connect (sockfd, (struct sockaddr *) &servaddr, sizeof (servaddr))) < 0)
    {
        CCLog ("[Error]: connect");
        exit (1);
    }

    while ((n = read (sockfd, recvbuf, sizeof (recvbuf))) > 0)
    {
        recvbuf[n] = 0x00;
        string sLogin;
        Login  login;

        sLogin = recvbuf;
        login.ParseFromString (sLogin);

        CCLog ("id=%d", login.clientid());
        CCLog ("type=%d", login.clienttype());

        CCLog ("quality=%f", login.quality());	// 服务器压缩包时，对float类型的操作必须是 （至少两位的有效小位数）
        CCLog ("username=%s", login.username ().c_str());
        CCLog ("password=%s", login.password ().c_str());
    }

    if (n < 0)
    {
        CCLog ("[Error]: read");
        exit (1);
    }
}

void HelloWorld::menuCloseCallback(CCObject* pSender)
{
    CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
