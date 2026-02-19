#include "ZorkMeshModule.h"
#include "GameEngine.h"
#if defined(HAS_TFT)
#include "GameUI.h"
// Device UI integration
#include "screens.h"  // For device-ui objects
#include "fonts.h"    // For custom fonts (ui_font_montserrat_*)
#include <lvgl.h>
#endif
#include "MeshService.h"
#include "NodeDB.h"
#include "PowerFSM.h"
#include "Router.h"
#include "configuration.h"
#include <Arduino.h>
#include "serialization/JSON.h"

// Protocol version for compatibility
#define ZORKMESH_PROTOCOL_VERSION 1

// Max remote players to track
#define MAX_REMOTE_PLAYERS 8
static RemotePlayer remotePlayers[MAX_REMOTE_PLAYERS];
static int remotePlayerCount = 0;
static portMUX_TYPE playerMutex = portMUX_INITIALIZER_UNLOCKED;

// Message queue for thread-safe UI updates (packet handler -> main loop)
// Keep small to avoid memory issues
#define MAX_PENDING_MESSAGES 8
#define MAX_MESSAGE_LEN 80
static char pendingMessages[MAX_PENDING_MESSAGES][MAX_MESSAGE_LEN];
static int pendingMessageCount = 0;
static portMUX_TYPE messageMutex = portMUX_INITIALIZER_UNLOCKED;

// Outgoing message queue for thread-safe mesh sending (UI -> main loop)
// Messages are queued from UI callbacks and sent from runOnce()
#define MAX_OUTGOING_MESSAGES 4
#define MAX_OUTGOING_LEN 200
static char outgoingMessages[MAX_OUTGOING_MESSAGES][MAX_OUTGOING_LEN];
static int outgoingMessageCount = 0;
static portMUX_TYPE outgoingMutex = portMUX_INITIALIZER_UNLOCKED;

// Raw incoming message queue - packet handler queues raw JSON, main loop parses
// This avoids JSON parsing in the radio callback which can cause crashes
#define MAX_RAW_INCOMING 4
#define MAX_RAW_LEN 200
static char rawIncoming[MAX_RAW_INCOMING][MAX_RAW_LEN];
static int rawIncomingCount = 0;
static portMUX_TYPE rawIncomingMutex = portMUX_INITIALIZER_UNLOCKED;

// Queue a message for display (called from main loop only now)
static void queueMessage(const char* msg) {
    // No mutex needed - both queue and dequeue run from main loop
    if (pendingMessageCount < MAX_PENDING_MESSAGES) {
        strncpy(pendingMessages[pendingMessageCount], msg, MAX_MESSAGE_LEN - 1);
        pendingMessages[pendingMessageCount][MAX_MESSAGE_LEN - 1] = '\0';
        pendingMessageCount++;
    }
}

// Dequeue one message (called from main loop only)
// Returns true if a message was dequeued
static bool dequeueMessage(char* outMsg, size_t maxLen) {
    // No mutex needed - both queue and dequeue run from main loop
    if (pendingMessageCount > 0) {
        strncpy(outMsg, pendingMessages[0], maxLen - 1);
        outMsg[maxLen - 1] = '\0';
        // Shift remaining messages
        for (int i = 1; i < pendingMessageCount; i++) {
            memcpy(pendingMessages[i-1], pendingMessages[i], MAX_MESSAGE_LEN);
        }
        pendingMessageCount--;
        return true;
    }
    return false;
}

// Queue an outgoing mesh message (thread-safe, called from UI callbacks)
static void queueOutgoingMessage(const char* msg) {
    portENTER_CRITICAL(&outgoingMutex);
    if (outgoingMessageCount < MAX_OUTGOING_MESSAGES) {
        strncpy(outgoingMessages[outgoingMessageCount], msg, MAX_OUTGOING_LEN - 1);
        outgoingMessages[outgoingMessageCount][MAX_OUTGOING_LEN - 1] = '\0';
        outgoingMessageCount++;
        LOG_DEBUG("ZorkMesh: Queued outgoing message (%d in queue)", outgoingMessageCount);
    } else {
        LOG_WARN("ZorkMesh: Outgoing queue full, dropping message");
    }
    portEXIT_CRITICAL(&outgoingMutex);
}

// Dequeue one outgoing message (thread-safe, called from runOnce)
static bool dequeueOutgoingMessage(char* outMsg, size_t maxLen) {
    bool found = false;
    portENTER_CRITICAL(&outgoingMutex);
    if (outgoingMessageCount > 0) {
        strncpy(outMsg, outgoingMessages[0], maxLen - 1);
        outMsg[maxLen - 1] = '\0';
        // Shift remaining messages
        for (int i = 1; i < outgoingMessageCount; i++) {
            memcpy(outgoingMessages[i-1], outgoingMessages[i], MAX_OUTGOING_LEN);
        }
        outgoingMessageCount--;
        found = true;
    }
    portEXIT_CRITICAL(&outgoingMutex);
    return found;
}

// Queue a raw incoming message (thread-safe, called from packet handler)
static void queueRawIncoming(const char* msg, size_t len) {
    portENTER_CRITICAL(&rawIncomingMutex);
    if (rawIncomingCount < MAX_RAW_INCOMING) {
        size_t copyLen = len < MAX_RAW_LEN - 1 ? len : MAX_RAW_LEN - 1;
        memcpy(rawIncoming[rawIncomingCount], msg, copyLen);
        rawIncoming[rawIncomingCount][copyLen] = '\0';
        rawIncomingCount++;
    }
    portEXIT_CRITICAL(&rawIncomingMutex);
}

// Dequeue one raw incoming message (thread-safe, called from runOnce)
static bool dequeueRawIncoming(char* outMsg, size_t maxLen) {
    bool found = false;
    portENTER_CRITICAL(&rawIncomingMutex);
    if (rawIncomingCount > 0) {
        strncpy(outMsg, rawIncoming[0], maxLen - 1);
        outMsg[maxLen - 1] = '\0';
        // Shift remaining messages
        for (int i = 1; i < rawIncomingCount; i++) {
            memcpy(rawIncoming[i-1], rawIncoming[i], MAX_RAW_LEN);
        }
        rawIncomingCount--;
        found = true;
    }
    portEXIT_CRITICAL(&rawIncomingMutex);
    return found;
}

// Forward declaration for sendGameMessageDirect (defined later with radio class)
static void sendGameMessageDirect(const char* jsonPayload);

// Forward declaration for processIncomingJson (defined later with radio class)
static void processIncomingJson(const char* jsonStr);

// Find or create a remote player entry (must hold playerMutex)
static RemotePlayer* findOrCreatePlayerLocked(const char* playerId) {
    // Search existing
    for (int i = 0; i < MAX_REMOTE_PLAYERS; i++) {
        if (remotePlayers[i].active && strcmp(remotePlayers[i].playerId, playerId) == 0) {
            return &remotePlayers[i];
        }
    }
    // Find empty slot
    for (int i = 0; i < MAX_REMOTE_PLAYERS; i++) {
        if (!remotePlayers[i].active) {
            memset(&remotePlayers[i], 0, sizeof(RemotePlayer));
            strncpy(remotePlayers[i].playerId, playerId, sizeof(remotePlayers[i].playerId) - 1);
            remotePlayers[i].active = true;
            remotePlayers[i].lastSeen = millis();
            remotePlayerCount++;
            return &remotePlayers[i];
        }
    }
    return nullptr; // No space
}

// Remove a player (must hold playerMutex)
static void removePlayerLocked(const char* playerId) {
    for (int i = 0; i < MAX_REMOTE_PLAYERS; i++) {
        if (remotePlayers[i].active && strcmp(remotePlayers[i].playerId, playerId) == 0) {
            remotePlayers[i].active = false;
            remotePlayerCount--;
            return;
        }
    }
}

// Get players in a specific room (thread-safe)
static int getPlayersInRoom(const char* roomId, RemotePlayer** result, int maxResults) {
    int count = 0;
    portENTER_CRITICAL(&playerMutex);
    for (int i = 0; i < MAX_REMOTE_PLAYERS && count < maxResults; i++) {
        if (remotePlayers[i].active && strcmp(remotePlayers[i].roomId, roomId) == 0) {
            result[count++] = &remotePlayers[i];
        }
    }
    portEXIT_CRITICAL(&playerMutex);
    return count;
}

// Global instances
ZorkMeshModule *zorkMeshModule = nullptr;
ZorkMeshModuleRadio *zorkMeshModuleRadio = nullptr;

// Use PRIVATE_APP port for game messages
static const meshtastic_PortNum ZORKMESH_PORTNUM = meshtastic_PortNum_PRIVATE_APP;

#if defined(HAS_TFT)
// ZorkMesh button in Tools panel
static lv_obj_t *zorkMeshButton = nullptr;
static lv_obj_t *zorkMeshButtonLabel = nullptr;
static lv_obj_t *mainScreenRef = nullptr;  // Reference to main Meshtastic screen

// Button click callback - launches ZorkMesh
static void zorkMeshButtonCallback(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        LOG_INFO("ZorkMesh button clicked - launching game");
        if (zorkMeshModule) {
            // Save reference to current screen before switching
            mainScreenRef = lv_screen_active();
            zorkMeshModule->showGameUI();
        }
    }
}

// Called from GameUI to return to Meshtastic
void zorkMeshReturnToMain()
{
    if (mainScreenRef) {
        lv_screen_load(mainScreenRef);
        LOG_INFO("ZorkMesh: Returned to main Meshtastic screen");
    }
}
#endif

/*
 * ZorkMeshModule - Main thread for periodic tasks
 */
ZorkMeshModule::ZorkMeshModule() : concurrency::OSThread("ZorkMesh")
{
    LOG_INFO("ZorkMesh Module initialized");
}

void ZorkMeshModule::startGame()
{
    if (!gameActive) {
        // Initialize game engine if needed
        if (!gameEngine) {
            gameEngine = new GameEngine();
            gameEngine->init();
            LOG_INFO("ZorkMesh GameEngine initialized");
        }

#if defined(HAS_TFT)
        // Initialize UI if needed
        if (!gameUI) {
            gameUI = new GameUI();
            gameUI->init();

            // Set command callback
            gameUI->setCommandCallback([this](const char* cmd) {
                this->processCommand(cmd);
            });

            // Set username callback
            gameUI->setUsernameCallback([this](const char* username) {
                this->handleUsernameSet(username);
            });

            // Set settings callback
            gameUI->setSettingsCallback([this](int action) {
                this->handleSettingsAction(action);
            });

            // Set splash done callback (handles first-run vs returning player)
            gameUI->setSplashDoneCallback([this]() {
                this->onSplashDone();
            });

            LOG_INFO("ZorkMesh GameUI initialized");
        }
#endif

        gameActive = true;
        LOG_INFO("ZorkMesh game started");

        // Send join message to mesh (queued for main thread)
        if (gameEngine && zorkMeshModuleRadio) {
            zorkMeshModuleRadio->sendJoin(
                gameEngine->getPlayerName(),
                gameEngine->getCurrentRoomId()
            );
        }
    }
}

void ZorkMeshModule::stopGame()
{
    if (gameActive) {
        // Send leave message (queued for main thread)
        if (gameEngine && zorkMeshModuleRadio) {
            zorkMeshModuleRadio->sendLeave(gameEngine->getPlayerName());
        }

        // Save game state
        if (gameEngine) {
            gameEngine->saveGame();
        }

#if defined(HAS_TFT)
        // Hide UI
        if (gameUI) {
            gameUI->hide();
        }
#endif

        gameActive = false;
        LOG_INFO("ZorkMesh game stopped");
    }
}

void ZorkMeshModule::showGameUI()
{
#if defined(HAS_TFT)
    if (!gameUI) {
        startGame(); // This will create the UI
    }

    if (gameUI) {
        gameUI->show();  // Shows splash screen first
        uiVisible = true;

        // Start observing keyboard input when UI is visible
        if (inputBroker) {
            inputObserver.observe(inputBroker);
        }
    }
#endif
}

#if defined(HAS_TFT)
void ZorkMeshModule::onSplashDone()
{
    // Called after splash screen finishes - handle first run vs returning player
    if (!gameEngine || !gameUI) return;

    const char* playerName = gameEngine->getPlayerName();
    if (!playerName || strlen(playerName) == 0) {
        // First run - show username prompt
        gameUI->showUsernamePrompt(nullptr);
    } else {
        // Returning player - show room description
        String look = gameEngine->processCommand("LOOK");
        gameUI->println(look.c_str());
        gameUI->setRoomName(gameEngine->getCurrentRoom() ?
            gameEngine->getCurrentRoom()->name : "Unknown");
        gameUI->setScore(gameEngine->getScore(), gameEngine->getMoves());
    }
}
#endif

void ZorkMeshModule::hideGameUI()
{
#if defined(HAS_TFT)
    uiVisible = false;

    if (gameUI) {
        gameUI->hide();
    }
#endif
}

#if defined(HAS_TFT)
void ZorkMeshModule::handleUsernameSet(const char* username)
{
    if (!gameEngine || !gameUI) return;

    gameEngine->setPlayerName(username);
    gameEngine->saveGame();

    // Show welcome message and initial look
    gameUI->clear();
    gameUI->println("=== ZorkMesh ===");
    gameUI->print("Welcome, ");
    gameUI->print(username);
    gameUI->println("!");
    gameUI->println("");

    String look = gameEngine->processCommand("LOOK");
    gameUI->println(look.c_str());
    gameUI->setRoomName(gameEngine->getCurrentRoom() ?
        gameEngine->getCurrentRoom()->name : "Unknown");
    gameUI->setScore(gameEngine->getScore(), gameEngine->getMoves());

    LOG_INFO("ZorkMesh: Player name set to %s", username);
}

void ZorkMeshModule::handleSettingsAction(int action)
{
    if (!gameEngine || !gameUI) return;

    switch (action) {
        case SETTINGS_RESET_LOCATION:
            // Reset player to starting location
            gameEngine->resetLocation();
            gameUI->println("[Location reset to West of House]");
            {
                String look = gameEngine->processCommand("LOOK");
                gameUI->println(look.c_str());
            }
            gameUI->setRoomName(gameEngine->getCurrentRoom() ?
                gameEngine->getCurrentRoom()->name : "Unknown");
            gameEngine->saveGame();
            break;

        case SETTINGS_RESET_INVENTORY:
            // Drop all items and reset to starting inventory
            gameEngine->resetInventory();
            gameUI->println("[Inventory reset]");
            gameEngine->saveGame();
            break;

        case SETTINGS_NEW_GAME:
            // Full game reset
            gameEngine->reset();
            gameUI->clear();
            gameUI->println("=== New Game Started ===");
            gameUI->println("");
            {
                String look = gameEngine->processCommand("LOOK");
                gameUI->println(look.c_str());
            }
            gameUI->setRoomName(gameEngine->getCurrentRoom() ?
                gameEngine->getCurrentRoom()->name : "Unknown");
            gameUI->setScore(gameEngine->getScore(), gameEngine->getMoves());
            // Don't save - let player make progress first
            break;

        default:
            break;
    }
}
#else
// Stub implementations for non-TFT builds
void ZorkMeshModule::handleUsernameSet(const char* username)
{
    (void)username;
}

void ZorkMeshModule::handleSettingsAction(int action)
{
    (void)action;
}

void ZorkMeshModule::onSplashDone()
{
}
#endif

void ZorkMeshModule::processCommand(const char* command)
{
    if (!gameEngine) return;

    // Handle CHAT command (mesh chat only, doesn't affect game)
    if (strncasecmp(command, "CHAT ", 5) == 0) {
        const char* msg = command + 5;
        if (msg && strlen(msg) > 0 && zorkMeshModuleRadio) {
#if defined(HAS_TFT)
            if (gameUI) {
                char localEcho[80];
                snprintf(localEcho, sizeof(localEcho), "You say: \"%s\"", msg);
                gameUI->println(localEcho);
                gameUI->println("");
            }
#endif
            zorkMeshModuleRadio->sendChat(gameEngine->getPlayerName(), msg);
        }
        return;
    }

    // Handle multiplayer-specific commands
    if (strncasecmp(command, "WHO", 3) == 0 || strncasecmp(command, "PLAYERS", 7) == 0) {
        const char* currentRoom = gameEngine->getCurrentRoomId();
        RemotePlayer* playersHere[MAX_REMOTE_PLAYERS];
        int count = getPlayersInRoom(currentRoom, playersHere, MAX_REMOTE_PLAYERS);

#if defined(HAS_TFT)
        if (gameUI) {
            if (count == 0) {
                gameUI->println("You are alone here.");
            } else if (count == 1) {
                char msg[64];
                snprintf(msg, sizeof(msg), "%s is here.", playersHere[0]->name);
                gameUI->println(msg);
            } else {
                String msg = "";
                for (int i = 0; i < count; i++) {
                    if (i > 0) msg += ", ";
                    if (i == count - 1 && count > 1) msg += "and ";
                    msg += playersHere[i]->name;
                }
                msg += " are here.";
                gameUI->println(msg.c_str());
            }
            gameUI->println("");
        }
#endif
        return;
    }

    // Get previous room ID to detect movement
    const char* prevRoom = gameEngine->getCurrentRoomId();

    // Process command
    String response = gameEngine->processCommand(command);

    // For LOOK command, append info about other players in the room
    if (strncasecmp(command, "LOOK", 4) == 0 || strcasecmp(command, "L") == 0) {
        const char* currentRoom = gameEngine->getCurrentRoomId();
        RemotePlayer* playersHere[MAX_REMOTE_PLAYERS];
        int count = getPlayersInRoom(currentRoom, playersHere, MAX_REMOTE_PLAYERS);

        if (count > 0) {
            response += "\n";
            if (count == 1) {
                response += playersHere[0]->name;
                response += " is here.";
            } else {
                for (int i = 0; i < count; i++) {
                    if (i > 0) {
                        if (i == count - 1) {
                            response += " and ";
                        } else {
                            response += ", ";
                        }
                    }
                    response += playersHere[i]->name;
                }
                response += " are here.";
            }
        }
    }

#if defined(HAS_TFT)
    // Display response
    if (gameUI) {
        gameUI->println(response.c_str());
        gameUI->println(""); // Blank line

        // Update status bar
        if (gameEngine->getCurrentRoom()) {
            gameUI->setRoomName(gameEngine->getCurrentRoom()->name);
        }
        gameUI->setScore(gameEngine->getScore(), gameEngine->getMoves());
    }
#endif

    // Check if player moved and send update to mesh (queued for main thread)
    const char* newRoom = gameEngine->getCurrentRoomId();
    if (strcmp(prevRoom, newRoom) != 0 && zorkMeshModuleRadio) {
        zorkMeshModuleRadio->sendMove(gameEngine->getPlayerName(), prevRoom, newRoom);
    }

    // Check for chat command (queued for main thread)
    if (strncasecmp(command, "SAY ", 4) == 0 || strncasecmp(command, "CHAT ", 5) == 0) {
        const char* msg = strchr(command, ' ');
        if (msg && zorkMeshModuleRadio) {
            zorkMeshModuleRadio->sendChat(gameEngine->getPlayerName(), msg + 1);
        }
    }
}

#if defined(HAS_TFT)
// Launch key: z (for Zork - no modifier needed)
#define ZORKMESH_LAUNCH_KEY 'z'

int ZorkMeshModule::handleInputEvent(const InputEvent* event)
{
    // Check for launch key (TAB) when game UI is NOT visible
    if (!uiVisible) {
        if (event->kbchar == ZORKMESH_LAUNCH_KEY) {
            LOG_INFO("ZorkMesh: TAB key pressed, launching game");
            showGameUI();
            return 1; // Consumed
        }
        return 0; // Let other handlers process this
    }

    // Game UI is visible - handle game input
    if (!gameUI) {
        return 0;
    }

    // Handle the keyboard character
    if (event->kbchar != 0) {
        // ESC key exits game
        if (event->kbchar == 0x1B) {
            hideGameUI();
            return 1;
        }
        // '[' key scrolls output up (Page Up)
        if (event->kbchar == '[') {
            gameUI->onKeyPress(0x02); // Custom code for page up
            return 1;
        }
        // ']' key scrolls output down (Page Down)
        if (event->kbchar == ']') {
            gameUI->onKeyPress(0x03); // Custom code for page down
            return 1;
        }
        gameUI->onKeyPress(event->kbchar);
        return 1; // Consumed
    }

    // Handle special keys
    switch (event->inputEvent) {
        case INPUT_BROKER_UP:
            // History up
            gameUI->onKeyPress(0x00); // Custom code for up
            return 1;
        case INPUT_BROKER_DOWN:
            // History down
            gameUI->onKeyPress(0x01); // Custom code for down
            return 1;
        case INPUT_BROKER_BACK:
        case INPUT_BROKER_CANCEL:
            // Exit game UI
            hideGameUI();
            return 1;
        default:
            break;
    }

    return 0; // Let other handlers process this
}
#endif

int32_t ZorkMeshModule::runOnce()
{
    if (firstTime) {
        // Initialize the radio module on first run
        if (!zorkMeshModuleRadio) {
            zorkMeshModuleRadio = new ZorkMeshModuleRadio();
            LOG_INFO("ZorkMesh Radio module created");
        }

#if defined(HAS_TFT)
        // Also observe keyboard input for future use
        if (inputBroker) {
            inputObserver.observe(inputBroker);
        }
#endif

        firstTime = false;
    }

#if defined(HAS_TFT)
    // Customize boot screen with ZorkMesh branding (runs once when boot_screen is available)
    static bool bootScreenCustomized = false;
    if (!bootScreenCustomized && objects.boot_screen != nullptr && objects.boot_logo != nullptr) {
        bootScreenCustomized = true;
        LOG_INFO("ZorkMesh: Customizing boot screen");

        // Keep Meshtastic logo centered but move up slightly to make room
        lv_obj_set_pos(objects.boot_logo, 0, -40);

        // Add "ZorkMesh Edition" text below the logo
        lv_obj_t* zorkLabel = lv_label_create(objects.boot_screen);
        lv_label_set_text(zorkLabel, "ZorkMesh Edition");
        lv_obj_set_style_text_color(zorkLabel, lv_color_hex(0x000000), 0);
        lv_obj_set_style_text_font(zorkLabel, &ui_font_montserrat_16, 0);
        lv_obj_align(zorkLabel, LV_ALIGN_CENTER, 0, 30);

        // Modify existing Meshtastic URL to show both sites
        if (objects.meshtastic_url) {
            lv_label_set_text(objects.meshtastic_url, "meshtastic.org  |  zorkmesh.com");
        }

        LOG_INFO("ZorkMesh: Boot screen customized");
    }

    // Keep trying to add ZorkMesh button until device-ui is ready
    // The UI initializes after modules, so we need to wait
    if (!zorkMeshButton && objects.main_screen != nullptr && objects.button_panel != nullptr) {
        LOG_INFO("ZorkMesh: Device UI ready, adding button to main menu bar");

        // Resize existing buttons to fit 7 buttons in the panel (was 6 at 36x36)
        // New size: 32x32 to fit all buttons within 240px height
        const int newButtonSize = 32;
        if (objects.home_button) lv_obj_set_size(objects.home_button, newButtonSize, newButtonSize);
        if (objects.nodes_button) lv_obj_set_size(objects.nodes_button, newButtonSize, newButtonSize);
        if (objects.groups_button) lv_obj_set_size(objects.groups_button, newButtonSize, newButtonSize);
        if (objects.messages_button) lv_obj_set_size(objects.messages_button, newButtonSize, newButtonSize);
        if (objects.map_button) lv_obj_set_size(objects.map_button, newButtonSize, newButtonSize);
        if (objects.settings_button) lv_obj_set_size(objects.settings_button, newButtonSize, newButtonSize);

        // Reduce padding in button panel to fit all buttons
        lv_obj_set_style_pad_top(objects.button_panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_bottom(objects.button_panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_pad_row(objects.button_panel, 2, LV_PART_MAIN | LV_STATE_DEFAULT);

        // Create ZorkMesh button in the main menu bar
        zorkMeshButton = lv_btn_create(objects.button_panel);
        lv_obj_set_size(zorkMeshButton, newButtonSize, newButtonSize);
        lv_obj_add_flag(zorkMeshButton, LV_OBJ_FLAG_SCROLL_CHAIN);
        lv_obj_remove_flag(zorkMeshButton, (lv_obj_flag_t)(LV_OBJ_FLAG_PRESS_LOCK | LV_OBJ_FLAG_SCROLL_CHAIN_HOR | LV_OBJ_FLAG_SCROLL_CHAIN_VER));

        // Style to match other menu buttons
        lv_obj_set_style_radius(zorkMeshButton, 6, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(zorkMeshButton, lv_color_hex(0x2d2d2d), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_bg_opa(zorkMeshButton, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_border_width(zorkMeshButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_shadow_width(zorkMeshButton, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

        // Pressed state
        lv_obj_set_style_bg_color(zorkMeshButton, lv_color_hex(0x67ea94), LV_PART_MAIN | LV_STATE_PRESSED);

        // Create "Z" label as icon (since we don't have a custom image)
        zorkMeshButtonLabel = lv_label_create(zorkMeshButton);
        lv_label_set_text(zorkMeshButtonLabel, "Z");
        lv_obj_center(zorkMeshButtonLabel);
        lv_obj_set_style_text_color(zorkMeshButtonLabel, lv_color_hex(0x67ea94), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(zorkMeshButtonLabel, &ui_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

        // Add click handler
        lv_obj_add_event_cb(zorkMeshButton, zorkMeshButtonCallback, LV_EVENT_CLICKED, nullptr);

        LOG_INFO("ZorkMesh: Button added to main menu bar successfully");
    }
#endif

    // If game is not active, sleep longer
    if (!gameActive) {
        return 10000; // Check every 10 seconds when inactive
    }

    // Process raw incoming JSON messages (safe to do JSON parsing here in main loop)
    {
        static char rawJson[MAX_RAW_LEN];
        // Process up to 2 messages per cycle
        for (int i = 0; i < 2 && dequeueRawIncoming(rawJson, sizeof(rawJson)); i++) {
            processIncomingJson(rawJson);
        }
    }

#if defined(HAS_TFT)
    // Process ONE queued message per cycle using deferred print (LVGL context safety)
    if (gameUI && gameUI->isVisible()) {
        static char msg[MAX_MESSAGE_LEN];
        if (dequeueMessage(msg, sizeof(msg))) {
            LOG_INFO("ZorkMesh: queueing deferred print: %s", msg);
            gameUI->printDeferred(msg);  // Uses LVGL timer - safe from any context
        }
    }
#endif

    // Process queued outgoing messages (send from main thread for safety)
    {
        static char outMsg[MAX_OUTGOING_LEN];
        // Send up to 2 messages per cycle to avoid flooding
        for (int i = 0; i < 2 && dequeueOutgoingMessage(outMsg, sizeof(outMsg)); i++) {
            sendGameMessageDirect(outMsg);
        }
    }

    uint32_t now = millis();

    // Send periodic heartbeat when game is active
    // TEMPORARILY DISABLED FOR DEBUGGING
    if (false && now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
        lastHeartbeat = now;
        if (gameEngine && zorkMeshModuleRadio) {
            zorkMeshModuleRadio->sendHeartbeat(
                gameEngine->getPlayerName(),
                gameEngine->getCurrentRoomId()
            );
            LOG_DEBUG("ZorkMesh heartbeat sent");
        }
    }

    // Clean up stale players (not seen in 3 minutes)
    static const uint32_t PLAYER_TIMEOUT_MS = 180000;
    portENTER_CRITICAL(&playerMutex);
    for (int i = 0; i < MAX_REMOTE_PLAYERS; i++) {
        if (remotePlayers[i].active && (now - remotePlayers[i].lastSeen > PLAYER_TIMEOUT_MS)) {
            char name[32];
            strncpy(name, remotePlayers[i].name, sizeof(name) - 1);
            name[sizeof(name) - 1] = '\0';
            remotePlayers[i].active = false;
            remotePlayerCount--;
            portEXIT_CRITICAL(&playerMutex);

#if defined(HAS_TFT)
            if (gameUI) {
                char msg[48];
                snprintf(msg, sizeof(msg), "[%s timed out]", name);
                gameUI->println(msg);
            }
#endif
            LOG_INFO("ZorkMesh: Player %s timed out", name);

            // Re-acquire mutex for next iteration
            portENTER_CRITICAL(&playerMutex);
        }
    }
    portEXIT_CRITICAL(&playerMutex);

    return 500; // Run every 500ms when active to process messages promptly
}

/*
 * ZorkMeshModuleRadio - Packet handler
 */
ZorkMeshModuleRadio::ZorkMeshModuleRadio()
    : SinglePortModule("ZorkMeshRadio", ZORKMESH_PORTNUM)
{
    LOG_INFO("ZorkMesh Radio listening on port %d", ZORKMESH_PORTNUM);
}

// Queue a message for sending (thread-safe, can be called from any context)
void ZorkMeshModuleRadio::sendGameMessage(const char* jsonPayload)
{
    size_t payloadLen = strlen(jsonPayload);
    if (payloadLen > MAX_LORA_PAYLOAD_LEN) {
        LOG_WARN("ZorkMesh message too long: %d bytes (max %d)", payloadLen, MAX_LORA_PAYLOAD_LEN);
        return;
    }
    if (payloadLen >= MAX_OUTGOING_LEN) {
        LOG_WARN("ZorkMesh message too long for queue: %d bytes", payloadLen);
        return;
    }

    // Queue the message to be sent from main thread
    queueOutgoingMessage(jsonPayload);
}

// Actually send a message (must be called from main thread/runOnce)
void ZorkMeshModuleRadio::sendDirect(const char* jsonPayload)
{
    size_t payloadLen = strlen(jsonPayload);

    meshtastic_MeshPacket *p = allocDataPacket();
    if (!p) {
        LOG_ERROR("ZorkMesh failed to allocate packet");
        return;
    }

    p->to = NODENUM_BROADCAST;
    p->decoded.want_response = false;
    p->want_ack = false;
    p->decoded.payload.size = payloadLen;
    memcpy(p->decoded.payload.bytes, jsonPayload, payloadLen);

    LOG_DEBUG("ZorkMesh sending: %s", jsonPayload);
    service->sendToMesh(p);
}

// Static wrapper for use in runOnce
static void sendGameMessageDirect(const char* jsonPayload)
{
    if (zorkMeshModuleRadio) {
        zorkMeshModuleRadio->sendDirect(jsonPayload);
    }
}

// Helper to generate player ID hash
static void generatePlayerId(const char* playerName, char* playerId) {
    if (!playerName || !playerId) {
        if (playerId) strcpy(playerId, "000000");
        return;
    }
    uint32_t hash = 0;
    for (const char* c = playerName; *c; c++) {
        hash = hash * 31 + *c;
    }
    snprintf(playerId, 7, "%06X", hash & 0xFFFFFF);
}

// Local player's ID (cached)
static char localPlayerId[8] = {0};

void ZorkMeshModuleRadio::sendJoin(const char* playerName, const char* roomId)
{
    generatePlayerId(playerName, localPlayerId);

    // Format: {"v":1,"t":"PJ","p":"ABC123","s":0,"d":{"n":"name","r":"roomid"}}
    char payload[192];
    snprintf(payload, sizeof(payload),
             "{\"v\":%d,\"t\":\"PJ\",\"p\":\"%s\",\"s\":0,\"d\":{\"n\":\"%s\",\"r\":\"%s\"}}",
             ZORKMESH_PROTOCOL_VERSION, localPlayerId, playerName, roomId);
    sendGameMessage(payload);
}

void ZorkMeshModuleRadio::sendLeave(const char* playerName)
{
    generatePlayerId(playerName, localPlayerId);

    char payload[96];
    snprintf(payload, sizeof(payload),
             "{\"v\":%d,\"t\":\"PL\",\"p\":\"%s\",\"s\":0,\"d\":{}}",
             ZORKMESH_PROTOCOL_VERSION, localPlayerId);
    sendGameMessage(payload);
}

void ZorkMeshModuleRadio::sendMove(const char* playerName, const char* fromRoom, const char* toRoom)
{
    generatePlayerId(playerName, localPlayerId);

    char payload[192];
    snprintf(payload, sizeof(payload),
             "{\"v\":%d,\"t\":\"PM\",\"p\":\"%s\",\"s\":0,\"d\":{\"f\":\"%s\",\"r\":\"%s\",\"n\":\"%s\"}}",
             ZORKMESH_PROTOCOL_VERSION, localPlayerId, fromRoom, toRoom, playerName);
    sendGameMessage(payload);
}

void ZorkMeshModuleRadio::sendChat(const char* playerName, const char* message)
{
    generatePlayerId(playerName, localPlayerId);

    // Escape any quotes in the message (simple approach - just truncate at quote)
    char safeMsg[128];
    strncpy(safeMsg, message, sizeof(safeMsg) - 1);
    safeMsg[sizeof(safeMsg) - 1] = '\0';
    for (char* p = safeMsg; *p; p++) {
        if (*p == '"') *p = '\'';  // Replace quotes with single quotes
    }

    char payload[256];
    snprintf(payload, sizeof(payload),
             "{\"v\":%d,\"t\":\"CH\",\"p\":\"%s\",\"s\":0,\"d\":{\"m\":\"%s\",\"n\":\"%s\"}}",
             ZORKMESH_PROTOCOL_VERSION, localPlayerId, safeMsg, playerName);
    sendGameMessage(payload);
}

void ZorkMeshModuleRadio::sendHeartbeat(const char* playerName, const char* roomId)
{
    if (!playerName || !roomId) {
        LOG_WARN("ZorkMesh: sendHeartbeat called with null args");
        return;
    }
    generatePlayerId(playerName, localPlayerId);

    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"v\":%d,\"t\":\"HB\",\"p\":\"%s\",\"s\":0,\"d\":{\"r\":\"%s\"}}",
             ZORKMESH_PROTOCOL_VERSION, localPlayerId, roomId);
    sendGameMessage(payload);
}

// Static buffers for handleReceived to avoid stack overflow
static char s_jsonBuf[200];  // Reduced from 256
static char s_msgType[8];
static char s_playerIdBuf[16];
static char s_name[32];      // Reduced from 64
static char s_roomId[16];
static char s_fromRoom[16];
static char s_toRoom[16];
static char s_message[64];   // Reduced from 128
static char s_msg[MAX_MESSAGE_LEN];

// Simple JSON string field extractor - no memory allocation
// Extracts value for "key":"value" pattern into dest buffer
static bool extractJsonString(const char* json, const char* key, char* dest, size_t destLen) {
    dest[0] = '\0';
    if (!json || !key || destLen == 0) return false;

    // Build search pattern: "key":"
    char pattern[32];
    snprintf(pattern, sizeof(pattern), "\"%s\":\"", key);

    const char* start = strstr(json, pattern);
    if (!start) return false;

    start += strlen(pattern);
    const char* end = strchr(start, '"');
    if (!end) return false;

    size_t len = end - start;
    if (len >= destLen) len = destLen - 1;

    memcpy(dest, start, len);
    dest[len] = '\0';
    return true;
}

// Process one raw incoming JSON message (called from main loop)
// Uses simple string parsing - NO dynamic memory allocation
static void processIncomingJson(const char* jsonStr)
{
    if (!jsonStr || strlen(jsonStr) < 10) {
        return;
    }

    // Extract message type "t":"XX"
    s_msgType[0] = '\0';
    extractJsonString(jsonStr, "t", s_msgType, sizeof(s_msgType));

    if (s_msgType[0] == '\0') {
        LOG_WARN("ZorkMesh: No message type in: %.40s", jsonStr);
        return;
    }

    LOG_INFO("ZorkMesh: type=%s", s_msgType);

    // Extract player ID "p":"XXXXXX"
    s_playerIdBuf[0] = '\0';
    extractJsonString(jsonStr, "p", s_playerIdBuf, sizeof(s_playerIdBuf));

    // Ignore messages from ourselves
    if (strlen(localPlayerId) > 0 && strcmp(s_playerIdBuf, localPlayerId) == 0) {
        return;
    }

    // Get current room for comparison
    const char* currentRoom = gameEngine ? gameEngine->getCurrentRoomId() : "";

    // Handle message types with mutex protection for player data
    if (strcmp(s_msgType, "PJ") == 0) {
        // Player Join - extract "n" (name) and "r" (room) from data
        s_name[0] = '\0';
        strncpy(s_roomId, "whous", sizeof(s_roomId) - 1);

        extractJsonString(jsonStr, "n", s_name, sizeof(s_name));
        extractJsonString(jsonStr, "r", s_roomId, sizeof(s_roomId));

        if (strlen(s_name) == 0) {
            strncpy(s_name, s_playerIdBuf, sizeof(s_name) - 1);
        }

        // Add player to tracking
        portENTER_CRITICAL(&playerMutex);
        RemotePlayer* player = findOrCreatePlayerLocked(s_playerIdBuf);
        if (player) {
            strncpy(player->name, s_name, sizeof(player->name) - 1);
            strncpy(player->roomId, s_roomId, sizeof(player->roomId) - 1);
            player->lastSeen = millis();
        }
        portEXIT_CRITICAL(&playerMutex);

        // Show join message with player name
        snprintf(s_msg, sizeof(s_msg), "[%s joined]", s_name);
        queueMessage(s_msg);
        LOG_INFO("ZorkMesh: Player %s joined in room %s", s_name, s_roomId);
    }
    else if (strcmp(s_msgType, "PL") == 0) {
        // Player Leave
        s_name[0] = '\0';

        portENTER_CRITICAL(&playerMutex);
        for (int i = 0; i < MAX_REMOTE_PLAYERS; i++) {
            if (remotePlayers[i].active && strcmp(remotePlayers[i].playerId, s_playerIdBuf) == 0) {
                strncpy(s_name, remotePlayers[i].name, sizeof(s_name) - 1);
                remotePlayers[i].active = false;
                remotePlayerCount--;
                break;
            }
        }
        portEXIT_CRITICAL(&playerMutex);

        if (strlen(s_name) > 0) {
            snprintf(s_msg, sizeof(s_msg), "[%s left]", s_name);
            queueMessage(s_msg);
            LOG_INFO("ZorkMesh: Player %s left", s_name);
        }
    }
    else if (strcmp(s_msgType, "PM") == 0) {
        // Player Move - extract "f" (from), "r" (to room), "n" (name)
        s_fromRoom[0] = '\0';
        s_toRoom[0] = '\0';
        s_name[0] = '\0';

        extractJsonString(jsonStr, "f", s_fromRoom, sizeof(s_fromRoom));
        extractJsonString(jsonStr, "r", s_toRoom, sizeof(s_toRoom));
        extractJsonString(jsonStr, "n", s_name, sizeof(s_name));

        portENTER_CRITICAL(&playerMutex);
        RemotePlayer* player = findOrCreatePlayerLocked(s_playerIdBuf);
        if (player) {
            if (strlen(s_name) > 0) {
                strncpy(player->name, s_name, sizeof(player->name) - 1);
            }
            strncpy(player->roomId, s_toRoom, sizeof(player->roomId) - 1);
            player->lastSeen = millis();
            strncpy(s_name, player->name, sizeof(s_name) - 1);
        }
        portEXIT_CRITICAL(&playerMutex);

        if (player && strlen(currentRoom) > 0) {
            if (strcmp(s_toRoom, currentRoom) == 0) {
                snprintf(s_msg, sizeof(s_msg), "%s arrived.", s_name);
                queueMessage(s_msg);
            } else if (strcmp(s_fromRoom, currentRoom) == 0) {
                snprintf(s_msg, sizeof(s_msg), "%s left.", s_name);
                queueMessage(s_msg);
            }
        }
    }
    else if (strcmp(s_msgType, "CH") == 0) {
        // Chat message - extract "m" (message) and "n" (name)
        s_message[0] = '\0';
        s_name[0] = '\0';

        extractJsonString(jsonStr, "m", s_message, sizeof(s_message));
        extractJsonString(jsonStr, "n", s_name, sizeof(s_name));

        LOG_INFO("ZorkMesh CH: name='%s' msg='%s'", s_name, s_message);

        portENTER_CRITICAL(&playerMutex);
        RemotePlayer* player = findOrCreatePlayerLocked(s_playerIdBuf);
        if (player) {
            if (strlen(s_name) > 0) {
                strncpy(player->name, s_name, sizeof(player->name) - 1);
            }
            player->lastSeen = millis();
            strncpy(s_name, player->name, sizeof(s_name) - 1);
        }
        portEXIT_CRITICAL(&playerMutex);

        if (player && strlen(s_message) > 0) {
            snprintf(s_msg, sizeof(s_msg), "%s: \"%s\"", s_name, s_message);
            queueMessage(s_msg);
            LOG_INFO("ZorkMesh: Chat queued from %s", s_name);
        } else {
            LOG_WARN("ZorkMesh: CH not queued - player=%p msglen=%d", player, strlen(s_message));
        }
    }
    else if (strcmp(s_msgType, "HB") == 0) {
        // Heartbeat - just update last seen, extract "r" (room)
        s_roomId[0] = '\0';
        extractJsonString(jsonStr, "r", s_roomId, sizeof(s_roomId));

        portENTER_CRITICAL(&playerMutex);
        RemotePlayer* player = findOrCreatePlayerLocked(s_playerIdBuf);
        if (player) {
            if (strlen(s_roomId) > 0) {
                strncpy(player->roomId, s_roomId, sizeof(player->roomId) - 1);
            }
            player->lastSeen = millis();
        }
        portEXIT_CRITICAL(&playerMutex);
    }
    else if (strcmp(s_msgType, "PA") == 0) {
        // Player Action - simplified, just log it
        LOG_INFO("ZorkMesh: Action from %s", s_playerIdBuf);

        portENTER_CRITICAL(&playerMutex);
        RemotePlayer* player = findOrCreatePlayerLocked(s_playerIdBuf);
        if (player) {
            player->lastSeen = millis();
        }
        portEXIT_CRITICAL(&playerMutex);
    }
    // No cleanup needed - no dynamic memory allocation
}

ProcessMessage ZorkMeshModuleRadio::handleReceived(const meshtastic_MeshPacket &mp)
{
    // Only process if game is active
    if (!zorkMeshModule || !zorkMeshModule->isGameActive()) {
        return ProcessMessage::CONTINUE;
    }

    // Ignore our own messages
    if (isFromUs(&mp)) {
        return ProcessMessage::CONTINUE;
    }

    auto &p = mp.decoded;
    if (p.payload.size == 0) {
        return ProcessMessage::CONTINUE;
    }

    // Log received message for debugging
    LOG_INFO("ZorkMesh received from 0x%0x: %.*s", mp.from, p.payload.size, p.payload.bytes);

    // Queue raw message for processing in main loop (avoids JSON parsing in callback)
    queueRawIncoming((const char*)p.payload.bytes, p.payload.size);

    // Wake screen on game message
    powerFSM.trigger(EVENT_RECEIVED_MSG);

    return ProcessMessage::CONTINUE;
}
