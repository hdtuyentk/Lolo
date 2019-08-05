#include "Square.h"
#include "base/ultil.h"

USING_NS_CC;
using namespace ui;

#define TIME_SCALE 0.2f

Square::Square()
: Layout()
, _squareId(0)
, isRotate(false)
{

}

Square* Square::create(Layout* lay)
{
	Square* square = new Square();
	square->init(lay);
	square->autorelease();
	return square;
}

bool Square::init(Layout* lay)
{
	if (!Layout::init())
		return false;
	initUI(lay);
	return true;
}

void Square::initUI(Layout* lay)
{
	_rootLayout = lay;
	_rootLayout->setTouchEnabled(false);
	_rootLayout->setAnchorPoint(Vec2(0.5, 0.5));
	lay->setPosition(Vec2::ZERO);
	this->addChild(lay);
	this->setContentSize(lay->getContentSize());

	_imgTop = GET_IMG(_rootLayout, "imgTop");
	_imgBottom = GET_IMG(_rootLayout, "imgBottom");
	_imgLeft = GET_IMG(_rootLayout, "imgLeft");
	_imgRight = GET_IMG(_rootLayout, "imgRight");
	_lblNumber = GET_TEXT(_rootLayout, "lblNumber");
	setSquareValue(1, false);
}

void Square::setColorSquare(int id)
{
	setSquareId(id);
	setColorById();
}

void Square::setXAndY(int x, int y)
{
	_x = x;
	_y = y;
}

void Square::setSquareValue(int value, bool action/* = true*/)
{
	if (!value)
		return;
	if (action)
	{
		TextRun::runText(_lblNumber, _value, value);
		_value = value;
	}
	else
	{
		_value = value;
		_lblNumber->setString(value == 1 ? "" : StringUtils::toString(value));
	}
}

void Square::onTickUpdateValue(float dt)
{

}

void Square::runActionFall()
{
	MoveTo* moveToAction = MoveTo::create(0.23f, Vec2(50 + (100 * _x), 550 - (100 * _y)));
	this->runAction(moveToAction);
}

void Square::scaleRectTop(float sizeScale, bool hasAction/* = false*/)
{
	if (hasAction)
	{
		_imgTop->setScale(sizeScale);
		return;
	}
	_imgTop->runAction(ScaleTo::create(TIME_SCALE, sizeScale));
}

void Square::scaleRectBottom(float sizeScale, bool hasAction/* = false*/)
{
	if (hasAction)
	{
		_imgBottom->setScale(sizeScale);
		return;
	}
	_imgBottom->runAction(ScaleTo::create(TIME_SCALE, sizeScale));
}

void Square::scaleRectLeft(float sizeScale, bool hasAction/* = false*/)
{
	if (hasAction)
	{
		_imgLeft->setScale(sizeScale);
		return;
	}
	_imgLeft->runAction(ScaleTo::create(TIME_SCALE, sizeScale));
}

void Square::scaleRectRight(float sizeScale, bool hasAction/* = false*/)
{
	if (hasAction)
	{
		_imgRight->setScale(sizeScale);
		return;
	}
	_imgRight->runAction(ScaleTo::create(TIME_SCALE, sizeScale));
}

void Square::deActiveFourSide()
{
	_imgTop->runAction(ScaleTo::create(TIME_SCALE, 0.0f));
	_imgBottom->runAction(ScaleTo::create(TIME_SCALE, 0.0f));
	_imgLeft->runAction(ScaleTo::create(TIME_SCALE, 0.0f));
	_imgRight->runAction(ScaleTo::create(TIME_SCALE, 0.0f));
}

void Square::setColorById() const
{
	Color3B color;
	switch (_squareId)
	{
	case ColorNumber::BLUE:
		color = Color3B(120, 174, 204);
		break;
	case ColorNumber::RED:
		color = Color3B(241, 115, 124);
		break;
	case ColorNumber::GREEN:
		color = Color3B(254, 206, 108);
		break;
	case ColorNumber::PURPLE:
		color = Color3B(57, 113, 142);
		break;
	default:
		color = Color3B::YELLOW;
		break;
	}
	_rootLayout->setColor(color);
	_imgTop->setColor(color);
	_imgBottom->setColor(color);
	_imgLeft->setColor(color);
	_imgRight->setColor(color);
}