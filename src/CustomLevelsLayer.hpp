#pragma once
#include "CustomLevels.hpp"
#include "CustomLevelPage.hpp"

// Own screen that shows the custom levels one page at a time (with arrows).
class CustomLevelsLayer : public CCLayer {
protected:
	int m_index = 0;
	CCNode* m_content = nullptr;
	CCLabelBMFont* m_counter = nullptr;
	CCMenuItemSpriteExtra* m_prevBtn = nullptr;
	CCMenuItemSpriteExtra* m_nextBtn = nullptr;

	bool init(int index);
	void refresh();

	void onBack(CCObject*);
	void onPrev(CCObject*);
	void onNext(CCObject*);
	void keyBackClicked() override;

public:
	static CustomLevelsLayer* create(int index);
	static CCScene* scene(int index);
};
