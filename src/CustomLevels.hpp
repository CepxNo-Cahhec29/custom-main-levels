#pragma once
#include <Geode/Geode.hpp>
#include <matjson.hpp>
#include <functional>

using namespace geode::prelude;

// One custom level stored in the mod's save file.
struct CustomLevelData {
	std::string name;
	std::string desc;
	std::string creator;
	std::string data;       // compressed level string, same format GD stores
	int audioTrack = 0;     // official song id
	int songID = 0;         // newgrounds song id
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
		});
	}
};

namespace CustomLevels {
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

	// Adds a level from the level edit screen and shows a popup.
	// Returns true only if it was newly added.
	inline bool tryAddFromEditor(GJGameLevel* level) {
		if (!level || std::string(level->m_levelString).empty()) {
			FLAlertLayer::create("Error", "This level has no data to add (is it empty?).", "OK")->show();
			return false;
		}

		CustomLevelData data;
		data.name = level->m_levelName;
		data.desc = level->m_levelDesc;
		data.creator = level->m_creatorName;
		data.data = level->m_levelString;
		data.audioTrack = level->m_audioTrack;
		data.songID = level->m_songID;

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

	// key used to remember which levels already gave their rewards
	inline std::string levelKey(GJGameLevel* level) {
		std::string name = level->m_levelName;
		std::string data = level->m_levelString;
		return fmt::format("{}:{}", name, std::hash<std::string>{}(data));
	}

	// The level currently being played from the custom levels screen,
	// and which entry to return to afterwards.
	inline GJGameLevel* g_playing = nullptr;
	inline int g_returnIndex = 0;

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
