#pragma once
#include <Geode/Geode.hpp>
#include <matjson.hpp>
#include <functional>
#include <algorithm>

using namespace geode::prelude;

// One custom level stored in the mod's save file.
struct CustomLevelData {
	std::string name;
	std::string desc;
	std::string creator;
	std::string data;       // compressed level string, same format GD stores
	int audioTrack = 0;     // official song id
	int songID = 0;         // newgrounds song id
	// look & rewards (set in the + setup screen)
	int color = 0;          // background color preset
	int difficulty = 1;     // difficulty face preset
	int feature = 0;        // 0 none, 1 featured, 2 epic, 3 legendary, 4 mythic
	int stars = 0;
	int orbs = 0;
	int diamonds = 0;
	int coins = 0;          // 0-3 gold coins
};

template <>
struct matjson::Serialize<CustomLevelData> {
	static geode::Result<CustomLevelData> fromJson(matjson::Value const& v) {
		CustomLevelData d;
		d.name = v["name"].asString().unwrapOr("Unnamed");
		d.desc = v["desc"].asString().unwrapOr("");
		d.creator = v["creator"].asString().unwrapOr("");
		d.data = v["data"].asString().unwrapOr("");
		d.audioTrack = static_cast<int>(v["audioTrack"].asInt().unwrapOr(0));
		d.songID = static_cast<int>(v["songID"].asInt().unwrapOr(0));
		d.color = static_cast<int>(v["color"].asInt().unwrapOr(0));
		d.difficulty = static_cast<int>(v["difficulty"].asInt().unwrapOr(1));
		d.feature = static_cast<int>(v["feature"].asInt().unwrapOr(0));
		d.stars = static_cast<int>(v["stars"].asInt().unwrapOr(0));
		d.orbs = static_cast<int>(v["orbs"].asInt().unwrapOr(0));
		d.diamonds = static_cast<int>(v["diamonds"].asInt().unwrapOr(0));
		d.coins = static_cast<int>(v["coins"].asInt().unwrapOr(0));
		return geode::Ok(d);
	}

	static matjson::Value toJson(CustomLevelData const& d) {
		return matjson::makeObject({
			{"name", d.name},
			{"desc", d.desc},
			{"creator", d.creator},
			{"data", d.data},
			{"audioTrack", d.audioTrack},
			{"songID", d.songID},
			{"color", d.color},
			{"difficulty", d.difficulty},
			{"feature", d.feature},
			{"stars", d.stars},
			{"orbs", d.orbs},
			{"diamonds", d.diamonds},
			{"coins", d.coins},
		});
	}
};

namespace CustomLevels {
	// ---------------- option tables ----------------
	struct DiffOption { char const* name; char const* frame; };
	inline std::vector<DiffOption> const& difficulties() {
		static const std::vector<DiffOption> v = {
			{ "Auto", "diffIcon_auto_btn_001.png" },
			{ "Easy", "diffIcon_01_btn_001.png" },
			{ "Normal", "diffIcon_02_btn_001.png" },
			{ "Hard", "diffIcon_03_btn_001.png" },
			{ "Harder", "diffIcon_04_btn_001.png" },
			{ "Insane", "diffIcon_05_btn_001.png" },
			{ "Easy Demon", "diffIcon_07_btn_001.png" },
			{ "Medium Demon", "diffIcon_08_btn_001.png" },
			{ "Hard Demon", "diffIcon_06_btn_001.png" },
			{ "Insane Demon", "diffIcon_09_btn_001.png" },
			{ "Extreme Demon", "diffIcon_10_btn_001.png" },
			{ "N/A", "diffIcon_00_btn_001.png" },
		};
		return v;
	}

	struct FeatureOption { char const* name; char const* frame; };
	inline std::vector<FeatureOption> const& features() {
		static const std::vector<FeatureOption> v = {
			{ "None", nullptr },
			{ "Featured", "GJ_featuredCoin_001.png" },
			{ "Epic", "GJ_epicCoin_001.png" },
			{ "Legendary", "GJ_epicCoin2_001.png" },
			{ "Mythic", "GJ_epicCoin3_001.png" },
		};
		return v;
	}

	struct ColorOption { char const* name; ccColor3B color; };
	inline std::vector<ColorOption> const& colors() {
		static const std::vector<ColorOption> v = {
			{ "Blue", { 40, 125, 255 } },
			{ "Green", { 125, 255, 0 } },
			{ "Orange", { 255, 125, 0 } },
			{ "Pink", { 255, 0, 200 } },
			{ "Cyan", { 0, 255, 255 } },
			{ "Purple", { 175, 80, 255 } },
			{ "Yellow", { 255, 255, 0 } },
			{ "Red", { 255, 60, 60 } },
			{ "Gray", { 150, 150, 150 } },
		};
		return v;
	}

	template <class T>
	inline int wrapIndex(int i, std::vector<T> const& v) {
		int n = static_cast<int>(v.size());
		if (n == 0) return 0;
		return ((i % n) + n) % n;
	}

	// ---------------- storage ----------------
	inline std::vector<CustomLevelData> load() {
		return Mod::get()->getSavedValue<std::vector<CustomLevelData>>("custom-levels");
	}

	inline void save(std::vector<CustomLevelData> const& levels) {
		Mod::get()->setSavedValue("custom-levels", levels);
	}

	// returns false if an identical level (same name + data) is already added
	inline bool add(CustomLevelData const& level) {
		auto levels = load();
		for (auto const& l : levels) {
			if (l.name == level.name && l.data == level.data) return false;
		}
		levels.push_back(level);
		save(levels);
		return true;
	}

	inline void remove(size_t index) {
		auto levels = load();
		if (index < levels.size()) {
			levels.erase(levels.begin() + index);
			save(levels);
		}
	}

	// true if this level (name + data) is already in the custom list
	inline bool contains(GJGameLevel* level) {
		if (!level) return false;
		std::string name = level->m_levelName;
		std::string data = level->m_levelString;
		for (auto const& l : load()) {
			if (l.name == name && l.data == data) return true;
		}
		return false;
	}

	inline int settingInt(char const* key) {
		return static_cast<int>(Mod::get()->getSettingValue<int64_t>(key));
	}

	// Data for a level using the default values from the mod settings.
	inline CustomLevelData dataFromLevel(GJGameLevel* level) {
		CustomLevelData data;
		data.name = level->m_levelName;
		data.desc = level->m_levelDesc;
		data.creator = level->m_creatorName;
		data.data = level->m_levelString;
		data.audioTrack = level->m_audioTrack;
		data.songID = level->m_songID;
		data.stars = settingInt("reward-stars");
		data.orbs = settingInt("reward-orbs");
		data.coins = settingInt("reward-coins");
		data.diamonds = settingInt("reward-diamonds");
		return data;
	}

	// Quick add (hotkey): uses the defaults and shows a popup.
	inline bool tryAddFromEditor(GJGameLevel* level) {
		if (!level || std::string(level->m_levelString).empty()) {
			FLAlertLayer::create("Error", "This level has no data to add (is it empty?).", "OK")->show();
			return false;
		}
		auto data = dataFromLevel(level);
		if (add(data)) {
			FLAlertLayer::create(
				"Added!",
				fmt::format("<cy>{}</c> was added to your custom levels.\nOpen <cg>Play</c> in the main menu and press <cg>Custom Levels</c>.", data.name),
				"OK"
			)->show();
			return true;
		}
		FLAlertLayer::create("Already added", "This level is already in the custom levels list.", "OK")->show();
		return false;
	}

	// ---------------- progress (best %) ----------------
	inline std::string levelKey(std::string const& name, std::string const& data) {
		return fmt::format("{}:{}", name, std::hash<std::string>{}(data));
	}

	inline std::string progressKey(CustomLevelData const& d, bool practice) {
		return fmt::format("best:{}:{}", levelKey(d.name, d.data), practice ? "p" : "n");
	}

	inline int bestPercent(CustomLevelData const& d, bool practice) {
		return Mod::get()->getSavedValue<int>(progressKey(d, practice));
	}

	inline void recordPercent(CustomLevelData const& d, bool practice, int percent) {
		percent = std::clamp(percent, 0, 100);
		if (percent > bestPercent(d, practice)) {
			Mod::get()->setSavedValue(progressKey(d, practice), percent);
		}
	}

	// ---------------- playing ----------------
	// The level currently being played from the custom levels screen,
	// and which entry to return to afterwards.
	inline GJGameLevel* g_playing = nullptr;
	inline int g_returnIndex = 0;

	inline bool currentEntry(CustomLevelData& out) {
		auto levels = load();
		if (g_returnIndex < 0 || g_returnIndex >= static_cast<int>(levels.size())) return false;
		out = levels[g_returnIndex];
		return true;
	}

	// Builds a playable GJGameLevel from stored data.
	// Uses the "editor" level type (2) with id 0, like playing a level from My Levels,
	// so no main-level progress or stats can be touched by the game itself.
	inline GJGameLevel* makeLevel(CustomLevelData const& d) {
		auto level = GJGameLevel::create();
		level->m_levelName = d.name;
		level->m_levelDesc = d.desc;
		level->m_creatorName = d.creator;
		level->m_levelString = d.data;
		level->m_audioTrack = d.audioTrack;
		level->m_songID = d.songID;
		level->m_levelType = static_cast<GJLevelType>(2);
		return level;
	}
}
