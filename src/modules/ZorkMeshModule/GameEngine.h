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
#define MAX_OBJECTS 100
#define MAX_SCORE 350

// Special location constants
#define LOC_INVENTORY "inventory"
#define LOC_NOWHERE nullptr
#define LOC_TROPHY_CASE "troph"

// Lamp constants
#define LAMP_MAX_LIFE 350
#define LAMP_DIM_THRESHOLD 30
#define LAMP_CRITICAL_THRESHOLD 10

// Villain IDs
#define VILLAIN_TROLL "troll"
#define VILLAIN_THIEF "thief"
#define VILLAIN_CYCLOPS "cyclo"

// Villain health values
#define TROLL_HEALTH 5
#define THIEF_HEALTH 7
#define CYCLOPS_HEALTH 127  // Max int8_t - can't be killed by normal combat

// Room flags
enum RoomFlags {
    RFLAG_LIGHT = 0x01,    // Room has natural light
    RFLAG_LAND = 0x02,     // Room is on land (vs water)
    RFLAG_WATER = 0x04,    // Room is water
    RFLAG_SACRED = 0x08,   // No fighting allowed
    RFLAG_MAZE = 0x10      // Maze room (confusing)
};

// Object flags (basic - extended flags defined in GameData.h)
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

// Game state flags for puzzles
struct GameFlags {
    bool rugMoved;        // Rug moved, trap door visible
    bool trollGone;       // Troll defeated
    bool grateOpen;       // Grating unlocked
    bool ropeTied;        // Rope tied to railing in dome
    bool gatesOpen;       // Gates of Hades open
    bool cyclopsGone;     // Cyclops defeated/fled
    bool lampOn;          // Lamp is on
    bool thiefActive;     // Thief is wandering

    GameFlags() : rugMoved(false), trollGone(false), grateOpen(false),
                  ropeTied(false), gatesOpen(false), cyclopsGone(false),
                  lampOn(false), thiefActive(false) {}
};

// Villain state
struct VillainState {
    int8_t health;        // Current health (-1 = dead)
    char roomId[MAX_ROOM_ID_LEN];  // Current location
    bool active;          // Is actively tracking player

    VillainState() : health(0), active(false) {
        roomId[0] = '\0';
    }
};

// Remote player info (from other mesh nodes)
struct RemotePlayer {
    char playerId[8];
    char name[MAX_NAME_LEN];
    char roomId[MAX_ROOM_ID_LEN];
    uint32_t lastSeen;        // millis() timestamp
    bool active;              // Is this slot in use
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

    // Reset location only (keep inventory and score)
    void resetLocation();

    // Reset inventory only (drop all items, keep location)
    void resetInventory();

    // Check if first run (no saved username)
    bool isFirstRun() const;

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
    int getInventoryCount() const;
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

    // Light/Darkness
    bool isRoomLit() const;
    bool hasLightSource() const;
    int getLampLife() const { return lampLife; }

    // Game flags
    const GameFlags& getFlags() const { return gameFlags; }

    // Combat
    bool isVillainHere(const char* villainId) const;

private:
    // Current state
    char currentRoomId[MAX_ROOM_ID_LEN];
    const Room* currentRoom;
    char playerName[MAX_NAME_LEN];
    char playerId[8];

    // Score and statistics
    int score;
    int moves;
    int lampLife;         // Lamp battery life remaining

    // Game state flags
    GameFlags gameFlags;

    // Villain states
    VillainState trollState;
    VillainState thiefState;
    VillainState cyclopsState;

    // Remote players
    std::map<String, RemotePlayer> remotePlayers;

    // Object state (mutable - changes during gameplay)
    char objectLocations[MAX_OBJECTS][MAX_ROOM_ID_LEN];  // Current location of each object
    uint16_t objectFlags[MAX_OBJECTS];                    // Current flags for each object

    // Internal methods
    const Room* findRoom(const char* roomId);
    bool moveToRoom(const char* roomId);
    void initObjectState();

    // Object finding
    int findObjectByName(const char* name, bool inInventory, bool inRoom);
    int findObjectInContainer(int containerIdx, const char* name);
    bool isObjectVisible(int objIdx);
    bool isObjectHere(int objIdx);  // In current room or containers in room
    bool isObjectInInventory(int objIdx);
    const char* getObjectLocation(int objIdx);
    void setObjectLocation(int objIdx, const char* location);

    // Get objects at a location
    std::vector<int> getObjectsAtLocation(const char* location);

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
    String cmdLight(const char* objectName);
    String cmdExtinguish(const char* objectName);
    String cmdAttack(const char* target, const char* weapon);
    String cmdMove(const char* objectName);
    String cmdTie(const char* objectName);
    String cmdUnlock(const char* objectName);
    String cmdSay(const char* word);
    String cmdWave(const char* objectName);
    String cmdRing(const char* objectName);

    // Parse direction from string
    Direction parseDirection(const char* dirStr);

    // Light/darkness helpers
    bool checkDarkness();  // Returns true if too dark to proceed
    void tickLamp();       // Decrease lamp life

    // Villain helpers
    void initVillains();
    void tickVillains();   // Villain AI each turn
    String villainAttack(const char* villainId);
    bool checkVillainBlock(Direction dir);  // Does villain block exit?
    int findVillainByName(const char* name);

    // Score helpers
    void checkTreasureScore();  // Award points for treasures in trophy case

    // Generate player ID from name
    void generatePlayerId();
};

// Global instance
extern GameEngine *gameEngine;
