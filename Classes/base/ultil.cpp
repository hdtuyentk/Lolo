#include "base/ultil.h"
#include "AudioEngine.h"

#pragma region TextRun
TextRun::TextRun()
: _animationTime(0.5)
, _delayTime(0.03)
{

}

void TextRun::addFlyText(Text* text, int changeValue)
{
	if (changeValue == 0)
		return;
	float flyTime = 2.0f;
	Text* txtFly = Text::create("+" + StringUtils::toString(changeValue), "ui/Font/Coiny-Cyrillic.ttf", 30);
	txtFly->setAnchorPoint(Vec2(0, 0));
	txtFly->setPosition(Vec2(text->getContentSize().width / 2, 20));
	txtFly->setColor(Color3B(149, 149, 149));
	text->addChild(txtFly);
	CallFunc* funcRemove = CallFunc::create([=]()
	{
		txtFly->removeFromParent();
	});
	auto moveAndRemove = Sequence::createWithTwoActions(MoveBy::create(flyTime, Vec2(0, 50)), funcRemove);
	auto fade = Sequence::createWithTwoActions(DelayTime::create(flyTime - 1.0f), FadeOut::create(1.0f));
	txtFly->runAction(Spawn::create(moveAndRemove, fade, nullptr));
}

void TextRun::handleTextRunning(Text* text, int curValue, int newValue, bool actionFly/* = false*/)
{
	if (curValue == newValue)
		return;
	this->retain();
	if (actionFly)
	{
		addFlyText(text, newValue - curValue);
	}
	_text = text;
	_curValue = curValue;
	_newValue = newValue;
	float repeat = _animationTime / _delayTime;
	_bonusNum = (newValue - curValue) / repeat;
	Director::getInstance()->getScheduler()->schedule(CC_CALLBACK_1(TextRun::updateText, this), text, 0.0f, false, "updateText");
}

void TextRun::updateText(float time)
{
	CC_ASSERT(_text);
	_curValue = (_curValue + _bonusNum) > _newValue ? _newValue : (_curValue + _bonusNum);
	_text->setString(StringUtils::toString(int(_curValue)));
	if (_curValue >= _newValue)
	{
		Director::getInstance()->getScheduler()->unschedule("updateText", _text);
		CC_SAFE_RELEASE(this);
	}
}

void TextRun::runText(Text* text, int curValue, int newValue, bool actionFly/* = false*/)
{
	return TextRun::create()->handleTextRunning(text, curValue, newValue, actionFly);
}

#pragma endregion

#pragma region SoundControl

SoundControl* SoundControl::_instance = nullptr;
SoundControl::SoundControl()
{

}

SoundControl::~SoundControl()
{
	_instance = nullptr;
}

SoundControl* SoundControl::getInstance()
{
	if (!_instance)
		_instance = new SoundControl();
	return _instance;
}

bool SoundControl::checkEnableSound()
{
	_isEnableSound = UserDefault::getInstance()->getBoolForKey("sound");
	return _isEnableSound;
}

void SoundControl::setEnableSound(bool enable)
{
	_isEnableSound = enable;
	UserDefault::getInstance()->setBoolForKey("sound", enable);
}

void SoundControl::playSound(std::string filePath, bool loop/* = false*/)
{
	if (filePath.empty())
		return;
	if (checkEnableSound())
		return;
	experimental::AudioEngine::play2d(filePath, loop);
}

void SoundControl::playSoundClick()
{
	if (checkEnableSound())
		return;
	experimental::AudioEngine::play2d(SoundName::CLICK, false);
}
#pragma endregion
