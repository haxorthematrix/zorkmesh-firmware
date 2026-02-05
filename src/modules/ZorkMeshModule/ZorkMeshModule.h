#pragma once

#include "SinglePortModule.h"
#include "concurrency/OSThread.h"

/**
 * ZorkMesh Module - Multiplayer Zork game over Meshtastic mesh network
 *
 * Uses PRIVATE_APP port (256) for game messages.
 * Protocol: Compact JSON matching PyMeshZork format
 * Message types: PJ (join), PL (leave), PM (move), CH (chat), HB (heartbeat), PA (action)
 */

// Forward declarations
class ZorkMeshModuleRadio;

/**
 * Main module thread - handles periodic tasks like heartbeat
 */
class ZorkMeshModule : public concurrency::OSThread
{
  public:
    ZorkMeshModule();

    // Start the game
    void startGame();

    // Stop the game
    void stopGame();

    // Check if game is active
    bool isGameActive() const { return gameActive; }

  protected:
    virtual int32_t runOnce() override;

  private:
    bool gameActive = false;
    bool firstTime = true;
    uint32_t lastHeartbeat = 0;

    static const uint32_t HEARTBEAT_INTERVAL_MS = 60000; // 1 minute
};

/**
 * Radio handler - receives and sends mesh packets
 */
class ZorkMeshModuleRadio : public SinglePortModule
{
  public:
    ZorkMeshModuleRadio();

    // Send a game message to the mesh
    void sendGameMessage(const char* jsonPayload);

    // Send player join message
    void sendJoin(const char* playerName, const char* roomId);

    // Send player leave message
    void sendLeave(const char* playerName);

    // Send player move message
    void sendMove(const char* playerName, const char* roomId);

    // Send chat message
    void sendChat(const char* playerName, const char* message);

    // Send heartbeat
    void sendHeartbeat(const char* playerName, const char* roomId);

  protected:
    virtual ProcessMessage handleReceived(const meshtastic_MeshPacket &mp) override;
};

// Global instances
extern ZorkMeshModule *zorkMeshModule;
extern ZorkMeshModuleRadio *zorkMeshModuleRadio;
