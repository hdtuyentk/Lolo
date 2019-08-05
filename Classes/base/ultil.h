#ifndef __ULTIL_H__
#define __ULTIL_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

using namespace cocos2d;
using namespace ui;

#include <sstream>
#include <string>

enum ColorNumber
{
	BLUE = 1,
	RED = 2,
	GREEN = 3,
	PURPLE = 4

};

namespace SoundName
{

	static std::string CELL_50 = "sound/cell50.mp3";
	static std::string CLICK = "sound/click.mp3";
	static std::string COMBO = "sound/combo.mp3";
	static std::string MATCH = "sound/match.mp3";
	static std::string NO_MATCH = "sound/no_match.mp3";
	static std::string GAME_OVER = "sound/game_over.mp3";
}

#define GET_IMG(_root_, _name_) (static_cast<ImageView*>(Helper::seekWidgetByName(_root_, _name_)))
#define GET_TEXT(_root_, _name_) (static_cast<Text*>(Helper::seekWidgetByName(_root_, _name_)))
#define GET_BUTTON(_root_, _name_) (static_cast<Button*>(Helper::seekWidgetByName(_root_, _name_)))
#define GET_LAY(_root_, _name_) (static_cast<Layout*>(Helper::seekWidgetByName(_root_, _name_)))

class TextRun : public Node
{
public:
	TextRun();
	CREATE_FUNC(TextRun);

	void addFlyText(Text* text, int changeValue);
	void handleTextRunning(Text* text, int curValue, int newValue, bool actionFly = false);
	void updateText(float time);
	static void runText(Text* text, int curValue, int newValue, bool actionFly = false);

private:
	Text* _text = nullptr;
	float _curValue = 0;
	float _newValue = 0;
	float _bonusNum = 0;
	CC_SYNTHESIZE(float, _animationTime, AnimationTime);
	CC_SYNTHESIZE(float, _delayTime, DelayTime);
};

class SoundControl
{
public:
	SoundControl();
	~SoundControl();
	static SoundControl* getInstance();
	bool checkEnableSound();
	void setEnableSound(bool enable);
	void playSound(std::string filePath, bool loop = false);
	void playSoundClick();

private:
	static SoundControl* _instance;
	bool _isEnableSound = false;
};

#endif // __ULTIL_H__