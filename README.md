# anti-afk

Small Windows utility that keeps you from going AFK in games. It cycles the movement keys (`W`/`D`/`A`/`S`), taps `5`, and fires the right mouse button at randomized intervals so activity looks natural.

## Controls

- `F1` — start
- `F2` — stop

Run as administrator if the game window is elevated (otherwise UIPI will block the synthesized input).

## Build

Open `anti-afk.sln` in Visual Studio 2022 and build `Release | x64`.

Or from the command line:

    msbuild anti-afk.sln -p:Configuration=Release -p:Platform=x64

Output: `x64\Release\anti-afk.exe`. The Release build statically links the CRT, so the exe runs on a stock Windows install with no VC++ Redistributable.
