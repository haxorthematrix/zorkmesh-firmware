#include "configuration.h"

#if defined(HAS_TFT)

#include "GameUI.h"
#include "GameEngine.h"
#include "input/InputDriver.h"  // For device-ui input group
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

// Max output text size (prevents memory exhaustion)
#define MAX_OUTPUT_TEXT_LEN 4000

// Font - use built-in font (montserrat 16 is available)
#define GAME_FONT       &lv_font_montserrat_16

// Forward declaration of back button callback
static void backButtonCallback(lv_event_t* e);

GameUI::GameUI()
    : screen(nullptr)
    , statusBar(nullptr)
    , backButton(nullptr)
    , settingsButton(nullptr)
    , roomLabel(nullptr)
    , scoreLabel(nullptr)
    , batteryLabel(nullptr)
    , outputArea(nullptr)
    , inputContainer(nullptr)
    , promptLabel(nullptr)
    , inputField(nullptr)
    , usernameDialog(nullptr)
    , usernameInput(nullptr)
    , usernameOkButton(nullptr)
    , usernamePromptVisible(false)
    , settingsScreen(nullptr)
    , settingsPlayerLabel(nullptr)
    , settingsStatsLabel(nullptr)
    , settingsMenuVisible(false)
    , splashScreen(nullptr)
    , splashTimer(nullptr)
    , splashVisible(false)
    , inputGroup(nullptr)
    , visible(false)
    , initialized(false)
    , cmdCallback(nullptr)
    , usernameCallback(nullptr)
    , settingsCallback(nullptr)
    , splashDoneCallback(nullptr)
    , historyCount(0)
    , historyIndex(-1)
    , deferredMsgCount(0)
    , deferredTimer(nullptr)
{
    memset(history, 0, sizeof(history));
    memset(deferredMsgs, 0, sizeof(deferredMsgs));
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
    lv_obj_set_style_pad_all(statusBar, 2, 0);
    lv_obj_set_flex_flow(statusBar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(statusBar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Back button (leftmost) - styled like a button
    backButton = lv_button_create(statusBar);
    lv_obj_set_size(backButton, 28, 20);
    lv_obj_set_style_bg_color(backButton, lv_color_hex(0x005500), 0);
    lv_obj_set_style_radius(backButton, 4, 0);
    lv_obj_set_style_border_width(backButton, 1, 0);
    lv_obj_set_style_border_color(backButton, COLOR_TEXT, 0);
    lv_obj_set_style_pad_all(backButton, 2, 0);

    lv_obj_t* backLabel = lv_label_create(backButton);
    lv_label_set_text(backLabel, "X");  // Simple X to exit
    lv_obj_center(backLabel);
    lv_obj_set_style_text_color(backLabel, COLOR_TEXT, 0);

    // Add click handler for back button
    lv_obj_add_event_cb(backButton, backButtonCallback, LV_EVENT_CLICKED, nullptr);

    // Settings button (gear icon or "S")
    settingsButton = lv_button_create(statusBar);
    lv_obj_set_size(settingsButton, 28, 20);
    lv_obj_set_style_bg_color(settingsButton, lv_color_hex(0x005500), 0);
    lv_obj_set_style_radius(settingsButton, 4, 0);
    lv_obj_set_style_border_width(settingsButton, 1, 0);
    lv_obj_set_style_border_color(settingsButton, COLOR_TEXT, 0);
    lv_obj_set_style_pad_all(settingsButton, 2, 0);

    lv_obj_t* settingsLabel = lv_label_create(settingsButton);
    lv_label_set_text(settingsLabel, "S");  // S for Settings
    lv_obj_center(settingsLabel);
    lv_obj_set_style_text_color(settingsLabel, COLOR_TEXT, 0);

    // Add click handler for settings button
    lv_obj_add_event_cb(settingsButton, settingsButtonCallback, LV_EVENT_CLICKED, this);

    // Room name
    roomLabel = lv_label_create(statusBar);
    lv_label_set_text(roomLabel, "West of House");
    lv_obj_set_style_text_color(roomLabel, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(roomLabel, GAME_FONT, 0);

    // Score (center)
    scoreLabel = lv_label_create(statusBar);
    lv_label_set_text(scoreLabel, "0/0");  // Shorter format for space
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

    // Make it read-only but allow scrolling
    lv_textarea_set_cursor_click_pos(outputArea, false);

    // Enable scrolling
    lv_obj_add_flag(outputArea, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(outputArea, LV_SCROLLBAR_MODE_AUTO);

    // Prevent output area from receiving focus (critical for keyboard input)
    lv_obj_remove_flag(outputArea, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_set_style_anim_duration(outputArea, 0, LV_PART_CURSOR);

    // Hide the cursor completely
    lv_textarea_set_cursor_click_pos(outputArea, false);
    lv_obj_set_style_opa(outputArea, LV_OPA_TRANSP, LV_PART_CURSOR);

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

    // Make input field focusable and give it initial focus
    lv_obj_add_flag(inputField, LV_OBJ_FLAG_CLICK_FOCUSABLE);
    lv_obj_add_state(inputField, LV_STATE_FOCUSED);
}

void GameUI::inputEventCallback(lv_event_t* e)
{
    GameUI* ui = (GameUI*)lv_event_get_user_data(e);
    if (ui) {
        ui->submitCommand();
    }
}

void GameUI::outputScrollCallback(lv_event_t* e)
{
    GameUI* ui = (GameUI*)lv_event_get_user_data(e);
    if (ui && ui->inputField) {
        // Refocus input field after scrolling output area
        lv_obj_add_state(ui->inputField, LV_STATE_FOCUSED);
    }
}

// Back button callback - returns to Meshtastic
static void backButtonCallback(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        LOG_INFO("GameUI: Back button clicked");
        if (gameUI) {
            gameUI->hide();
        }
        zorkMeshReturnToMain();
    }
}

void GameUI::show()
{
    if (!initialized) {
        init();
    }

    visible = true;

    // Show splash screen first (will auto-transition to game screen)
    showSplashScreen();

    LOG_INFO("GameUI: Screen shown (splash first)");
}

void GameUI::hide()
{
    visible = false;
    // Note: Switching back to main Meshtastic screen is handled by the caller
    LOG_INFO("GameUI: Screen hidden");
}

void GameUI::ensureInputFocus()
{
    // Only act if main game screen is active (not splash, settings, or username prompt)
    if (!visible || splashVisible || settingsMenuVisible || usernamePromptVisible) {
        return;
    }

    if (inputField) {
        lv_group_t* group = InputDriver::getInputGroup();
        if (group) {
            lv_group_focus_obj(inputField);
        }
        lv_obj_add_state(inputField, LV_STATE_FOCUSED);
    }
}

// Static buffer for text accumulation (avoid malloc)
static char s_textBuf[MAX_OUTPUT_TEXT_LEN + 128];

void GameUI::print(const char* text)
{
    if (!outputArea) return;
    if (!text || text[0] == '\0') return;

    // Get current text
    const char* current = lv_textarea_get_text(outputArea);
    size_t currentLen = current ? strlen(current) : 0;
    size_t newLen = strlen(text);

    // Build new text in static buffer (avoid lv_textarea_add_text which may cause issues)
    if (currentLen + newLen >= MAX_OUTPUT_TEXT_LEN) {
        // Too long - start fresh with just the new text
        snprintf(s_textBuf, sizeof(s_textBuf), "[...]\n%s", text);
    } else {
        // Append to existing
        if (currentLen > 0) {
            memcpy(s_textBuf, current, currentLen);
        }
        memcpy(s_textBuf + currentLen, text, newLen);
        s_textBuf[currentLen + newLen] = '\0';
    }

    // Set full text at once (safer than incremental add)
    lv_textarea_set_text(outputArea, s_textBuf);

    // Scroll to bottom to show latest text
    lv_obj_scroll_to_y(outputArea, LV_COORD_MAX, LV_ANIM_OFF);

    // Keep focus on input field (not the output area)
    if (inputField) {
        lv_obj_add_state(inputField, LV_STATE_FOCUSED);
    }
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
        char buf[16];
        snprintf(buf, sizeof(buf), "%d/%d", score, moves);  // Compact format: score/moves
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
    if (!visible) return;

    // If splash screen is visible, any key dismisses it
    if (splashVisible) {
        hideSplashScreen();
        // hideSplashScreen already handles focus activation
        return;
    }

    if (!inputField) return;

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
            lv_obj_invalidate(inputField);  // Force redraw
            break;
        case 0x00: // Up arrow (custom code from ZorkMeshModule)
            historyUp();
            break;
        case 0x01: // Down arrow (custom code from ZorkMeshModule)
            historyDown();
            break;
        case 0x02: // Page Up
            if (outputArea) {
                lv_obj_scroll_by(outputArea, 0, 80, LV_ANIM_OFF);
                // Keep focus on input field
                lv_obj_add_state(inputField, LV_STATE_FOCUSED);
            }
            break;
        case 0x03: // Page Down
            if (outputArea) {
                lv_obj_scroll_by(outputArea, 0, -80, LV_ANIM_OFF);
                // Keep focus on input field
                lv_obj_add_state(inputField, LV_STATE_FOCUSED);
            }
            break;
        default:
            // Regular character
            if (key >= 0x20 && key < 0x7F) {
                // Get current text and append the new character
                const char* current = lv_textarea_get_text(inputField);
                size_t len = current ? strlen(current) : 0;
                if (len < 78) {  // Leave room for char + null
                    static char inputBuf[80];
                    if (current && len > 0) {
                        memcpy(inputBuf, current, len);
                    }
                    inputBuf[len] = (char)key;
                    inputBuf[len + 1] = '\0';
                    lv_textarea_set_text(inputField, inputBuf);
                    lv_textarea_set_cursor_pos(inputField, LV_TEXTAREA_CURSOR_LAST);
                    lv_obj_invalidate(inputField);  // Force redraw
                }
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
        lv_obj_invalidate(inputField);  // Force redraw
    }
}

void GameUI::historyDown()
{
    if (historyIndex >= historyCount) return;

    historyIndex++;
    if (historyIndex >= historyCount) {
        lv_textarea_set_text(inputField, "");
        lv_obj_invalidate(inputField);  // Force redraw
    } else if (history[historyIndex]) {
        lv_textarea_set_text(inputField, history[historyIndex]);
        lv_textarea_set_cursor_pos(inputField, LV_TEXTAREA_CURSOR_LAST);
        lv_obj_invalidate(inputField);  // Force redraw
    }
}

// ============ Settings Button Callback ============

void GameUI::settingsButtonCallback(lv_event_t* e)
{
    GameUI* ui = (GameUI*)lv_event_get_user_data(e);
    if (ui) {
        // Get current stats from game engine
        extern GameEngine* gameEngine;
        if (gameEngine) {
            ui->showSettingsMenu(
                gameEngine->getPlayerName(),
                gameEngine->getScore(),
                gameEngine->getMoves(),
                gameEngine->getLampLife()
            );
        }
    }
}

// ============ Username Dialog ============

void GameUI::createUsernameDialog()
{
    if (usernameDialog) return;  // Already created

    // Create modal dialog
    usernameDialog = lv_obj_create(screen);
    lv_obj_set_size(usernameDialog, 280, 140);
    lv_obj_center(usernameDialog);
    lv_obj_set_style_bg_color(usernameDialog, lv_color_hex(0x002200), 0);
    lv_obj_set_style_border_color(usernameDialog, COLOR_TEXT, 0);
    lv_obj_set_style_border_width(usernameDialog, 2, 0);
    lv_obj_set_style_radius(usernameDialog, 8, 0);
    lv_obj_set_style_pad_all(usernameDialog, 10, 0);

    // Title
    lv_obj_t* title = lv_label_create(usernameDialog);
    lv_label_set_text(title, "Enter Your Name");
    lv_obj_set_style_text_color(title, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(title, GAME_FONT, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);

    // Input field
    usernameInput = lv_textarea_create(usernameDialog);
    lv_obj_set_size(usernameInput, 240, 36);
    lv_obj_align(usernameInput, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(usernameInput, lv_color_hex(0x001100), 0);
    lv_obj_set_style_text_color(usernameInput, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(usernameInput, GAME_FONT, 0);
    lv_obj_set_style_border_color(usernameInput, COLOR_TEXT, 0);
    lv_textarea_set_one_line(usernameInput, true);
    lv_textarea_set_max_length(usernameInput, 20);
    lv_textarea_set_placeholder_text(usernameInput, "Adventurer");

    // OK button
    usernameOkButton = lv_button_create(usernameDialog);
    lv_obj_set_size(usernameOkButton, 80, 30);
    lv_obj_align(usernameOkButton, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_set_style_bg_color(usernameOkButton, lv_color_hex(0x005500), 0);
    lv_obj_set_style_radius(usernameOkButton, 4, 0);

    lv_obj_t* okLabel = lv_label_create(usernameOkButton);
    lv_label_set_text(okLabel, "OK");
    lv_obj_center(okLabel);
    lv_obj_set_style_text_color(okLabel, COLOR_TEXT, 0);

    lv_obj_add_event_cb(usernameOkButton, usernameOkCallback, LV_EVENT_CLICKED, this);

    // Hide by default
    lv_obj_add_flag(usernameDialog, LV_OBJ_FLAG_HIDDEN);
}

void GameUI::showUsernamePrompt(const char* currentName)
{
    if (!usernameDialog) {
        createUsernameDialog();
    }

    if (currentName && strlen(currentName) > 0) {
        lv_textarea_set_text(usernameInput, currentName);
    } else {
        lv_textarea_set_text(usernameInput, "");
    }

    lv_obj_remove_flag(usernameDialog, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_state(usernameInput, LV_STATE_FOCUSED);
    usernamePromptVisible = true;
}

void GameUI::hideUsernamePrompt()
{
    if (usernameDialog) {
        lv_obj_add_flag(usernameDialog, LV_OBJ_FLAG_HIDDEN);
    }
    usernamePromptVisible = false;
}

void GameUI::submitUsername()
{
    if (!usernameInput) return;

    const char* name = lv_textarea_get_text(usernameInput);
    if (!name || strlen(name) == 0) {
        name = "Adventurer";  // Default name
    }

    hideUsernamePrompt();

    if (usernameCallback) {
        usernameCallback(name);
    }
}

void GameUI::usernameOkCallback(lv_event_t* e)
{
    GameUI* ui = (GameUI*)lv_event_get_user_data(e);
    if (ui) {
        ui->submitUsername();
    }
}

// ============ Settings Screen ============

void GameUI::createSettingsScreen()
{
    if (settingsScreen) return;  // Already created

    // Create settings screen
    settingsScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(settingsScreen, COLOR_BG, 0);

    // Title bar
    lv_obj_t* titleBar = lv_obj_create(settingsScreen);
    lv_obj_set_size(titleBar, SCREEN_WIDTH, 30);
    lv_obj_set_pos(titleBar, 0, 0);
    lv_obj_set_style_bg_color(titleBar, COLOR_STATUS_BG, 0);
    lv_obj_set_style_border_width(titleBar, 0, 0);

    lv_obj_t* title = lv_label_create(titleBar);
    lv_label_set_text(title, "ZorkMesh Settings");
    lv_obj_center(title);
    lv_obj_set_style_text_color(title, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(title, GAME_FONT, 0);

    // Stats area
    lv_obj_t* statsArea = lv_obj_create(settingsScreen);
    lv_obj_set_size(statsArea, SCREEN_WIDTH - 20, 50);
    lv_obj_set_pos(statsArea, 10, 35);
    lv_obj_set_style_bg_color(statsArea, lv_color_hex(0x001100), 0);
    lv_obj_set_style_border_color(statsArea, COLOR_TEXT, 0);
    lv_obj_set_style_border_width(statsArea, 1, 0);
    lv_obj_set_style_radius(statsArea, 4, 0);
    lv_obj_set_style_pad_all(statsArea, 5, 0);

    settingsPlayerLabel = lv_label_create(statsArea);
    lv_label_set_text(settingsPlayerLabel, "Player: Unknown");
    lv_obj_set_style_text_color(settingsPlayerLabel, COLOR_TEXT, 0);
    lv_obj_align(settingsPlayerLabel, LV_ALIGN_TOP_LEFT, 0, 0);

    settingsStatsLabel = lv_label_create(statsArea);
    lv_label_set_text(settingsStatsLabel, "Score: 0  Moves: 0  Lamp: 350");
    lv_obj_set_style_text_color(settingsStatsLabel, COLOR_TEXT, 0);
    lv_obj_align(settingsStatsLabel, LV_ALIGN_BOTTOM_LEFT, 0, 0);

    // Menu buttons
    const char* buttonLabels[] = {
        "Change Username",
        "Reset Location",
        "Reset Inventory",
        "New Game",
        "Back to Game",
        "Exit to Meshtastic"
    };

    for (int i = 0; i < 6; i++) {
        lv_obj_t* btn = lv_button_create(settingsScreen);
        lv_obj_set_size(btn, SCREEN_WIDTH - 40, 26);
        lv_obj_set_pos(btn, 20, 90 + i * 28);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x003300), 0);
        lv_obj_set_style_radius(btn, 4, 0);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_border_color(btn, COLOR_TEXT, 0);

        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, buttonLabels[i]);
        lv_obj_center(label);
        lv_obj_set_style_text_color(label, COLOR_TEXT, 0);

        // Store action index in user data
        lv_obj_set_user_data(btn, (void*)(intptr_t)i);
        lv_obj_add_event_cb(btn, settingsMenuCallback, LV_EVENT_CLICKED, this);
    }
}

void GameUI::showSettingsMenu(const char* username, int score, int moves, int lampLife)
{
    if (!settingsScreen) {
        createSettingsScreen();
    }

    // Update labels
    char buf[64];
    snprintf(buf, sizeof(buf), "Player: %s", username ? username : "Unknown");
    lv_label_set_text(settingsPlayerLabel, buf);

    snprintf(buf, sizeof(buf), "Score: %d  Moves: %d  Lamp: %d", score, moves, lampLife);
    lv_label_set_text(settingsStatsLabel, buf);

    lv_screen_load(settingsScreen);
    settingsMenuVisible = true;
}

void GameUI::hideSettingsMenu()
{
    if (screen) {
        lv_screen_load(screen);
    }
    settingsMenuVisible = false;
}

void GameUI::updateSettingsStats(int score, int moves, int lampLife)
{
    if (settingsStatsLabel) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Score: %d  Moves: %d  Lamp: %d", score, moves, lampLife);
        lv_label_set_text(settingsStatsLabel, buf);
    }
}

void GameUI::settingsMenuCallback(lv_event_t* e)
{
    GameUI* ui = (GameUI*)lv_event_get_user_data(e);
    lv_obj_t* btn = (lv_obj_t*)lv_event_get_target(e);
    int action = (int)(intptr_t)lv_obj_get_user_data(btn);

    if (!ui) return;

    switch (action) {
        case SETTINGS_CHANGE_USERNAME:
            ui->hideSettingsMenu();
            ui->showUsernamePrompt(nullptr);  // Will get current name from callback
            break;

        case SETTINGS_BACK_TO_GAME:
            ui->hideSettingsMenu();
            break;

        case SETTINGS_EXIT_TO_MESHTASTIC:
            ui->hideSettingsMenu();
            ui->hide();
            zorkMeshReturnToMain();
            break;

        default:
            // Other actions handled by callback
            if (ui->settingsCallback) {
                ui->settingsCallback(action);
            }
            ui->hideSettingsMenu();
            break;
    }
}

// ============ Splash Screen ============

void GameUI::createSplashScreen()
{
    if (splashScreen) return;  // Already created

    splashScreen = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(splashScreen, COLOR_BG, 0);

    // Create container for centered content
    lv_obj_t* container = lv_obj_create(splashScreen);
    lv_obj_set_size(container, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_center(container);
    lv_obj_set_style_bg_color(container, COLOR_BG, 0);
    lv_obj_set_style_border_width(container, 0, 0);
    lv_obj_set_style_pad_all(container, 0, 0);

    // Layout: Lantern on left, text on right
    // +----------------------------------+
    // |              ZORK                |
    // |    [*]       MESH                |
    // |   /###\   Multiplayer Adventure  |
    // |  |#####|   over Meshtastic       |
    // |   \###/                          |
    // |         Press any key to start   |
    // +----------------------------------+

    // Main title "ZORK" at top center
    lv_obj_t* titleLabel = lv_label_create(container);
    lv_label_set_text(titleLabel, "Z O R K");
    lv_obj_set_style_text_color(titleLabel, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(titleLabel, GAME_FONT, 0);
    lv_obj_align(titleLabel, LV_ALIGN_TOP_MID, 0, 20);

    // Lantern icon on left side (simplified)
    lv_obj_t* lanternLabel = lv_label_create(container);
    lv_label_set_text(lanternLabel,
        "  _\n"
        " |*|\n"
        "/###\\\n"
        "|###|\n"
        "\\###/\n"
        "  |\n"
        " '-'"
    );
    lv_obj_set_style_text_color(lanternLabel, lv_color_hex(0xFFAA00), 0);  // Lantern orange/yellow
    lv_obj_set_style_text_font(lanternLabel, GAME_FONT, 0);
    lv_obj_align(lanternLabel, LV_ALIGN_LEFT_MID, 40, 0);

    // "MESH" subtitle on right
    lv_obj_t* subtitleLabel = lv_label_create(container);
    lv_label_set_text(subtitleLabel, "M E S H");
    lv_obj_set_style_text_color(subtitleLabel, lv_color_hex(0x67ea94), 0);  // Meshtastic green
    lv_obj_set_style_text_font(subtitleLabel, GAME_FONT, 0);
    lv_obj_align(subtitleLabel, LV_ALIGN_CENTER, 40, -20);

    // Tagline on right
    lv_obj_t* taglineLabel = lv_label_create(container);
    lv_label_set_text(taglineLabel, "Multiplayer Adventure\nover Meshtastic");
    lv_obj_set_style_text_color(taglineLabel, COLOR_TEXT, 0);
    lv_obj_set_style_text_font(taglineLabel, GAME_FONT, 0);
    lv_obj_set_style_text_align(taglineLabel, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(taglineLabel, LV_ALIGN_CENTER, 40, 25);

    // Version info at bottom center
    lv_obj_t* versionLabel = lv_label_create(container);
    lv_label_set_text(versionLabel, "v1.0 - Press any key");
    lv_obj_set_style_text_color(versionLabel, lv_color_hex(0x888888), 0);
    lv_obj_align(versionLabel, LV_ALIGN_BOTTOM_MID, 0, -15);
}

void GameUI::showSplashScreen()
{
    if (!splashScreen) {
        createSplashScreen();
    }

    lv_screen_load(splashScreen);
    splashVisible = true;

    // Auto-dismiss after 3 seconds
    if (splashTimer) {
        lv_timer_delete(splashTimer);
    }
    splashTimer = lv_timer_create(splashTimerCallback, 3000, this);
    lv_timer_set_repeat_count(splashTimer, 1);

    LOG_INFO("GameUI: Splash screen shown");
}

void GameUI::hideSplashScreen()
{
    if (splashTimer) {
        lv_timer_delete(splashTimer);
        splashTimer = nullptr;
    }

    splashVisible = false;

    // Load the main game screen
    if (screen) {
        lv_screen_load(screen);
    }

    // Add input field to device-ui's input group and focus it
    if (inputField) {
        lv_group_t* group = InputDriver::getInputGroup();
        if (group) {
            // Add to group if not already added
            lv_group_add_obj(group, inputField);
            // Focus this object in the group (makes keyboard input work)
            lv_group_focus_obj(inputField);
            LOG_INFO("GameUI: Input field added to device-ui input group");
        }
        lv_obj_add_state(inputField, LV_STATE_FOCUSED);
    }

    LOG_INFO("GameUI: Splash screen hidden");

    // Call completion callback (triggers username prompt or game start)
    if (splashDoneCallback) {
        splashDoneCallback();
    }
}

void GameUI::splashTimerCallback(lv_timer_t* timer)
{
    GameUI* ui = (GameUI*)lv_timer_get_user_data(timer);
    if (ui) {
        ui->hideSplashScreen();
    }
}

// ============ Deferred Print (Thread-safe LVGL updates) ============

void GameUI::printDeferred(const char* text)
{
    if (!text || text[0] == '\0') return;

    // Add to deferred queue (thread-safe - just writes to array)
    if (deferredMsgCount < MAX_DEFERRED_MSGS) {
        strncpy(deferredMsgs[deferredMsgCount], text, MAX_DEFERRED_LEN - 1);
        deferredMsgs[deferredMsgCount][MAX_DEFERRED_LEN - 1] = '\0';
        deferredMsgCount++;

        LOG_INFO("GameUI: Queued deferred msg %d: %s", deferredMsgCount, text);

        // Start timer if not running (timer runs in LVGL context)
        if (!deferredTimer) {
            deferredTimer = lv_timer_create(deferredPrintCallback, 10, this);
            lv_timer_set_repeat_count(deferredTimer, -1);  // Repeat until stopped
            LOG_INFO("GameUI: Started deferred print timer");
        }
    } else {
        LOG_WARN("GameUI: Deferred queue full, dropping message");
    }
}

void GameUI::deferredPrintCallback(lv_timer_t* timer)
{
    GameUI* ui = (GameUI*)lv_timer_get_user_data(timer);
    if (!ui) return;

    // Process one message per callback (runs in LVGL context - safe!)
    if (ui->deferredMsgCount > 0) {
        const char* msg = ui->deferredMsgs[0];
        LOG_INFO("GameUI: Deferred print: %s", msg);

        // Actually print (now we're in LVGL context)
        ui->println(msg);

        // Shift queue down
        for (int i = 0; i < ui->deferredMsgCount - 1; i++) {
            memcpy(ui->deferredMsgs[i], ui->deferredMsgs[i + 1], MAX_DEFERRED_LEN);
        }
        ui->deferredMsgCount--;
    }

    // Stop timer when queue is empty
    if (ui->deferredMsgCount == 0) {
        if (ui->deferredTimer) {
            lv_timer_delete(ui->deferredTimer);
            ui->deferredTimer = nullptr;
            LOG_INFO("GameUI: Stopped deferred print timer");
        }
    }
}

#endif // HAS_TFT
