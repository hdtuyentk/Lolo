#ifndef __INTRO_SCENE_H__
#define __INTRO_SCENE_H__

#include "cocos2d.h"
#include "base/BaseScene.h"
#include "ui/CocosGUI.h"

using namespace cocos2d::ui;

class IntroScene : public BaseScene
{
public:
	IntroScene();
	CREATE_FUNC(IntroScene);
	bool init() override;
	void initUI() override;
};
#endif //__INTRO_SCENE_H__
