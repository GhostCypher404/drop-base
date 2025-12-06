# DropBase — SDL3 + Clay Bootstrap (BangDev Edition)

DropBase is a tiny SDL3 + Clay starter template used inside BangDev projects as a "drop-in base app":

- Single-file build system using `nob.c`
- SDL3 window + renderer initialization
- SDL_ttf font loading
- SDL_image texture loading
- Clay UI integration (via SDL3 renderer backend)
- Carbonite Template example layout (sidebar + text area)

Use this as a starting point for small tools, prototypes and internal editors.

---

## Features

- **SDL3-based app loop** using `SDL_MAIN_USE_CALLBACKS`
- **Text rendering** via `SDL3_ttf`
- **Texture loading** via `SDL3_image`
- **Immediate-mode UI** with [Clay](https://github.com/nicbarker/clay)
- **Carbonite Template** example layout:
  - Sidebar with selectable documents
  - Scrollable main content area
- **Single-file build** with `nob.c` (no CMake required)
- Optional `CMakeLists.txt` for people who prefer CMake

---

## Dependencies

You need the following libraries installed on your system:

- `SDL3`
- `SDL3_ttf`
- `SDL3_image`
- `pkg-config` (for CMake flow, optional)

On macOS with Homebrew:

```sh
brew install sdl3 sdl3_ttf sdl3_image pkgconf


⸻

Building with nob (recommended)

From the project root:

cc nob.c -o nob
./nob build
./nob run

What this does:
	•	./nob build
Compiles main.c, the SDL3 renderer backend and the Carbonite Template into a dropbase binary.
	•	./nob run
Runs ./dropbase.

⸻

Building with CMake (alternative)

If you prefer CMake:

mkdir -p build
cd build
cmake ..
cmake --build .
./clay_examples_sdl3_simple_demo

Note: the CMake file was adapted from the original Clay examples and kept as an alternative path.
The canonical BangDev path is the nob.c single-file build.

⸻

Project Layout

drop_base/
├─ main.c                          # DropBase SDL3 + Clay bootstrap (BangDev style)
├─ nob.c                           # Single-file build system (nob)
├─ nob.h                           # nob header
├─ clay.h                          # Clay immediate-mode UI library
├─ renderers/
│  └─ SDL3/
│     └─ clay_renderer_SDL3.c      # Clay → SDL3 renderer bridge
├─ examples/
│  └─ shared-layouts/
│     └─ carbonite_template.c      # Carbonite Template layout (sidebar + text)
├─ resources/
│  ├─ Roboto-Regular.ttf           # Default UI font
│  └─ sample.png                   # Sample image used in the simple layout
├─ CMakeLists.txt                  # Optional CMake build
└─ .gitignore                      # Ignore build outputs and temp files


⸻

Usage
	•	Press Space to toggle between:
	•	Carbonite Template layout (text editor–style demo)
	•	Simple image layout (full-window texture with aspect ratio)

This repository is meant to be cloned, copied and modified as a starting point for:
	•	Small SDL tools
	•	Internal BangDev editors
	•	Prototyping UI experiments with Clay

⸻


