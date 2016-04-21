/**
**Author:cuihanbing & lihua
**Date:2016/03/28
**Description:绑定spine和cocos studio 动画的类
**/

#ifndef __ANIMATIONCOMMANDLAYER_H__
#define __ANIMATIONCOMMANDLAYER_H__

#include "LessonCommonLayer.h"
#include "../player/Player.h"
#include "../player/AnimationDisplayer.h"
using namespace cocostudio::timeline;//动画相关的类位于cocostuio::timeline当中

namespace myplayer {
	typedef enum
	{
		IntroAnimation,
		EndAnimation
	}AnimationType;

	class AnimationCommandLayer : public LessonCommonLayer
	{

	public:
		AnimationCommandLayer();
		~AnimationCommandLayer();

		static AnimationCommandLayer * create(AnimationType type = IntroAnimation);
		bool init(AnimationType type = IntroAnimation);

		//创建layer
		void createLayer();

		void onEnter();
		//跳到游戏界面
		void gotoNext(Ref* pSender);

	private:
		void onFrameEvent(Frame * frame);

	private:
		Size visibleSize;        //背景宽度
		Node* pNode;

		Sprite* m_pBgSpr = NULL;        //csb中背景
		//Sprite* m_pMonkeySpr = NULL;    //csb中猴子对象
		//Sprite* m_pPangxieSpr = NULL;   //csb中螃蟹对象

// 		spine::SkeletonAnimation* m_pMonkeyPlayer = NULL;  //猴子动画对象
// 		spine::SkeletonAnimation* m_pPangxiePlayer = NULL; //螃蟹动画对象
		//std::vector<spine::SkeletonAnimation*>  m_pSpinePlayerVec;

// 		int m_pCurAnimNO;//当前执行到json中动画顺序的对象
// 		int m_pCurObjectNo;
		AnimationType m_aniType;
		std::shared_ptr<TemplateAnimationVO> m_animationVO;

		std::vector<Player*>  m_pSpinePlayerVec;
		std::vector<Sprite*>  m_PlayerVec;    //从json中获取的当前课时动画中的对象存放
		std::string m_PlayerName;
	};
}
#endif
