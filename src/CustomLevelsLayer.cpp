#include "CustomLevelsLayer.hpp"
#include "Ui.hpp"

static CCNode* makeBar(float width, ccColor3B color, int percent) {
	auto node = CCNode::create();

	auto bg = CCSprite::create("GJ_progressBar_001.png");
	auto size = bg->getContentSize();
	float scaleX = size.width > 0.f ? width / size.width : 1.f;
	bg->setColor({ 0, 0, 0 });
	bg->setOpacity(125);
	bg->setScaleX(scaleX);
	bg->setScaleY(.95f);
	node->addChild(bg);

	if (percent > 0) {
		auto fill = CCSprite::create("GJ_progressBar_001.png");
		float pct = std::min(percent, 100) / 100.f;
		fill->setColor(color);
		fill->setTextureRect(CCRect(0.f, 0.f, size.width * pct, size.height));
		fill->setAnchorPoint({ 0.f, .5f });
		fill->setScaleX(scaleX * .992f);
		fill->setScaleY(.86f * .95f);
		fill->setPosition({ -width / 2.f + width * .004f, 0.f });
		node->addChild(fill);
	}

	auto label = CCLabelBMFont::create(fmt::format("{}%", percent).c_str(), "bigFont.fnt");
	label->setScale(.55f);
	node->addChild(label, 5);
	return node;
}

static CCLabelBMFont* makeLabel(std::string const& text, char const* font, float scale, CCPoint pos) {
	auto label = CCLabelBMFont::create(text.c_str(), font);
	label->setScale(scale);
	label->setPosition(pos);
	return label;
}

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

	// background (color changes per level)
	m_bg = CCSprite::create("GJ_gradientBG.png");
	{
		auto size = m_bg->getContentSize();
		m_bg->setAnchorPoint({ 0.f, 0.f });
		m_bg->setScaleX(winSize.width / size.width);
		m_bg->setScaleY(winSize.height / size.height);
		m_bg->setZOrder(-10);
		this->addChild(m_bg);
	}

	// corner art
	auto bl = makeFrame("GJ_sideArt_001.png");
	bl->setAnchorPoint({ 0.f, 0.f });
	bl->setPosition({ -1.f, -1.f });
	this->addChild(bl, 2);

	auto br = makeFrame("GJ_sideArt_001.png");
	br->setFlipX(true);
	br->setAnchorPoint({ 1.f, 0.f });
	br->setPosition({ winSize.width + 1.f, -1.f });
	this->addChild(br, 2);

	auto menu = CCMenu::create();
	menu->setPosition(0.f, 0.f);
	this->addChild(menu, 10);

	auto backBtn = CCMenuItemSpriteExtra::create(makeFrame("GJ_arrow_01_001.png"), this, menu_selector(CustomLevelsLayer::onBack));
	backBtn->setPosition(24.f, winSize.height - 22.f);
	menu->addChild(backBtn);

	auto infoBtn = CCMenuItemSpriteExtra::create(makeFrame("GJ_infoIcon_001.png"), this, menu_selector(CustomLevelsLayer::onInfo));
	infoBtn->setPosition(winSize.width - 20.f, winSize.height - 20.f);
	menu->addChild(infoBtn);

	m_prevBtn = CCMenuItemSpriteExtra::create(makeFrame("navArrowBtn_001.png"), this, menu_selector(CustomLevelsLayer::onPrev));
	m_prevBtn->setPosition(25.f, winSize.height / 2);
	menu->addChild(m_prevBtn);

	auto nextSpr = makeFrame("navArrowBtn_001.png");
	nextSpr->setFlipX(true);
	m_nextBtn = CCMenuItemSpriteExtra::create(nextSpr, this, menu_selector(CustomLevelsLayer::onNext));
	m_nextBtn->setPosition(winSize.width - 25.f, winSize.height / 2);
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

	auto content = CCNode::create();
	this->addChild(content, 1);
	m_content = content;

	if (count == 0) {
		m_bg->setColor(CustomLevels::colors()[0].color);
		auto label = CCLabelBMFont::create(
			"No custom levels yet.\nOpen My Levels, pick a level\nand press the + button.",
			"bigFont.fnt"
		);
		label->setScale(.5f);
		label->setPosition(winSize / 2);
		content->addChild(label);
		return;
	}

	if (m_index < 0) m_index = count - 1;
	if (m_index >= count) m_index = 0;
	auto const& d = levels[m_index];

	auto const& colorList = CustomLevels::colors();
	m_bg->setColor(colorList[CustomLevels::wrapIndex(d.color, colorList)].color);

	// ---------------- the level card (tap to play) ----------------
	float cardW = 339.f;
	float cardH = 95.f;
	auto card = CCNode::create();
	card->setContentSize({ cardW, cardH });

	auto cardBg = CCScale9Sprite::create("square02b_001.png");
	cardBg->setContentSize({ cardW, cardH });
	cardBg->setColor({ 0, 0, 0 });
	cardBg->setOpacity(110);
	cardBg->setPosition({ cardW / 2, cardH / 2 });
	card->addChild(cardBg);

	// difficulty face + feature glow
	auto const& diffList = CustomLevels::difficulties();
	auto const& featList = CustomLevels::features();
	CCPoint facePos { 36.f, 50.f };
	auto const& feat = featList[CustomLevels::wrapIndex(d.feature, featList)];
	if (feat.frame) {
		auto glow = makeFrame(feat.frame);
		glow->setPosition(facePos);
		card->addChild(glow, 1);
	}
	auto face = makeFrame(diffList[CustomLevels::wrapIndex(d.difficulty, diffList)].frame);
	face->setPosition(facePos);
	face->setScale(.95f);
	card->addChild(face, 2);

	// name
	auto nameLabel = CCLabelBMFont::create(d.name.c_str(), "bigFont.fnt");
	nameLabel->limitLabelWidth(225.f, 1.f, .3f);
	nameLabel->setPosition({ 190.f, 46.f });
	card->addChild(nameLabel, 2);

	// stars (top right)
	if (d.stars > 0) {
		card->addChild(makeLabel(std::to_string(d.stars), "goldFont.fnt", .65f, { 305.f, 82.f }), 2);
		auto star = makeFrame("GJ_starsIcon_001.png");
		star->setPosition({ 324.f, 82.f });
		card->addChild(star, 2);
	}

	// orbs (bottom left)
	int bestNormal = CustomLevels::bestPercent(d, false);
	int bestPractice = CustomLevels::bestPercent(d, true);
	if (d.orbs > 0) {
		int earned = static_cast<int>(static_cast<long long>(d.orbs) * std::min(bestNormal, 100) / 100);
		card->addChild(makeLabel(fmt::format("{}/{}", earned, d.orbs), "bigFont.fnt", .5f, { 38.f, 12.f }), 2);
		auto orb = makeFrame("currencyOrbIcon_001.png");
		orb->setPosition({ 78.f, 12.f });
		card->addChild(orb, 2);
	}

	// coins (bottom right), gold when the level is completed
	for (int i = 0; i < std::clamp(d.coins, 0, 3); i++) {
		auto coin = makeFrame("secretCoin_01_001.png");
		coin->setPosition({ 272.f + 26.f * i, 15.f });
		if (bestNormal < 100) coin->setColor({ 120, 120, 120 });
		card->addChild(coin, 2);
	}

	auto cardMenu = CCMenu::create();
	cardMenu->setPosition(0.f, 0.f);
	content->addChild(cardMenu);
	auto cardBtn = CCMenuItemSpriteExtra::create(card, this, menu_selector(CustomLevelsLayer::onPlay));
	cardBtn->setPosition({ winSize.width / 2, winSize.height - 99.6f });
	cardMenu->addChild(cardBtn);

	// ---------------- progress bars ----------------
	float cx = winSize.width / 2;
	content->addChild(makeLabel("Normal Mode", "bigFont.fnt", .55f, { cx, 149.f }));
	auto normalBar = makeBar(cardW, { 0, 255, 0 }, bestNormal);
	normalBar->setPosition({ cx, 130.f });
	content->addChild(normalBar);

	content->addChild(makeLabel("Practice Mode", "bigFont.fnt", .55f, { cx, 99.f }));
	auto practiceBar = makeBar(cardW, { 0, 255, 255 }, bestPractice);
	practiceBar->setPosition({ cx, 80.f });
	content->addChild(practiceBar);

	// ---------------- page dots ----------------
	auto dots = CCDrawNode::create();
	float spacing = 16.f;
	float startX = cx - spacing * (count - 1) / 2.f;
	for (int i = 0; i < count; i++) {
		ccColor4F color = i == m_index ? ccColor4F { 1.f, 1.f, 1.f, 1.f } : ccColor4F { .55f, .55f, .55f, 1.f };
		dots->drawDot({ startX + spacing * i, 15.f }, 3.5f, color);
	}
	content->addChild(dots);
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

void CustomLevelsLayer::onPlay(CCObject*) {
	auto levels = CustomLevels::load();
	if (m_index < 0 || m_index >= static_cast<int>(levels.size())) return;

	auto level = CustomLevels::makeLevel(levels[m_index]);
	CustomLevels::g_playing = level;
	CustomLevels::g_returnIndex = m_index;
	CCDirector::get()->replaceScene(CCTransitionFade::create(.5f, PlayLayer::scene(level, false, false)));
}

void CustomLevelsLayer::onInfo(CCObject*) {
	auto levels = CustomLevels::load();
	if (m_index < 0 || m_index >= static_cast<int>(levels.size())) return;
	auto const& d = levels[m_index];

	auto const& diffList = CustomLevels::difficulties();
	auto const& featList = CustomLevels::features();
	auto const& colorList = CustomLevels::colors();
	std::string text = fmt::format(
		"<cy>{}</c>\nby {}\nDifficulty: {}\nFeature: {}\nBackground: {}\nStars: {}   Orbs: {}\nDiamonds: {}   Coins: {}",
		d.name,
		d.creator.empty() ? std::string("-") : d.creator,
		diffList[CustomLevels::wrapIndex(d.difficulty, diffList)].name,
		featList[CustomLevels::wrapIndex(d.feature, featList)].name,
		colorList[CustomLevels::wrapIndex(d.color, colorList)].name,
		d.stars, d.orbs, d.diamonds, d.coins
	);

	int index = m_index;
	createQuickPopup(
		"Level info", text, "Close", "Remove",
		[index](FLAlertLayer*, bool btn2) {
			if (!btn2) return;
			createQuickPopup(
				"Remove level",
				"Remove this level from the custom list?\n(It stays in My Levels.)",
				"No", "Yes",
				[index](FLAlertLayer*, bool yes) {
					if (!yes) return;
					CustomLevels::remove(static_cast<size_t>(index));
					CCDirector::get()->replaceScene(
						CCTransitionFade::create(.5f, CustomLevelsLayer::scene(index > 0 ? index - 1 : 0))
					);
				}
			);
		}
	);
}
