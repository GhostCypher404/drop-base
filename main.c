// ============================================================
// DropBase — SDL3 + Clay demo bootstrap
// BangDev Edition — comments in English only
// ------------------------------------------------------------
// Responsibilities:
//  - Initialize SDL3 window + renderer
//  - Initialize SDL_ttf and SDL_image bindings
//  - Wire Clay UI system to SDL3 backend
//  - Toggle between Carbonite layout and simple image layout
//  - Provide a clean template for future DropBase tools
// ============================================================

#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"

#include <stdio.h>
#include <stdbool.h>

#include "renderers/SDL3/clay_renderer_SDL3.c"
#include "examples/shared-layouts/carbonite_template.c"  // ← our generic UI

// ------------------------------------------------------------
// Constants & basic configuration
// ------------------------------------------------------------

static const Uint32 FONT_ID         = 0;
static const Uint32 CLAY_FONT_COUNT = 1;

static const int   INITIAL_WINDOW_WIDTH  = 640;
static const int   INITIAL_WINDOW_HEIGHT = 480;
static const char *WINDOW_TITLE          = "DropBase — Carbonite Template";

// Palette (reserved for future BangDev UI styling)
static const Clay_Color COLOR_ORANGE = (Clay_Color){225, 138, 50, 255};
static const Clay_Color COLOR_BLUE   = (Clay_Color){111, 173, 162, 255};
static const Clay_Color COLOR_LIGHT  = (Clay_Color){224, 215, 210, 255};

// ------------------------------------------------------------
// Application state
// ------------------------------------------------------------

typedef struct app_state {
    SDL_Window            *window;
    Clay_SDL3RendererData  rendererData;
    CarboniteTemplate_Data carbonite;    // ← we use Carbonite, not ClayVideoDemo
} AppState;

static SDL_Texture *sample_image = NULL;
static bool         show_demo    = true;

// ------------------------------------------------------------
// Clay text measurement callback
// ------------------------------------------------------------
// Adapts SDL_ttf to Clay's text measurement API.
// Clay calls this to know how big a given text will be.
// ------------------------------------------------------------

static inline Clay_Dimensions SDL_MeasureText(Clay_StringSlice text,
                                              Clay_TextElementConfig *config,
                                              void *userData)
{
    TTF_Font **fonts = (TTF_Font **)userData;
    TTF_Font  *font  = fonts[config->fontId];
    int width  = 0;
    int height = 0;

    TTF_SetFontSize(font, config->fontSize);
    if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to measure text: %s", SDL_GetError());
    }

    return (Clay_Dimensions){ (float)width, (float)height };
}

// ------------------------------------------------------------
// Clay error handler
// ------------------------------------------------------------

static void HandleClayErrors(Clay_ErrorData errorData)
{
    // For now just print to stdout.
    // Later we can route to a BangDev debug overlay or log file.
    printf("%s", errorData.errorText.chars);
}

// ------------------------------------------------------------
// Simple Clay layout that shows a single SDL_Texture
// ------------------------------------------------------------

static Clay_RenderCommandArray ClayImageSample_CreateLayout(void)
{
    Clay_BeginLayout();

    Clay_Sizing layoutExpand = {
        .width  = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };

    CLAY(CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing          = layoutExpand,
            .padding         = CLAY_PADDING_ALL(16),
            .childGap        = 16
        }
    }) {
        CLAY(CLAY_ID("SampleImage"), {
            .layout = {
                .sizing = layoutExpand
            },
            .aspectRatio = { 23.0f / 42.0f },
            .image = {
                .imageData = sample_image,
            }
        });
    }

    return Clay_EndLayout();
}

// ------------------------------------------------------------
// SDL_AppInit — application initialization
// ------------------------------------------------------------

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // Initialize SDL_ttf
    if (!TTF_Init()) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "TTF_Init failed: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // NOTE:
    // For SDL3_image, explicit IMG_Init() is optional for many formats.
    // We keep it simple and rely on SDL_image lazy initialization here.

    // Allocate main application state
    AppState *state = SDL_calloc(1, sizeof(AppState));
    if (!state) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate AppState");
        return SDL_APP_FAILURE;
    }
    *appstate = state;

    // Create window + renderer
    if (!SDL_CreateWindowAndRenderer(
            WINDOW_TITLE,
            INITIAL_WINDOW_WIDTH,
            INITIAL_WINDOW_HEIGHT,
            0,
            &state->window,
            &state->rendererData.renderer))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to create window and renderer: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_SetWindowResizable(state->window, true);

    // Create text engine bound to this renderer
    state->rendererData.textEngine =
        TTF_CreateRendererTextEngine(state->rendererData.renderer);
    if (!state->rendererData.textEngine) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to create text engine from renderer: %s",
                     SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Allocate Clay font array (for now only 1 font)
    state->rendererData.fonts =
        SDL_calloc(CLAY_FONT_COUNT, sizeof(TTF_Font *));
    if (!state->rendererData.fonts) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to allocate memory for the font array");
        return SDL_APP_FAILURE;
    }

    // Load font from resources
    TTF_Font *font = TTF_OpenFont("resources/Roboto-Regular.ttf", 24);
    if (!font) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to load font: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    state->rendererData.fonts[FONT_ID] = font;

    // Load sample image as SDL_Texture
    sample_image =
        IMG_LoadTexture(state->rendererData.renderer, "resources/sample.png");
    if (!sample_image) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Failed to load image: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // --------------------------------------------------------
    // Initialize Clay
    // --------------------------------------------------------
    uint64_t totalMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = (Clay_Arena){
        .memory   = SDL_malloc(totalMemorySize),
        .capacity = totalMemorySize
    };

    int width  = 0;
    int height = 0;
    SDL_GetWindowSize(state->window, &width, &height);

    Clay_Initialize(
        clayMemory,
        (Clay_Dimensions){ (float)width, (float)height },
        (Clay_ErrorHandler){ HandleClayErrors }
    );

    Clay_SetMeasureTextFunction(SDL_MeasureText, state->rendererData.fonts);

    // Initialize Carbonite template data (our neutral layout)
    state->carbonite = CarboniteTemplate_Initialize();

    return SDL_APP_CONTINUE;
}

// ------------------------------------------------------------
// SDL_AppEvent — event processing
// ------------------------------------------------------------

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    (void)appstate;
    SDL_AppResult ret_val = SDL_APP_CONTINUE;

    switch (event->type) {
    case SDL_EVENT_QUIT:
        ret_val = SDL_APP_SUCCESS;
        break;

    case SDL_EVENT_KEY_UP:
        // Space toggles between Carbonite layout and simple image layout
        if (event->key.scancode == SDL_SCANCODE_SPACE) {
            show_demo = !show_demo;
        }
        break;

    case SDL_EVENT_WINDOW_RESIZED:
        // Notify Clay about the new window size
        Clay_SetLayoutDimensions(
            (Clay_Dimensions){
                (float)event->window.data1,
                (float)event->window.data2
            }
        );
        break;

    case SDL_EVENT_MOUSE_MOTION:
        Clay_SetPointerState(
            (Clay_Vector2){ event->motion.x, event->motion.y },
            (event->motion.state & SDL_BUTTON_LMASK) != 0
        );
        break;

    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        Clay_SetPointerState(
            (Clay_Vector2){ event->button.x, event->button.y },
            event->button.button == SDL_BUTTON_LEFT
        );
        break;

    case SDL_EVENT_MOUSE_WHEEL:
        Clay_UpdateScrollContainers(
            true,
            (Clay_Vector2){ event->wheel.x, event->wheel.y },
            0.01f
        );
        break;

    default:
        break;
    }

    return ret_val;
}

// ------------------------------------------------------------
// SDL_AppIterate — per-frame update + render
// ------------------------------------------------------------

SDL_AppResult SDL_AppIterate(void *appstate)
{
    AppState *state = (AppState *)appstate;

    // Decide which UI layout to render this frame
    Clay_RenderCommandArray render_commands =
        (show_demo
            ? CarboniteTemplate_CreateLayout(&state->carbonite)
            : ClayImageSample_CreateLayout());

    // Clear the screen
    SDL_SetRenderDrawColor(state->rendererData.renderer, 0, 0, 0, 255);
    SDL_RenderClear(state->rendererData.renderer);

    // Let Clay issue all draw commands into SDL3
    SDL_Clay_RenderClayCommands(&state->rendererData, &render_commands);

    // Present on screen
    SDL_RenderPresent(state->rendererData.renderer);

    return SDL_APP_CONTINUE;
}

// ------------------------------------------------------------
// SDL_AppQuit — cleanup
// ------------------------------------------------------------

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    if (result != SDL_APP_SUCCESS) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,
                     "Application failed to run");
    }

    AppState *state = (AppState *)appstate;

    if (sample_image) {
        SDL_DestroyTexture(sample_image);
        sample_image = NULL;
    }

    if (state) {
        if (state->rendererData.renderer) {
            SDL_DestroyRenderer(state->rendererData.renderer);
        }

        if (state->window) {
            SDL_DestroyWindow(state->window);
        }

        if (state->rendererData.fonts) {
            for (Uint32 i = 0; i < CLAY_FONT_COUNT; i++) {
                if (state->rendererData.fonts[i]) {
                    TTF_CloseFont(state->rendererData.fonts[i]);
                }
            }
            SDL_free(state->rendererData.fonts);
        }

        if (state->rendererData.textEngine) {
            TTF_DestroyRendererTextEngine(state->rendererData.textEngine);
        }

        SDL_free(state);
    }

    // Global SDL_ttf shutdown
    TTF_Quit();
}
