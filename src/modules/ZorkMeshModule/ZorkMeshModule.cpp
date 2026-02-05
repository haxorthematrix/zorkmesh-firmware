#include "ZorkMeshModule.h"
#include "GameEngine.h"
#ifdef T_DECK
#include "GameUI.h"
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
        gameUI->show();
        uiVisible = true;

        // Start observing keyboard input when UI is visible
        if (inputBroker) {
            inputObserver.observe(inputBroker);
        }

        // Show initial room description
        if (gameEngine) {
            String look = gameEngine->processCommand("LOOK");
            gameUI->println(look.c_str());
            gameUI->setRoomName(gameEngine->getCurrentRoom() ?
                gameEngine->getCurrentRoom()->name : "Unknown");
            gameUI->setScore(gameEngine->getScore(), gameEngine->getMoves());
        }
    }
#endif
}

void ZorkMeshModule::hideGameUI()
{
#ifdef T_DECK
    uiVisible = false;

    if (gameUI) {
        gameUI->hide();
    }
#endif
}

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
int ZorkMeshModule::handleInputEvent(const InputEvent* event)
{
    // Only handle input when game UI is visible
    if (!uiVisible || !gameUI) {
        return 0; // Let other handlers process this
    }

    // Handle the keyboard character
    if (event->kbchar != 0) {
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
        firstTime = false;
    }

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
