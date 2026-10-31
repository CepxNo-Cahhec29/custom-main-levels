# Custom Main Levels (Geode mod)

Adds your own levels to the official main levels list (the big **Play** button).

## Build it

Requirements: [Geode SDK + CLI](https://docs.geode-sdk.org/getting-started/), CMake, a C++ toolchain.

```bash
geode sdk install      # once
geode config setup     # once
cd custom-main-levels
geode build            # produces yourname.custom-main-levels.geode
```

Copy the generated `.geode` file into your `Geometry Dash/geode/mods` folder
(or install it through the Geode mods menu).

No local toolchain? Push this folder to a GitHub repo — `.github/workflows/build.yml`
builds for Windows, macOS, Android and iOS, and uploads a combined `.geode`.

## Before publishing
- Change `id` and `developer` in `mod.json` (e.g. `yourname.custom-main-levels`).
- Add a 336x336 (or similar) `logo.png` next to `mod.json`.
- Update `repository` in `mod.json`.

## How it works
- `LevelSelectLayer::init` is hooked: stored levels are appended to the `BoomScrollLayer` as extra pages.
- `EditLevelLayer::init` is hooked: adds an **Add to Main Levels** button that saves the level's
  name, description, creator, song and level string to the mod's save file.
- `LevelSelectLayer::colorForPage` is hooked: custom pages cycle through a color palette instead of reusing/overflowing RobTop's colors.
- `LevelSelectLayer::colorForPage` is hooked so custom pages get their own background colors.
- Each page is a `CustomLevelPage` with a play button (starts `PlayLayer`) and a remove button.

## Project layout
```
mod.json
CMakeLists.txt
about.md / changelog.md
src/main.cpp              hooks
src/CustomLevels.hpp      storage + GJGameLevel builder
src/CustomLevelPage.*     the page UI
.github/workflows/build.yml
```
