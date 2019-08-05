#include "BaseScene.h"

BaseScene::BaseScene()
: Scene()
{
	
}

void BaseScene::initUI()
{
	root_layer_ = Layer::create();
	this->addChild(root_layer_);

	listener_ = EventListenerTouchOneByOne::create();
	listener_->setSwallowTouches(true);
	listener_->onTouchBegan = [=](Touch* touch, Event* event){
		return true;
	};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener_, this);
}

bool BaseScene::init()
{
	Scene::init();
	initUI();
	return true;
}