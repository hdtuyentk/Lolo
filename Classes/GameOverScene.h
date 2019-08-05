#ifndef __GAME_OVER_SCENE_H__
#define __GAME_OVER_SCENE_H__

#include "cocos2d.h"
#include "base/BaseScene.h"

class GameOverScene : public BaseScene
{
public:
	GameOverScene();
	CREATE_FUNC(GameOverScene);
	bool init() override;
	void initUI() override;
	void setViewScore(int score);
	void updateStar(int star);
	void cloneSquare(Layout* lay);
	void drawBoard();

private:
	void createNewSquare(int id, int value, int i, int j);
	Layout* _square = nullptr;
};
#endif //__GAME_OVER_SCENE_H__
