#pragma once
#include "CustomLevels.hpp"

// One "card" showing a custom level (name, play and remove buttons).
class CustomLevelPage : public CCLayer {
protected:
	CustomLevelData m_data;
	size_t m_index = 0;

	bool init(CustomLevelData const& data, size_t index);

	void onPlay(CCObject*);
	void onRemove(CCObject*);

public:
	static CustomLevelPage* create(CustomLevelData const& data, size_t index);
};
