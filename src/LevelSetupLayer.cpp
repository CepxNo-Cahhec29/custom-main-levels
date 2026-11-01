#include "LevelSetupLayer.hpp"
#include "Ui.hpp"

LevelSetupLayer::~LevelSetupLayer() {
	CC_SAFE_RELEASE(m_level);
}

LevelSetupLayer* LevelSetupLayer::create(GJGameLevel* level) {
	auto ret = new LevelSetupLayer();
	if (ret && ret->init(level)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

CCScene* LevelSetupLayer::scene(GJGameLevel* level) {
	auto scene = CCScene::create();
	scene->addChild(LevelSetupLayer::create(level));
	return scene;
}

static CCLabelBMFont* smallLabel(std::string const& text, CCPoint pos) {
	auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
	label->setScale(.4f);
	label->setPosition(pos);
	return label;
}

bool LevelSetupLayer::init(GJGameLevel* level) {
	if (!CCLayer::init()) return false;

	m_level = level;
	CC_SAFE_RETAIN(m_level);

	auto winSize = CCDirector::get()->getWinSize();
	auto defaults = CustomLevels::dataFromLevel(level);

	// background (live preview of the chosen color)
	m_bg = CCSprite::create("GJ_gradientBG.png");
	{
		auto size = m_bg->getContentSize();
		m_bg->setAnchorPoint({ 0.f, 0.f });
		m_bg->setScaleX(winSize.width / size.width);
		m_bg->setScaleY(winSize.height / size.height);
		m_bg->setZOrder(-10);
		this->addChild(m_bg);
	}

	auto title = CCLabelBMFont::create("Custom Level Setup", "goldFont.fnt");
	title->setScale(.8f);
	title->setPosition(winSize.width / 2, winSize.height - 20.f);
	this->addChild(title);

	std::string levelName = level->m_levelName;
	auto sub = CCLabelBMFont::create(levelName.c_str(), "bigFont.fnt");
	sub->limitLabelWidth(300.f, .5f, .2f);
	sub->setPosition(winSize.width / 2, winSize.height - 42.f);
	this->addChild(sub);

	float lx = 150.f;
	float rx = 420.f;
	float y1 = 235.f, y2 = 185.f, y3 = 135.f, y4 = 85.f;

	// left column
	this->addChild(smallLabel("Difficulty", { lx, y1 + 22.f }));
	this->addChild(smallLabel("Feature", { lx, y2 + 22.f }));
	this->addChild(smallLabel("Background", { lx, y3 + 22.f }));
	this->addChild(smallLabel("Coins", { lx, y4 + 22.f }));

	// right column
	this->addChild(smallLabel("Stars", { rx, y1 + 22.f }));
	this->addChild(smallLabel("Orbs", { rx, y2 + 22.f }));
	this->addChild(smallLabel("Diamonds", { rx, y3 + 22.f }));

	m_stars = TextInput::create(120.f, "0");
	m_stars->setString(std::to_string(defaults.stars));
	m_stars->setFilter("0123456789");
	m_stars->setPosition({ rx, y1 });
	this->addChild(m_stars);

	m_orbs = TextInput::create(120.f, "0");
	m_orbs->setString(std::to_string(defaults.orbs));
	m_orbs->setFilter("0123456789");
	m_orbs->setPosition({ rx, y2 });
	this->addChild(m_orbs);

	m_diamonds = TextInput::create(120.f, "0");
	m_diamonds->setString(std::to_string(defaults.diamonds));
	m_diamonds->setFilter("0123456789");
	m_diamonds->setPosition({ rx, y3 });
	this->addChild(m_diamonds);

	m_coins = std::clamp(defaults.coins, 0, 3);

	auto menu = CCMenu::create();
	menu->setPosition(0.f, 0.f);
	this->addChild(menu, 10);

	m_diffSpr = ButtonSprite::create("Easy");
	m_diffSpr->setScale(.6f);
	auto diffBtn = CCMenuItemSpriteExtra::create(m_diffSpr, this, menu_selector(LevelSetupLayer::onDifficulty));
	diffBtn->setPosition({ lx, y1 });
	menu->addChild(diffBtn);

	m_featSpr = ButtonSprite::create("None");
	m_featSpr->setScale(.6f);
	auto featBtn = CCMenuItemSpriteExtra::create(m_featSpr, this, menu_selector(LevelSetupLayer::onFeature));
	featBtn->setPosition({ lx, y2 });
	menu->addChild(featBtn);

	m_colorSpr = ButtonSprite::create("Blue");
	m_colorSpr->setScale(.6f);
	auto colorBtn = CCMenuItemSpriteExtra::create(m_colorSpr, this, menu_selector(LevelSetupLayer::onColor));
	colorBtn->setPosition({ lx, y3 });
	menu->addChild(colorBtn);

	m_coinsSpr = ButtonSprite::create("0");
	m_coinsSpr->setScale(.6f);
	auto coinsBtn = CCMenuItemSpriteExtra::create(m_coinsSpr, this, menu_selector(LevelSetupLayer::onCoins));
	coinsBtn->setPosition({ lx, y4 });
	menu->addChild(coinsBtn);

	auto addSpr = ButtonSprite::create("Add");
	addSpr->setScale(.8f);
	auto addBtn = CCMenuItemSpriteExtra::create(addSpr, this, menu_selector(LevelSetupLayer::onAdd));
	addBtn->setPosition({ winSize.width / 2 + 60.f, 28.f });
	menu->addChild(addBtn);

	auto cancelSpr = ButtonSprite::create("Cancel");
	cancelSpr->setScale(.8f);
	auto cancelBtn = CCMenuItemSpriteExtra::create(cancelSpr, this, menu_selector(LevelSetupLayer::onCancel));
	cancelBtn->setPosition({ winSize.width / 2 - 60.f, 28.f });
	menu->addChild(cancelBtn);

	this->setKeypadEnabled(true);
	this->refreshControls();
	return true;
}

void LevelSetupLayer::refreshControls() {
	auto const& diffList = CustomLevels::difficulties();
	auto const& featList = CustomLevels::features();
	auto const& colorList = CustomLevels::colors();

	m_difficulty = CustomLevels::wrapIndex(m_difficulty, diffList);
	m_feature = CustomLevels::wrapIndex(m_feature, featList);
	m_color = CustomLevels::wrapIndex(m_color, colorList);

	m_diffSpr->setString(diffList[m_difficulty].name);
	m_featSpr->setString(featList[m_feature].name);
	m_colorSpr->setString(colorList[m_color].name);
	m_coinsSpr->setString(std::to_string(m_coins).c_str());

	m_bg->setColor(colorList[m_color].color);

	// small preview of the difficulty face with its feature glow
	if (m_preview) {
		m_preview->removeFromParent();
		m_preview = nullptr;
	}
	auto winSize = CCDirector::get()->getWinSize();
	auto preview = CCNode::create();
	preview->setPosition({ 420.f, 85.f });
	if (featList[m_feature].frame) {
		preview->addChild(makeFrame(featList[m_feature].frame), 1);
	}
	preview->addChild(makeFrame(diffList[m_difficulty].frame), 2);
	this->addChild(preview, 5);
	m_preview = preview;
	(void)winSize;
}

void LevelSetupLayer::onDifficulty(CCObject*) {
	m_difficulty++;
	this->refreshControls();
}

void LevelSetupLayer::onFeature(CCObject*) {
	m_feature++;
	this->refreshControls();
}

void LevelSetupLayer::onColor(CCObject*) {
	m_color++;
	this->refreshControls();
}

void LevelSetupLayer::onCoins(CCObject*) {
	m_coins = (m_coins + 1) % 4;
	this->refreshControls();
}

void LevelSetupLayer::onAdd(CCObject*) {
	auto parse = [](TextInput* input, int maxValue) {
		int v = std::atoi(input->getString().c_str());
		return std::clamp(v, 0, maxValue);
	};

	if (std::string(m_level->m_levelString).empty()) {
		FLAlertLayer::create("Error", "This level has no data to add (is it empty?).", "OK")->show();
		return;
	}

	auto data = CustomLevels::dataFromLevel(m_level);
	data.difficulty = m_difficulty;
	data.feature = m_feature;
	data.color = m_color;
	data.coins = m_coins;
	data.stars = parse(m_stars, 100);
	data.orbs = parse(m_orbs, 100000);
	data.diamonds = parse(m_diamonds, 1000);

	bool added = CustomLevels::add(data);
	Notification::create(
		added ? "Added to Custom Levels" : "Already in Custom Levels",
		added ? NotificationIcon::Success : NotificationIcon::Warning,
		2.5f
	)->show();

	CCDirector::get()->replaceScene(CCTransitionFade::create(.5f, EditLevelLayer::scene(m_level)));
}

void LevelSetupLayer::onCancel(CCObject*) {
	CCDirector::get()->replaceScene(CCTransitionFade::create(.5f, EditLevelLayer::scene(m_level)));
}

void LevelSetupLayer::keyBackClicked() {
	this->onCancel(nullptr);
}
