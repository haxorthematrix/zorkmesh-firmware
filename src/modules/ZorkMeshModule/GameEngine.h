#pragma once

#include <Arduino.h>
#include <vector>
#include <map>

/**
 * GameEngine - Core Zork game logic for T-Deck
 *
 * Manages game state, room navigation, inventory, and puzzles.
 * Compatible with PyMeshZork multiplayer protocol.
 */

// Maximum sizes
#define MAX_INVENTORY 20
#define MAX_ROOM_ID_LEN 16
#define MAX_NAME_LEN 64
#define MAX_DESC_LEN 512

// Room flags
enum RoomFlags {
    RFLAG_LIGHT = 0x01,    // Room has natural light
    RFLAG_LAND = 0x02,     // Room is on land (vs water)
    RFLAG_WATER = 0x04,    // Room is water
    RFLAG_SACRED = 0x08,   // No fighting allowed
    RFLAG_MAZE = 0x10      // Maze room (confusing)
};

// Object flags
enum ObjectFlags {
    OFLAG_TAKEABLE = 0x01,    // Can be picked up
    OFLAG_CONTAINER = 0x02,   // Can hold other objects
    OFLAG_OPEN = 0x04,        // Currently open
    OFLAG_LOCKED = 0x08,      // Currently locked
    OFLAG_ON = 0x10,          // Light source is on
    OFLAG_READABLE = 0x20,    // Can be read
    OFLAG_WEAPON = 0x40       // Can be used as weapon
};

// Direction enumeration
enum Direction {
    DIR_NORTH = 0,
    DIR_SOUTH,
    DIR_EAST,
    DIR_WEST,
    DIR_NORTHEAST,
    DIR_NORTHWEST,
    DIR_SOUTHEAST,
    DIR_SOUTHWEST,
    DIR_UP,
    DIR_DOWN,
    DIR_IN,
    DIR_OUT,
    DIR_COUNT
};

// Exit structure
struct RoomExit {
    Direction direction;
    const char* destination;  // Room ID
    bool blocked;
    const char* blockMessage;
};

// Room structure
struct Room {
    const char* id;
    const char* name;
    const char* description;
    const char* shortDesc;
    uint8_t flags;
    RoomExit exits[DIR_COUNT];
    uint8_t numExits;
};

// Object structure
struct GameObject {
    const char* id;
    const char* name;
    const char* description;
    const char* location;     // Room ID or "inventory" or container ID
    uint8_t flags;
    uint16_t weight;
};

// Remote player info (from other mesh nodes)
struct RemotePlayer {
    char playerId[8];
    char name[MAX_NAME_LEN];
    char roomId[MAX_ROOM_ID_LEN];
    uint32_t lastSeen;        // millis() timestamp
};

/**
 * GameEngine class
 */
class GameEngine {
public:
    GameEngine();
    ~GameEngine();

    // Initialize the game
    bool init();

    // Reset game to starting state
    void reset();

    // Process a player command, returns response text
    String processCommand(const char* command);

    // Get current room info
    const Room* getCurrentRoom() const { return currentRoom; }
    const char* getCurrentRoomId() const { return currentRoomId; }

    // Get player name
    const char* getPlayerName() const { return playerName; }
    void setPlayerName(const char* name);

    // Get player ID (6-char hex)
    const char* getPlayerId() const { return playerId; }

    // Inventory management
    int getInventoryCount() const { return inventoryCount; }
    const char* getInventoryItem(int index) const;
    bool hasItem(const char* itemId) const;

    // Score and moves
    int getScore() const { return score; }
    int getMoves() const { return moves; }

    // Remote player tracking
    void updateRemotePlayer(const char* pid, const char* name, const char* roomId);
    void removeRemotePlayer(const char* pid);
    std::vector<RemotePlayer*> getPlayersInRoom(const char* roomId);

    // Save/Load game state (to NVS)
    bool saveGame();
    bool loadGame();

private:
    // Current state
    char currentRoomId[MAX_ROOM_ID_LEN];
    const Room* currentRoom;
    char playerName[MAX_NAME_LEN];
    char playerId[8];

    // Inventory (object IDs)
    char inventory[MAX_INVENTORY][MAX_ROOM_ID_LEN];
    int inventoryCount;

    // Score and statistics
    int score;
    int moves;

    // Remote players
    std::map<String, RemotePlayer> remotePlayers;

    // Internal methods
    const Room* findRoom(const char* roomId);
    GameObject* findObject(const char* objectId);
    bool moveToRoom(const char* roomId);

    // Command handlers
    String cmdLook();
    String cmdGo(Direction dir);
    String cmdTake(const char* objectName);
    String cmdDrop(const char* objectName);
    String cmdInventory();
    String cmdExamine(const char* objectName);
    String cmdOpen(const char* objectName);
    String cmdClose(const char* objectName);
    String cmdRead(const char* objectName);

    // Parse direction from string
    Direction parseDirection(const char* dirStr);

    // Generate player ID from name
    void generatePlayerId();
};

// Global instance
extern GameEngine *gameEngine;
