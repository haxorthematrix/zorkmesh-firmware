#pragma once

#include "configuration.h"

#ifdef T_DECK

#include <lvgl.h>
#include <functional>

/**
 * GameUI - LVGL-based terminal interface for ZorkMesh
 *
 * Layout (320x240 landscape):
 * +----------------------------------+
 * | Status Bar (room, score, batt)   | 20px
 * +----------------------------------+
 * |                                  |
 * |     Terminal Output Area         | 190px
 * |     (scrollable text)            |
 * |                                  |
 * +----------------------------------+
 * | > command input                  | 30px
 * +----------------------------------+
 */

// Callback type for command input
typedef std::function<void(const char* command)> CommandCallback;

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

    // Process keyboard input (called from input handler)
    void onKeyPress(uint8_t key);

    // Get the LVGL screen object
    lv_obj_t* getScreen() const { return screen; }

private:
    // Screen and widgets
    lv_obj_t* screen;           // Main game screen
    lv_obj_t* statusBar;        // Top status bar
    lv_obj_t* roomLabel;        // Room name label
    lv_obj_t* scoreLabel;       // Score display
    lv_obj_t* batteryLabel;     // Battery indicator
    lv_obj_t* outputArea;       // Scrollable text area for game output
    lv_obj_t* inputContainer;   // Container for input line
    lv_obj_t* promptLabel;      // ">" prompt
    lv_obj_t* inputField;       // Text input field

    // State
    bool visible;
    bool initialized;
    CommandCallback cmdCallback;

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
    void submitCommand();
    void historyUp();
    void historyDown();
    void addToHistory(const char* cmd);

    // Static callbacks for LVGL
    static void inputEventCallback(lv_event_t* e);
};

// Global instance
extern GameUI* gameUI;

#endif // T_DECK
