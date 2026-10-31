#include "CustomLevelsLayer.hpp"

CustomLevelsLayer* CustomLevelsLayer::create(int index) {
	auto ret = new CustomLevelsLayer();
	if (ret && ret->init(index)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

CCScene* CustomLevelsLayer::scene(int index) {
	auto scene = CCScene::create();
	scene->addChild(CustomLevelsLayer::create(index));
	return scene;
}

bool CustomLevelsLayer::init(int index) {
	if (!CCLayer::init()) return false;

	CustomLevels::g_playing = nullptr;
	m_index = index;

	auto winSize = CCDirector::get()->getWinSize();

	// background
	auto bg = CCSprite::create("GJ_gradientBG.png");
	if (bg) {
		auto size = bg->getContentSize();
		bg->setAnchorPoint({ 0.f, 0.f });
		bg->setScaleX(winSize.width / size.width);
		bg->setScaleY(winSize.height / size.height);
		bg->setColor({ 40, 125, 255 });
		bg->setZOrder(-10);
		this->addChild(bg);
	}

	auto title = CCLabelBMFont::create("Custom Levels", "goldFont.fnt");
	title->setPosition(winSize.width / 2, winSize.height - 22.f);
	this->addChild(title);

	m_counter = CCLabelBMFont::create("", "bigFont.fnt");
	m_counter->setScale(.5f);
	m_counter->setPosition(winSize.width / 2, 22.f);
	this->addChild(m_counter);

	auto menu = CCMenu::create();
	menu->setPosition(0.f, 0.f);
	this->addChild(menu, 10);

	auto backSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
	auto backBtn = CCMenuItemSpriteExtra::create(backSpr, this, menu_selector(CustomLevelsLayer::onBack));
	backBtn->setPosition(25.f, winSize.height - 25.f);
	menu->addChild(backBtn);

	auto prevSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
	m_prevBtn = CCMenuItemSpriteExtra::create(prevSpr, this, menu_selector(CustomLevelsLayer::onPrev));
	m_prevBtn->setPosition(35.f, winSize.height / 2);
	menu->addChild(m_prevBtn);

	auto nextSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
	nextSpr->setFlipX(true);
	m_nextBtn = CCMenuItemSpriteExtra::create(nextSpr, this, menu_selector(CustomLevelsLayer::onNext));
	m_nextBtn->setPosition(winSize.width - 35.f, winSize.height / 2);
	menu->addChild(m_nextBtn);

	this->setKeypadEnabled(true);
	this->refresh();
	return true;
}

void CustomLevelsLayer::refresh() {
	if (m_content) {
		m_content->removeFromParent();
		m_content = nullptr;
	}

	auto winSize = CCDirector::get()->getWinSize();
	auto levels = CustomLevels::load();
	int count = static_cast<int>(levels.size());

	m_prevBtn->setVisible(count > 1);
	m_nextBtn->setVisible(count > 1);

	if (count == 0) {
		auto label = CCLabelBMFont::create(
			"No custom levels yet.\nOpen My Levels, pick a level\nand press Add to Main Levels.",
			"bigFont.fnt"
		);
		label->setScale(.5f);
		label->setPosition(winSize / 2);
		this->addChild(label);
		m_content = label;
		m_counter->setString("");
		return;
	}

	if (m_index < 0) m_index = count - 1;
	if (m_index >= count) m_index = 0;

	auto page = CustomLevelPage::create(levels[m_index], static_cast<size_t>(m_index));
	this->addChild(page);
	m_content = page;

	m_counter->setString(fmt::format("{} / {}", m_index + 1, count).c_str());
}

void CustomLevelsLayer::onBack(CCObject*) {
	CCDirector::get()->replaceScene(CCTransitionFade::create(.5f, LevelSelectLayer::scene(0)));
}

void CustomLevelsLayer::keyBackClicked() {
	this->onBack(nullptr);
}

void CustomLevelsLayer::onPrev(CCObject*) {
	m_index--;
	this->refresh();
}

void CustomLevelsLayer::onNext(CCObject*) {
	m_index++;
	this->refresh();
}
