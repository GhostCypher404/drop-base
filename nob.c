// ============================================================
// nob.c — BangDev DROP_BASE Build System
// ------------------------------------------------------------
// build:  cc nob.c -o nob && ./nob build
// run:    ./nob run
// ============================================================

#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv) {

    // --------------------------------------------------------
    // BUILD: compile DropBase SDL3 + Clay demo
    // --------------------------------------------------------
    if (argc > 1 && strcmp(argv[1], "build") == 0) {
        Nob_Cmd cmd = {0};
        nob_cmd_append(&cmd, "cc");

        // Translation units
        nob_cmd_append(&cmd, "main.c");
        nob_cmd_append(&cmd, "renderers/SDL3/clay_renderer_SDL3.c");
        // NOTE: clay-video-demo.c is included from main.c, so we do NOT
        // compile it as a separate TU here.

        // Includes
        nob_cmd_append(&cmd, "-I.");
        nob_cmd_append(&cmd, "-Irenderers/SDL3");
        nob_cmd_append(&cmd, "-Iexamples/shared-layouts");

        // Homebrew SDL3 paths (arm64 macOS /opt/homebrew)
        nob_cmd_append(&cmd, "-I/opt/homebrew/include");
        nob_cmd_append(&cmd, "-L/opt/homebrew/lib");

        // SDL3 libs
        nob_cmd_append(&cmd, "-lSDL3");
        nob_cmd_append(&cmd, "-lSDL3_ttf");
        nob_cmd_append(&cmd, "-lSDL3_image");

        // Output
        nob_cmd_append(&cmd, "-o");
        nob_cmd_append(&cmd, "dropbase");

        printf("[INFO] 🔥 BUILDING DROP_BASE...\n");
        if (!nob_cmd_run_sync(cmd)) {
            fprintf(stderr, "[ERROR] Build failed.\n");
            return 1;
        }
        printf("[INFO] ✅ Build OK. Run: ./nob run\n");
        return 0;
    }

    // --------------------------------------------------------
    // RUN: execute the built binary
    // --------------------------------------------------------
    if (argc > 1 && strcmp(argv[1], "run") == 0) {
        if (!nob_cmd_run_sync(
                (Nob_Cmd){ .items = (char*[]){"./dropbase", NULL}, .count = 1 }))
        {
            fprintf(stderr,
                    "[ERROR] Could not run ./dropbase (did you build first?).\n");
            return 1;
        }
        return 0;
    }

    // --------------------------------------------------------
    // Help
    // --------------------------------------------------------
    printf("DropBase Build System\n");
    printf("  ./nob build  -> compile\n");
    printf("  ./nob run    -> execute\n\n");
    return 0;
}
