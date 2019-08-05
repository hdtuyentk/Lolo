#ifndef BaseScene_h__
#define BaseScene_h__

#include "cocos2d.h"
#include "editor-support/cocostudio/CocoStudio.h"

USING_NS_CC;
using namespace ui;
using namespace cocostudio;

class BaseScene :public cocos2d::Scene
{
public:
	BaseScene();
	bool init() override;
	virtual void initUI();
	
protected:
	Layout * m_Layout = nullptr;
	Layer* root_layer_ = nullptr;
	EventListenerTouchOneByOne* listener_;

};

#endif // BaseScene_h__
