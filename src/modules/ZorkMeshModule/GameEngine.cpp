#include "GameEngine.h"
#include "GameData.h"
#include "configuration.h"
#include <Preferences.h>
#include <cstring>

// Global instance
GameEngine *gameEngine = nullptr;

// NVS namespace for save data
static const char* NVS_NAMESPACE = "zorkmesh";

GameEngine::GameEngine()
    : currentRoom(nullptr)
    , inventoryCount(0)
    , score(0)
    , moves(0)
{
    memset(currentRoomId, 0, sizeof(currentRoomId));
    memset(playerName, 0, sizeof(playerName));
    memset(playerId, 0, sizeof(playerId));
    memset(inventory, 0, sizeof(inventory));
}

GameEngine::~GameEngine()
{
}

bool GameEngine::init()
{
    LOG_INFO("GameEngine initializing");

    // Try to load saved game
    if (!loadGame()) {
        // Start fresh
        reset();
    }

    return true;
}

void GameEngine::reset()
{
    // Start in West of House
    strncpy(currentRoomId, "whous", sizeof(currentRoomId) - 1);
    currentRoom = findRoom(currentRoomId);

    // Clear inventory
    memset(inventory, 0, sizeof(inventory));
    inventoryCount = 0;

    // Reset score
    score = 0;
    moves = 0;

    LOG_INFO("GameEngine reset to starting state");
}

void GameEngine::setPlayerName(const char* name)
{
    strncpy(playerName, name, sizeof(playerName) - 1);
    playerName[sizeof(playerName) - 1] = '\0';
    generatePlayerId();
    LOG_INFO("Player name set: %s (ID: %s)", playerName, playerId);
}

void GameEngine::generatePlayerId()
{
    // Generate 6-char hex hash of name (matching PyMeshZork)
    uint32_t hash = 0;
    for (const char* c = playerName; *c; c++) {
        hash = hash * 31 + *c;
    }
    snprintf(playerId, sizeof(playerId), "%06X", hash & 0xFFFFFF);
}

String GameEngine::processCommand(const char* command)
{
    moves++;

    // Parse command
    char cmdCopy[128];
    strncpy(cmdCopy, command, sizeof(cmdCopy) - 1);
    cmdCopy[sizeof(cmdCopy) - 1] = '\0';

    // Convert to uppercase for matching
    for (char* p = cmdCopy; *p; p++) {
        *p = toupper(*p);
    }

    // Tokenize
    char* verb = strtok(cmdCopy, " ");
    char* noun = strtok(nullptr, " ");

    if (!verb) {
        return "I beg your pardon?";
    }

    // Direction shortcuts
    if (strcmp(verb, "N") == 0 || strcmp(verb, "NORTH") == 0) return cmdGo(DIR_NORTH);
    if (strcmp(verb, "S") == 0 || strcmp(verb, "SOUTH") == 0) return cmdGo(DIR_SOUTH);
    if (strcmp(verb, "E") == 0 || strcmp(verb, "EAST") == 0) return cmdGo(DIR_EAST);
    if (strcmp(verb, "W") == 0 || strcmp(verb, "WEST") == 0) return cmdGo(DIR_WEST);
    if (strcmp(verb, "NE") == 0 || strcmp(verb, "NORTHEAST") == 0) return cmdGo(DIR_NORTHEAST);
    if (strcmp(verb, "NW") == 0 || strcmp(verb, "NORTHWEST") == 0) return cmdGo(DIR_NORTHWEST);
    if (strcmp(verb, "SE") == 0 || strcmp(verb, "SOUTHEAST") == 0) return cmdGo(DIR_SOUTHEAST);
    if (strcmp(verb, "SW") == 0 || strcmp(verb, "SOUTHWEST") == 0) return cmdGo(DIR_SOUTHWEST);
    if (strcmp(verb, "U") == 0 || strcmp(verb, "UP") == 0) return cmdGo(DIR_UP);
    if (strcmp(verb, "D") == 0 || strcmp(verb, "DOWN") == 0) return cmdGo(DIR_DOWN);
    if (strcmp(verb, "IN") == 0 || strcmp(verb, "ENTER") == 0) return cmdGo(DIR_IN);
    if (strcmp(verb, "OUT") == 0 || strcmp(verb, "EXIT") == 0) return cmdGo(DIR_OUT);

    // Standard verbs
    if (strcmp(verb, "LOOK") == 0 || strcmp(verb, "L") == 0) return cmdLook();
    if (strcmp(verb, "GO") == 0 && noun) return cmdGo(parseDirection(noun));
    if (strcmp(verb, "TAKE") == 0 || strcmp(verb, "GET") == 0) return cmdTake(noun);
    if (strcmp(verb, "DROP") == 0 || strcmp(verb, "PUT") == 0) return cmdDrop(noun);
    if (strcmp(verb, "INVENTORY") == 0 || strcmp(verb, "I") == 0) return cmdInventory();
    if (strcmp(verb, "EXAMINE") == 0 || strcmp(verb, "X") == 0) return cmdExamine(noun);
    if (strcmp(verb, "OPEN") == 0) return cmdOpen(noun);
    if (strcmp(verb, "CLOSE") == 0) return cmdClose(noun);
    if (strcmp(verb, "READ") == 0) return cmdRead(noun);

    // Special commands
    if (strcmp(verb, "SCORE") == 0) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Your score is %d in %d moves.", score, moves);
        return String(buf);
    }

    if (strcmp(verb, "QUIT") == 0 || strcmp(verb, "Q") == 0) {
        return "Use the menu to exit the game.";
    }

    if (strcmp(verb, "SAVE") == 0) {
        if (saveGame()) {
            return "Game saved.";
        } else {
            return "Save failed.";
        }
    }

    if (strcmp(verb, "RESTORE") == 0 || strcmp(verb, "LOAD") == 0) {
        if (loadGame()) {
            return "Game restored.\n\n" + cmdLook();
        } else {
            return "No saved game found.";
        }
    }

    if (strcmp(verb, "HELP") == 0 || strcmp(verb, "?") == 0) {
        return "Commands: LOOK, GO <dir>, TAKE, DROP, INVENTORY, EXAMINE, OPEN, CLOSE, READ, SCORE, SAVE, RESTORE\n"
               "Directions: N, S, E, W, NE, NW, SE, SW, UP, DOWN, IN, OUT";
    }

    return "I don't understand that.";
}

String GameEngine::cmdLook()
{
    if (!currentRoom) {
        return "You are nowhere.";
    }

    String result = String(currentRoom->name) + "\n\n";
    result += currentRoom->description;

    // Show exits
    result += "\n\nExits: ";
    bool first = true;
    for (int i = 0; i < DIR_COUNT; i++) {
        if (currentRoom->exits[i].destination && !currentRoom->exits[i].blocked) {
            if (!first) result += ", ";
            first = false;
            switch (i) {
                case DIR_NORTH: result += "north"; break;
                case DIR_SOUTH: result += "south"; break;
                case DIR_EAST: result += "east"; break;
                case DIR_WEST: result += "west"; break;
                case DIR_NORTHEAST: result += "northeast"; break;
                case DIR_NORTHWEST: result += "northwest"; break;
                case DIR_SOUTHEAST: result += "southeast"; break;
                case DIR_SOUTHWEST: result += "southwest"; break;
                case DIR_UP: result += "up"; break;
                case DIR_DOWN: result += "down"; break;
                case DIR_IN: result += "in"; break;
                case DIR_OUT: result += "out"; break;
            }
        }
    }
    if (first) result += "none";

    // Show other players in room
    auto players = getPlayersInRoom(currentRoomId);
    for (auto* p : players) {
        result += "\n";
        result += p->name;
        result += " is here.";
    }

    return result;
}

String GameEngine::cmdGo(Direction dir)
{
    if (dir < 0 || dir >= DIR_COUNT) {
        return "I don't know how to go that way.";
    }

    if (!currentRoom) {
        return "You can't go anywhere from here.";
    }

    const RoomExit& exit = currentRoom->exits[dir];
    if (!exit.destination) {
        return "You can't go that way.";
    }

    if (exit.blocked && exit.blockMessage) {
        return String(exit.blockMessage);
    }

    if (!moveToRoom(exit.destination)) {
        return "You can't go that way.";
    }

    // Auto-look on room change
    return cmdLook();
}

bool GameEngine::moveToRoom(const char* roomId)
{
    const Room* room = findRoom(roomId);
    if (!room) {
        LOG_WARN("Room not found: %s", roomId);
        return false;
    }

    strncpy(currentRoomId, roomId, sizeof(currentRoomId) - 1);
    currentRoom = room;

    LOG_DEBUG("Moved to room: %s", roomId);
    return true;
}

String GameEngine::cmdTake(const char* objectName)
{
    if (!objectName) {
        return "Take what?";
    }

    if (inventoryCount >= MAX_INVENTORY) {
        return "You're carrying too many things.";
    }

    // TODO: Find object by name and add to inventory
    return "You don't see that here.";
}

String GameEngine::cmdDrop(const char* objectName)
{
    if (!objectName) {
        return "Drop what?";
    }

    // TODO: Find object in inventory and drop it
    return "You're not carrying that.";
}

String GameEngine::cmdInventory()
{
    if (inventoryCount == 0) {
        return "You are empty-handed.";
    }

    String result = "You are carrying:\n";
    for (int i = 0; i < inventoryCount; i++) {
        // TODO: Get object name from ID
        result += "  ";
        result += inventory[i];
        result += "\n";
    }
    return result;
}

String GameEngine::cmdExamine(const char* objectName)
{
    if (!objectName) {
        return "Examine what?";
    }

    // TODO: Find object and return description
    return "You see nothing special about that.";
}

String GameEngine::cmdOpen(const char* objectName)
{
    if (!objectName) {
        return "Open what?";
    }

    // TODO: Implement open logic
    return "You can't open that.";
}

String GameEngine::cmdClose(const char* objectName)
{
    if (!objectName) {
        return "Close what?";
    }

    // TODO: Implement close logic
    return "You can't close that.";
}

String GameEngine::cmdRead(const char* objectName)
{
    if (!objectName) {
        return "Read what?";
    }

    // TODO: Implement read logic
    return "There's nothing written on that.";
}

Direction GameEngine::parseDirection(const char* dirStr)
{
    if (!dirStr) return (Direction)-1;

    if (strcmp(dirStr, "N") == 0 || strcmp(dirStr, "NORTH") == 0) return DIR_NORTH;
    if (strcmp(dirStr, "S") == 0 || strcmp(dirStr, "SOUTH") == 0) return DIR_SOUTH;
    if (strcmp(dirStr, "E") == 0 || strcmp(dirStr, "EAST") == 0) return DIR_EAST;
    if (strcmp(dirStr, "W") == 0 || strcmp(dirStr, "WEST") == 0) return DIR_WEST;
    if (strcmp(dirStr, "NE") == 0 || strcmp(dirStr, "NORTHEAST") == 0) return DIR_NORTHEAST;
    if (strcmp(dirStr, "NW") == 0 || strcmp(dirStr, "NORTHWEST") == 0) return DIR_NORTHWEST;
    if (strcmp(dirStr, "SE") == 0 || strcmp(dirStr, "SOUTHEAST") == 0) return DIR_SOUTHEAST;
    if (strcmp(dirStr, "SW") == 0 || strcmp(dirStr, "SOUTHWEST") == 0) return DIR_SOUTHWEST;
    if (strcmp(dirStr, "U") == 0 || strcmp(dirStr, "UP") == 0) return DIR_UP;
    if (strcmp(dirStr, "D") == 0 || strcmp(dirStr, "DOWN") == 0) return DIR_DOWN;
    if (strcmp(dirStr, "IN") == 0 || strcmp(dirStr, "ENTER") == 0) return DIR_IN;
    if (strcmp(dirStr, "OUT") == 0 || strcmp(dirStr, "EXIT") == 0) return DIR_OUT;

    return (Direction)-1;
}

const Room* GameEngine::findRoom(const char* roomId)
{
    // Search in game data
    for (int i = 0; i < ROOM_COUNT; i++) {
        if (strcmp(ROOMS[i].id, roomId) == 0) {
            return &ROOMS[i];
        }
    }
    return nullptr;
}

GameObject* GameEngine::findObject(const char* objectId)
{
    // TODO: Search in game data
    return nullptr;
}

const char* GameEngine::getInventoryItem(int index) const
{
    if (index < 0 || index >= inventoryCount) {
        return nullptr;
    }
    return inventory[index];
}

bool GameEngine::hasItem(const char* itemId) const
{
    for (int i = 0; i < inventoryCount; i++) {
        if (strcmp(inventory[i], itemId) == 0) {
            return true;
        }
    }
    return false;
}

void GameEngine::updateRemotePlayer(const char* pid, const char* name, const char* roomId)
{
    String key(pid);
    RemotePlayer& player = remotePlayers[key];

    strncpy(player.playerId, pid, sizeof(player.playerId) - 1);
    strncpy(player.name, name, sizeof(player.name) - 1);
    strncpy(player.roomId, roomId, sizeof(player.roomId) - 1);
    player.lastSeen = millis();

    LOG_DEBUG("Remote player updated: %s (%s) in %s", name, pid, roomId);
}

void GameEngine::removeRemotePlayer(const char* pid)
{
    String key(pid);
    remotePlayers.erase(key);
    LOG_DEBUG("Remote player removed: %s", pid);
}

std::vector<RemotePlayer*> GameEngine::getPlayersInRoom(const char* roomId)
{
    std::vector<RemotePlayer*> result;
    uint32_t now = millis();

    for (auto& kv : remotePlayers) {
        // Skip stale players (no heartbeat in 5 minutes)
        if (now - kv.second.lastSeen > 300000) {
            continue;
        }
        if (strcmp(kv.second.roomId, roomId) == 0) {
            result.push_back(&kv.second);
        }
    }
    return result;
}

bool GameEngine::saveGame()
{
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, false)) {
        LOG_ERROR("Failed to open NVS for save");
        return false;
    }

    prefs.putString("room", currentRoomId);
    prefs.putString("player", playerName);
    prefs.putInt("score", score);
    prefs.putInt("moves", moves);
    prefs.putInt("invCount", inventoryCount);

    // Save inventory
    for (int i = 0; i < inventoryCount; i++) {
        char key[16];
        snprintf(key, sizeof(key), "inv%d", i);
        prefs.putString(key, inventory[i]);
    }

    prefs.end();
    LOG_INFO("Game saved: room=%s, score=%d, moves=%d", currentRoomId, score, moves);
    return true;
}

bool GameEngine::loadGame()
{
    Preferences prefs;
    if (!prefs.begin(NVS_NAMESPACE, true)) {
        LOG_DEBUG("No saved game found");
        return false;
    }

    if (!prefs.isKey("room")) {
        prefs.end();
        return false;
    }

    String room = prefs.getString("room", "whous");
    String player = prefs.getString("player", "");

    strncpy(currentRoomId, room.c_str(), sizeof(currentRoomId) - 1);
    currentRoom = findRoom(currentRoomId);

    if (player.length() > 0) {
        setPlayerName(player.c_str());
    }

    score = prefs.getInt("score", 0);
    moves = prefs.getInt("moves", 0);
    inventoryCount = prefs.getInt("invCount", 0);

    // Load inventory
    for (int i = 0; i < inventoryCount && i < MAX_INVENTORY; i++) {
        char key[16];
        snprintf(key, sizeof(key), "inv%d", i);
        String item = prefs.getString(key, "");
        strncpy(inventory[i], item.c_str(), sizeof(inventory[i]) - 1);
    }

    prefs.end();
    LOG_INFO("Game loaded: room=%s, score=%d, moves=%d", currentRoomId, score, moves);
    return true;
}
