// Rewards for completing a custom level (stars, orbs, coins, diamonds).
// Self-contained: if it ever fails to build, this file can be deleted.
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "CustomLevels.hpp"

using namespace geode::prelude;

class $modify(CMLPlayLayer, PlayLayer) {
	void levelComplete() {
		PlayLayer::levelComplete();

		if (!m_level || m_level != CustomLevels::g_playing) return;
		if (m_isPracticeMode) return;

		auto mod = Mod::get();
		int stars = static_cast<int>(mod->getSettingValue<int64_t>("reward-stars"));
		int orbs = static_cast<int>(mod->getSettingValue<int64_t>("reward-orbs"));
		int coins = static_cast<int>(mod->getSettingValue<int64_t>("reward-coins"));
		int diamonds = static_cast<int>(mod->getSettingValue<int64_t>("reward-diamonds"));
		if (stars + orbs + coins + diamonds <= 0) return;

		bool safeMode = mod->getSettingValue<bool>("safe-mode");

		if (!safeMode) {
			// only the first completion pays out (optional)
			if (mod->getSettingValue<bool>("reward-once")) {
				auto done = mod->getSavedValue<std::vector<std::string>>("rewarded-levels");
				auto key = CustomLevels::levelKey(m_level);
				if (std::find(done.begin(), done.end(), key) != done.end()) return;
				done.push_back(key);
				mod->setSavedValue("rewarded-levels", done);
			}

			// stat ids: 6 stars, 14 orbs, 8 secret coins, 13 diamonds
			auto gsm = GameStatsManager::sharedState();
			auto give = [&](char const* key, int amount) {
				if (amount > 0) gsm->setStat(key, gsm->getStat(key) + amount);
			};
			give("6", stars);
			give("14", orbs);
			give("8", coins);
			give("13", diamonds);
		}

		// shown in both modes (in safe mode nothing is actually saved)
		std::vector<std::string> parts;
		if (stars > 0) parts.push_back(fmt::format("+{} stars", stars));
		if (orbs > 0) parts.push_back(fmt::format("+{} orbs", orbs));
		if (coins > 0) parts.push_back(fmt::format("+{} coins", coins));
		if (diamonds > 0) parts.push_back(fmt::format("+{} diamonds", diamonds));

		std::string text;
		for (size_t i = 0; i < parts.size(); i++) {
			if (i > 0) text += ", ";
			text += parts[i];
		}
		Notification::create(text, NotificationIcon::Success, 3.f)->show();
	}
};
