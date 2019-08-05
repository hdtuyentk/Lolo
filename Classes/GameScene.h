#pragma once
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "editor-support/cocostudio/CocoStudio.h"
#include "base/BaseScene.h"
#include "Square.h"

USING_NS_CC;
using namespace ui;
using namespace cocostudio;

#define MATRIX_ROW 6
#define MATRIX_COL 6
enum StateBtnSupport
{
	UN_TOUCH,
	TOUCHED
};

struct SaveDataObj
{
	std::unordered_map<std::string, int> _dataInt;
	std::unordered_map<std::string, std::vector<int>> _dataArrInt;

	void push(const std::string& key, int value){ _dataInt.insert({ key, value }); };
	void push(const std::string& key, const std::vector<int>& value){ _dataArrInt.insert({ key, value }); };
};

class GameScene : public BaseScene
{
public:
	GameScene();
	CREATE_FUNC(GameScene);
	bool init() override;
	void initUI() override;
	bool onTouchBegan(Touch* touch, Event* e);
	void onTouchBtnSupport(Ref* ref);

private:
	int _firstPosX = 50;
	int _firstPosY = 550;
	int _sizeCell = 100;
	int _minSquareHasConnect = 3;
	int _score = 0;
	int _highScore = 0;
	int _maxScoreInSquare = 50;
	int _starNeedUseSp = 0;
	int _starHad = 0;
	float _percentRandMaxNumber = 0.04f;
	bool _flagClick = true;
	bool _flagRandMaxNumber = false;
	bool _isMergeCombo = false;

	Square* _vSquare[MATRIX_ROW][MATRIX_COL];
	Layout* _pnlBoard = nullptr;
	StateBtnSupport _stateBtnSp = StateBtnSupport::UN_TOUCH;

private:
	int randomColorSquare(int min, int max) const;
	void createNewSquare(int id, int i, int j, bool fall = false);
	void createNewMatrix();
	void connectSquareSameId(bool isConnect = true);
	std::vector<int> findSquareSameId(int x, int y);
	bool checkIdInArray(std::vector<int> vIndex, int id);
	void clickOnSquare(int id, int x, int y);
	void moveSquare(int x, int y, std::vector<int> vIndex);
	void deActiveFourSideUp(std::vector<int> vIndex);
	void deActiveSquareSameId(std::vector<int> vIndex);
	void setScore(int num, bool actionRun = true, bool actionFly = false);
	void setHighScore(int num, bool actionRun = true);
	int getScoreInSquare(int x, int y, std::vector<int> vIndex);
	void fillSquareBlank();
	void createNewSquareAndFillSquareBlank();
	void doActionFall();
	bool changeSquareId();
	void deActiveSquaresHasTwoLength();
	std::vector<int> findSquareWillDel(int x, int y, int length);
	void delSquareHasNumberMax();
	bool checkLose();
	void checkEndGame();
	void setFlagClick(bool flag);
	void setStarNeedUse(int star);
	void setStarHad(int star);
	void handleSupport(int x, int y);
	void saveGame();
	void loadGame();
	void initNewGame();
	void delDataSaved();
	void handleBtnSetting(Ref* ref);
	void checkImgSound(Layout* lay);
};

#endif