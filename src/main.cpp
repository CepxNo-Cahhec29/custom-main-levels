#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include "CustomLevels.hpp"
#include "CustomLevelsLayer.hpp"

using namespace geode::prelude;

// ---------------------------------------------------------------------------
// Main levels screen (Play button): add a "Custom Levels" button.
// The official page list is NOT touched (GD recycles its pages internally).
// ---------------------------------------------------------------------------
class $modify(CMLLevelSelectLayer, LevelSelectLayer) {
	bool init(int page) {
		if (!LevelSelectLayer::init(page)) return false;

		auto winSize = CCDirector::get()->getWinSize();

		auto menu = CCMenu::create();
		menu->setID("custom-levels-menu"_spr);
		menu->setPosition(0.f, 0.f);
		this->addChild(menu, 100);

		auto spr = ButtonSprite::create("Custom Levels");
		spr->setScale(.6f);
		auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(CMLLevelSelectLayer::onCustomLevels));
		btn->setID("custom-levels-button"_spr);
		btn->setPosition(winSize.width - 70.f, 22.f);
		menu->addChild(btn);

		return true;
	}

	void onCustomLevels(CCObject*) {
		CCDirector::get()->replaceScene(CCTransitionFade::create(.5f, CustomLevelsLayer::scene(0)));
	}
};

// ---------------------------------------------------------------------------
// When leaving a custom level, go back to the custom levels screen.
// ---------------------------------------------------------------------------
class $modify(CMLGameManager, GameManager) {
	void returnToLastScene(GJGameLevel* level) {
		if (level && level == CustomLevels::g_playing) {
			CustomLevels::g_playing = nullptr;
			CCDirector::get()->replaceScene(
				CCTransitionFade::create(.5f, CustomLevelsLayer::scene(CustomLevels::g_returnIndex))
			);
			return;
		}
		GameManager::returnToLastScene(level);
	}
};

// ---------------------------------------------------------------------------
// My Levels -> level edit screen: small "+" button (position set in settings)
// ---------------------------------------------------------------------------
static CCPoint addButtonPosition(std::string const& pos, CCSize const& win) {
	if (pos == "Left of buttons") return { win.width / 2 - 190.f, win.height / 2 - 25.f };
	if (pos == "Top left") return { 65.f, win.height - 22.f };
	if (pos == "Top right") return { win.width - 65.f, win.height - 22.f };
	if (pos == "Bottom center") return { win.width / 2, 80.f };
	return { win.width / 2 + 190.f, win.height / 2 - 25.f }; // "Right of buttons"
}

class $modify(CMLEditLevelLayer, EditLevelLayer) {
	struct Fields {
		CCMenuItemSpriteExtra* addButton = nullptr;
	};

	bool init(GJGameLevel* level) {
		if (!EditLevelLayer::init(level)) return false;

		auto mod = Mod::get();

		// "Show button" off = showcase mode (use the hotkey instead)
		if (!mod->getSettingValue<bool>("show-add-button")) return true;

		// already in the custom list -> no button
		if (mod->getSettingValue<bool>("hide-if-added") && CustomLevels::contains(level)) return true;

		auto winSize = CCDirector::get()->getWinSize();

		auto menu = CCMenu::create();
		menu->setID("add-to-main-menu"_spr);
		menu->setPosition(0.f, 0.f);
		this->addChild(menu, 100);

		auto spr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
		spr->setScale(static_cast<float>(mod->getSettingValue<double>("button-scale")));
		auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(CMLEditLevelLayer::onAddToMain));
		btn->setID("add-to-main-button"_spr);
		btn->setPosition(addButtonPosition(mod->getSettingValue<std::string>("button-position"), winSize));
		menu->addChild(btn);

		m_fields->addButton = btn;
		return true;
	}

	void onAddToMain(CCObject*) {
		if (CustomLevels::tryAddFromEditor(m_level)) {
			if (m_fields->addButton && Mod::get()->getSettingValue<bool>("hide-if-added")) {
				m_fields->addButton->setVisible(false);
			}
		}
	}
};
