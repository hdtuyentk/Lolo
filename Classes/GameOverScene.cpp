#include "GameOverScene.h"
#include "base/ultil.h"
#include "GameScene.h"
#include "MainScene.h"

USING_NS_CC;
using namespace ui;

GameOverScene::GameOverScene()
: BaseScene()
{

}

bool GameOverScene::init()
{
	initUI();
	return true;
}

void GameOverScene::initUI()
{
	BaseScene::initUI();
	m_Layout = static_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("ui/GameOverScene.ExportJson"));
	this->addChild(m_Layout);
	this->setContentSize(m_Layout->getContentSize());

	GET_BUTTON(m_Layout, "btnReplay")->addClickEventListener([=](Ref* ref)
	{
		SoundControl::getInstance()->playSoundClick();
		Director::getInstance()->replaceScene(TransitionFade::create(0.2f, GameScene::create()));
	});

	GET_BUTTON(m_Layout, "btnHome")->addClickEventListener([=](Ref* ref)
	{
		SoundControl::getInstance()->playSoundClick();
		Director::getInstance()->replaceScene(TransitionFade::create(0.2f, MainScene::create()));
	});

	setonEnterTransitionDidFinishCallback([=]()
	{
		drawBoard();
	});
    
    auto btn_shop = GET_BUTTON(m_Layout, "btnShop");
    auto btn_rank = GET_BUTTON(m_Layout, "btnRank");
    auto btn_share = GET_BUTTON(m_Layout, "btnShare");
    btn_shop->setVisible(false);
    btn_rank->setVisible(false);
    btn_share->setVisible(false);
}

void GameOverScene::createNewSquare(int id, int value, int i, int j)
{
	int sizeSquare = 70;
	const float x = 35 + (sizeSquare * j);
	float y = 385 - (sizeSquare * i);

	Layout* pnlrect = static_cast<Layout*>(_square->clone());
	pnlrect->setScale(0.67f);
	//pnlrect->setAnchorPoint(Vec2(0, 0));
	Square* square = Square::create(pnlrect);
	square->setPosition(Vec2(x, y));
	square->setColorSquare(id);
	square->setSquareValue(value);
	GET_LAY(m_Layout, "pnlBoard")->addChild(square);
}

void GameOverScene::cloneSquare(Layout* lay)
{
	_square = lay;
}

void GameOverScene::drawBoard()
{
	auto data = UserDefault::getInstance()->getStringForKey("save_game_temp");
	try
	{
		if (data.empty())
			return;
		rapidjson::Document document;
		document.Parse<0>(data.c_str());
		const rapidjson::Value& value = document;
		for (int i = 0; i < DICTOOL->getArrayCount_json(value, "vSquare") / 2; i++)
		{
			int squareId = DICTOOL->getIntValueFromArray_json(value, "vSquare", (i * 2));
			int squareValue = DICTOOL->getIntValueFromArray_json(value, "vSquare", (i * 2 + 1));
			int pi = i / MATRIX_ROW;
			int pj = i % MATRIX_COL;
			createNewSquare(squareId, squareValue, pi, pj);
		}
	}
	catch (std::exception& e)
	{

	}
}

void GameOverScene::setViewScore(int score)
{
	GET_TEXT(m_Layout, "lblScore")->setString(StringUtils::toString(score));
}

void GameOverScene::updateStar(int star)
{
	GET_TEXT(m_Layout, "lblStarEarn")->setString(StringUtils::toString(star));
	int currentStar = UserDefault::getInstance()->getIntegerForKey("star_had");
	currentStar += star;
	UserDefault::getInstance()->setIntegerForKey("star_had", currentStar);
	GET_TEXT(m_Layout, "lblStarHad")->setString(StringUtils::toString(currentStar));
}
