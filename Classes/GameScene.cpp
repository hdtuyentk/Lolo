#include "GameScene.h"
#include "base/ultil.h"
#include <fstream>
#include "json/stringbuffer.h"
#include "json/writer.h"
#include "GameOverScene.h"
#include "MainScene.h"

#define TIME_MOVE_SQUARE 0.25
#define TIME_DELAY_SCALE_SQUARE 1.0

void buildJson(std::string* msg, SaveDataObj& data)
{
	rapidjson::Document document;
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value body(rapidjson::kObjectType);

	if (data._dataInt.size() > 0)
	{
		for (const auto& element : data._dataInt)
		{
			rapidjson::Value a_key(element.first.c_str(), allocator);
			body.AddMember(a_key, element.second, allocator);
		}
	}
	if (data._dataArrInt.size() > 0)
	{
		for (const auto& element : data._dataArrInt)
		{
			rapidjson::Value myArray(rapidjson::kArrayType);
			auto& aArr = element.second;
			for (const auto& arrElement : aArr)
			{
				myArray.PushBack(arrElement, allocator);
			}
			rapidjson::Value a_key(element.first.c_str(), allocator);
			body.AddMember(a_key, myArray, allocator);
		}
	}

	rapidjson::StringBuffer sb2;
	rapidjson::Writer<rapidjson::StringBuffer> writer2(sb2);
	body.Accept(writer2);

	msg->assign(sb2.GetString());
}

GameScene::GameScene()
: BaseScene()
{

}

bool GameScene::init()
{
	initUI();
	return true;
}

void GameScene::initUI()
{
	BaseScene::initUI();
	m_Layout = static_cast<Layout*>(GUIReader::getInstance()->widgetFromJsonFile("ui/GameScene.ExportJson"));
	this->addChild(m_Layout);
	this->setContentSize(m_Layout->getContentSize());

	_pnlBoard = GET_LAY(GET_IMG(m_Layout, "imgBg"), "pnlBoard");
	loadGame();
	m_Layout->setTouchEnabled(false);
	_pnlBoard->setTouchEnabled(false);
	listener_->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);

	GET_BUTTON(m_Layout, "btnSp")->addClickEventListener(CC_CALLBACK_1(GameScene::onTouchBtnSupport, this));
	GET_BUTTON(m_Layout, "btnSetting")->addClickEventListener(CC_CALLBACK_1(GameScene::handleBtnSetting, this));
}

void GameScene::initNewGame()
{
	_flagClick = true;
	setScore(0, false);
	setHighScore(UserDefault::getInstance()->getIntegerForKey("high_score"), false);
	_stateBtnSp = StateBtnSupport::UN_TOUCH;
	_flagRandMaxNumber = false;
	setStarNeedUse(15);
	setStarHad(UserDefault::getInstance()->getIntegerForKey("star_had"));
	createNewMatrix();
	connectSquareSameId();
	saveGame();
}

void GameScene::delDataSaved()
{
	UserDefault::getInstance()->setStringForKey("save_game_temp", UserDefault::getInstance()->getStringForKey("save_game"));
	UserDefault::getInstance()->deleteValueForKey("save_game");
}

void GameScene::handleBtnSetting(Ref* ref)
{
	SoundControl::getInstance()->playSoundClick();
	Layout* pnlSetting = GET_LAY(m_Layout, "pnlSetting");
	pnlSetting->setVisible(true);
	Layout* pnlInside = GET_LAY(pnlSetting, "pnlInside");
	MoveTo* moveIn = MoveTo::create(0.5f, Vec2(pnlInside->getPositionX(),
		pnlSetting->getContentSize().height - pnlInside->getContentSize().height));
	pnlInside->runAction(EaseBounceOut::create(moveIn));

	pnlSetting->addClickEventListener([=](Ref* ref)
	{
		MoveTo* moveOut = MoveTo::create(0.3f, Vec2(pnlInside->getPositionX(),
			pnlSetting->getContentSize().height));
		Sequence* seq = Sequence::createWithTwoActions(moveOut, CallFunc::create([=]()
		{
			pnlSetting->setVisible(false);
		}));
		pnlInside->runAction(seq);
	});
	GET_BUTTON(pnlInside, "btnHome")->addClickEventListener([=](Ref* ref)
	{
		SoundControl::getInstance()->playSoundClick();
		Director::getInstance()->replaceScene(TransitionFade::create(0.3f, MainScene::create()));
	});
	checkImgSound(pnlInside);
	GET_BUTTON(pnlInside, "btnSound")->addClickEventListener([=](Ref* ref)
	{
		bool isEnableSound = SoundControl::getInstance()->checkEnableSound();
		SoundControl::getInstance()->setEnableSound(!isEnableSound);
		checkImgSound(pnlInside);
		SoundControl::getInstance()->playSoundClick();
	});
	GET_BUTTON(pnlInside, "btnRestart")->addClickEventListener([=](Ref* ref)
	{
		SoundControl::getInstance()->playSoundClick();
		delDataSaved();
		_score = 0;
		_highScore = 0;
		initNewGame();
		MoveTo* moveOut = MoveTo::create(0.3f, Vec2(pnlInside->getPositionX(),
			pnlSetting->getContentSize().height));
		Sequence* seq = Sequence::createWithTwoActions(moveOut, CallFunc::create([=]()
		{
			pnlSetting->setVisible(false);
		}));
		pnlInside->runAction(seq);
	});
	GET_BUTTON(pnlInside, "btnContinuePlay")->addClickEventListener([=](Ref* ref)
	{
		SoundControl::getInstance()->playSoundClick();
		MoveTo* moveOut = MoveTo::create(0.3f, Vec2(pnlInside->getPositionX(),
			pnlSetting->getContentSize().height));
		Sequence* seq = Sequence::createWithTwoActions(moveOut, CallFunc::create([=]()
		{
			pnlSetting->setVisible(false);
		}));
		pnlInside->runAction(seq);
	});
}

void GameScene::checkImgSound(Layout* lay)
{
	if (!SoundControl::getInstance()->checkEnableSound())
	{
		GET_IMG(lay, "imgSound")->loadTexture("btn/setting/soundOn.png", Widget::TextureResType::PLIST);
	}
	else
	{
		GET_IMG(lay, "imgSound")->loadTexture("btn/setting/soundOff.png", Widget::TextureResType::PLIST);
	}
}


bool GameScene::onTouchBegan(Touch* touch, Event* event)
{
	float minPosXOfMatrix = 470;
	float maxPosXOfMatrix = 1065;
	float minPosYOfMatrix = 65;
	float maxPosYOfMatrix = 660;
	Vec2 touchLocation = touch->getLocation();
	if (touchLocation.x >= minPosXOfMatrix && touchLocation.x <= maxPosXOfMatrix
		&& touchLocation.y >= minPosYOfMatrix && touchLocation.y <= maxPosYOfMatrix)
	{
		if (!_flagClick)
			return false;
		setFlagClick(false);
		int x = (touchLocation.x - minPosXOfMatrix) / _sizeCell;
		int y = (maxPosYOfMatrix - touchLocation.y) / _sizeCell;
		CCLOG("x = %d, y = %d", x, y);
		if (_stateBtnSp == StateBtnSupport::TOUCHED)
		{
			_stateBtnSp = StateBtnSupport::UN_TOUCH;
			GET_IMG(m_Layout, "imgSupport")->setColor(Color3B(205, 205, 205));
			setStarHad(_starHad - _starNeedUseSp);
			setStarNeedUse(_starNeedUseSp * 2);
			handleSupport(x, y);
		}
		else
		{
			clickOnSquare(_vSquare[y][x]->getSquareId(), y, x);
		}
	}
	return true;
}

void GameScene::handleSupport(int x, int y)
{
	_vSquare[y][x]->setVisible(false);
	fillSquareBlank();
	createNewSquareAndFillSquareBlank();
	doActionFall();
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.2f), CallFunc::create([=]()
	{
		connectSquareSameId();
		setFlagClick(true);
	})));
}

void GameScene::saveGame()
{
	SaveDataObj objData;
	objData.push("score", _score);
	objData.push("flagRandMaxNumber", int(_flagRandMaxNumber));
	objData.push("starNeedUseSp", _starNeedUseSp);
	std::vector<int> vInt;
	for (int i = 0; i < MATRIX_ROW; i++)
	{
		for (int j = 0; j < MATRIX_COL; j++)
		{
			vInt.push_back(_vSquare[i][j]->getSquareId());
			vInt.push_back(_vSquare[i][j]->getSquareValue());
		}
	}
	objData.push("vSquare", vInt);
	std::string data;
	buildJson(&data, objData);
	UserDefault::getInstance()->setStringForKey("save_game", data);
}

void GameScene::loadGame()
{
	auto data = UserDefault::getInstance()->getStringForKey("save_game");
	if (data.empty())
	{
		initNewGame();
		return;
	}
	try
	{
		rapidjson::Document document;
		document.Parse<0>(data.c_str());
		const rapidjson::Value& value = document;
		int score = DICTOOL->getIntValue_json(value, "score");
		setHighScore(UserDefault::getInstance()->getIntegerForKey("high_score"), false);
		setScore(score, false);
		_starNeedUseSp = DICTOOL->getIntValue_json(value, "starNeedUseSp");
		setStarNeedUse(_starNeedUseSp);
		setStarHad(UserDefault::getInstance()->getIntegerForKey("star_had"));
		_flagRandMaxNumber = DICTOOL->getIntValue_json(value, "flagRandMaxNumber");
		for (int i = 0; i < DICTOOL->getArrayCount_json(value, "vSquare") / 2; i++)
		{
			int squareId = DICTOOL->getIntValueFromArray_json(value, "vSquare", (i * 2));
			int squareValue = DICTOOL->getIntValueFromArray_json(value, "vSquare", (i * 2 + 1));
			int pi = i / MATRIX_ROW;
			int pj = i % MATRIX_COL;
			createNewSquare(squareId, pi, pj);
			_vSquare[pi][pj]->setSquareValue(squareValue);
		}
		connectSquareSameId();
		if (checkLose())
			initNewGame();
	}
	catch (std::exception& e)
	{
		initNewGame();
	}
}

void GameScene::onTouchBtnSupport(Ref* ref)
{
	if (_starHad < _starNeedUseSp)
		return;
	SoundControl::getInstance()->playSoundClick();
	if (_stateBtnSp == StateBtnSupport::UN_TOUCH)
	{
		_stateBtnSp = StateBtnSupport::TOUCHED;
		GET_IMG(m_Layout, "imgSupport")->setColor(Color3B(161, 161, 161));
		connectSquareSameId(false);
	}
	else if (_stateBtnSp == StateBtnSupport::TOUCHED)
	{
		_stateBtnSp = StateBtnSupport::UN_TOUCH;
		GET_IMG(m_Layout, "imgSupport")->setColor(Color3B(205, 205, 205));
		connectSquareSameId();
	}

	//_vSquare[5][2]->setColorSquare(4);
	//_vSquare[5][2]->setSquareValue(50);
	//_vSquare[5][3]->setColorSquare(4);
	//_vSquare[5][3]->setSquareValue(50);
	//_vSquare[5][4]->setColorSquare(4);
	//_vSquare[5][4]->setSquareValue(50);
}

void GameScene::clickOnSquare(int id, int x, int y)
{
	auto index = findSquareSameId(x, y);
	if (int(index.size()) < _minSquareHasConnect)
	{
		SoundControl::getInstance()->playSound(SoundName::NO_MATCH);
		setFlagClick(true);
		return;
	}
	SoundControl::getInstance()->playSound(SoundName::MATCH);
	deActiveFourSideUp(index);
	moveSquare(x, y, index);

	auto call1 = CallFunc::create([=]()
	{
		deActiveSquareSameId(index);
	});
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(TIME_MOVE_SQUARE), call1));

	setScore(index.size());
	const int scoreInSquare = getScoreInSquare(x, y, index);
	_vSquare[x][y]->setSquareValue(scoreInSquare);

	auto call2 = CallFunc::create([=]()
	{
		fillSquareBlank();
		createNewSquareAndFillSquareBlank();
		doActionFall();
	});
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.3f), call2));

	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.35f), CallFunc::create([=]()
	{
		changeSquareId();
	})));

	if (!_isMergeCombo)
	{
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), CallFunc::create([=]()
		{
			deActiveSquaresHasTwoLength();
			connectSquareSameId();
		})));
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.7f), CallFunc::create([=]()
		{
			checkEndGame();
		})));
	}
	else
	{
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.5f), CallFunc::create([=]()
		{
			delSquareHasNumberMax();
		})));
		_isMergeCombo = false;
	}
}

void GameScene::deActiveFourSideUp(std::vector<int> vIndex)
{
	for (int index : vIndex)
	{
		int x = index / 10;
		int y = index % 10;
		_vSquare[x][y]->deActiveFourSide();
		for (int xUp = x - 1; xUp >= 0; xUp--)
		{
			_vSquare[xUp][y]->deActiveFourSide();
			if (y > 0 && _vSquare[xUp][y - 1]->getImgRight()->getScale() == 1)
				_vSquare[xUp][y - 1]->scaleRectRight(0);
			if (y < MATRIX_COL - 1 && _vSquare[xUp][y + 1]->getImgLeft()->getScale() == 1)
				_vSquare[xUp][y + 1]->scaleRectLeft(0);
		}
	}
}

void GameScene::deActiveSquareSameId(std::vector<int> vIndex)
{
	for (int i = 1; i < int(vIndex.size()); i++)
	{
		int x = vIndex[i] / 10;
		int y = vIndex[i] % 10;
		_vSquare[x][y]->setVisible(false);
	}
}

void GameScene::fillSquareBlank()
{
	for (int i = MATRIX_ROW - 1; i >= 0; i--)
	{
		for (int j = MATRIX_COL - 1; j >= 0; j--)
		{
			if (!_vSquare[i][j]->isVisible())
			{
				for (int i2 = i - 1; i2 >= 0; i2--)
				{
					if (_vSquare[i2][j]->isVisible())
					{
						auto temp = _vSquare[i][j];
						_vSquare[i][j] = _vSquare[i2][j];
						_vSquare[i2][j] = temp;
						_vSquare[i][j]->setXAndY(j, i);
						_vSquare[i2][j]->setXAndY(j, i2);
						_vSquare[i2][j]->deActiveFourSide();
						break;
					}
				}
			}
		}
	}
}

void GameScene::createNewSquareAndFillSquareBlank()
{
	for (int i = MATRIX_ROW - 1; i >= 0; i--)
	{
		for (int j = MATRIX_COL - 1; j >= 0; j--)
		{
			if (!_vSquare[i][j]->isVisible())
			{
				int id = randomColorSquare(1, 3);
				createNewSquare(id, i, j, true);
				if (id == ColorNumber::PURPLE)
					_vSquare[i][j]->setSquareValue(_maxScoreInSquare);
			}
		}
	}
}

void GameScene::doActionFall()
{
	for (int i = MATRIX_ROW - 1; i >= 0; i--)
	{
		for (int j = MATRIX_COL - 1; j >= 0; j--)
		{
			_vSquare[i][j]->runActionFall();
		}
	}
}

bool GameScene::changeSquareId()
{
	for (int i = 0; i < MATRIX_ROW; i++)
	{
		for (int j = 0; j < MATRIX_COL; j++)
		{
			if (_vSquare[i][j]->getSquareValue() >= _maxScoreInSquare && !_vSquare[i][j]->getIsRotate())
			{
				auto funcChangeId = CallFunc::create([=]()
				{
					_vSquare[i][j]->deActiveFourSide();
					_vSquare[i][j]->runAction(RotateBy::create(0.24f, 360));
					_vSquare[i][j]->setColorSquare(ColorNumber::PURPLE);
					_vSquare[i][j]->setIsRotate(true);
					SoundControl::getInstance()->playSound(SoundName::CELL_50);

				});
				this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.05f), funcChangeId));
				_flagRandMaxNumber = true;
				return true;
			}
		}
	}
	return false;
}

void GameScene::deActiveSquaresHasTwoLength()
{
	for (int i = MATRIX_ROW - 1; i >= 0; i--)
	{
		for (int j = MATRIX_COL - 1; j >= 0; j--)
		{
			int len = findSquareSameId(i, j).size();
			if (len == (_minSquareHasConnect - 1))
				_vSquare[i][j]->deActiveFourSide();
		}
	}
}

std::vector<int> GameScene::findSquareWillDel(int x, int y, int length)
{
	std::vector<int> idx;
	int k = 1;
	for (int i = 0; i < length; i++)
	{
		if (y - k >= 0 && length > 0) {    // trai
			idx.push_back(x * 10 + (y - k));
			length--;
		}
		else {
			length--;
		}
		if (y + k <= MATRIX_COL - 1 && length > 0) {   // phai
			idx.push_back(x * 10 + (y + k));
			length--;
		}
		else {
			length--;
		}
		if (x - k >= 0 && length > 0) {    // tren
			idx.push_back((x - k) * 10 + y);
			length--;
		}
		else {
			length--;
		}
		if (x + k <= MATRIX_ROW - 1 && length > 0) {   // duoi
			idx.push_back((x + k) * 10 + y);
			length--;
		}
		else {
			length--;
		}
		if (x - k >= 0 && y - k >= 0 && length > 0) {  // trai tren
			idx.push_back((x - k) * 10 + (y - k));
			length--;
		}
		else {
			length--;
		}
		if (x - k >= 0 && y + k <= MATRIX_COL - 1 && length > 0) { // phai tren
			idx.push_back((x - k) * 10 + (y + k));
			length--;
		}
		else {
			length--;
		}
		if (x + k <= MATRIX_ROW - 1 && y - k >= 0 && length > 0) { // trai duoi
			//if ((y * 10 + (x - k)) + 10 <= 55)
			//	idx.push_back((y * 10 + (x - k)) + 10);
			idx.push_back((x + k) * 10 + (y - k));
			length--;
		}
		else {
			length--;
		}
		if (x + k <= MATRIX_ROW - 1 && y + k <= MATRIX_COL - 1 && length > 0) {    // phai duoi
			//if ((y * 10 + (x + k)) + 10 <= 55)
			//	idx.push_back((y * 10 + (x + k)) + 10);
			idx.push_back((x + k) * 10 + (y + k));
			length--;
		}
		else {
			length--;
		}
		k++;
	}


	return idx;
}

void GameScene::delSquareHasNumberMax()
{
	SoundControl::getInstance()->playSound(SoundName::COMBO);
	int x = 0, y = 0, score = 0;
	for (int i = 0; i < MATRIX_ROW; i++)
	{
		for (int j = 0; j < MATRIX_COL; j++)
		{
			if (_vSquare[i][j]->getSquareValue() >= (_maxScoreInSquare * _minSquareHasConnect))
			{
				x = i;
				y = j;
				score = _vSquare[i][j]->getSquareValue();
				break;
			}
		}
	}
	int lengthRemove = (2 * (score / _maxScoreInSquare)) - 4;
	std::vector<int> vIndex = findSquareWillDel(x, y, lengthRemove);
	CallFunc* funcHideSquareAround = CallFunc::create([=]()
	{
		_vSquare[x][y]->runAction(ScaleTo::create(0.2f, 1.5));
		deActiveFourSideUp(vIndex);
		for (int i = 0; i < int(vIndex.size()); i++)
		{
			int xTemp = vIndex[i] / 10;
			int yTemp = vIndex[i] % 10;
			_vSquare[xTemp][yTemp]->getTextNumber()->setVisible(false);
			_vSquare[xTemp][yTemp]->setColorSquare(ColorNumber::PURPLE);
		}
	});
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.1f), funcHideSquareAround));
	_vSquare[x][y]->runAction(Sequence::create(DelayTime::create(0.25), ScaleTo::create(0.3f, 0), CallFunc::create([=]()
	{
		_vSquare[x][y]->setVisible(false);
	}), nullptr));

	CallFunc* funcScaleSquareAround = CallFunc::create([=]()
	{
		for (int index : vIndex)
		{
			int xTemp = index / 10;
			int yTemp = index % 10;
			if (yTemp > 0 && _vSquare[xTemp][yTemp - 1]->getImgRight()->getScale() == 1)
				_vSquare[xTemp][yTemp - 1]->scaleRectRight(0);
			if (yTemp < MATRIX_COL - 1 && _vSquare[xTemp][yTemp + 1]->getImgLeft()->getScale() == 1)
				_vSquare[xTemp][yTemp + 1]->scaleRectLeft(0);
			if (xTemp < MATRIX_ROW - 1 && _vSquare[xTemp + 1][yTemp]->getImgTop()->getScale() == 1)
				_vSquare[xTemp + 1][yTemp]->scaleRectTop(0);
			_vSquare[xTemp][yTemp]->runAction(Sequence::createWithTwoActions(ScaleTo::create(0.2f, 0), CallFunc::create([=]()
			{
				_vSquare[xTemp][yTemp]->setVisible(false);
			})));
		}
	});
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.55f), funcScaleSquareAround));

	CallFunc* funcCommon = CallFunc::create([=]()
	{
		fillSquareBlank();
		createNewSquareAndFillSquareBlank();
		doActionFall();
	});
	this->runAction(Sequence::createWithTwoActions(DelayTime::create(0.8f), funcCommon));

	this->runAction(Sequence::createWithTwoActions(DelayTime::create(1.0f), CallFunc::create([=]()
	{
		deActiveSquaresHasTwoLength();
		connectSquareSameId();
		setScore(500, true, true);
	})));

	this->runAction(Sequence::createWithTwoActions(DelayTime::create(1.2f), CallFunc::create([=]()
	{
		checkEndGame();
	})));
}

bool GameScene::checkLose()
{
	for (int i = 0; i < MATRIX_ROW; i++)
	{
		for (int j = 0; j < MATRIX_COL; j++)
		{
			if (int(findSquareSameId(i, j).size()) >= _minSquareHasConnect)
				return false;
		}
	}
	CCLOG("end game");
	return true;
}

void GameScene::checkEndGame()
{
	if (checkLose())
	{
		delDataSaved();
		SoundControl::getInstance()->playSound(SoundName::GAME_OVER);
		CallFunc* func = CallFunc::create([=]()
		{
			float starEarned = float(_score) / 100;
			GameOverScene* gameOver = GameOverScene::create();
			gameOver->setViewScore(_score);
			gameOver->updateStar(ceil(starEarned));
			gameOver->cloneSquare(static_cast<Layout*>(Helper::seekWidgetByName(m_Layout, "pnlRect")));
			Director::getInstance()->replaceScene(TransitionFade::create(0.3f, gameOver));
		});
		this->runAction(Sequence::createWithTwoActions(DelayTime::create(3.1f), func));
		return;
	}
	setFlagClick(true);
}

void GameScene::setFlagClick(bool flag)
{
	_flagClick = flag;
	if (flag)
		saveGame();
}

void GameScene::setStarNeedUse(int star)
{
	_starNeedUseSp = star;
	GET_TEXT(m_Layout, "lblStarNeed")->setString(StringUtils::toString(_starNeedUseSp));
}

void GameScene::setStarHad(int star)
{
	_starHad = star;
	UserDefault::getInstance()->setIntegerForKey("star_had", _starHad);
	GET_TEXT(m_Layout, "lblStar")->setString(StringUtils::toString(_starHad));
}

void GameScene::setScore(int num, bool actionRun/* = true*/, bool actionFly/* = false*/)
{
	const int newScore = _score + num;
	if (actionRun)
		TextRun::runText(GET_TEXT(m_Layout, "lblScore"), _score, newScore, actionFly);
	else
		GET_TEXT(m_Layout, "lblScore")->setString(StringUtils::toString(newScore));
	_score += num;
	if (_highScore < _score)
	{
		int numHighScore = _score - _highScore;
		setHighScore(numHighScore, actionRun);
	}
}

void GameScene::setHighScore(int num, bool actionRun/* = true*/)
{
	const int newHighScore = _highScore + num;
	if (actionRun)
		TextRun::runText(GET_TEXT(m_Layout, "lblHighScore"), _highScore, newHighScore);
	else
		GET_TEXT(m_Layout, "lblHighScore")->setString(StringUtils::toString(newHighScore));
	_highScore += num;
	UserDefault::getInstance()->setIntegerForKey("high_score", _highScore);
}

int GameScene::getScoreInSquare(int x, int y, std::vector<int> vIndex)
{
	int valueInSquare = _vSquare[x][y]->getSquareValue();
	for (int index : vIndex)
	{
		int xTemp = index / 10;
		int yTemp = index % 10;
		if (xTemp == x && yTemp == y)
			continue;
		valueInSquare += _vSquare[xTemp][yTemp]->getSquareValue();
	}
	if (valueInSquare >= (_maxScoreInSquare * _minSquareHasConnect))
		_isMergeCombo = true;
	else if (valueInSquare > _maxScoreInSquare)
		valueInSquare = _maxScoreInSquare;
	return valueInSquare;
}

void GameScene::moveSquare(int x, int y, std::vector<int> vIndex)
{
	for (int index : vIndex)
	{
		int xTemp = index / 10;
		int yTemp = index % 10;
		if (xTemp == x && yTemp == y)
			continue;
		_vSquare[xTemp][yTemp]->setLocalZOrder(0);
		_vSquare[xTemp][yTemp]->runAction(MoveTo::create(TIME_MOVE_SQUARE, _vSquare[x][y]->getPosition()));
	}
}

int GameScene::randomColorSquare(int min, int max) const
{
	const float percentHasSquareMaxNumber = rand_0_1();
	if (_flagRandMaxNumber && percentHasSquareMaxNumber <= _percentRandMaxNumber)
	{
		return ColorNumber::PURPLE;
	}
	return min + rand() % ((max + 1) - min);
}

void GameScene::createNewSquare(int id, int i, int j, bool fall/* = false*/)
{
	const float x = _firstPosX + (_sizeCell * j);
	float y = _firstPosY - (_sizeCell * i);
	if (fall)
		y = (_firstPosY + 300) - (_sizeCell * i);

	Layout* pnlrect = static_cast<Layout*>(Helper::seekWidgetByName(m_Layout, "pnlRect")->clone());
	Square* newSquare = Square::create(pnlrect);
	newSquare->setPosition(Vec2(x, y));
	newSquare->setColorSquare(id);
	if (id == ColorNumber::PURPLE)
	{
		newSquare->setIsRotate(true);
		newSquare->setSquareValue(_maxScoreInSquare);
	}
	newSquare->setXAndY(j, i);
	newSquare->setTouchEnabled(false);
	_vSquare[i][j] = newSquare;
	_pnlBoard->addChild(newSquare, 50);
}

void GameScene::createNewMatrix()
{
	_pnlBoard->removeAllChildren();
	std::ofstream a_file("matrix.txt");
	for (int i = 0; i < MATRIX_ROW; i++)
	{
		std::string strArr;
		for (int j = 0; j < MATRIX_COL; j++)
		{
			const int num = randomColorSquare(1, 3);
			createNewSquare(num, i, j);
			strArr = strArr + StringUtils::toString(num) + " ";
		}
		a_file << strArr << "\n";
	}
	a_file.close();
}

void GameScene::connectSquareSameId(bool isConnect/* = true*/)
{
	for (int i = 0; i < MATRIX_ROW; i++)
	{
		for (int j = 0; j < MATRIX_COL; j++)
		{
			if (!isConnect)
			{
				_vSquare[i][j]->deActiveFourSide();
				continue;
			}
			std::vector<int> vIndex = findSquareSameId(i, j);
			if (int(vIndex.size()) < _minSquareHasConnect)
				continue;
			int id = _vSquare[i][j]->getSquareId();
			for (int k = 0; k < int(vIndex.size()); k++)
			{
				int xTemp = vIndex.at(k) / 10;
				int yTemp = vIndex.at(k) % 10;
				if (xTemp > 0 && (_vSquare[xTemp - 1][yTemp]->getSquareId() == id))
					_vSquare[xTemp][yTemp]->scaleRectTop(1.0f);
				if (xTemp < MATRIX_ROW - 1 && (_vSquare[xTemp + 1][yTemp]->getSquareId() == id))
					_vSquare[xTemp][yTemp]->scaleRectBottom(1.0f);
				if (yTemp > 0 && (_vSquare[xTemp][yTemp - 1]->getSquareId() == id))
					_vSquare[xTemp][yTemp]->scaleRectLeft(1.0f);
				if (yTemp < MATRIX_COL - 1 && (_vSquare[xTemp][yTemp + 1]->getSquareId() == id))
					_vSquare[xTemp][yTemp]->scaleRectRight(1.0f);
			}
		}
	}
}

std::vector<int> GameScene::findSquareSameId(int x, int y)
{
	std::vector<int> idx;
	idx.push_back(x * 10 + y);
	int id = _vSquare[x][y]->getSquareId();
	for (int i = 0; i < int(idx.size()); i++)
	{
		int tempIndex = idx[i];
		int xTemp = tempIndex / 10;
		int yTemp = tempIndex % 10;
		if (xTemp > 0 && (_vSquare[xTemp - 1][yTemp]->getSquareId() == id) && !checkIdInArray(idx, tempIndex - 10))
			idx.push_back(tempIndex - 10);
		if (xTemp < MATRIX_ROW - 1 && (_vSquare[xTemp + 1][yTemp]->getSquareId() == id) && !checkIdInArray(idx, tempIndex + 10))
			idx.push_back(tempIndex + 10);
		if (yTemp > 0 && (_vSquare[xTemp][yTemp - 1]->getSquareId() == id) && !checkIdInArray(idx, tempIndex - 1))
			idx.push_back(tempIndex - 1);
		if (yTemp < MATRIX_COL - 1 && (_vSquare[xTemp][yTemp + 1]->getSquareId() == id) && !checkIdInArray(idx, tempIndex + 1))
			idx.push_back(tempIndex + 1);
	}

	return idx;
}

bool GameScene::checkIdInArray(std::vector<int> vIndex, int id)
{
	for (int i : vIndex)
	{
		if (i == id)
			return true;
	}
	return false;
}
