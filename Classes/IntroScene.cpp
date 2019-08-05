#include "MainScene.h"
#include "IntroScene.h"

USING_NS_CC;
using namespace ui;

IntroScene::IntroScene()
: BaseScene()
{
	setonEnterTransitionDidFinishCallback([=]()
	{
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(1.0f), CallFunc::create([=]()
		{
			Director::getInstance()->replaceScene(TransitionFade::create(0.75f, MainScene::create()));
		})));
	});
}

bool IntroScene::init()
{
	initUI();
	return true;
}

void IntroScene::initUI()
{
	BaseScene::initUI();
	m_Layout = static_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("ui/IntroScene.ExportJson"));
	this->addChild(m_Layout);
	this->setContentSize(m_Layout->getContentSize());
	
}