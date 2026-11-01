#pragma once
#include "CustomLevels.hpp"

// Screen opened by the + button: choose look and rewards, then add the level.
class LevelSetupLayer : public CCLayer {
protected:
	GJGameLevel* m_level = nullptr;
	CCSprite* m_bg = nullptr;
	CCNode* m_preview = nullptr;
	ButtonSprite* m_diffSpr = nullptr;
	ButtonSprite* m_featSpr = nullptr;
	ButtonSprite* m_colorSpr = nullptr;
	ButtonSprite* m_coinsSpr = nullptr;
	TextInput* m_stars = nullptr;
	TextInput* m_orbs = nullptr;
	TextInput* m_diamonds = nullptr;
	int m_difficulty = 1;
	int m_feature = 0;
	int m_color = 0;
	int m_coins = 0;

	bool init(GJGameLevel* level);
	void refreshControls();

	void onDifficulty(CCObject*);
	void onFeature(CCObject*);
	void onColor(CCObject*);
	void onCoins(CCObject*);
	void onAdd(CCObject*);
	void onCancel(CCObject*);
	void keyBackClicked() override;

public:
	~LevelSetupLayer() override;
	static LevelSetupLayer* create(GJGameLevel* level);
	static CCScene* scene(GJGameLevel* level);
};
