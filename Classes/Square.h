#ifndef __SQUARE_H__
#define __SQUARE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace ui;

class Square : public cocos2d::ui::Layout
{
public:
	Square();
	static Square* create(Layout* lay);
	bool init(Layout* lay);
	void initUI(Layout* lay);
	void setColorSquare(int id);
	void setXAndY(int x, int y);
	void setSquareValue(int value, bool action = true);
	void runActionFall();
	void scaleRectTop(float sizeScale, bool hasAction = false);
	void scaleRectBottom(float sizeScale, bool hasAction = false);
	void scaleRectLeft(float sizeScale, bool hasAction = false);
	void scaleRectRight(float sizeScale, bool hasAction = false);
	void deActiveFourSide();
	ImageView* getImgTop(){ return _imgTop; };
	ImageView* getImgBottom(){ return _imgBottom; };
	ImageView* getImgLeft(){ return _imgLeft; };
	ImageView* getImgRight(){ return _imgRight; };
	Text* getTextNumber(){ return _lblNumber; };

private:
	Layout* _rootLayout = nullptr;
	ImageView* _imgTop = nullptr;
	ImageView* _imgBottom = nullptr;
	ImageView* _imgLeft = nullptr;
	ImageView* _imgRight = nullptr;
	Text* _lblNumber = nullptr;

	int _x = 0;
	int _y = 0;
	int _maxValue = 50;

	CC_SYNTHESIZE(int, _squareId, SquareId);
	CC_SYNTHESIZE_READONLY(int, _value, SquareValue);
	CC_SYNTHESIZE(bool, isRotate, IsRotate);

private:
	void setColorById() const;
	void onTickUpdateValue(float dt);
};

#endif // __SQUARE_H__
