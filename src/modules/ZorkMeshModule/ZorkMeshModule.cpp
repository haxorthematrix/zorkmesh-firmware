#include "ZorkMeshModule.h"
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
        gameActive = true;
        LOG_INFO("ZorkMesh game started");
    }
}

void ZorkMeshModule::stopGame()
{
    if (gameActive) {
        gameActive = false;
        LOG_INFO("ZorkMesh game stopped");
    }
}

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
