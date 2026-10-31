# v1.0.1
- Custom pages now have their own cycling background colors (hook on `LevelSelectLayer::colorForPage`).

# v1.0.0
- Initial release: add levels from My Levels into the main levels list.

# v1.0.1
- Custom pages now have their own background colors (colorForPage hook).

# v1.0.2
- Fix CI: use the bindings that match the Geode SDK version; target GD 2.2074.

# v1.0.3
- Target GD 2.2081: CI builds against the nightly Geode SDK + matching bindings (main).

# v1.0.4
- Match the current Geode SDK (5.11.2) and GD 2.2091. Removed iOS from CI.

# v1.0.5
- Set the target GD version to 2.2081 (the version the game reports with Geode 5.11.2).

# v1.0.6
- GD version back to 2.2091: Geode SDK 5.11.2 requires it at build time.

# v1.0.7
- CI pinned to the released Geode SDK v5.11.2 (matches the installed loader, GD 2.2081) instead of nightly.

# v1.0.8
- Add to Main Levels button moved to the free gap under the three big buttons; added a log line for debugging.

# v1.0.9
- Fix compile errors: GJLevelType::Local does not exist in these bindings (use value 1), removed stray m_firstCustomPage line.

# v1.1.0
- Fix crash when opening Main Levels: the official page list is no longer modified.
- New "Custom Levels" button on the Main Levels screen opens a dedicated screen (arrows, play, remove).

# v1.1.1
- "Add to Main Levels" is now a small + icon button next to the big buttons.

# v1.2.0
- Settings: button position and size, hide button when level already added, hotkey (F1-F12) for showcases.
- Settings: custom rewards (stars, orbs, coins, diamonds) and a safe mode (default on) that only shows them.
