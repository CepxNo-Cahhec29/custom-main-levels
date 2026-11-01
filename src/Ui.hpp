#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

// Sprite from a frame name; if the frame does not exist an empty sprite is
// returned (so a wrong name never shows the "missing texture" square).
inline CCSprite* makeFrame(char const* name) {
	if (name) {
		if (auto frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name)) {
			return CCSprite::createWithSpriteFrame(frame);
		}
	}
	return CCSprite::create();
}
