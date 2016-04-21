#include "AnimationCommandLayer.h"
#include "TeachLayer.h"
#include "MainScene.h"
#include "../tools/CocosToast.h"
#include "../data/GameData.h"
#include "../data/VOPlayer.h"
#include "../data/VOUser.h"
#include "../tools/WapStringUtils.h"
#include "../player/SpineCache.h"
#define SPINE_ANIMATION_PLAYER_TAG 10022 //固定绑定的spine动画的player的tag

namespace myplayer {

	AnimationCommandLayer::AnimationCommandLayer()
	{

	}

	AnimationCommandLayer::~AnimationCommandLayer()
	{
		MusicAudio::getInstance()->resumeAudio();
		Director::getInstance()->resume();
	}

	AnimationCommandLayer* AnimationCommandLayer::create(AnimationType type)
	{
		AnimationCommandLayer *pRet = new(std::nothrow) AnimationCommandLayer();
		if (pRet && pRet->init(type))
		{ 
			pRet->autorelease();
			return pRet;
		}
		else
		{
			delete pRet;
			pRet = NULL;
			return NULL;
		}
	}

	bool AnimationCommandLayer::init(AnimationType type)
	{
		if (!LessonCommonLayer::init())
		{
			return false;
		}
		m_aniType = type;
		//setLayerTouch();
		createLayer();
		GameData::getInstance()->m_mainScene->m_animationLayer = this;

		return true;
	}

	void AnimationCommandLayer::createLayer()
	{
		MusicAudio::getInstance()->stopMusic();

		visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();

		std::string pathStr = "Animation/";
		if (m_aniType == IntroAnimation)
		{
			pathStr.append("IntroLesson").append(to_string(GameData::getInstance()->voUser->m_lessonId)).append(".csb");
			m_animationVO = m_AnimVO;
		}
		else if (m_aniType == EndAnimation)
		{
			pathStr.append("EndLesson").append(to_string(GameData::getInstance()->voUser->m_lessonId)).append(".csb");
			m_animationVO = m_AnimEndVO;
		}
		MusicAudio::getInstance()->play(m_animationVO->m_AnimBgMusic, 0, true);

		CCLOG(pathStr.c_str());
		if (! FileUtils::getInstance()->isFileExist(pathStr))
		{
			CocosToast::createToastWithIconv("此动画文件不存在");
			return;
		}
		pNode = CSLoader::createNode(pathStr);
		pNode->setPosition(visibleSize.width / 2, visibleSize.height / 2);
		pNode->setAnchorPoint(Point(0.5f, 0.5f));
		this->addChild(pNode);

		for (int i = 0; i<m_animationVO->m_SpineAnimVector.size(); i++)
		{
			Player* pPlayer = Player::create(m_animationVO->m_SpineAnimVector[i]);//创建spine动画player
			m_pSpinePlayerVec.push_back(pPlayer);
			Sprite *sp = (Sprite *)(seekWidgetByName(pNode, m_animationVO->m_CocosAnimVector[i]));//从csb获取每一个需要加载动作的精灵
			sp->addChild(pPlayer, 0, SPINE_ANIMATION_PLAYER_TAG);//创建spine动画Player，并加载到相对应的精灵中（绑定）
			m_PlayerVec.push_back(sp);
		}

		auto action = CSLoader::createTimeline(pathStr);
		//帧事件监听 
		action->setFrameEventCallFunc(CC_CALLBACK_1(AnimationCommandLayer::onFrameEvent, this));
		//关于CC_CALLBACK_1需要点C++11的基础才能知道是咋回事，这里只要照着写就行。想了解可以查下std::Bind 
		action->gotoFrameAndPlay(0, false);
		pNode->runAction(action);

		//创建button里面的参数必填，不能设为NULL，就算没有也得用""，因为该函数是形参为引用变量。
		//播放暂停按钮
		Button* playBtn = createButton("image/button/ui_pauseU.png", "", "", Point(10, 10), "pause");
		playBtn->setAnchorPoint(Point(0, 0));
		this->addChild(playBtn, 10);

		auto theCallBack = [this](Ref* sender, Widget::TouchEventType touchType)->void{

			Button* p = static_cast<Button*>(sender);
			std::string name = p->getName();
			if (touchType == Widget::TouchEventType::ENDED)
			{
				if (p->getName() == "pause")
				{
					CCLOG("Button pause");
					MusicAudio::getInstance()->play("audio/effect/ui_button.wav", 1, false);
					Director::getInstance()->pause();
					SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
					p->loadTextureNormal("image/button/ui_playU.png");
					p->setName("play");
				}
				else if (p->getName() == "play")
				{
					CCLOG("Button play");
					MusicAudio::getInstance()->play("audio/effect/ui_button.wav", 1, false);
					SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
					Director::getInstance()->resume();
					p->loadTextureNormal("image/button/ui_pauseU.png");
					p->setName("pause");
				}
			}
		};
		playBtn->addTouchEventListener(theCallBack);
	}

	void AnimationCommandLayer::onFrameEvent(Frame * frame)
	{

		EventFrame* evnt = dynamic_cast<EventFrame*>(frame);
		std::string aniStr = evnt->getEvent();

		if (!evnt)
		{
			return;
		}

		Sprite* sp = (Sprite*)evnt->getNode();
		if (aniStr != "")//spine动画 compare(0, 3, "spi")
		{
			Player * player = (Player *)sp->getChildByTag(SPINE_ANIMATION_PLAYER_TAG);
			player->playRoleAction(aniStr.c_str(), true);
			CCLOG("%s Spine Animation is %s", player->getName().c_str(), aniStr.c_str());
		}
//暂时去掉下面的代码，如果以后需要执行cocos studio做不出来的动画，那么修改规则，用下面的代码 cuihanbing
// 		else if (str.compare(0, 3, "act") == 0)//需要cocos2d执行的动画
// 		{
// 			if (str == "act_chuxian")
// 			{
// 				evnt->getNode()->setOpacity(256);
// 				CCLOG("Action is %s", str.c_str());
// 			}
// 		}
	}

	void AnimationCommandLayer::gotoNext(Ref* pSender)
	{
		auto parent = static_cast<MainScene*>(this->getParent());
		if (parent)
		{
			if (m_aniType == IntroAnimation)
			{
				//教学
				auto layer = TeachLayer::create();
				parent->addChild(layer);
			}
			else if (m_aniType == EndAnimation)
			{
				//结束歌
				auto layer = FinishLayer::create();
				parent->addChild(layer);
			}
			removeFromParent();
			parent->m_animationLayer = NULL;
		}
	}

	void AnimationCommandLayer::onEnter()
	{
		LessonCommonLayer::onEnter();
		NotificationCenter::getInstance()->addObserver(this, callfuncO_selector(AnimationCommandLayer::gotoNext), "updateLayer", NULL);
	}

}