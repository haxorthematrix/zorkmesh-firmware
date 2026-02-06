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
        zorkMeshModuleRadio->sendMove(gameEngine->getPlayerName(), newRoom);
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

    // Send periodic heartbeat when game is active
    uint32_t now = millis();
    if (now - lastHeartbeat >= HEARTBEAT_INTERVAL_MS) {
        lastHeartbeat = now;
        // TODO: Send heartbeat with current player info
        LOG_DEBUG("ZorkMesh heartbeat");
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

void ZorkMeshModuleRadio::sendJoin(const char* playerName, const char* roomId)
{
    // Generate player ID (6-char hex hash of name)
    char playerId[7];
    uint32_t hash = 0;
    for (const char* c = playerName; *c; c++) {
        hash = hash * 31 + *c;
    }
    snprintf(playerId, sizeof(playerId), "%06X", hash & 0xFFFFFF);

    char payload[128];
    snprintf(payload, sizeof(payload),
             "{\"t\":\"PJ\",\"p\":\"%s\",\"n\":\"%s\",\"r\":\"%s\"}",
             playerId, playerName, roomId);
    sendGameMessage(payload);
}

void ZorkMeshModuleRadio::sendLeave(const char* playerName)
{
    char playerId[7];
    uint32_t hash = 0;
    for (const char* c = playerName; *c; c++) {
        hash = hash * 31 + *c;
    }
    snprintf(playerId, sizeof(playerId), "%06X", hash & 0xFFFFFF);

    char payload[64];
    snprintf(payload, sizeof(payload),
             "{\"t\":\"PL\",\"p\":\"%s\"}",
             playerId);
    sendGameMessage(payload);
}

void ZorkMeshModuleRadio::sendMove(const char* playerName, const char* roomId)
{
    char playerId[7];
    uint32_t hash = 0;
    for (const char* c = playerName; *c; c++) {
        hash = hash * 31 + *c;
    }
    snprintf(playerId, sizeof(playerId), "%06X", hash & 0xFFFFFF);

    char payload[64];
    snprintf(payload, sizeof(payload),
             "{\"t\":\"PM\",\"p\":\"%s\",\"r\":\"%s\"}",
             playerId, roomId);
    sendGameMessage(payload);
}

void ZorkMeshModuleRadio::sendChat(const char* playerName, const char* message)
{
    char playerId[7];
    uint32_t hash = 0;
    for (const char* c = playerName; *c; c++) {
        hash = hash * 31 + *c;
    }
    snprintf(playerId, sizeof(playerId), "%06X", hash & 0xFFFFFF);

    char payload[256];
    snprintf(payload, sizeof(payload),
             "{\"t\":\"CH\",\"p\":\"%s\",\"m\":\"%s\"}",
             playerId, message);
    sendGameMessage(payload);
}

void ZorkMeshModuleRadio::sendHeartbeat(const char* playerName, const char* roomId)
{
    char playerId[7];
    uint32_t hash = 0;
    for (const char* c = playerName; *c; c++) {
        hash = hash * 31 + *c;
    }
    snprintf(playerId, sizeof(playerId), "%06X", hash & 0xFFFFFF);

    char payload[64];
    snprintf(payload, sizeof(payload),
             "{\"t\":\"HB\",\"p\":\"%s\",\"r\":\"%s\"}",
             playerId, roomId);
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

    // TODO: Parse JSON and dispatch to game engine
    // Message types: PJ (join), PL (leave), PM (move), CH (chat), HB (heartbeat), PA (action)

    // Wake screen on game message
    powerFSM.trigger(EVENT_RECEIVED_MSG);

    return ProcessMessage::CONTINUE; // Let other modules see this too
}
