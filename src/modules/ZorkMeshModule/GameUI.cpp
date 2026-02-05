#include "configuration.h"

#ifdef T_DECK

#include "GameUI.h"
#include "GameEngine.h"
#include <Arduino.h>

// Global instance
GameUI* gameUI = nullptr;

// Screen dimensions (T-Deck is 320x240 in landscape)
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// Layout constants
#define STATUS_BAR_HEIGHT   24
#define INPUT_LINE_HEIGHT   28
#define OUTPUT_AREA_HEIGHT  (SCREEN_HEIGHT - STATUS_BAR_HEIGHT - INPUT_LINE_HEIGHT)

// Colors
#define COLOR_BG        lv_color_hex(0x000000)  // Black background
#define COLOR_TEXT      lv_color_hex(0x00FF00)  // Green text (classic terminal)
#define COLOR_STATUS_BG lv_color_hex(0x003300)  // Dark green status bar
#define COLOR_INPUT_BG  lv_color_hex(0x001100)  // Very dark green input

// Font - use built-in font (montserrat 16 is available)
#define GAME_FONT       &lv_font_montserrat_16

GameUI::GameUI()
    : screen(nullptr)
    , statusBar(nullptr)
    , roomLabel(nullptr)
    , scoreLabel(nullptr)
    , batteryLabel(nullptr)
    , outputArea(nullptr)
    , inputContainer(nullptr)
    , promptLabel(nullptr)
    , inputField(nullptr)
    , visible(false)
    , initialized(false)
    , cmdCallback(nullptr)
    , historyCount(0)
    , historyIndex(-1)
{
    memset(history, 0, sizeof(history));
}

GameUI::~GameUI()
{
    // Free history
    for (int i = 0; i < historyCount; i++) {
        if (history[i]) {
            free(history[i]);
            history[i] = nullptr;
        }
    }

    // LVGL objects are freed when screen is deleted
    if (screen) {
        lv_obj_del(screen);
        screen = nullptr;
    }
}

bool GameUI::init()
{
    if (initialized) {
        return true;
    }

    LOG_INFO("GameUI: Initializing...");

    createScreen();

    initialized = true;
    LOG_INFO("GameUI: Initialized successfully");
    return true;
}

void GameUI::createScreen()
{
    // Create a new screen
    screen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen, COLOR_BG, 0);

    createStatusBar();
    createOutputArea();
    createInputLine();
}

void GameUI::createStatusBar()
{
    // Status bar container
    statusBar = lv_obj_create(screen);
    lv_obj_set_size(statusBar, SCREEN_WIDTH, STATUS_BAR_HEIGHT);
    lv_obj_set_pos(statusBar, 0, 0);
    lv_obj_set_style_bg_color(statusBar, COLOR_STATUS_BG, 0);
    lv_obj_set_style_border_width(statusBar, 0, 0);
    lv_obj_set_style_pad_all(statusBar, 4, 0);
    lv_obj_set_flex_flow(statusBar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(statusBar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Room name (left)
    roomLabel = lv_label_create(statusBar);
    lv_label_set_text(roomLabel, "West of House");
    lv_obj_set_style_text_color(roomLabel, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(roomLabel, GAME_FONT, 0);

    // Score (center)
    scoreLabel = lv_label_create(statusBar);
    lv_label_set_text(scoreLabel, "Score: 0  Moves: 0");
    lv_obj_set_style_text_color(scoreLabel, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(scoreLabel, GAME_FONT, 0);

    // Battery (right)
    batteryLabel = lv_label_create(statusBar);
    lv_label_set_text(batteryLabel, "100%");
    lv_obj_set_style_text_color(batteryLabel, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(batteryLabel, GAME_FONT, 0);
}

void GameUI::createOutputArea()
{
    // Output text area (scrollable)
    outputArea = lv_textarea_create(screen);
    lv_obj_set_size(outputArea, SCREEN_WIDTH - 4, OUTPUT_AREA_HEIGHT);
    lv_obj_set_pos(outputArea, 2, STATUS_BAR_HEIGHT);
    lv_obj_set_style_bg_color(outputArea, COLOR_BG, 0);
    lv_obj_set_style_text_color(outputArea, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(outputArea, GAME_FONT, 0);
    lv_obj_set_style_border_width(outputArea, 0, 0);
    lv_obj_set_style_pad_all(outputArea, 4, 0);

    // Make it read-only (display only)
    lv_textarea_set_cursor_click_pos(outputArea, false);
    lv_obj_remove_flag(outputArea, LV_OBJ_FLAG_CLICKABLE);

    // Enable scrolling
    lv_obj_set_scroll_dir(outputArea, LV_DIR_VER);

    // Welcome message
    lv_textarea_set_text(outputArea,
        "=== ZorkMesh ===\n"
        "Multiplayer Zork over Meshtastic\n"
        "\n"
        "Type HELP for commands.\n"
        "Type LOOK to see your surroundings.\n"
        "\n");
}

void GameUI::createInputLine()
{
    // Input container
    inputContainer = lv_obj_create(screen);
    lv_obj_set_size(inputContainer, SCREEN_WIDTH, INPUT_LINE_HEIGHT);
    lv_obj_set_pos(inputContainer, 0, SCREEN_HEIGHT - INPUT_LINE_HEIGHT);
    lv_obj_set_style_bg_color(inputContainer, COLOR_INPUT_BG, 0);
    lv_obj_set_style_border_width(inputContainer, 1, 0);
    lv_obj_set_style_border_color(inputContainer, COLOR_TEXT, 0);
    lv_obj_set_style_pad_all(inputContainer, 2, 0);
    lv_obj_set_flex_flow(inputContainer, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(inputContainer, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Prompt ">"
    promptLabel = lv_label_create(inputContainer);
    lv_label_set_text(promptLabel, "> ");
    lv_obj_set_style_text_color(promptLabel, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(promptLabel, GAME_FONT, 0);

    // Text input field
    inputField = lv_textarea_create(inputContainer);
    lv_obj_set_flex_grow(inputField, 1);
    lv_obj_set_height(inputField, INPUT_LINE_HEIGHT - 8);
    lv_obj_set_style_bg_color(inputField, COLOR_INPUT_BG, 0);
    lv_obj_set_style_text_color(inputField, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(inputField, GAME_FONT, 0);
    lv_obj_set_style_border_width(inputField, 0, 0);
    lv_obj_set_style_pad_all(inputField, 2, 0);

    // Single line mode
    lv_textarea_set_one_line(inputField, true);
    lv_textarea_set_placeholder_text(inputField, "Enter command...");

    // Event handler for Enter key
    lv_obj_add_event_cb(inputField, inputEventCallback, LV_EVENT_READY, this);
}

void GameUI::inputEventCallback(lv_event_t* e)
{
    GameUI* ui = (GameUI*)lv_event_get_user_data(e);
    if (ui) {
        ui->submitCommand();
    }
}

void GameUI::show()
{
    if (!initialized) {
        init();
    }

    if (screen) {
        lv_screen_load(screen);
        visible = true;

        // Focus on input field
        lv_obj_add_state(inputField, LV_STATE_FOCUSED);

        LOG_INFO("GameUI: Screen shown");
    }
}

void GameUI::hide()
{
    visible = false;
    // Note: Switching back to main Meshtastic screen is handled by the caller
    LOG_INFO("GameUI: Screen hidden");
}

void GameUI::print(const char* text)
{
    if (!outputArea) return;

    // Append text to output area
    lv_textarea_add_text(outputArea, text);

    // Auto-scroll to bottom
    lv_textarea_set_cursor_pos(outputArea, LV_TEXTAREA_CURSOR_LAST);
}

void GameUI::println(const char* text)
{
    print(text);
    print("\n");
}

void GameUI::clear()
{
    if (outputArea) {
        lv_textarea_set_text(outputArea, "");
    }
}

void GameUI::setRoomName(const char* name)
{
    if (roomLabel) {
        lv_label_set_text(roomLabel, name);
    }
}

void GameUI::setScore(int score, int moves)
{
    if (scoreLabel) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Score: %d  Moves: %d", score, moves);
        lv_label_set_text(scoreLabel, buf);
    }
}

void GameUI::setBattery(int percent)
{
    if (batteryLabel) {
        char buf[8];
        snprintf(buf, sizeof(buf), "%d%%", percent);
        lv_label_set_text(batteryLabel, buf);
    }
}

void GameUI::submitCommand()
{
    if (!inputField) return;

    const char* text = lv_textarea_get_text(inputField);
    if (!text || strlen(text) == 0) return;

    // Add to history
    addToHistory(text);

    // Echo command to output
    print("> ");
    println(text);

    // Call command callback
    if (cmdCallback) {
        cmdCallback(text);
    }

    // Clear input field
    lv_textarea_set_text(inputField, "");

    // Reset history navigation
    historyIndex = historyCount;
}

void GameUI::onKeyPress(uint8_t key)
{
    if (!inputField || !visible) return;

    // Handle special keys
    switch (key) {
        case 0x0D: // Enter
            submitCommand();
            break;
        case 0x1B: // Escape
            // Could be used to exit game
            break;
        case 0x08: // Backspace
            lv_textarea_delete_char(inputField);
            break;
        case 0x00: // Up arrow (depends on keyboard mapping)
            historyUp();
            break;
        case 0x01: // Down arrow
            historyDown();
            break;
        default:
            // Regular character
            if (key >= 0x20 && key < 0x7F) {
                char str[2] = {(char)key, '\0'};
                lv_textarea_add_text(inputField, str);
            }
            break;
    }
}

void GameUI::addToHistory(const char* cmd)
{
    if (!cmd || strlen(cmd) == 0) return;

    // Don't add duplicates of the last command
    if (historyCount > 0 && strcmp(history[historyCount - 1], cmd) == 0) {
        return;
    }

    // Free oldest if full
    if (historyCount >= MAX_HISTORY) {
        free(history[0]);
        memmove(history, history + 1, (MAX_HISTORY - 1) * sizeof(char*));
        historyCount = MAX_HISTORY - 1;
    }

    // Add new command
    history[historyCount] = strdup(cmd);
    historyCount++;
    historyIndex = historyCount;
}

void GameUI::historyUp()
{
    if (historyCount == 0 || historyIndex <= 0) return;

    historyIndex--;
    if (history[historyIndex]) {
        lv_textarea_set_text(inputField, history[historyIndex]);
        lv_textarea_set_cursor_pos(inputField, LV_TEXTAREA_CURSOR_LAST);
    }
}

void GameUI::historyDown()
{
    if (historyIndex >= historyCount) return;

    historyIndex++;
    if (historyIndex >= historyCount) {
        lv_textarea_set_text(inputField, "");
    } else if (history[historyIndex]) {
        lv_textarea_set_text(inputField, history[historyIndex]);
        lv_textarea_set_cursor_pos(inputField, LV_TEXTAREA_CURSOR_LAST);
    }
}

#endif // T_DECK
