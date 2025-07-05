
# Pac-Man Clone with Raylib

This is a simple Pac-Man clone developed in C using the [Raylib](https://www.raylib.com/) library.

## Features
- Classic Pac-Man gameplay with dots and ghosts
- Multiple levels loaded from text files
- Power-up system (turns Pac-Man white and increases score)
- Ghost AI using Manhattan distance to chase Pac-Man
- Pause functionality (`P` key)
- Game over and game win screens
- Score saving to `score.txt`
- Background music and sound effects
- Simple menu system

## Controls
- Arrow keys: Move Pac-Man
- `P`: Pause or resume the game
- `Enter`: Start game from menu
- `Esc`: Exit from menu
- `Space`: Return to menu after game over/win

## How to Compile (macOS/Linux)
Make sure Raylib is installed.

```
gcc -o pacman main.c -lraylib -lm -ldl -lpthread
```

Or use your existing `task.json` and `launch.json` in VS Code for one-click build/run.

## Folder Structure

```
project-folder/
├── assets/
│   ├── 415806__sunsai__desert-background-music.wav
│   ├── 334209__wasabiwielder__eating-crisps.wav
│   └── 76376__deleted_user_877451__game_over.wav
├── levels/
│   ├── level1.txt
│   ├── level2.txt
│   ├── level3.txt
│   └── level4.txt
├── score.txt
├── main.c
└── README.md
```

## Screenshots

### Menu Screen
![Menu Screenshot](assets/pacman%20menu.png)

### Gameplay Screen
![Gameplay Screenshot](assets/pacman%20gameplay.png)

## License
This is an educational project. Feel free to modify and share.

---
Created by Doğukan Vahit Bayrak
