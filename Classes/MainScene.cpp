#include "MainScene.h"
#include "base/ultil.h"
#include "GameScene.h"

USING_NS_CC;
using namespace ui;

MainScene::MainScene()
: BaseScene()
{

}

bool MainScene::init()
{
	initUI();
	return true;
}

void MainScene::initUI()
{
	BaseScene::initUI();
	m_Layout = static_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("ui/MainScene.ExportJson"));
	this->addChild(m_Layout);
	this->setContentSize(m_Layout->getContentSize());

	GET_TEXT(m_Layout, "lblHighScore")->setString(StringUtils::toString(UserDefault::getInstance()->getIntegerForKey("high_score")));
	GET_TEXT(m_Layout, "lblStarHad")->setString(StringUtils::toString(UserDefault::getInstance()->getIntegerForKey("star_had")));
	GET_BUTTON(m_Layout, "btnPlayGame")->addClickEventListener([=](Ref* ref)
	{
		SoundControl::getInstance()->playSoundClick();
		Director::getInstance()->replaceScene(TransitionFade::create(0.3f, GameScene::create()));
	});
    
    auto btn_shop = GET_BUTTON(m_Layout, "btnBuyStar");
    auto btn_rate = GET_BUTTON(m_Layout, "btnRate");
    auto btn_top = GET_BUTTON(m_Layout, "btnRank");
    auto btn_setting = GET_BUTTON(m_Layout, "btnSetting");
    
    btn_shop->setVisible(false);
    btn_rate->setVisible(false);
    btn_top->setVisible(false);
    btn_setting->setVisible(false);
    
    auto size = Director::getInstance()->getWinSize();
    
    auto img = GET_IMG(m_Layout, "imgRibbon");
    auto pos_img = img->getPosition();
    img->setPosition(Vec2(size.width - pos_img.x,size.height - pos_img.y - img->getContentSize().height));
}
