// Hotkey to add the open level to the custom list (for showcases, with the
// button hidden). Self-contained: if it ever fails to build, delete this file.
#include <Geode/Geode.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include "CustomLevels.hpp"

using namespace geode::prelude;

static enumKeyCodes hotkeyFromSettings() {
	auto s = Mod::get()->getSettingValue<std::string>("add-hotkey");
	if (s.size() >= 2 && s[0] == 'F') {
		int n = std::atoi(s.c_str() + 1);
		if (n >= 1 && n <= 12) return static_cast<enumKeyCodes>(111 + n); // F1 = 112
	}
	return KEY_Unknown;
}

class $modify(CMLKeyboardDispatcher, CCKeyboardDispatcher) {
	bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool isRepeat, double timestamp) {
		if (down && !isRepeat && key != KEY_Unknown && key == hotkeyFromSettings()) {
			if (auto scene = CCDirector::get()->getRunningScene()) {
				if (auto layer = scene->getChildByType<EditLevelLayer>(0)) {
					CustomLevels::tryAddFromEditor(layer->m_level);
				}
			}
		}
		return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, isRepeat, timestamp);
	}
};
