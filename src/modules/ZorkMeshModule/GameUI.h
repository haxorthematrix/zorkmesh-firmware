#pragma once

#include "configuration.h"

#if defined(HAS_TFT)

#include <lvgl.h>
#include <functional>

/**
 * GameUI - LVGL-based terminal interface for ZorkMesh
 *
 * Layout (320x240 landscape):
 * +----------------------------------+
 * | Status Bar (room, score, batt)   | 24px
 * +----------------------------------+
 * |                                  |
 * |     Terminal Output Area         | 188px
 * |     (scrollable text)            |
 * |                                  |
 * +----------------------------------+
 * | > command input                  | 28px
 * +----------------------------------+
 *
 * Settings Menu:
 * +----------------------------------+
 * |       ZorkMesh Settings          |
 * +----------------------------------+
 * | Player: [username]               |
 * | Score: X  Moves: Y  Lamp: Z      |
 * +----------------------------------+
 * | [Change Username]                |
 * | [Reset Location]                 |
 * | [Reset Inventory]                |
 * | [New Game]                       |
 * | [Back to Game]                   |
 * +----------------------------------+
 */

// Callback types
typedef std::function<void(const char* command)> CommandCallback;
typedef std::function<void(const char* username)> UsernameCallback;
typedef std::function<void(int action)> SettingsCallback;
typedef std::function<void()> SplashDoneCallback;

// Settings menu actions
enum SettingsAction {
    SETTINGS_CHANGE_USERNAME = 0,
    SETTINGS_RESET_LOCATION,
    SETTINGS_RESET_INVENTORY,
    SETTINGS_NEW_GAME,
    SETTINGS_BACK_TO_GAME,
    SETTINGS_EXIT_TO_MESHTASTIC
};

class GameUI {
public:
    GameUI();
    ~GameUI();

    // Initialize the game screen
    bool init();

    // Show/hide the game screen
    void show();
    void hide();
    bool isVisible() const { return visible; }

    // Terminal output
    void print(const char* text);
    void println(const char* text);
    void clear();

    // Status bar updates
    void setRoomName(const char* name);
    void setScore(int score, int moves);
    void setBattery(int percent);

    // Set callback for when user enters a command
    void setCommandCallback(CommandCallback callback) { cmdCallback = callback; }

    // Set callback for username entry
    void setUsernameCallback(UsernameCallback callback) { usernameCallback = callback; }

    // Set callback for settings actions
    void setSettingsCallback(SettingsCallback callback) { settingsCallback = callback; }

    // Set callback for when splash screen finishes
    void setSplashDoneCallback(SplashDoneCallback callback) { splashDoneCallback = callback; }

    // Process keyboard input (called from input handler)
    void onKeyPress(uint8_t key);

    // Get the LVGL screen object
    lv_obj_t* getScreen() const { return screen; }

    // Username prompt (for first run)
    void showUsernamePrompt(const char* currentName = nullptr);
    void hideUsernamePrompt();
    bool isUsernamePromptVisible() const { return usernamePromptVisible; }

    // Settings menu
    void showSettingsMenu(const char* username, int score, int moves, int lampLife);
    void hideSettingsMenu();
    bool isSettingsMenuVisible() const { return settingsMenuVisible; }

    // Update stats display in settings
    void updateSettingsStats(int score, int moves, int lampLife);

    // Splash screen
    void showSplashScreen();
    void hideSplashScreen();
    bool isSplashVisible() const { return splashVisible; }

private:
    // Screen and widgets
    lv_obj_t* screen;           // Main game screen
    lv_obj_t* statusBar;        // Top status bar
    lv_obj_t* backButton;       // Back button to exit game
    lv_obj_t* settingsButton;   // Settings button
    lv_obj_t* roomLabel;        // Room name label
    lv_obj_t* scoreLabel;       // Score display
    lv_obj_t* batteryLabel;     // Battery indicator
    lv_obj_t* outputArea;       // Scrollable text area for game output
    lv_obj_t* inputContainer;   // Container for input line
    lv_obj_t* promptLabel;      // ">" prompt
    lv_obj_t* inputField;       // Text input field

    // Username prompt widgets
    lv_obj_t* usernameDialog;       // Modal dialog for username
    lv_obj_t* usernameInput;        // Text input for username
    lv_obj_t* usernameOkButton;     // OK button
    bool usernamePromptVisible;

    // Settings menu widgets
    lv_obj_t* settingsScreen;       // Settings screen
    lv_obj_t* settingsPlayerLabel;  // Player name display
    lv_obj_t* settingsStatsLabel;   // Stats display
    bool settingsMenuVisible;

    // Splash screen widgets
    lv_obj_t* splashScreen;         // Splash screen
    lv_timer_t* splashTimer;        // Timer to auto-dismiss splash
    bool splashVisible;

    // State
    bool visible;
    bool initialized;
    CommandCallback cmdCallback;
    UsernameCallback usernameCallback;
    SettingsCallback settingsCallback;
    SplashDoneCallback splashDoneCallback;

    // Command history
    static const int MAX_HISTORY = 10;
    char* history[MAX_HISTORY];
    int historyCount;
    int historyIndex;

    // Output buffer
    static const int MAX_OUTPUT_LINES = 100;
    String outputBuffer;

    // Internal methods
    void createScreen();
    void createStatusBar();
    void createOutputArea();
    void createInputLine();
    void createUsernameDialog();
    void createSettingsScreen();
    void createSplashScreen();
    void submitCommand();
    void submitUsername();
    void historyUp();
    void historyDown();
    void addToHistory(const char* cmd);

    // Static callbacks for LVGL
    static void inputEventCallback(lv_event_t* e);
    static void usernameOkCallback(lv_event_t* e);
    static void settingsButtonCallback(lv_event_t* e);
    static void settingsMenuCallback(lv_event_t* e);
    static void splashTimerCallback(lv_timer_t* timer);
    static void deferredPrintCallback(lv_timer_t* timer);

    // Deferred print queue (for thread-safe LVGL updates)
    static const int MAX_DEFERRED_MSGS = 4;
    static const int MAX_DEFERRED_LEN = 80;
    char deferredMsgs[MAX_DEFERRED_MSGS][MAX_DEFERRED_LEN];
    int deferredMsgCount;
    lv_timer_t* deferredTimer;

public:
    // Queue a message for deferred printing (thread-safe)
    void printDeferred(const char* text);
};

// Global instance
extern GameUI* gameUI;

// Function to return to Meshtastic main screen (defined in ZorkMeshModule.cpp)
extern void zorkMeshReturnToMain();

#endif // HAS_TFT
