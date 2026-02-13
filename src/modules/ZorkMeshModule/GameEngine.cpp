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
    , score(0)
    , moves(0)
    , lampLife(LAMP_MAX_LIFE)
{
    memset(currentRoomId, 0, sizeof(currentRoomId));
    memset(playerName, 0, sizeof(playerName));
    memset(playerId, 0, sizeof(playerId));
    memset(objectLocations, 0, sizeof(objectLocations));
    memset(objectFlags, 0, sizeof(objectFlags));
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

    // Initialize objects from game data
    initObjectState();

    // Reset score and lamp
    score = 0;
    moves = 0;
    lampLife = LAMP_MAX_LIFE;

    // Reset game flags
    gameFlags = GameFlags();

    // Initialize villains
    initVillains();

    LOG_INFO("GameEngine reset to starting state");
}

void GameEngine::resetLocation()
{
    // Reset player to starting room, keep inventory and score
    strncpy(currentRoomId, "whous", sizeof(currentRoomId) - 1);
    currentRoom = findRoom(currentRoomId);

    // Reset lamp if it was on (to save battery)
    if (gameFlags.lampOn) {
        gameFlags.lampOn = false;
    }

    saveGame();
    LOG_INFO("Location reset to West of House");
}

void GameEngine::resetInventory()
{
    // Drop all items in current room
    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (objectLocations[i][0] != '\0' &&
            strcmp(objectLocations[i], LOC_INVENTORY) == 0) {
            // Drop item in current room
            strncpy(objectLocations[i], currentRoomId, sizeof(objectLocations[i]) - 1);
        }
    }

    saveGame();
    LOG_INFO("Inventory reset - all items dropped");
}

bool GameEngine::isFirstRun() const
{
    // First run if no player name has been set
    return playerName[0] == '\0';
}

void GameEngine::initVillains()
{
    // Troll starts in Troll Room
    trollState.health = TROLL_HEALTH;
    strncpy(trollState.roomId, "mtrol", sizeof(trollState.roomId) - 1);
    trollState.active = true;

    // Thief starts in Treasure Room (his lair)
    thiefState.health = THIEF_HEALTH;
    strncpy(thiefState.roomId, "treas", sizeof(thiefState.roomId) - 1);
    thiefState.active = false;  // Becomes active when player enters underground

    // Cyclops starts in Cyclops Room
    cyclopsState.health = CYCLOPS_HEALTH;
    strncpy(cyclopsState.roomId, "mcycl", sizeof(cyclopsState.roomId) - 1);
    cyclopsState.active = true;
}

void GameEngine::initObjectState()
{
    // Initialize object locations and flags from static data
    for (int i = 0; i < OBJECT_COUNT && i < MAX_OBJECTS; i++) {
        // Copy initial location
        if (OBJECTS[i].initialLocation) {
            strncpy(objectLocations[i], OBJECTS[i].initialLocation,
                    sizeof(objectLocations[i]) - 1);
        } else {
            objectLocations[i][0] = '\0';  // Nowhere
        }
        // Copy initial flags
        objectFlags[i] = OBJECTS[i].flags;
    }
    LOG_DEBUG("Initialized %d objects", OBJECT_COUNT);
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
    String warnings = "";

    // Tick lamp if on
    if (gameFlags.lampOn) {
        tickLamp();

        // Lamp warnings
        if (lampLife == LAMP_DIM_THRESHOLD) {
            warnings = "Your lamp is getting dim.\n\n";
        } else if (lampLife == LAMP_CRITICAL_THRESHOLD) {
            warnings = "Your lamp is nearly dead!\n\n";
        } else if (lampLife <= 0) {
            gameFlags.lampOn = false;
            warnings = "Your lamp has run out of power.\n\n";
        }
    }

    // Check for grue in darkness (before processing command)
    if (!isRoomLit() && checkDarkness()) {
        return "Oh no! You have walked into the slavering fangs of a lurking grue!\n\n*** You have died ***";
    }

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
        return warnings + "I beg your pardon?";
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
        return "Commands: LOOK, GO, TAKE, DROP, I, EXAMINE, OPEN, CLOSE, READ\n"
               "LIGHT, EXTINGUISH, ATTACK, MOVE, TIE, UNLOCK, SAY, WAVE, RING\n"
               "SCORE, SAVE, RESTORE, QUIT\n"
               "Dirs: N S E W NE NW SE SW UP DOWN IN OUT\n"
               "Multiplayer: CHAT <msg>, WHO";
    }

    // Lamp commands
    if (strcmp(verb, "LIGHT") == 0 || strcmp(verb, "TURN") == 0) {
        // Handle "TURN ON LAMP" or "LIGHT LAMP"
        if (noun && (strcmp(noun, "ON") == 0)) {
            char* obj = strtok(nullptr, " ");
            return cmdLight(obj);
        }
        return cmdLight(noun);
    }
    if (strcmp(verb, "EXTINGUISH") == 0 || strcmp(verb, "OFF") == 0) {
        return cmdExtinguish(noun);
    }

    // Combat
    if (strcmp(verb, "ATTACK") == 0 || strcmp(verb, "KILL") == 0 ||
        strcmp(verb, "FIGHT") == 0 || strcmp(verb, "HIT") == 0) {
        char* weapon = strtok(nullptr, " ");  // "WITH sword"
        if (weapon && strcmp(weapon, "WITH") == 0) {
            weapon = strtok(nullptr, " ");
        }
        return cmdAttack(noun, weapon);
    }

    // Physical actions
    if (strcmp(verb, "MOVE") == 0 || strcmp(verb, "LIFT") == 0 || strcmp(verb, "RAISE") == 0) {
        return cmdMove(noun);
    }

    // Tie/untie
    if (strcmp(verb, "TIE") == 0) {
        return cmdTie(noun);
    }

    // Unlock
    if (strcmp(verb, "UNLOCK") == 0) {
        return cmdUnlock(noun);
    }

    // Say (magic words)
    if (strcmp(verb, "SAY") == 0) {
        return cmdSay(noun);
    }

    // Wave (scepter)
    if (strcmp(verb, "WAVE") == 0) {
        return cmdWave(noun);
    }

    // Ring (bell)
    if (strcmp(verb, "RING") == 0) {
        return cmdRing(noun);
    }

    // Wait
    if (strcmp(verb, "WAIT") == 0 || strcmp(verb, "Z") == 0) {
        return "Time passes...";
    }

    // Diagnose
    if (strcmp(verb, "DIAGNOSE") == 0) {
        return "You are in good health.";
    }

    return "I don't understand that.";
}

String GameEngine::cmdLook()
{
    if (!currentRoom) {
        return "You are nowhere.";
    }

    // Check if room is dark
    if (!isRoomLit()) {
        return "It is pitch black. You are likely to be eaten by a grue.";
    }

    String result = String(currentRoom->name) + "\n\n";
    result += currentRoom->description;

    // Show villains in room
    if (isVillainHere(VILLAIN_TROLL)) {
        result += "\nA menacing troll, brandishing a bloody axe, blocks your way.";
    }
    if (isVillainHere(VILLAIN_THIEF)) {
        result += "\nA seedy-looking individual with a large bag lurks in the shadows.";
    }
    if (isVillainHere(VILLAIN_CYCLOPS)) {
        result += "\nA huge cyclops is here, eyeing you suspiciously.";
    }

    // Show objects in room
    auto objects = getObjectsAtLocation(currentRoomId);
    for (int objIdx : objects) {
        // Use object's description if it has one, otherwise format name
        if (OBJECTS[objIdx].description && strlen(OBJECTS[objIdx].description) > 0) {
            result += "\n";
            result += OBJECTS[objIdx].description;
        } else {
            result += "\nThere is a ";
            result += OBJECTS[objIdx].name;
            result += " here.";
        }

        // Show contents of open containers
        if ((objectFlags[objIdx] & OFLAG_CONTAINER) &&
            ((objectFlags[objIdx] & OFLAG_OPEN) || (objectFlags[objIdx] & OFLAG_TRANSPARENT))) {
            auto contents = getObjectsAtLocation(OBJECTS[objIdx].id);
            if (!contents.empty()) {
                result += " The ";
                result += OBJECTS[objIdx].name;
                result += " contains:";
                for (int cIdx : contents) {
                    result += "\n  ";
                    result += OBJECTS[cIdx].name;
                }
            }
        }
    }

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

    // Check if villain blocks exit
    if (checkVillainBlock(dir)) {
        return "The troll fends off your attempt to pass.";
    }

    const RoomExit& exit = currentRoom->exits[dir];
    if (!exit.destination) {
        return "You can't go that way.";
    }

    if (exit.blocked && exit.blockMessage) {
        // Check for conditional exits based on puzzle flags
        bool canPass = false;

        // Trap door requires rug moved
        if (strcmp(currentRoomId, "lroom") == 0 && dir == DIR_DOWN) {
            canPass = gameFlags.rugMoved;
        }
        // Dome requires rope tied
        if (strcmp(currentRoomId, "dome") == 0 && dir == DIR_DOWN) {
            canPass = gameFlags.ropeTied;
        }
        // Grating requires grate open
        if (strstr(exit.blockMessage, "grat") && gameFlags.grateOpen) {
            canPass = true;
        }
        // Gates of Hades
        if (strstr(exit.blockMessage, "gate") && gameFlags.gatesOpen) {
            canPass = true;
        }
        // Cyclops room west exit
        if (strcmp(currentRoomId, "mcycl") == 0 && dir == DIR_WEST) {
            canPass = gameFlags.cyclopsGone;
        }

        if (!canPass) {
            return String(exit.blockMessage);
        }
    }

    if (!moveToRoom(exit.destination)) {
        return "You can't go that way.";
    }

    // Activate thief when entering underground
    if (!gameFlags.thiefActive && currentRoom) {
        // Check if this is an underground room (no natural light)
        if (!(currentRoom->flags & RFLAG_LIGHT)) {
            gameFlags.thiefActive = true;
        }
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

    // Check inventory limit
    if (getInventoryCount() >= MAX_INVENTORY) {
        return "You're carrying too many things.";
    }

    // First check if it's already in inventory
    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx >= 0) {
        return "You're already carrying that.";
    }

    // Find in room
    objIdx = findObjectByName(objectName, false, true);
    if (objIdx < 0) {
        return "You don't see that here.";
    }

    // Check if it's takeable
    if (!(objectFlags[objIdx] & OFLAG_TAKEABLE)) {
        return "You can't take that.";
    }

    // Check if it's an actor
    if (objectFlags[objIdx] & OFLAG_ACTOR) {
        return "I don't think that would be a good idea.";
    }

    // Take it
    setObjectLocation(objIdx, LOC_INVENTORY);

    String result = "Taken: ";
    result += OBJECTS[objIdx].name;
    return result;
}

String GameEngine::cmdDrop(const char* objectName)
{
    if (!objectName) {
        return "Drop what?";
    }

    // Find in inventory
    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        return "You're not carrying that.";
    }

    // Drop it in current room
    setObjectLocation(objIdx, currentRoomId);

    String result = "Dropped: ";
    result += OBJECTS[objIdx].name;

    // Award points for dropping treasures in trophy case
    if (strcmp(currentRoomId, LOC_TROPHY_CASE) == 0 ||
        strcmp(currentRoomId, "lroom") == 0) {  // Living room has trophy case
        if (objectFlags[objIdx] & OFLAG_TREASURE) {
            int value = OBJECTS[objIdx].value;
            if (value > 0) {
                score += value;
                result += "\n[+";
                result += String(value);
                result += " points!]";
            }
        }
    }

    return result;
}

String GameEngine::cmdInventory()
{
    auto items = getObjectsAtLocation(LOC_INVENTORY);

    if (items.empty()) {
        return "You are empty-handed.";
    }

    String result = "You are carrying:\n";
    for (int objIdx : items) {
        result += "  ";
        result += OBJECTS[objIdx].name;

        // Show contents of carried containers
        if ((objectFlags[objIdx] & OFLAG_CONTAINER) &&
            ((objectFlags[objIdx] & OFLAG_OPEN) || (objectFlags[objIdx] & OFLAG_TRANSPARENT))) {
            auto contents = getObjectsAtLocation(OBJECTS[objIdx].id);
            if (!contents.empty()) {
                result += " (containing: ";
                bool first = true;
                for (int cIdx : contents) {
                    if (!first) result += ", ";
                    first = false;
                    result += OBJECTS[cIdx].name;
                }
                result += ")";
            }
        }
        result += "\n";
    }
    return result;
}

int GameEngine::getInventoryCount() const
{
    int count = 0;
    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (objectLocations[i][0] != '\0' &&
            strcmp(objectLocations[i], LOC_INVENTORY) == 0) {
            count++;
        }
    }
    return count;
}

String GameEngine::cmdExamine(const char* objectName)
{
    if (!objectName) {
        return "Examine what?";
    }

    // Try inventory first, then room
    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        objIdx = findObjectByName(objectName, false, true);
    }

    if (objIdx < 0) {
        return "You can't see any such thing.";
    }

    // Return examine text if available, otherwise description
    String result;
    if (OBJECTS[objIdx].examineText && strlen(OBJECTS[objIdx].examineText) > 0) {
        result = OBJECTS[objIdx].examineText;
    } else if (OBJECTS[objIdx].description && strlen(OBJECTS[objIdx].description) > 0) {
        result = OBJECTS[objIdx].description;
    } else {
        result = "You see nothing special about the ";
        result += OBJECTS[objIdx].name;
        result += ".";
    }

    // Show contents of containers
    if (objectFlags[objIdx] & OFLAG_CONTAINER) {
        if (!(objectFlags[objIdx] & OFLAG_OPEN) && !(objectFlags[objIdx] & OFLAG_TRANSPARENT)) {
            result += " It is closed.";
        } else {
            auto contents = getObjectsAtLocation(OBJECTS[objIdx].id);
            if (contents.empty()) {
                result += " It is empty.";
            } else {
                result += " It contains:";
                for (int cIdx : contents) {
                    result += "\n  ";
                    result += OBJECTS[cIdx].name;
                }
            }
        }
    }

    return result;
}

String GameEngine::cmdOpen(const char* objectName)
{
    if (!objectName) {
        return "Open what?";
    }

    // Try inventory first, then room
    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        objIdx = findObjectByName(objectName, false, true);
    }

    if (objIdx < 0) {
        return "You can't see any such thing.";
    }

    // Check if it's a container or door
    if (!(objectFlags[objIdx] & OFLAG_CONTAINER) && !(objectFlags[objIdx] & OFLAG_DOOR)) {
        return "You can't open that.";
    }

    // Check if locked
    if (objectFlags[objIdx] & OFLAG_LOCKED) {
        return "It's locked.";
    }

    // Check if already open
    if (objectFlags[objIdx] & OFLAG_OPEN) {
        return "It's already open.";
    }

    // Open it
    objectFlags[objIdx] |= OFLAG_OPEN;

    String result = "Opened.";

    // Show contents
    if (objectFlags[objIdx] & OFLAG_CONTAINER) {
        auto contents = getObjectsAtLocation(OBJECTS[objIdx].id);
        if (!contents.empty()) {
            result += " The ";
            result += OBJECTS[objIdx].name;
            result += " contains:";
            for (int cIdx : contents) {
                result += "\n  ";
                result += OBJECTS[cIdx].name;
            }
        }
    }

    return result;
}

String GameEngine::cmdClose(const char* objectName)
{
    if (!objectName) {
        return "Close what?";
    }

    // Try inventory first, then room
    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        objIdx = findObjectByName(objectName, false, true);
    }

    if (objIdx < 0) {
        return "You can't see any such thing.";
    }

    // Check if it's a container or door
    if (!(objectFlags[objIdx] & OFLAG_CONTAINER) && !(objectFlags[objIdx] & OFLAG_DOOR)) {
        return "You can't close that.";
    }

    // Check if already closed
    if (!(objectFlags[objIdx] & OFLAG_OPEN)) {
        return "It's already closed.";
    }

    // Close it
    objectFlags[objIdx] &= ~OFLAG_OPEN;

    return "Closed.";
}

String GameEngine::cmdRead(const char* objectName)
{
    if (!objectName) {
        return "Read what?";
    }

    // Try inventory first, then room
    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        objIdx = findObjectByName(objectName, false, true);
    }

    if (objIdx < 0) {
        return "You can't see any such thing.";
    }

    // Check if readable
    if (!(objectFlags[objIdx] & OFLAG_READABLE)) {
        String result = "There's nothing written on the ";
        result += OBJECTS[objIdx].name;
        result += ".";
        return result;
    }

    // Return read text
    if (OBJECTS[objIdx].readText && strlen(OBJECTS[objIdx].readText) > 0) {
        return String(OBJECTS[objIdx].readText);
    }

    return "It's blank.";
}

// ============ Light/Darkness ============

bool GameEngine::isRoomLit() const
{
    if (!currentRoom) return false;

    // Check if room has natural light
    if (currentRoom->flags & RFLAG_LIGHT) {
        return true;
    }

    // Check if we have a lit light source
    return hasLightSource();
}

bool GameEngine::hasLightSource() const
{
    // Check inventory for lit lamp
    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (strcmp(objectLocations[i], LOC_INVENTORY) != 0) continue;

        // Check if it's a light source and it's on
        if ((objectFlags[i] & OFLAG_LIGHTSOURCE) && (objectFlags[i] & OFLAG_ON)) {
            return true;
        }

        // Special check for lamp by ID
        if (strcmp(OBJECTS[i].id, "lamp") == 0 && gameFlags.lampOn) {
            return true;
        }
    }
    return false;
}

bool GameEngine::checkDarkness()
{
    if (!currentRoom) return false;

    // If room is lit, no problem
    if (isRoomLit()) return false;

    // In darkness - 25% chance of grue attack per turn
    if (random(100) < 25) {
        return true;  // Player eaten by grue
    }
    return false;
}

void GameEngine::tickLamp()
{
    if (!gameFlags.lampOn) return;

    lampLife--;

    // Lamp warnings handled in processCommand
}

String GameEngine::cmdLight(const char* objectName)
{
    if (!objectName) {
        return "Light what?";
    }

    // Find the object
    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        objIdx = findObjectByName(objectName, false, true);
    }

    if (objIdx < 0) {
        return "You don't have that.";
    }

    // Check if it's a light source
    if (!(objectFlags[objIdx] & OFLAG_LIGHTSOURCE)) {
        String result = "You can't light the ";
        result += OBJECTS[objIdx].name;
        result += ".";
        return result;
    }

    // Special handling for lamp
    if (strcmp(OBJECTS[objIdx].id, "lamp") == 0) {
        if (gameFlags.lampOn) {
            return "The lamp is already on.";
        }
        if (lampLife <= 0) {
            return "The lamp has no power.";
        }
        gameFlags.lampOn = true;
        objectFlags[objIdx] |= OFLAG_ON;
        return "The lamp is now on.";
    }

    // Generic light source
    if (objectFlags[objIdx] & OFLAG_ON) {
        return "It's already lit.";
    }

    objectFlags[objIdx] |= OFLAG_ON;
    String result = "The ";
    result += OBJECTS[objIdx].name;
    result += " is now on.";
    return result;
}

String GameEngine::cmdExtinguish(const char* objectName)
{
    if (!objectName) {
        return "Turn off what?";
    }

    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        objIdx = findObjectByName(objectName, false, true);
    }

    if (objIdx < 0) {
        return "You don't have that.";
    }

    // Special handling for lamp
    if (strcmp(OBJECTS[objIdx].id, "lamp") == 0) {
        if (!gameFlags.lampOn) {
            return "The lamp is already off.";
        }
        gameFlags.lampOn = false;
        objectFlags[objIdx] &= ~OFLAG_ON;
        return "The lamp is now off.";
    }

    if (!(objectFlags[objIdx] & OFLAG_ON)) {
        return "It's already off.";
    }

    objectFlags[objIdx] &= ~OFLAG_ON;
    String result = "The ";
    result += OBJECTS[objIdx].name;
    result += " is now off.";
    return result;
}

// ============ Combat ============

bool GameEngine::isVillainHere(const char* villainId) const
{
    if (strcmp(villainId, VILLAIN_TROLL) == 0) {
        return trollState.health > 0 && strcmp(trollState.roomId, currentRoomId) == 0;
    }
    if (strcmp(villainId, VILLAIN_THIEF) == 0) {
        return thiefState.health > 0 && strcmp(thiefState.roomId, currentRoomId) == 0;
    }
    if (strcmp(villainId, VILLAIN_CYCLOPS) == 0) {
        return cyclopsState.health > 0 && strcmp(cyclopsState.roomId, currentRoomId) == 0;
    }
    return false;
}

int GameEngine::findVillainByName(const char* name)
{
    if (!name) return -1;

    // Normalize to uppercase
    char nameLower[32];
    strncpy(nameLower, name, sizeof(nameLower) - 1);
    for (char* p = nameLower; *p; p++) *p = tolower(*p);

    if (strstr(nameLower, "troll") && isVillainHere(VILLAIN_TROLL)) return 0;
    if (strstr(nameLower, "thief") && isVillainHere(VILLAIN_THIEF)) return 1;
    if ((strstr(nameLower, "cyclops") || strstr(nameLower, "cyclo")) && isVillainHere(VILLAIN_CYCLOPS)) return 2;

    return -1;
}

String GameEngine::cmdAttack(const char* target, const char* weapon)
{
    if (!target) {
        return "Attack what?";
    }

    // Find villain
    int villainIdx = findVillainByName(target);
    if (villainIdx < 0) {
        // Check if it's a non-villain object
        int objIdx = findObjectByName(target, false, true);
        if (objIdx >= 0) {
            if (!(objectFlags[objIdx] & OFLAG_ACTOR)) {
                return "Attacking that would be pointless.";
            }
        }
        return "I don't see that here.";
    }

    // Find weapon
    int weaponIdx = -1;
    if (weapon) {
        weaponIdx = findObjectByName(weapon, true, false);
    } else {
        // Find any weapon in inventory
        for (int i = 0; i < OBJECT_COUNT; i++) {
            if (strcmp(objectLocations[i], LOC_INVENTORY) == 0 &&
                (objectFlags[i] & OFLAG_WEAPON)) {
                weaponIdx = i;
                break;
            }
        }
    }

    if (weaponIdx < 0) {
        return "Strangle with your bare hands?";
    }

    // Get villain info
    VillainState* villain = nullptr;
    const char* villainName = nullptr;

    switch (villainIdx) {
        case 0:
            villain = &trollState;
            villainName = "troll";
            break;
        case 1:
            villain = &thiefState;
            villainName = "thief";
            break;
        case 2:
            villain = &cyclopsState;
            villainName = "cyclops";
            // Cyclops can't be killed by normal combat
            return "The cyclops is too powerful! Perhaps there's another way...";
    }

    if (!villain) return "Attack failed.";

    // Combat!
    String result = "You swing the ";
    result += OBJECTS[weaponIdx].name;
    result += " at the ";
    result += villainName;
    result += "!\n";

    // Hit chance (50-85%)
    int hitChance = 50 + (OBJECTS[weaponIdx].value / 10);
    if (hitChance > 85) hitChance = 85;

    if (random(100) < hitChance) {
        // Hit!
        int damage = 1 + random(5);
        villain->health -= damage;

        if (villain->health <= 0) {
            result += "Your blow strikes true! The ";
            result += villainName;
            result += " falls dead!";

            // Set puzzle flags
            if (villainIdx == 0) gameFlags.trollGone = true;

            // Score for killing villain
            score += 10;
            return result;
        } else {
            result += "You wound the ";
            result += villainName;
            result += "!";
        }
    } else {
        const char* missMsgs[] = {
            "You miss!",
            "Your swing goes wide!",
            "The attack is parried!"
        };
        result += missMsgs[random(3)];
    }

    return result;
}

String GameEngine::villainAttack(const char* villainId)
{
    VillainState* villain = nullptr;
    const char* villainName = nullptr;
    int strength = 3;

    if (strcmp(villainId, VILLAIN_TROLL) == 0) {
        villain = &trollState;
        villainName = "troll";
        strength = 3;
    } else if (strcmp(villainId, VILLAIN_THIEF) == 0) {
        villain = &thiefState;
        villainName = "thief";
        strength = 5;
    } else if (strcmp(villainId, VILLAIN_CYCLOPS) == 0) {
        villain = &cyclopsState;
        villainName = "cyclops";
        strength = 8;
    }

    if (!villain || villain->health <= 0) return "";

    // Villain attacks (30% chance each turn)
    if (random(100) >= 30) {
        return "";
    }

    String result = "The ";
    result += villainName;

    int hitChance = 50 + strength * 5;
    if (random(100) < hitChance) {
        const char* hitMsgs[] = {
            " hits you!",
            " wounds you!",
            " strikes you!"
        };
        result += hitMsgs[random(3)];
        // Note: We're not tracking player health for simplicity
        // In a full implementation, player could die after multiple hits
    } else {
        result += " misses!";
    }

    return result;
}

void GameEngine::tickVillains()
{
    // Check if villains attack
    if (isVillainHere(VILLAIN_TROLL)) {
        // Troll attacks if in room
    }
    if (isVillainHere(VILLAIN_THIEF)) {
        // Thief may attack or steal
    }
}

bool GameEngine::checkVillainBlock(Direction dir)
{
    // Troll blocks passage to east/west in troll room
    if (strcmp(currentRoomId, "mtrol") == 0 && !gameFlags.trollGone) {
        if (dir == DIR_EAST || dir == DIR_WEST) {
            return true;  // Blocked
        }
    }
    return false;
}

// ============ Puzzle Actions ============

String GameEngine::cmdMove(const char* objectName)
{
    if (!objectName) {
        return "Move what?";
    }

    int objIdx = findObjectByName(objectName, false, true);
    if (objIdx < 0) {
        return "I don't see that here.";
    }

    // Special case: rug in living room
    if (strcmp(OBJECTS[objIdx].id, "rug") == 0 && strcmp(currentRoomId, "lroom") == 0) {
        if (gameFlags.rugMoved) {
            return "The rug has already been moved.";
        }
        gameFlags.rugMoved = true;
        score += 2;
        return "With great effort, the rug is moved to one side of the room, revealing the dusty cover of a closed trap door.";
    }

    // Special case: leaves at grating
    if (strcmp(OBJECTS[objIdx].id, "leave") == 0) {
        return "A grating appears beneath the pile of leaves!";
    }

    return "Moving that reveals nothing.";
}

String GameEngine::cmdTie(const char* objectName)
{
    if (!objectName) {
        return "Tie what?";
    }

    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        return "You don't have that.";
    }

    // Only rope can be tied
    if (strcmp(OBJECTS[objIdx].id, "rope") != 0) {
        return "You can't tie that.";
    }

    // Must be in dome room
    if (strcmp(currentRoomId, "dome") != 0) {
        return "There's nothing here to tie the rope to.";
    }

    if (gameFlags.ropeTied) {
        return "The rope is already tied to the railing.";
    }

    gameFlags.ropeTied = true;
    setObjectLocation(objIdx, "dome");  // Drop rope in room (tied)
    score += 2;
    return "The rope is now tied to the railing and dangles down into the darkness below.";
}

String GameEngine::cmdUnlock(const char* objectName)
{
    if (!objectName) {
        return "Unlock what?";
    }

    // Need keys
    if (!hasItem("keys")) {
        return "You don't have anything to unlock it with.";
    }

    int objIdx = findObjectByName(objectName, false, true);
    if (objIdx < 0) {
        return "I don't see that here.";
    }

    // Grating
    if (strcmp(OBJECTS[objIdx].id, "grati") == 0 || strstr(OBJECTS[objIdx].name, "grat")) {
        if (gameFlags.grateOpen) {
            return "The grating is already unlocked.";
        }
        gameFlags.grateOpen = true;
        objectFlags[objIdx] |= OFLAG_OPEN;
        return "The grating is now unlocked.";
    }

    // Generic door/container
    if (objectFlags[objIdx] & OFLAG_LOCKED) {
        objectFlags[objIdx] &= ~OFLAG_LOCKED;
        return "Unlocked.";
    }

    return "It's not locked.";
}

String GameEngine::cmdSay(const char* word)
{
    if (!word) {
        return "Say what?";
    }

    // Magic words
    char wordLower[32];
    strncpy(wordLower, word, sizeof(wordLower) - 1);
    for (char* p = wordLower; *p; p++) *p = tolower(*p);

    // Odysseus/Ulysses defeats cyclops
    if (strcmp(wordLower, "odysseus") == 0 || strcmp(wordLower, "ulysses") == 0) {
        if (strcmp(currentRoomId, "mcycl") == 0 && cyclopsState.health > 0) {
            cyclopsState.health = -1;  // Defeated
            gameFlags.cyclopsGone = true;
            score += 10;
            return "The cyclops cowers in terror! \"No, not again! NOBODY will get me this time!\" The cyclops flees through a door to the west.";
        }
        return "Nothing happens.";
    }

    // Classic adventure game easter eggs
    if (strcmp(wordLower, "xyzzy") == 0 || strcmp(wordLower, "plugh") == 0) {
        return "A hollow voice says 'Fool.'";
    }

    String result = "You say \"";
    result += word;
    result += "\" but nothing happens.";
    return result;
}

String GameEngine::cmdWave(const char* objectName)
{
    if (!objectName) {
        return "Wave what?";
    }

    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        return "You're not carrying that.";
    }

    // Scepter at rainbow
    if (strcmp(OBJECTS[objIdx].id, "scept") == 0) {
        if (strcmp(currentRoomId, "mrain") == 0) {
            score += 5;
            return "The scepter glows brightly and a shimmering path appears across the rainbow!";
        }
    }

    String result = "You wave the ";
    result += OBJECTS[objIdx].name;
    result += ". Nothing happens.";
    return result;
}

String GameEngine::cmdRing(const char* objectName)
{
    if (!objectName) {
        return "Ring what?";
    }

    int objIdx = findObjectByName(objectName, true, false);
    if (objIdx < 0) {
        return "You're not carrying that.";
    }

    if (strcmp(OBJECTS[objIdx].id, "bell") != 0) {
        return "You can't ring that.";
    }

    // Bell, book, candles puzzle at Hades entrance
    if (strcmp(currentRoomId, "entrc") == 0) {
        bool hasCandles = hasItem("candl");
        bool hasBook = hasItem("book");

        if (hasCandles && hasBook) {
            gameFlags.gatesOpen = true;
            score += 5;
            return "The bell rings with a pure tone. The gates of Hades swing open!";
        }
        return "The bell rings but nothing happens. Perhaps you need something else?";
    }

    return "The bell rings clearly.";
}

// ============ Score Tracking ============

void GameEngine::checkTreasureScore()
{
    // Award points for treasures in trophy case
    auto trophyContents = getObjectsAtLocation(LOC_TROPHY_CASE);
    int treasureScore = 0;

    for (int objIdx : trophyContents) {
        if (objectFlags[objIdx] & OFLAG_TREASURE) {
            treasureScore += OBJECTS[objIdx].value;
        }
    }

    // Update score (treasures in trophy case)
    // Note: This is simplified - full implementation would track which treasures
    // have already been scored
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

const char* GameEngine::getObjectLocation(int objIdx)
{
    if (objIdx < 0 || objIdx >= OBJECT_COUNT) return nullptr;
    if (objectLocations[objIdx][0] == '\0') return nullptr;
    return objectLocations[objIdx];
}

void GameEngine::setObjectLocation(int objIdx, const char* location)
{
    if (objIdx < 0 || objIdx >= OBJECT_COUNT) return;
    if (location) {
        strncpy(objectLocations[objIdx], location, sizeof(objectLocations[objIdx]) - 1);
        objectLocations[objIdx][sizeof(objectLocations[objIdx]) - 1] = '\0';
    } else {
        objectLocations[objIdx][0] = '\0';  // Nowhere
    }
}

bool GameEngine::isObjectInInventory(int objIdx)
{
    const char* loc = getObjectLocation(objIdx);
    return loc && strcmp(loc, LOC_INVENTORY) == 0;
}

bool GameEngine::isObjectVisible(int objIdx)
{
    if (objIdx < 0 || objIdx >= OBJECT_COUNT) return false;
    return (objectFlags[objIdx] & OFLAG_VISIBLE) != 0;
}

bool GameEngine::isObjectHere(int objIdx)
{
    if (objIdx < 0 || objIdx >= OBJECT_COUNT) return false;

    const char* loc = getObjectLocation(objIdx);
    if (!loc) return false;

    // Direct match - object is in current room
    if (strcmp(loc, currentRoomId) == 0) return true;

    // Check if object is in a container that's here
    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (i == objIdx) continue;
        const char* containerLoc = getObjectLocation(i);
        if (!containerLoc) continue;

        // If this object is in container i, and container i is here
        if (strcmp(loc, OBJECTS[i].id) == 0) {
            // Container must be in room and open/transparent for contents to be visible
            if (strcmp(containerLoc, currentRoomId) == 0) {
                if ((objectFlags[i] & OFLAG_OPEN) || (objectFlags[i] & OFLAG_TRANSPARENT)) {
                    return true;
                }
            }
        }
    }

    return false;
}

std::vector<int> GameEngine::getObjectsAtLocation(const char* location)
{
    std::vector<int> result;
    if (!location) return result;

    for (int i = 0; i < OBJECT_COUNT; i++) {
        const char* loc = getObjectLocation(i);
        if (loc && strcmp(loc, location) == 0 && isObjectVisible(i)) {
            result.push_back(i);
        }
    }
    return result;
}

// Helper to check if a word matches an object name
static bool nameMatches(const char* input, const char* objectName)
{
    // Convert input to lowercase for comparison
    char inputLower[64];
    strncpy(inputLower, input, sizeof(inputLower) - 1);
    inputLower[sizeof(inputLower) - 1] = '\0';
    for (char* p = inputLower; *p; p++) *p = tolower(*p);

    // Convert object name to lowercase
    char nameLower[64];
    strncpy(nameLower, objectName, sizeof(nameLower) - 1);
    nameLower[sizeof(nameLower) - 1] = '\0';
    for (char* p = nameLower; *p; p++) *p = tolower(*p);

    // Check exact match
    if (strcmp(inputLower, nameLower) == 0) return true;

    // Check if input matches any word in the object name
    char nameCopy[64];
    strncpy(nameCopy, nameLower, sizeof(nameCopy));
    char* word = strtok(nameCopy, " ");
    while (word) {
        if (strcmp(inputLower, word) == 0) return true;
        word = strtok(nullptr, " ");
    }

    return false;
}

int GameEngine::findObjectByName(const char* name, bool inInventory, bool inRoom)
{
    if (!name) return -1;

    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (!isObjectVisible(i)) continue;

        bool matches = nameMatches(name, OBJECTS[i].name);

        // Also check object ID
        if (!matches) {
            char idLower[32], inputLower[64];
            strncpy(idLower, OBJECTS[i].id, sizeof(idLower) - 1);
            strncpy(inputLower, name, sizeof(inputLower) - 1);
            for (char* p = idLower; *p; p++) *p = tolower(*p);
            for (char* p = inputLower; *p; p++) *p = tolower(*p);
            if (strcmp(idLower, inputLower) == 0) matches = true;
        }

        if (!matches) continue;

        // Check location constraints
        if (inInventory && isObjectInInventory(i)) return i;
        if (inRoom && isObjectHere(i)) return i;
    }

    return -1;
}

int GameEngine::findObjectInContainer(int containerIdx, const char* name)
{
    if (containerIdx < 0 || containerIdx >= OBJECT_COUNT) return -1;
    if (!name) return -1;

    const char* containerId = OBJECTS[containerIdx].id;

    for (int i = 0; i < OBJECT_COUNT; i++) {
        const char* loc = getObjectLocation(i);
        if (!loc || strcmp(loc, containerId) != 0) continue;
        if (!isObjectVisible(i)) continue;

        if (nameMatches(name, OBJECTS[i].name)) return i;
    }

    return -1;
}

bool GameEngine::hasItem(const char* itemId) const
{
    for (int i = 0; i < OBJECT_COUNT; i++) {
        if (strcmp(OBJECTS[i].id, itemId) == 0) {
            if (objectLocations[i][0] != '\0' &&
                strcmp(objectLocations[i], LOC_INVENTORY) == 0) {
                return true;
            }
            return false;
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
    prefs.putInt("objCount", OBJECT_COUNT);
    prefs.putInt("lampLife", lampLife);

    // Save game flags as bitmask
    uint16_t flagBits = 0;
    if (gameFlags.rugMoved) flagBits |= 0x01;
    if (gameFlags.trollGone) flagBits |= 0x02;
    if (gameFlags.grateOpen) flagBits |= 0x04;
    if (gameFlags.ropeTied) flagBits |= 0x08;
    if (gameFlags.gatesOpen) flagBits |= 0x10;
    if (gameFlags.cyclopsGone) flagBits |= 0x20;
    if (gameFlags.lampOn) flagBits |= 0x40;
    if (gameFlags.thiefActive) flagBits |= 0x80;
    prefs.putUShort("flags", flagBits);

    // Save villain states
    prefs.putChar("trollHP", trollState.health);
    prefs.putChar("thiefHP", thiefState.health);
    prefs.putChar("cyclopsHP", cyclopsState.health);
    prefs.putString("thiefRoom", thiefState.roomId);

    // Save object locations and flags
    for (int i = 0; i < OBJECT_COUNT; i++) {
        char locKey[16], flagKey[16];
        snprintf(locKey, sizeof(locKey), "ol%d", i);
        snprintf(flagKey, sizeof(flagKey), "of%d", i);
        prefs.putString(locKey, objectLocations[i]);
        prefs.putUShort(flagKey, objectFlags[i]);
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
    lampLife = prefs.getInt("lampLife", LAMP_MAX_LIFE);

    // Load game flags
    uint16_t flagBits = prefs.getUShort("flags", 0);
    gameFlags.rugMoved = (flagBits & 0x01) != 0;
    gameFlags.trollGone = (flagBits & 0x02) != 0;
    gameFlags.grateOpen = (flagBits & 0x04) != 0;
    gameFlags.ropeTied = (flagBits & 0x08) != 0;
    gameFlags.gatesOpen = (flagBits & 0x10) != 0;
    gameFlags.cyclopsGone = (flagBits & 0x20) != 0;
    gameFlags.lampOn = (flagBits & 0x40) != 0;
    gameFlags.thiefActive = (flagBits & 0x80) != 0;

    // Load villain states
    trollState.health = prefs.getChar("trollHP", TROLL_HEALTH);
    thiefState.health = prefs.getChar("thiefHP", THIEF_HEALTH);
    cyclopsState.health = prefs.getChar("cyclopsHP", CYCLOPS_HEALTH);

    String thiefRoom = prefs.getString("thiefRoom", "treas");
    strncpy(thiefState.roomId, thiefRoom.c_str(), sizeof(thiefState.roomId) - 1);

    // Set villain room IDs that don't change
    strncpy(trollState.roomId, "mtrol", sizeof(trollState.roomId) - 1);
    strncpy(cyclopsState.roomId, "mcycl", sizeof(cyclopsState.roomId) - 1);

    // Load object state
    int savedObjCount = prefs.getInt("objCount", 0);
    if (savedObjCount > 0) {
        // Initialize with defaults first
        initObjectState();

        // Then load saved state
        int loadCount = (savedObjCount < OBJECT_COUNT) ? savedObjCount : OBJECT_COUNT;
        for (int i = 0; i < loadCount; i++) {
            char locKey[16], flagKey[16];
            snprintf(locKey, sizeof(locKey), "ol%d", i);
            snprintf(flagKey, sizeof(flagKey), "of%d", i);

            String loc = prefs.getString(locKey, "");
            strncpy(objectLocations[i], loc.c_str(), sizeof(objectLocations[i]) - 1);
            objectFlags[i] = prefs.getUShort(flagKey, OBJECTS[i].flags);
        }
    } else {
        // Old save format or no objects - use defaults
        initObjectState();
    }

    prefs.end();
    LOG_INFO("Game loaded: room=%s, score=%d, moves=%d", currentRoomId, score, moves);
    return true;
}
