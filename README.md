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
