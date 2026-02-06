#include "ZorkMeshModule.h"
#include "GameEngine.h"
#ifdef T_DECK
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

// Find or create a remote player entry
static RemotePlayer* findOrCreatePlayer(const char* playerId) {
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

// Remove a player
static void removePlayer(const char* playerId) {
    for (int i = 0; i < MAX_REMOTE_PLAYERS; i++) {
        if (remotePlayers[i].active && strcmp(remotePlayers[i].playerId, playerId) == 0) {
            remotePlayers[i].active = false;
            remotePlayerCount--;
            return;
        }
    }
}

// Get players in a specific room
static int getPlayersInRoom(const char* roomId, RemotePlayer** result, int maxResults) {
    int count = 0;
    for (int i = 0; i < MAX_REMOTE_PLAYERS && count < maxResults; i++) {
        if (remotePlayers[i].active && strcmp(remotePlayers[i].roomId, roomId) == 0) {
            result[count++] = &remotePlayers[i];
        }
    }
    return count;
}

// Global instances
ZorkMeshModule *zorkMeshModule = nullptr;
ZorkMeshModuleRadio *zorkMeshModuleRadio = nullptr;

// Use PRIVATE_APP port for game messages
static const meshtastic_PortNum ZORKMESH_PORTNUM = meshtastic_PortNum_PRIVATE_APP;

#ifdef T_DECK
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

#ifdef T_DECK
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

        // Send join message to mesh
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
        // Send leave message
        if (gameEngine && zorkMeshModuleRadio) {
            zorkMeshModuleRadio->sendLeave(gameEngine->getPlayerName());
        }

        // Save game state
        if (gameEngine) {
            gameEngine->saveGame();
        }

#ifdef T_DECK
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
#ifdef T_DECK
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

#ifdef T_DECK
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
#ifdef T_DECK
    uiVisible = false;

    if (gameUI) {
        gameUI->hide();
    }
#endif
}

#ifdef T_DECK
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
// Stub implementations for non-T_DECK builds
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

    // Handle multiplayer-specific commands
    if (strncasecmp(command, "WHO", 3) == 0 || strncasecmp(command, "PLAYERS", 7) == 0) {
        const char* currentRoom = gameEngine->getCurrentRoomId();
        RemotePlayer* playersHere[MAX_REMOTE_PLAYERS];
        int count = getPlayersInRoom(currentRoom, playersHere, MAX_REMOTE_PLAYERS);

#ifdef T_DECK
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

#ifdef T_DECK
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

    // Check if player moved and send update to mesh
    const char* newRoom = gameEngine->getCurrentRoomId();
    if (strcmp(prevRoom, newRoom) != 0 && zorkMeshModuleRadio) {
        zorkMeshModuleRadio->sendMove(gameEngine->getPlayerName(), prevRoom, newRoom);
    }

    // Check for chat command
    if (strncasecmp(command, "SAY ", 4) == 0 || strncasecmp(command, "CHAT ", 5) == 0) {
        const char* msg = strchr(command, ' ');
        if (msg && zorkMeshModuleRadio) {
            zorkMeshModuleRadio->sendChat(gameEngine->getPlayerName(), msg + 1);
        }
    }
}

#ifdef T_DECK
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

#ifdef T_DECK
        // Also observe keyboard input for future use
        if (inputBroker) {
            inputObserver.observe(inputBroker);
        }
#endif

        firstTime = false;
    }

#ifdef T_DECK
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

    uint32_t now = millis();

    // Send periodic heartbeat when game is active
    if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
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
    for (int i = 0; i < MAX_REMOTE_PLAYERS; i++) {
        if (remotePlayers[i].active && (now - remotePlayers[i].lastSeen > PLAYER_TIMEOUT_MS)) {
#ifdef T_DECK
            if (gameUI) {
                char msg[64];
                snprintf(msg, sizeof(msg), "[%s has timed out]", remotePlayers[i].name);
                gameUI->println(msg);
            }
#endif
            LOG_INFO("ZorkMesh: Player %s timed out", remotePlayers[i].name);
            remotePlayers[i].active = false;
            remotePlayerCount--;
        }
    }

    return 5000; // Run every 5 seconds when active
}

/*
 * ZorkMeshModuleRadio - Packet handler
 */
ZorkMeshModuleRadio::ZorkMeshModuleRadio()
    : SinglePortModule("ZorkMeshRadio", ZORKMESH_PORTNUM)
{
    LOG_INFO("ZorkMesh Radio listening on port %d", ZORKMESH_PORTNUM);
}

void ZorkMeshModuleRadio::sendGameMessage(const char* jsonPayload)
{
    size_t payloadLen = strlen(jsonPayload);
    if (payloadLen > MAX_LORA_PAYLOAD_LEN) {
        LOG_WARN("ZorkMesh message too long: %d bytes (max %d)", payloadLen, MAX_LORA_PAYLOAD_LEN);
        return;
    }

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

// Helper to generate player ID hash
static void generatePlayerId(const char* playerName, char* playerId) {
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
    generatePlayerId(playerName, localPlayerId);

    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"v\":%d,\"t\":\"HB\",\"p\":\"%s\",\"s\":0,\"d\":{\"r\":\"%s\"}}",
             ZORKMESH_PROTOCOL_VERSION, localPlayerId, roomId);
    sendGameMessage(payload);
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

    // Null-terminate the payload for JSON parsing
    char jsonBuf[256];
    size_t copyLen = p.payload.size < sizeof(jsonBuf) - 1 ? p.payload.size : sizeof(jsonBuf) - 1;
    memcpy(jsonBuf, p.payload.bytes, copyLen);
    jsonBuf[copyLen] = '\0';

    // Parse JSON message using SimpleJSON
    JSONValue* root = JSON::Parse(jsonBuf);
    if (!root || !root->IsObject()) {
        LOG_WARN("ZorkMesh JSON parse error");
        delete root;
        return ProcessMessage::CONTINUE;
    }

    // Extract common fields
    const char* msgType = "";
    const char* playerId = "";

    if (root->HasChild("t") && root->Child("t")->IsString()) {
        msgType = root->Child("t")->AsString().c_str();
    }
    if (root->HasChild("p") && root->Child("p")->IsString()) {
        playerId = root->Child("p")->AsString().c_str();
    }

    // Ignore messages from ourselves (by player ID)
    if (strlen(localPlayerId) > 0 && strcmp(playerId, localPlayerId) == 0) {
        delete root;
        return ProcessMessage::CONTINUE;
    }

    // Get the data object
    JSONValue* data = root->HasChild("d") ? root->Child("d") : nullptr;

    // Get current room for comparison
    const char* currentRoom = gameEngine ? gameEngine->getCurrentRoomId() : "";

    // Handle message types
    if (strcmp(msgType, "PJ") == 0) {
        // Player Join
        const char* name = playerId;
        const char* roomId = "whous";
        if (data && data->IsObject()) {
            if (data->HasChild("n") && data->Child("n")->IsString()) {
                name = data->Child("n")->AsString().c_str();
            }
            if (data->HasChild("r") && data->Child("r")->IsString()) {
                roomId = data->Child("r")->AsString().c_str();
            }
        }

        RemotePlayer* player = findOrCreatePlayer(playerId);
        if (player) {
            strncpy(player->name, name, sizeof(player->name) - 1);
            strncpy(player->roomId, roomId, sizeof(player->roomId) - 1);
            player->lastSeen = millis();

            // Notify UI
#ifdef T_DECK
            if (gameUI) {
                char msg[64];
                snprintf(msg, sizeof(msg), "[%s has entered the game]", player->name);
                gameUI->println(msg);
            }
#endif
            LOG_INFO("ZorkMesh: Player %s joined in room %s", player->name, roomId);
        }
    }
    else if (strcmp(msgType, "PL") == 0) {
        // Player Leave
        RemotePlayer* player = nullptr;
        for (int i = 0; i < MAX_REMOTE_PLAYERS; i++) {
            if (remotePlayers[i].active && strcmp(remotePlayers[i].playerId, playerId) == 0) {
                player = &remotePlayers[i];
                break;
            }
        }

        if (player) {
#ifdef T_DECK
            if (gameUI) {
                char msg[64];
                snprintf(msg, sizeof(msg), "[%s has left the game]", player->name);
                gameUI->println(msg);
            }
#endif
            LOG_INFO("ZorkMesh: Player %s left", player->name);
            removePlayer(playerId);
        }
    }
    else if (strcmp(msgType, "PM") == 0) {
        // Player Move
        const char* fromRoom = "";
        const char* toRoom = "";
        const char* name = "";
        if (data && data->IsObject()) {
            if (data->HasChild("f") && data->Child("f")->IsString()) {
                fromRoom = data->Child("f")->AsString().c_str();
            }
            if (data->HasChild("r") && data->Child("r")->IsString()) {
                toRoom = data->Child("r")->AsString().c_str();
            }
            if (data->HasChild("n") && data->Child("n")->IsString()) {
                name = data->Child("n")->AsString().c_str();
            }
        }

        RemotePlayer* player = findOrCreatePlayer(playerId);
        if (player) {
            if (strlen(name) > 0) {
                strncpy(player->name, name, sizeof(player->name) - 1);
            }
            strncpy(player->roomId, toRoom, sizeof(player->roomId) - 1);
            player->lastSeen = millis();

            // Check if player entered or left our room
#ifdef T_DECK
            if (gameUI && strlen(currentRoom) > 0) {
                if (strcmp(toRoom, currentRoom) == 0) {
                    char msg[48];
                    snprintf(msg, sizeof(msg), "%s has arrived.", player->name);
                    gameUI->println(msg);
                } else if (strcmp(fromRoom, currentRoom) == 0) {
                    char msg[48];
                    snprintf(msg, sizeof(msg), "%s has left.", player->name);
                    gameUI->println(msg);
                }
            }
#endif
            LOG_INFO("ZorkMesh: Player %s moved from %s to %s", player->name, fromRoom, toRoom);
        }
    }
    else if (strcmp(msgType, "CH") == 0) {
        // Chat message
        const char* message = "";
        const char* name = "";
        if (data && data->IsObject()) {
            if (data->HasChild("m") && data->Child("m")->IsString()) {
                message = data->Child("m")->AsString().c_str();
            }
            if (data->HasChild("n") && data->Child("n")->IsString()) {
                name = data->Child("n")->AsString().c_str();
            }
        }

        RemotePlayer* player = findOrCreatePlayer(playerId);
        if (player) {
            if (strlen(name) > 0) {
                strncpy(player->name, name, sizeof(player->name) - 1);
            }
            player->lastSeen = millis();

#ifdef T_DECK
            if (gameUI) {
                char msg[160];
                snprintf(msg, sizeof(msg), "%s says: \"%s\"", player->name, message);
                gameUI->println(msg);
            }
#endif
            LOG_INFO("ZorkMesh: Chat from %s: %s", player->name, message);
        }
    }
    else if (strcmp(msgType, "HB") == 0) {
        // Heartbeat - just update last seen
        const char* roomId = "";
        if (data && data->IsObject() && data->HasChild("r") && data->Child("r")->IsString()) {
            roomId = data->Child("r")->AsString().c_str();
        }

        RemotePlayer* player = findOrCreatePlayer(playerId);
        if (player) {
            if (strlen(roomId) > 0) {
                strncpy(player->roomId, roomId, sizeof(player->roomId) - 1);
            }
            player->lastSeen = millis();
        }
    }
    else if (strcmp(msgType, "PA") == 0) {
        // Player Action
        const char* verb = "";
        const char* objId = "";
        const char* roomId = "";
        if (data && data->IsObject()) {
            if (data->HasChild("v") && data->Child("v")->IsString()) {
                verb = data->Child("v")->AsString().c_str();
            }
            if (data->HasChild("o") && data->Child("o")->IsString()) {
                objId = data->Child("o")->AsString().c_str();
            }
            if (data->HasChild("r") && data->Child("r")->IsString()) {
                roomId = data->Child("r")->AsString().c_str();
            }
        }

        RemotePlayer* player = findOrCreatePlayer(playerId);
        if (player) {
            player->lastSeen = millis();

            // Only show if in same room
#ifdef T_DECK
            if (gameUI && strlen(currentRoom) > 0 && strcmp(roomId, currentRoom) == 0) {
                char msg[80];
                if (strlen(objId) > 0) {
                    snprintf(msg, sizeof(msg), "%s %ss the %s.", player->name, verb, objId);
                } else {
                    snprintf(msg, sizeof(msg), "%s %ss.", player->name, verb);
                }
                gameUI->println(msg);
            }
#endif
        }
    }

    // Clean up
    delete root;

    // Wake screen on game message
    powerFSM.trigger(EVENT_RECEIVED_MSG);

    return ProcessMessage::CONTINUE; // Let other modules see this too
}
