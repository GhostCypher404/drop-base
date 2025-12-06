// ============================================================
// Carbonite Template — BangDev UI bootstrap layout
// ------------------------------------------------------------
// Purpose:
//  - Provide a generic "first app" layout:
//      * Header bar with buttons
//      * Sidebar with selectable items
//      * Scrollable main content area
//  - Serve as a reusable template for BangDev tools
//  - No project-specific lore: only generic placeholder content
// ============================================================

#include "../../clay.h"
#include <stdlib.h>

// NOTE: keep these internal to avoid symbol collisions
static const int FONT_ID_BODY_16 = 0;
static Clay_Color COLOR_WHITE = { 255, 255, 255, 255 };

// ------------------------------------------------------------
// Small helpers for header and dropdown items
// ------------------------------------------------------------

static void Carbonite_RenderHeaderButton(Clay_String text) {
    CLAY_AUTO_ID({
        .layout = { .padding = { 16, 16, 8, 8 }},
        .backgroundColor = { 140, 140, 140, 255 },
        .cornerRadius = CLAY_CORNER_RADIUS(5)
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

static void Carbonite_RenderDropdownMenuItem(Clay_String text) {
    CLAY_AUTO_ID({
        .layout = { .padding = CLAY_PADDING_ALL(16) }
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = FONT_ID_BODY_16,
            .fontSize = 16,
            .textColor = { 255, 255, 255, 255 }
        }));
    }
}

// ------------------------------------------------------------
// Generic "document" model for sidebar + content
// ------------------------------------------------------------

typedef struct {
    Clay_String title;
    Clay_String contents;
} Carbonite_Document;

typedef struct {
    Carbonite_Document *documents;
    uint32_t length;
} Carbonite_DocumentArray;

// Static storage for a small list of documents
static Carbonite_Document carboniteDocumentsRaw[4];

static Carbonite_DocumentArray carboniteDocuments = {
    .length = 4,
    .documents = carboniteDocumentsRaw
};

// ------------------------------------------------------------
// Small frame arena used to store per-frame UI helper data
// ------------------------------------------------------------

typedef struct {
    intptr_t offset;
    intptr_t memory;
} Carbonite_Arena;

typedef struct {
    int32_t selectedDocumentIndex;
    float yOffset;
    Carbonite_Arena frameArena;
} CarboniteTemplate_Data;

typedef struct {
    int32_t requestedDocumentIndex;
    int32_t *selectedDocumentIndex;
} Carbonite_SidebarClickData;

// ------------------------------------------------------------
// Sidebar interaction callback
// ------------------------------------------------------------

static void Carbonite_HandleSidebarInteraction(
    Clay_ElementId elementId,
    Clay_PointerData pointerData,
    void *userData
) {
    (void) elementId;
    Carbonite_SidebarClickData *clickData = (Carbonite_SidebarClickData *)userData;

    if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        if (clickData->requestedDocumentIndex >= 0 &&
            clickData->requestedDocumentIndex < (int)carboniteDocuments.length)
        {
            *clickData->selectedDocumentIndex = clickData->requestedDocumentIndex;
        }
    }
}

// ------------------------------------------------------------
// Initialization
// ------------------------------------------------------------

CarboniteTemplate_Data CarboniteTemplate_Initialize(void) {
    // Generic, neutral demo content — safe starting point for tools
    carboniteDocuments.documents[0] = (Carbonite_Document){
        .title    = CLAY_STRING("Welcome"),
        .contents = CLAY_STRING(
            "Welcome to the Carbonite Template.\n\n"
            "- Header bar with buttons\n"
            "- Sidebar with selectable items\n"
            "- Scrollable main content area\n\n"
            "Use this as a neutral starting point for BangDev tools."
        )
    };

    carboniteDocuments.documents[1] = (Carbonite_Document){
        .title    = CLAY_STRING("Notes"),
        .contents = CLAY_STRING(
            "This panel can be used as a notes view, log viewer,\n"
            "or any text-centric panel you want to prototype.\n\n"
            "You can replace this text with live data later."
        )
    };

    carboniteDocuments.documents[2] = (Carbonite_Document){
        .title    = CLAY_STRING("Inspector"),
        .contents = CLAY_STRING(
            "Imagine this as an 'Inspector' panel showing properties\n"
            "of the selected entity, asset, or node from your tool.\n\n"
            "The structure (title + body) stays the same."
        )
    };

    carboniteDocuments.documents[3] = (Carbonite_Document){
        .title    = CLAY_STRING("About"),
        .contents = CLAY_STRING(
            "Carbonite Template v0.1\n"
            "BangDev UI bootstrap for desktop tools.\n\n"
            "Header + sidebar + scrollable view.\n"
            "Clay handles layout, SDL3 handles rendering."
        )
    };

    CarboniteTemplate_Data data = {
        .selectedDocumentIndex = 0,
        .yOffset = 0.0f,
        .frameArena = {
            .memory = (intptr_t)malloc(1024), // simple scratch buffer
            .offset = 0
        }
    };

    return data;
}

// ------------------------------------------------------------
// Layout creation
// ------------------------------------------------------------

Clay_RenderCommandArray CarboniteTemplate_CreateLayout(CarboniteTemplate_Data *data)
{
    data->frameArena.offset = 0;

    Clay_BeginLayout();

    Clay_Sizing layoutExpand = {
        .width  = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };

    Clay_Color contentBackgroundColor = (Clay_Color){ 90, 90, 90, 255 };

    // Outer frame
    CLAY(CLAY_ID("OuterContainer"), {
        .backgroundColor = { 43, 41, 51, 255 },
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing          = layoutExpand,
            .padding         = CLAY_PADDING_ALL(16),
            .childGap        = 16
        }
    }) {
        // ----------------------------------------------------
        // Header bar
        // ----------------------------------------------------
        CLAY(CLAY_ID("HeaderBar"), {
            .layout = {
                .sizing = {
                    .height = CLAY_SIZING_FIXED(60),
                    .width  = CLAY_SIZING_GROW(0)
                },
                .padding = { 16, 16, 0, 0 },
                .childGap = 16,
                .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
            },
            .backgroundColor = contentBackgroundColor,
            .cornerRadius    = CLAY_CORNER_RADIUS(8)
        }) {
            // File button + dropdown
            CLAY(CLAY_ID("FileButton"), {
                .layout = { .padding = { 16, 16, 8, 8 }},
                .backgroundColor = (Clay_Color){ 140, 140, 140, 255 },
                .cornerRadius    = CLAY_CORNER_RADIUS(5)
            }) {
                CLAY_TEXT(CLAY_STRING("File"), CLAY_TEXT_CONFIG({
                    .fontId   = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = (Clay_Color){ 255, 255, 255, 255 }
                }));

                bool fileMenuVisible =
                    Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FileButton"))) ||
                    Clay_PointerOver(Clay_GetElementId(CLAY_STRING("FileMenu")));

                if (fileMenuVisible) {
                    CLAY(CLAY_ID("FileMenu"), {
                        .floating = {
                            .attachTo     = CLAY_ATTACH_TO_PARENT,
                            .attachPoints = {
                                .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM
                            },
                        },
                        .layout = {
                            .padding = { 0, 0, 8, 8 }
                        }
                    }) {
                        CLAY_AUTO_ID({
                            .layout = {
                                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                .sizing = {
                                    .width = CLAY_SIZING_FIXED(200)
                                },
                            },
                            .backgroundColor = (Clay_Color){ 40, 40, 40, 255 },
                            .cornerRadius    = CLAY_CORNER_RADIUS(8)
                        }) {
                            Carbonite_RenderDropdownMenuItem(CLAY_STRING("New"));
                            Carbonite_RenderDropdownMenuItem(CLAY_STRING("Open"));
                            Carbonite_RenderDropdownMenuItem(CLAY_STRING("Close"));
                        }
                    }
                }
            }

            Carbonite_RenderHeaderButton(CLAY_STRING("Edit"));
            CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_GROW(0) }}}) {}
            Carbonite_RenderHeaderButton(CLAY_STRING("View"));
            Carbonite_RenderHeaderButton(CLAY_STRING("Tools"));
            Carbonite_RenderHeaderButton(CLAY_STRING("Help"));
        }

        // ----------------------------------------------------
        // Lower content (sidebar + main content)
        // ----------------------------------------------------
        CLAY(CLAY_ID("LowerContent"), {
            .layout = {
                .sizing   = layoutExpand,
                .childGap = 16
            }
        }) {
            // Sidebar
            CLAY(CLAY_ID("Sidebar"), {
                .backgroundColor = contentBackgroundColor,
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .padding         = CLAY_PADDING_ALL(16),
                    .childGap        = 8,
                    .sizing = {
                        .width  = CLAY_SIZING_FIXED(250),
                        .height = CLAY_SIZING_GROW(0)
                    }
                }
            }) {
                for (int i = 0; i < (int)carboniteDocuments.length; i++) {
                    Carbonite_Document document = carboniteDocuments.documents[i];

                    Clay_LayoutConfig sidebarButtonLayout = {
                        .sizing  = { .width = CLAY_SIZING_GROW(0) },
                        .padding = CLAY_PADDING_ALL(16)
                    };

                    if (i == data->selectedDocumentIndex) {
                        CLAY_AUTO_ID({
                            .layout         = sidebarButtonLayout,
                            .backgroundColor = (Clay_Color){ 120, 120, 120, 255 },
                            .cornerRadius    = CLAY_CORNER_RADIUS(8)
                        }) {
                            CLAY_TEXT(document.title, CLAY_TEXT_CONFIG({
                                .fontId   = FONT_ID_BODY_16,
                                .fontSize = 20,
                                .textColor = (Clay_Color){ 255, 255, 255, 255 }
                            }));
                        }
                    } else {
                        Carbonite_SidebarClickData *clickData =
                            (Carbonite_SidebarClickData *)(data->frameArena.memory + data->frameArena.offset);

                        *clickData = (Carbonite_SidebarClickData){
                            .requestedDocumentIndex = i,
                            .selectedDocumentIndex  = &data->selectedDocumentIndex
                        };

                        data->frameArena.offset += (intptr_t)sizeof(Carbonite_SidebarClickData);

                        CLAY_AUTO_ID({
                            .layout = sidebarButtonLayout,
                            .backgroundColor = (Clay_Color){
                                120, 120, 120,
                                Clay_Hovered() ? 120 : 0
                            },
                            .cornerRadius = CLAY_CORNER_RADIUS(8)
                        }) {
                            Clay_OnHover(Carbonite_HandleSidebarInteraction, clickData);
                            CLAY_TEXT(document.title, CLAY_TEXT_CONFIG({
                                .fontId   = FONT_ID_BODY_16,
                                .fontSize = 20,
                                .textColor = (Clay_Color){ 255, 255, 255, 255 }
                            }));
                        }
                    }
                }
            }

            // Main content
            CLAY(CLAY_ID("MainContent"), {
                .backgroundColor = contentBackgroundColor,
                .clip = {
                    .vertical    = true,
                    .childOffset = Clay_GetScrollOffset()
                },
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childGap        = 16,
                    .padding         = CLAY_PADDING_ALL(16),
                    .sizing          = layoutExpand
                }
            }) {
                Carbonite_Document selectedDocument =
                    carboniteDocuments.documents[data->selectedDocumentIndex];

                CLAY_TEXT(selectedDocument.title, CLAY_TEXT_CONFIG({
                    .fontId   = FONT_ID_BODY_16,
                    .fontSize = 24,
                    .textColor = COLOR_WHITE
                }));

                CLAY_TEXT(selectedDocument.contents, CLAY_TEXT_CONFIG({
                    .fontId   = FONT_ID_BODY_16,
                    .fontSize = 18,
                    .textColor = COLOR_WHITE
                }));
            }
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    // Optional vertical offset tweak
    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommand *cmd = Clay_RenderCommandArray_Get(&renderCommands, i);
        cmd->boundingBox.y += data->yOffset;
    }

    return renderCommands;
}
