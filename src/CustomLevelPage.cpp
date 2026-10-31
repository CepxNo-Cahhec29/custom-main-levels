#include "CustomLevelPage.hpp"
#include "CustomLevelsLayer.hpp"

CustomLevelPage* CustomLevelPage::create(CustomLevelData const& data, size_t index) {
	auto ret = new CustomLevelPage();
	if (ret && ret->init(data, index)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool CustomLevelPage::init(CustomLevelData const& data, size_t index) {
	if (!CCLayer::init()) return false;

	m_data = data;
	m_index = index;

	auto winSize = CCDirector::get()->getWinSize();
	this->setContentSize(winSize);

	auto bg = CCScale9Sprite::create("GJ_square01.png");
	bg->setContentSize({ 340.f, 180.f });
	bg->setPosition(winSize / 2 + CCPoint { 0.f, 5.f });
	this->addChild(bg);

	auto nameLabel = CCLabelBMFont::create(m_data.name.c_str(), "bigFont.fnt");
	nameLabel->limitLabelWidth(300.f, 1.f, .3f);
	nameLabel->setPosition(winSize.width / 2, winSize.height / 2 + 65.f);
	this->addChild(nameLabel);

	std::string sub = "Custom Level";
	if (!m_data.creator.empty()) sub += " by " + m_data.creator;
	auto subLabel = CCLabelBMFont::create(sub.c_str(), "goldFont.fnt");
	subLabel->limitLabelWidth(300.f, .6f, .2f);
	subLabel->setPosition(winSize.width / 2, winSize.height / 2 + 42.f);
	this->addChild(subLabel);

	auto menu = CCMenu::create();
	menu->setPosition(0.f, 0.f);
	this->addChild(menu);

	auto playSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
	playSpr->setScale(.85f);
	auto playBtn = CCMenuItemSpriteExtra::create(playSpr, this, menu_selector(CustomLevelPage::onPlay));
	playBtn->setPosition(winSize.width / 2, winSize.height / 2 - 5.f);
	menu->addChild(playBtn);

	auto delSpr = CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
	delSpr->setScale(.8f);
	auto delBtn = CCMenuItemSpriteExtra::create(delSpr, this, menu_selector(CustomLevelPage::onRemove));
	delBtn->setPosition(winSize.width / 2 + 155.f, winSize.height / 2 - 75.f);
	menu->addChild(delBtn);

	return true;
}

void CustomLevelPage::onPlay(CCObject*) {
	auto level = CustomLevels::makeLevel(m_data);
	CustomLevels::g_playing = level;
	CustomLevels::g_returnIndex = static_cast<int>(m_index);
	auto scene = PlayLayer::scene(level, false, false);
	CCDirector::get()->replaceScene(CCTransitionFade::create(.5f, scene));
}

void CustomLevelPage::onRemove(CCObject*) {
	auto index = static_cast<int>(m_index);
	createQuickPopup(
		"Remove level",
		fmt::format("Remove <cy>{}</c> from the custom levels list?\n(It stays in My Levels.)", m_data.name),
		"Cancel", "Remove",
		[index](FLAlertLayer*, bool btn2) {
			if (!btn2) return;
			CustomLevels::remove(static_cast<size_t>(index));
			CCDirector::get()->replaceScene(
				CCTransitionFade::create(.5f, CustomLevelsLayer::scene(index > 0 ? index - 1 : 0))
			);
		}
	);
}
