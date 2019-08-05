#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"
#include "base/BaseScene.h"

class MainScene : public BaseScene
{
public:
	MainScene();
	CREATE_FUNC(MainScene);
	bool init() override;
	void initUI() override;
};
#endif //__MAIN_SCENE_H__
