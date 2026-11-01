#pragma once
#include "CustomLevels.hpp"

// Screen that looks like the official Main Levels screen, but for custom levels.
class CustomLevelsLayer : public CCLayer {
protected:
	int m_index = 0;
	CCSprite* m_bg = nullptr;
	std::vector<CCSprite*> m_groundTiles;
	CCNode* m_content = nullptr;
	CCMenuItemSpriteExtra* m_prevBtn = nullptr;
	CCMenuItemSpriteExtra* m_nextBtn = nullptr;

	bool init(int index);
	void refresh();

	void onBack(CCObject*);
	void onPrev(CCObject*);
	void onNext(CCObject*);
	void onInfo(CCObject*);
	void onPlay(CCObject*);
	void keyBackClicked() override;

public:
	static CustomLevelsLayer* create(int index);
	static CCScene* scene(int index);
};
