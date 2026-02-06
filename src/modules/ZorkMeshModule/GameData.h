#pragma once

/**
 * GameData.h - Auto-generated from PyMeshZork JSON data
 *
 * Rooms: 98
 * Objects: 57
 */

#include "GameEngine.h"

// Extended object flags
#define OFLAG_VISIBLE      0x0001
#define OFLAG_TRANSPARENT  0x0002
#define OFLAG_DOOR         0x0004
#define OFLAG_LIGHTSOURCE  0x0008
#define OFLAG_FOOD         0x0010
#define OFLAG_DRINKABLE    0x0020
#define OFLAG_TIEABLE      0x0040
#define OFLAG_TREASURE     0x0080
#define OFLAG_ACTOR        0x0100
#define OFLAG_VILLAIN      0x0200
#define OFLAG_FIGHTER      0x0400

// Object data structure for static initialization
struct GameObjectData {
    const char* id;
    const char* name;
    const char* description;
    const char* examineText;
    const char* readText;
    const char* initialLocation;
    uint16_t flags;
    uint8_t size;
    uint8_t capacity;
    uint8_t value;
};

// Number of rooms: 98
#define ROOM_COUNT 98

// Room ID to index mapping
#define ROOM_WHOUS 0
#define ROOM_NHOUS 1
#define ROOM_SHOUS 2
#define ROOM_EHOUS 3
#define ROOM_KITCH 4
#define ROOM_LROOM 5
#define ROOM_ATTIC 6
#define ROOM_CELLA 7
#define ROOM_MTROL 8
#define ROOM_FORE1 9
#define ROOM_FORE2 10
#define ROOM_FORE3 11
#define ROOM_CLEAR 12
#define ROOM_UPTREE 13
#define ROOM_CANYV 14
#define ROOM_LEDGE 15
#define ROOM_CNYBM 16
#define ROOM_STRMV 17
#define ROOM_STREA 18
#define ROOM_MGRAT 19
#define ROOM_MAZE1 20
#define ROOM_MAZE2 21
#define ROOM_MAZE3 22
#define ROOM_MAZE4 23
#define ROOM_MAZE5 24
#define ROOM_MAZ15 25
#define ROOM_DCELL 26
#define ROOM_ESTOF 27
#define ROOM_GALRY 28
#define ROOM_STUDI 29
#define ROOM_EWWCH 30
#define ROOM_ROUND 31
#define ROOM_NSRM1 32
#define ROOM_CHASM 33
#define ROOM_RESER 34
#define ROOM_RSOUT 35
#define ROOM_SSMRM 36
#define ROOM_LLRM2 37
#define ROOM_LLD2 38
#define ROOM_DAMP 39
#define ROOM_WFISS 40
#define ROOM_WCAVE 41
#define ROOM_ENGRA 42
#define ROOM_DOME 43
#define ROOM_MTORC 44
#define ROOM_EGYPT 45
#define ROOM_ECHOR 46
#define ROOM_TEMP1 47
#define ROOM_TEMP2 48
#define ROOM_TREAS 49
#define ROOM_ENTRC 50
#define ROOM_HADES 51
#define ROOM_SMELE 52
#define ROOM_GAS 53
#define ROOM_COAL1 54
#define ROOM_COAL2 55
#define ROOM_COAL3 56
#define ROOM_COAL4 57
#define ROOM_TSHAF 58
#define ROOM_BSHAF 59
#define ROOM_MMACH 60
#define ROOM_MCYCL 61
#define ROOM_STRNG 62
#define ROOM_RIVR1 63
#define ROOM_RIVR2 64
#define ROOM_RIVR3 65
#define ROOM_RIVR4 66
#define ROOM_RIVR5 67
#define ROOM_SBEAC 68
#define ROOM_SCAVE 69
#define ROOM_CAROU 70
#define ROOM_RIDDL 71
#define ROOM_PEARL 72
#define ROOM_FALLS 73
#define ROOM_MRAIN 74
#define ROOM_POG 75
#define ROOM_MBARR 76
#define ROOM_FCHMP 77
#define ROOM_DAMMA 78
#define ROOM_DLOBB 79
#define ROOM_MAINT 80
#define ROOM_DAMRO 81
#define ROOM_BLROO 82
#define ROOM_VLBOT 83
#define ROOM_VAIR1 84
#define ROOM_VAIR2 85
#define ROOM_VAIR3 86
#define ROOM_VAIR4 87
#define ROOM_LEDG2 88
#define ROOM_LEDG3 89
#define ROOM_LEDG4 90
#define ROOM_LIBRA 91
#define ROOM_BKENT 92
#define ROOM_BKTVW 93
#define ROOM_BKTWI 94
#define ROOM_BKVAU 95
#define ROOM_BKBOX 96
#define ROOM_CRYPT 97

// Room data
static const Room ROOMS[ROOM_COUNT] = {
    // West of House
    {
        "whous",  // id
        "West of House",  // name
        "You are standing in an open field west of a white house, with a boarded front door.",  // description
        "West of House",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "nhous", false, nullptr },
            { DIR_SOUTH, "shous", false, nullptr },
            { DIR_EAST, nullptr, true, "The door is boarded and you can't remove the boards." },
            { DIR_WEST, "fore1", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, "fore1", false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, "fore1", false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        5  // numExits
    },

    // North of House
    {
        "nhous",  // id
        "North of House",  // name
        "You are facing the north side of a white house. There is no door here, and all the windows are boarded up. To the north a narrow path winds through the trees.",  // description
        "North of House",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "fore3", false, nullptr },
            { DIR_SOUTH, "whous", false, nullptr },
            { DIR_EAST, "ehous", false, nullptr },
            { DIR_WEST, "fore1", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        4  // numExits
    },

    // South of House
    {
        "shous",  // id
        "South of House",  // name
        "You are facing the south side of a white house. There is no door here, and all the windows are boarded.",  // description
        "South of House",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "whous", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "ehous", false, nullptr },
            { DIR_WEST, "fore1", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Behind House
    {
        "ehous",  // id
        "Behind House",  // name
        "You are behind the white house. A path leads into the forest to the east. In one corner of the house there is a small window which is slightly ajar.",  // description
        "Behind House",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "nhous", false, nullptr },
            { DIR_SOUTH, "shous", false, nullptr },
            { DIR_EAST, "clear", false, nullptr },
            { DIR_WEST, "kitch", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, "kitch", false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        5  // numExits
    },

    // Kitchen
    {
        "kitch",  // id
        "Kitchen",  // name
        "You are in the kitchen of the white house. A table seems to have been used recently for the preparation of food. A passage leads to the west and a dark staircase can be seen leading upward. A dark chimney leads down and to the east is a small window which is open.",  // description
        "Kitchen",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND | RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "ehous", false, nullptr },
            { DIR_WEST, "lroom", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "attic", false, nullptr },
            { DIR_DOWN, "cella", true, "Only Santa Claus climbs down chimneys." },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        4  // numExits
    },

    // Living Room
    {
        "lroom",  // id
        "Living Room",  // name
        "You are in the living room. There is a doorway to the east, a wooden door with strange gothic lettering to the west, which appears to be nailed shut, a trophy case, and a large oriental rug in the center of the room.",  // description
        "Living Room",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND | RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "kitch", false, nullptr },
            { DIR_WEST, nullptr, true, "The door is nailed shut." },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "cella", true, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Attic
    {
        "attic",  // id
        "Attic",  // name
        "This is the attic. The only exit is a stairway leading down. A large coil of rope is lying in the corner. On a table is a nasty-looking knife.",  // description
        "Attic",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND | RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "kitch", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Cellar
    {
        "cella",  // id
        "Cellar",  // name
        "You are in a dark and damp cellar with a narrow passageway leading north, and a crawlway to the south. On the west is the bottom of a steep metal ramp which is unclimbable.",  // description
        "Cellar",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "mtrol", false, nullptr },
            { DIR_SOUTH, "estof", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, true, "The ramp is too steep to climb." },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "lroom", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Troll Room
    {
        "mtrol",  // id
        "Troll Room",  // name
        "This is a small room with passages to the east and south and a forbidding hole leading west. Bloodstains and deep scratches (perhaps made by straining fingers) mar the walls.",  // description
        "Troll Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "cella", false, nullptr },
            { DIR_EAST, "maze1", false, nullptr },
            { DIR_WEST, "ewwch", true, "The troll fends you off with a menacing gesture." },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Forest
    {
        "fore1",  // id
        "Forest",  // name
        "This is a forest, with trees in all directions. To the east, there appears to be sunlight.",  // description
        "Forest",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "fore3", false, nullptr },
            { DIR_SOUTH, "fore2", false, nullptr },
            { DIR_EAST, "whous", false, nullptr },
            { DIR_WEST, "fore2", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        4  // numExits
    },

    // Forest
    {
        "fore2",  // id
        "Forest",  // name
        "This is a dimly lit forest, with large trees all around.",  // description
        "Forest",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "fore1", false, nullptr },
            { DIR_SOUTH, "fore2", false, nullptr },
            { DIR_EAST, "shous", false, nullptr },
            { DIR_WEST, "fore2", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "fore2", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        5  // numExits
    },

    // Forest Path
    {
        "fore3",  // id
        "Forest Path",  // name
        "This is a path winding through a dimly lit forest. The path heads north-south here. One particularly large tree with some low branches stands at the edge of the path.",  // description
        "Forest Path",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "clear", false, nullptr },
            { DIR_SOUTH, "nhous", false, nullptr },
            { DIR_EAST, "fore1", false, nullptr },
            { DIR_WEST, "fore1", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "uptree", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        5  // numExits
    },

    // Clearing
    {
        "clear",  // id
        "Clearing",  // name
        "You are in a clearing, with a forest surrounding you on all sides. A path leads south.",  // description
        "Clearing",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "fore3", false, nullptr },
            { DIR_EAST, "canyv", false, nullptr },
            { DIR_WEST, "fore1", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Up a Tree
    {
        "uptree",  // id
        "Up a Tree",  // name
        "You are about 10 feet above the ground nestled among some large branches. The nearest branch above you is above your reach. Beside you on the branch is a small bird's nest.",  // description
        "Up a Tree",  // shortDesc
        RFLAG_LIGHT | RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "fore3", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Canyon View
    {
        "canyv",  // id
        "Canyon View",  // name
        "You are at the top of the Great Canyon on its west wall. From here there is a marvelous view of the canyon and parts of the Frigid River upstream. Across the canyon, the walls of the White Cliffs join the mighty ramparts of the Flathead Mountains to the east. Following the canyon upstream to the north, Aragain Falls may be seen, complete with rainbow. The canyon stretches off in the distance to the southwest.",  // description
        "Canyon View",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "ledge", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "clear", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "ledge", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Rocky Ledge
    {
        "ledge",  // id
        "Rocky Ledge",  // name
        "You are on a ledge about halfway up the wall of the river canyon. You can see from here that the Great Canyon stretches off into the distance. Far below you can see the whitish froth of the river against the craggy cliffs of the gorge. Above you the face of the cliff rises vertically out of sight. There is no way up from here without climbing equipment.",  // description
        "Rocky Ledge",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "canyv", false, nullptr },
            { DIR_DOWN, "cnybm", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Canyon Bottom
    {
        "cnybm",  // id
        "Canyon Bottom",  // name
        "You are beneath the walls of the river canyon which may be climbable here. The canyon runs north and south.",  // description
        "Canyon Bottom",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "mrain", false, nullptr },
            { DIR_SOUTH, "strmv", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "ledge", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Stream View
    {
        "strmv",  // id
        "Stream View",  // name
        "You are standing on a path beside a gently flowing stream. The path follows the stream through the forest.",  // description
        "Stream View",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "cnybm", false, nullptr },
            { DIR_SOUTH, "strea", false, nullptr },
            { DIR_EAST, "strea", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Stream
    {
        "strea",  // id
        "Stream",  // name
        "You are on the gently sloping bank of a stream, which is flowing down from somewhere above. Upstream you can see a wooden dam blocking the river.",  // description
        "Stream",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "strmv", false, nullptr },
            { DIR_SOUTH, "mgrat", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "strmv", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Grating Room
    {
        "mgrat",  // id
        "Grating Room",  // name
        "You are in a small room near the surface. A shaft of sunlight from above illuminates the room. In one corner is a pile of leaves. A narrow passage leads north.",  // description
        "Grating Room",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "strea", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "clear", false, nullptr },
            { DIR_DOWN, "mgrat", true, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Maze
    {
        "maze1",  // id
        "Maze",  // name
        "You are in a maze of twisty little passages, all alike.",  // description
        "Maze",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "maze2", false, nullptr },
            { DIR_SOUTH, "maze4", false, nullptr },
            { DIR_EAST, "maze3", false, nullptr },
            { DIR_WEST, "mtrol", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        4  // numExits
    },

    // Maze
    {
        "maze2",  // id
        "Maze",  // name
        "You are in a maze of twisty little passages, all alike.",  // description
        "Maze",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "maze3", false, nullptr },
            { DIR_SOUTH, "maze1", false, nullptr },
            { DIR_EAST, "maze5", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Maze
    {
        "maze3",  // id
        "Maze",  // name
        "You are in a maze of twisty little passages, all alike.",  // description
        "Maze",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "maze4", false, nullptr },
            { DIR_SOUTH, "maze2", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "maze1", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Maze
    {
        "maze4",  // id
        "Maze",  // name
        "You are in a maze of twisty little passages, all alike.",  // description
        "Maze",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "maze1", false, nullptr },
            { DIR_SOUTH, "maze3", false, nullptr },
            { DIR_EAST, "dcell", false, nullptr },
            { DIR_WEST, "maz15", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        4  // numExits
    },

    // Maze
    {
        "maze5",  // id
        "Maze",  // name
        "You are in a maze of twisty little passages, all alike.",  // description
        "Maze",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "maz15", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "maze5", false, nullptr },
            { DIR_WEST, "maze2", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Grating Room
    {
        "maz15",  // id
        "Grating Room",  // name
        "You are in a small room near the surface. A shaft of sunlight from above illuminates the room. In one corner is a pile of leaves. A narrow passage leads north.",  // description
        "Grating Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "maze4", false, nullptr },
            { DIR_EAST, "maze5", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Dead End
    {
        "dcell",  // id
        "Dead End",  // name
        "You have come to a dead end in the maze.",  // description
        "Dead End",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "maze4", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // East of Chasm
    {
        "estof",  // id
        "East of Chasm",  // name
        "You are on the east edge of a chasm, the bottom of which cannot be seen. The east edge is a narrow ledge looking across to a thin land bridge.",  // description
        "East of Chasm",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "cella", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, "galry", false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Gallery
    {
        "galry",  // id
        "Gallery",  // name
        "This is an art gallery. Most of the paintings have been stolen by vandals with exceptional taste. A doorway leads south.",  // description
        "Gallery",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "estof", false, nullptr },
            { DIR_SOUTH, "studi", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Studio
    {
        "studi",  // id
        "Studio",  // name
        "This is what was once an artist's studio. It appears the artist has moved on, perhaps scared away by the vandals. Many empty frames line the walls.",  // description
        "Studio",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "galry", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "kitch", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // East-West Passage
    {
        "ewwch",  // id
        "East-West Passage",  // name
        "This is a narrow east-west passageway. There is a hole in the floor from which a foul stench emanates.",  // description
        "East-West Passage",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "mtrol", false, nullptr },
            { DIR_WEST, "round", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "smele", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Round Room
    {
        "round",  // id
        "Round Room",  // name
        "This is a circular stone room with passages in all directions. Several of them have unfortunately been blocked by cave-ins.",  // description
        "Round Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "nsrm1", false, nullptr },
            { DIR_SOUTH, "ssmrm", false, nullptr },
            { DIR_EAST, "ewwch", false, nullptr },
            { DIR_WEST, "llrm2", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, "engra", false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        5  // numExits
    },

    // North-South Passage
    {
        "nsrm1",  // id
        "North-South Passage",  // name
        "This is a narrow north-south passage.",  // description
        "North-South Passage",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "chasm", false, nullptr },
            { DIR_SOUTH, "round", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Chasm
    {
        "chasm",  // id
        "Chasm",  // name
        "A chasm runs southwest to northeast and the path follows it. You are on the south side of the chasm, where a crack opens into a dark passage.",  // description
        "Chasm",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "nsrm1", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, "reser", false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Reservoir
    {
        "reser",  // id
        "Reservoir",  // name
        "You are at the edge of a large underground reservoir. There is a path along the edge of the reservoir.",  // description
        "Reservoir",  // shortDesc
        RFLAG_WATER,  // flags
        {  // exits
            { DIR_NORTH, "rsout", false, nullptr },
            { DIR_SOUTH, "chasm", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Reservoir South
    {
        "rsout",  // id
        "Reservoir South",  // name
        "You are at the southern edge of a large underground reservoir.",  // description
        "Reservoir South",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "reser", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "strea", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Mirror Room
    {
        "ssmrm",  // id
        "Mirror Room",  // name
        "You are in a large room with a mirror on each wall. Above you hangs a large chandelier.",  // description
        "Mirror Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "round", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Loud Room
    {
        "llrm2",  // id
        "Loud Room",  // name
        "This is a large room with a ceiling which cannot be detected from the ground. There is a narrow passage from east to west and a stone stairway leading upward.",  // description
        "Loud Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "round", false, nullptr },
            { DIR_WEST, "lld2", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "dome", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Loud Room
    {
        "lld2",  // id
        "Loud Room",  // name
        "This is a large room. A narrow passage goes west, and a stone stairway leads upward. The room is eerie in its quietness.",  // description
        "Loud Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "llrm2", false, nullptr },
            { DIR_WEST, "damp", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "dome", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Damp Cave
    {
        "damp",  // id
        "Damp Cave",  // name
        "This is a cave. The walls are covered with moisture. A passage leads south, and there is a crawlway to the west.",  // description
        "Damp Cave",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "wfiss", false, nullptr },
            { DIR_EAST, "lld2", false, nullptr },
            { DIR_WEST, "wcave", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // White Cliffs Beach
    {
        "wfiss",  // id
        "White Cliffs Beach",  // name
        "You are on a narrow strip of beach beside the Frigid River.",  // description
        "White Cliffs Beach",  // shortDesc
        RFLAG_LAND | RFLAG_WATER,  // flags
        {  // exits
            { DIR_NORTH, "damp", false, nullptr },
            { DIR_SOUTH, "rivr1", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // White Cliffs Cave
    {
        "wcave",  // id
        "White Cliffs Cave",  // name
        "You are in a small cave. There is a passage to the east and a crawlway to the north.",  // description
        "White Cliffs Cave",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "engra", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "damp", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Engravings Cave
    {
        "engra",  // id
        "Engravings Cave",  // name
        "You are in a cave with engravings on the wall. An exit leads northwest and a crawlway leads south.",  // description
        "Engravings Cave",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "wcave", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, "round", false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Dome Room
    {
        "dome",  // id
        "Dome Room",  // name
        "You are at the periphery of a large dome, which forms the ceiling of another room below. Protecting you from a precipitous drop is a wooden railing which circles the dome.",  // description
        "Dome Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "llrm2", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "mtorc", true, "You cannot go down without a rope." },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Torch Room
    {
        "mtorc",  // id
        "Torch Room",  // name
        "This is a large room with a high ceiling. A wooden door leads west. On the walls of the room are three torches. One is lit, one is out, and one is missing.",  // description
        "Torch Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "temp1", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "egypt", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "dome", true, "You cannot climb up without a rope." },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Egyptian Room
    {
        "egypt",  // id
        "Egyptian Room",  // name
        "This is a room which looks like an Egyptian tomb. A passage leads north.",  // description
        "Egyptian Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "echor", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "mtorc", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Echo Room
    {
        "echor",  // id
        "Echo Room",  // name
        "This is a very large room with a low ceiling. There are exits in all directions.",  // description
        "Echo Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "echor", false, nullptr },
            { DIR_SOUTH, "egypt", false, nullptr },
            { DIR_EAST, "echor", false, nullptr },
            { DIR_WEST, "echor", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        4  // numExits
    },

    // Temple
    {
        "temp1",  // id
        "Temple",  // name
        "This is the north end of a large temple. On the east and west walls are engravings. A few, like 'Strangle the Troll' and 'Defenestrate the Ogre', are still legible. Large doors at the north end of the temple are closed.",  // description
        "Temple",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "mtorc", false, nullptr },
            { DIR_SOUTH, "temp2", false, nullptr },
            { DIR_EAST, "treas", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Altar
    {
        "temp2",  // id
        "Altar",  // name
        "This is the south end of a large temple. In front of you is what appears to be an altar. On the altar is a large black book.",  // description
        "Altar",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "temp1", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "entrc", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Treasure Room
    {
        "treas",  // id
        "Treasure Room",  // name
        "This is a huge room filled with piles of treasure. Valuable items are scattered everywhere.",  // description
        "Treasure Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "temp1", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Entrance to Hades
    {
        "entrc",  // id
        "Entrance to Hades",  // name
        "You are outside a large gateway which blocks the entrance to Hades.",  // description
        "Entrance to Hades",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "hades", true, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "temp2", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Land of the Living Dead
    {
        "hades",  // id
        "Land of the Living Dead",  // name
        "You are in a gloomy room lit only by a flickering light. On the wall hangs a bell pull.",  // description
        "Land of the Living Dead",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "entrc", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Smelly Room
    {
        "smele",  // id
        "Smelly Room",  // name
        "This is a small room with a foul stench. A passage leads south.",  // description
        "Smelly Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "gas", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "ewwch", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Gas Room
    {
        "gas",  // id
        "Gas Room",  // name
        "This room has a damp floor. A passage leads west.",  // description
        "Gas Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "smele", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "coal1", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Coal Mine
    {
        "coal1",  // id
        "Coal Mine",  // name
        "This is a dead-end in a coal mine.",  // description
        "Coal Mine",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "coal2", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "gas", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Ladder Top
    {
        "coal2",  // id
        "Ladder Top",  // name
        "This is the top of a wooden ladder. There is a room to the north.",  // description
        "Ladder Top",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "tshaf", false, nullptr },
            { DIR_SOUTH, "coal1", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "coal3", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Ladder Bottom
    {
        "coal3",  // id
        "Ladder Bottom",  // name
        "This is the bottom of a wooden ladder.",  // description
        "Ladder Bottom",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "coal4", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "coal2", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Dead End
    {
        "coal4",  // id
        "Dead End",  // name
        "You have come to a dead end.",  // description
        "Dead End",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "coal3", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Shaft Room
    {
        "tshaf",  // id
        "Shaft Room",  // name
        "This is a small room with a shaft descending into the darkness.",  // description
        "Shaft Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "coal2", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "bshaf", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Shaft Bottom
    {
        "bshaf",  // id
        "Shaft Bottom",  // name
        "This is the bottom of the shaft. A passage leads south.",  // description
        "Shaft Bottom",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "mmach", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "tshaf", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Machine Room
    {
        "mmach",  // id
        "Machine Room",  // name
        "This is a large room dominated by a huge machine. The machine is quiet and cold.",  // description
        "Machine Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "bshaf", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Cyclops Room
    {
        "mcycl",  // id
        "Cyclops Room",  // name
        "This is a room with a cyclops in it.",  // description
        "Cyclops Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "strng", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "treas", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Strange Passage
    {
        "strng",  // id
        "Strange Passage",  // name
        "This is a strange passage that leads in many directions.",  // description
        "Strange Passage",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "mcycl", false, nullptr },
            { DIR_WEST, "lroom", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Frigid River
    {
        "rivr1",  // id
        "Frigid River",  // name
        "You are on the Frigid River in a small magic boat. The river is smooth here.",  // description
        "Frigid River",  // shortDesc
        RFLAG_WATER,  // flags
        {  // exits
            { DIR_NORTH, "wfiss", false, nullptr },
            { DIR_SOUTH, "rivr2", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Frigid River
    {
        "rivr2",  // id
        "Frigid River",  // name
        "The river is moving faster here. The banks look rocky.",  // description
        "Frigid River",  // shortDesc
        RFLAG_WATER,  // flags
        {  // exits
            { DIR_NORTH, "rivr1", false, nullptr },
            { DIR_SOUTH, "rivr3", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, true, "The shore is too rocky to land." },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Frigid River
    {
        "rivr3",  // id
        "Frigid River",  // name
        "The river continues through a narrow canyon. The current is swift.",  // description
        "Frigid River",  // shortDesc
        RFLAG_WATER,  // flags
        {  // exits
            { DIR_NORTH, "rivr2", false, nullptr },
            { DIR_SOUTH, "rivr4", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Frigid River
    {
        "rivr4",  // id
        "Frigid River",  // name
        "You can hear the roar of a waterfall somewhere downstream.",  // description
        "Frigid River",  // shortDesc
        RFLAG_WATER,  // flags
        {  // exits
            { DIR_NORTH, "rivr3", false, nullptr },
            { DIR_SOUTH, "rivr5", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "sbeac", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Frigid River
    {
        "rivr5",  // id
        "Frigid River",  // name
        "The river is rushing toward a waterfall ahead!",  // description
        "Frigid River",  // shortDesc
        RFLAG_WATER,  // flags
        {  // exits
            { DIR_NORTH, "rivr4", false, nullptr },
            { DIR_SOUTH, "falls", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Sandy Beach
    {
        "sbeac",  // id
        "Sandy Beach",  // name
        "You are on a sandy beach at the bank of the Frigid River. There is a cave to the south.",  // description
        "Sandy Beach",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "scave", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Sandy Cave
    {
        "scave",  // id
        "Sandy Cave",  // name
        "This is a sandy cave near the river.",  // description
        "Sandy Cave",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "sbeac", false, nullptr },
            { DIR_SOUTH, "carou", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Carousel Room
    {
        "carou",  // id
        "Carousel Room",  // name
        "You are in a circular room which is spinning. There are exits in all directions, but it is hard to tell which one to take.",  // description
        "Carousel Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "scave", false, nullptr },
            { DIR_SOUTH, "riddl", false, nullptr },
            { DIR_EAST, "carou", false, nullptr },
            { DIR_WEST, "carou", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        4  // numExits
    },

    // Riddle Room
    {
        "riddl",  // id
        "Riddle Room",  // name
        "This is a room with a large inscription on the wall.",  // description
        "Riddle Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "carou", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "pearl", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Pearl Room
    {
        "pearl",  // id
        "Pearl Room",  // name
        "This is a small room with an opening to the east.",  // description
        "Pearl Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "riddl", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Aragain Falls
    {
        "falls",  // id
        "Aragain Falls",  // name
        "You are at the top of Aragain Falls, a magnificent waterfall with a drop of some 450 feet. The only path here is on the western edge.",  // description
        "Aragain Falls",  // shortDesc
        RFLAG_LIGHT | RFLAG_WATER,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "mrain", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // On the Rainbow
    {
        "mrain",  // id
        "On the Rainbow",  // name
        "You are on top of a beautiful rainbow arching over the falls. From here, you could go either east to the falls, west to the cliff, or down through the mist.",  // description
        "On the Rainbow",  // shortDesc
        RFLAG_LIGHT | RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "falls", false, nullptr },
            { DIR_WEST, "pog", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "mbarr", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // End of Rainbow
    {
        "pog",  // id
        "End of Rainbow",  // name
        "You are on a small, rocky beach on the west shore of the river. The falls are visible to the east.",  // description
        "End of Rainbow",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "mrain", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, "cnybm", false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Dam Base
    {
        "mbarr",  // id
        "Dam Base",  // name
        "You are at the base of Flood Control Dam #3. The water is cascading over the dam and into a large reservoir.",  // description
        "Dam Base",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "fchmp", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "mrain", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Dam
    {
        "fchmp",  // id
        "Dam",  // name
        "You are standing on the top of Flood Control Dam #3, which was quite a feat of engineering in its day.",  // description
        "Dam",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "dlobb", false, nullptr },
            { DIR_SOUTH, "mbarr", false, nullptr },
            { DIR_EAST, "damma", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        3  // numExits
    },

    // Dam
    {
        "damma",  // id
        "Dam",  // name
        "You are on the east edge of the dam. From here you can see the mighty Frigid River stretching away into the distance.",  // description
        "Dam",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "fchmp", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Dam Lobby
    {
        "dlobb",  // id
        "Dam Lobby",  // name
        "This room appears to have been the waiting room for tourists who visited the dam. There are exits to the north and east. A flight of stairs leads down.",  // description
        "Dam Lobby",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "maint", false, nullptr },
            { DIR_SOUTH, "fchmp", false, nullptr },
            { DIR_EAST, "damro", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "reser", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        4  // numExits
    },

    // Maintenance Room
    {
        "maint",  // id
        "Maintenance Room",  // name
        "This is what appears to have been the maintenance room for the dam. There are various tools scattered around.",  // description
        "Maintenance Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "dlobb", false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Dam
    {
        "damro",  // id
        "Dam",  // name
        "This is a room on the east side of the dam. A stairway leads down.",  // description
        "Dam",  // shortDesc
        RFLAG_LIGHT | RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "dlobb", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "reser", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Blue Room
    {
        "blroo",  // id
        "Blue Room",  // name
        "This is a blue room.",  // description
        "Blue Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        0  // numExits
    },

    // Volcano Bottom
    {
        "vlbot",  // id
        "Volcano Bottom",  // name
        "You are at the bottom of a volcanic shaft. The walls are warm to the touch.",  // description
        "Volcano Bottom",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "vair1", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // In the Balloon
    {
        "vair1",  // id
        "In the Balloon",  // name
        "You are in a hot air balloon.",  // description
        "In the Balloon",  // shortDesc
        RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "vlbot", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // In the Balloon
    {
        "vair2",  // id
        "In the Balloon",  // name
        "You are in a hot air balloon, drifting in the volcanic shaft.",  // description
        "In the Balloon",  // shortDesc
        RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        0  // numExits
    },

    // In the Balloon
    {
        "vair3",  // id
        "In the Balloon",  // name
        "You are in a hot air balloon, high in the volcanic shaft.",  // description
        "In the Balloon",  // shortDesc
        RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        0  // numExits
    },

    // In the Balloon
    {
        "vair4",  // id
        "In the Balloon",  // name
        "You are in a hot air balloon at the top of the volcanic shaft.",  // description
        "In the Balloon",  // shortDesc
        RFLAG_LIGHT,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        0  // numExits
    },

    // Narrow Ledge
    {
        "ledg2",  // id
        "Narrow Ledge",  // name
        "You are on a narrow ledge inside the volcano.",  // description
        "Narrow Ledge",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        0  // numExits
    },

    // Narrow Ledge
    {
        "ledg3",  // id
        "Narrow Ledge",  // name
        "You are on a narrow ledge inside the volcano.",  // description
        "Narrow Ledge",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        0  // numExits
    },

    // Narrow Ledge
    {
        "ledg4",  // id
        "Narrow Ledge",  // name
        "You are on a narrow ledge at the top of the volcanic shaft.",  // description
        "Narrow Ledge",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "libra", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Library
    {
        "libra",  // id
        "Library",  // name
        "This is a library, with books covering the walls. A stairway leads down.",  // description
        "Library",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "ledg4", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, "bkent", false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Bank Entrance
    {
        "bkent",  // id
        "Bank Entrance",  // name
        "This is the entrance to a bank. There is a door to the north.",  // description
        "Bank Entrance",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "bktvw", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "libra", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Viewing Room
    {
        "bktvw",  // id
        "Viewing Room",  // name
        "This is a viewing room overlooking the bank.",  // description
        "Viewing Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "bkent", false, nullptr },
            { DIR_EAST, "bktwi", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Teller's Room
    {
        "bktwi",  // id
        "Teller's Room",  // name
        "This appears to have been a teller's room. There are broken windows everywhere.",  // description
        "Teller's Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, "bkvau", false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "bktvw", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Bank Vault
    {
        "bkvau",  // id
        "Bank Vault",  // name
        "This is the bank vault. The door is extremely heavy.",  // description
        "Bank Vault",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, "bktwi", false, nullptr },
            { DIR_EAST, "bkbox", false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        2  // numExits
    },

    // Safety Deposit Room
    {
        "bkbox",  // id
        "Safety Deposit Room",  // name
        "This room contains rows of safety deposit boxes.",  // description
        "Safety Deposit Room",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, "bkvau", false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

    // Crypt
    {
        "crypt",  // id
        "Crypt",  // name
        "This is a dark and musty crypt.",  // description
        "Crypt",  // shortDesc
        RFLAG_LAND,  // flags
        {  // exits
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "hades", false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr },
        },
        1  // numExits
    },

};

// Number of objects: 57
#define OBJECT_COUNT 57

// Object ID to index mapping
#define OBJ_MAILB 0
#define OBJ_LEAFL 1
#define OBJ_SWORD 2
#define OBJ_LAMP 3
#define OBJ_TCASE 4
#define OBJ_RUG 5
#define OBJ_WINDO 6
#define OBJ_ROPE 7
#define OBJ_KNIFE 8
#define OBJ_TROLL 9
#define OBJ_AXE 10
#define OBJ_SACK 11
#define OBJ_GARLI 12
#define OBJ_FOOD 13
#define OBJ_BOTTL 14
#define OBJ_WATER 15
#define OBJ_NEST 16
#define OBJ_EGG 17
#define OBJ_LEAVE 18
#define OBJ_GRATE 19
#define OBJ_COAL 20
#define OBJ_MACHI 21
#define OBJ_DIAMO 22
#define OBJ_KEYS 23
#define OBJ_BAR 24
#define OBJ_COFFI 25
#define OBJ_TORCH 26
#define OBJ_BELL 27
#define OBJ_BOOK 28
#define OBJ_CANDL 29
#define OBJ_MATCH 30
#define OBJ_WRENC 31
#define OBJ_SCREW 32
#define OBJ_CYCLO 33
#define OBJ_CHALI 34
#define OBJ_THIEF 35
#define OBJ_STILL 36
#define OBJ_POT 37
#define OBJ_BUOY 38
#define OBJ_SHOVE 39
#define OBJ_SCARA 40
#define OBJ_TRUNK 41
#define OBJ_PAINT 42
#define OBJ_IBOAT 43
#define OBJ_PUMP 44
#define OBJ_STATU 45
#define OBJ_SAFFR 46
#define OBJ_BILLS 47
#define OBJ_PORTR 48
#define OBJ_BAUBL 49
#define OBJ_CANAR 50
#define OBJ_GHOST 51
#define OBJ_SKELE 52
#define OBJ_BALLO 53
#define OBJ_RECEP 54
#define OBJ_BRICK 55
#define OBJ_FUSE 56

// Object data
static const GameObjectData OBJECTS[OBJECT_COUNT] = {
    // small mailbox
    {
        "mailb",  // id
        "small mailbox",  // name
        "There is a small mailbox here.",  // description
        "The mailbox is a small mailbox.",  // examineText
        "",  // readText
        "whous",  // initialLocation
        OFLAG_VISIBLE | OFLAG_CONTAINER,  // flags
        0,  // size
        10,  // capacity
        0  // value
    },

    // leaflet
    {
        "leafl",  // id
        "leaflet",  // name
        "",  // description
        "The leaflet is a small piece of paper.",  // examineText
        "WELCOME TO ZORK!\n\nZORK is a game of adventure, danger, and low cunning. In it you will explore some of the most amazing territory ever seen by mortals. No computer should be without one!",  // readText
        "mailb",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_READABLE,  // flags
        2,  // size
        0,  // capacity
        0  // value
    },

    // elvish sword
    {
        "sword",  // id
        "elvish sword",  // name
        "Above the trophy case hangs an elvish sword of great antiquity.",  // description
        "The sword is of exquisite craftsmanship. It is inscribed with ancient elvish runes.",  // examineText
        "",  // readText
        "lroom",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_WEAPON,  // flags
        30,  // size
        0,  // capacity
        0  // value
    },

    // brass lantern
    {
        "lamp",  // id
        "brass lantern",  // name
        "There is a brass lantern (battery-powered) here.",  // description
        "The lamp is a battery-powered brass lantern.",  // examineText
        "",  // readText
        "lroom",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_LIGHTSOURCE,  // flags
        15,  // size
        0,  // capacity
        0  // value
    },

    // trophy case
    {
        "tcase",  // id
        "trophy case",  // name
        "",  // description
        "The trophy case is empty.",  // examineText
        "",  // readText
        "lroom",  // initialLocation
        OFLAG_VISIBLE | OFLAG_CONTAINER | OFLAG_TRANSPARENT | OFLAG_OPEN,  // flags
        0,  // size
        100,  // capacity
        0  // value
    },

    // oriental rug
    {
        "rug",  // id
        "oriental rug",  // name
        "",  // description
        "The rug is extremely beautiful and tightly woven. It does not appear to be attached to the floor.",  // examineText
        "",  // readText
        "lroom",  // initialLocation
        OFLAG_VISIBLE,  // flags
        0,  // size
        0,  // capacity
        0  // value
    },

    // small window
    {
        "windo",  // id
        "small window",  // name
        "",  // description
        "The window is slightly ajar.",  // examineText
        "",  // readText
        "ehous",  // initialLocation
        OFLAG_VISIBLE | OFLAG_DOOR | OFLAG_OPEN,  // flags
        0,  // size
        0,  // capacity
        0  // value
    },

    // coil of rope
    {
        "rope",  // id
        "coil of rope",  // name
        "A large coil of rope is lying in the corner.",  // description
        "The rope is strong and about 50 feet long.",  // examineText
        "",  // readText
        "attic",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TIEABLE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // nasty knife
    {
        "knife",  // id
        "nasty knife",  // name
        "On a table is a nasty-looking knife.",  // description
        "The knife looks very sharp and unpleasant.",  // examineText
        "",  // readText
        "attic",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_WEAPON,  // flags
        20,  // size
        0,  // capacity
        0  // value
    },

    // troll
    {
        "troll",  // id
        "troll",  // name
        "A nasty-looking troll, brandishing a bloody axe, blocks all passages out of the room.",  // description
        "The troll is a large, nasty creature that wants nothing more than to kill you.",  // examineText
        "",  // readText
        "mtrol",  // initialLocation
        OFLAG_VISIBLE | OFLAG_ACTOR | OFLAG_VILLAIN | OFLAG_FIGHTER,  // flags
        0,  // size
        0,  // capacity
        0  // value
    },

    // bloody axe
    {
        "axe",  // id
        "bloody axe",  // name
        "",  // description
        "The axe is covered in blood and looks extremely dangerous.",  // examineText
        "",  // readText
        nullptr,  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_WEAPON,  // flags
        0,  // size
        0,  // capacity
        0  // value
    },

    // brown sack
    {
        "sack",  // id
        "brown sack",  // name
        "On the table is an elongated brown sack, smelling of hot peppers.",  // description
        "The sack is an ordinary brown sack.",  // examineText
        "",  // readText
        "kitch",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_CONTAINER,  // flags
        9,  // size
        15,  // capacity
        0  // value
    },

    // clove of garlic
    {
        "garli",  // id
        "clove of garlic",  // name
        "",  // description
        "The garlic is a standard cooking ingredient.",  // examineText
        "",  // readText
        "sack",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_FOOD,  // flags
        4,  // size
        0,  // capacity
        0  // value
    },

    // lunch
    {
        "food",  // id
        "lunch",  // name
        "",  // description
        "The lunch looks delicious.",  // examineText
        "",  // readText
        "sack",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_FOOD,  // flags
        6,  // size
        0,  // capacity
        0  // value
    },

    // glass bottle
    {
        "bottl",  // id
        "glass bottle",  // name
        "A bottle is sitting on the table.",  // description
        "The glass bottle contains a quantity of water.",  // examineText
        "",  // readText
        "kitch",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_CONTAINER | OFLAG_TRANSPARENT,  // flags
        7,  // size
        5,  // capacity
        0  // value
    },

    // quantity of water
    {
        "water",  // id
        "quantity of water",  // name
        "",  // description
        "It looks like ordinary water.",  // examineText
        "",  // readText
        "bottl",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_DRINKABLE,  // flags
        4,  // size
        0,  // capacity
        0  // value
    },

    // bird's nest
    {
        "nest",  // id
        "bird's nest",  // name
        "Beside you on the branch is a small bird's nest.",  // description
        "The nest looks like it was made by a songbird of some kind.",  // examineText
        "",  // readText
        "uptree",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_CONTAINER,  // flags
        5,  // size
        5,  // capacity
        0  // value
    },

    // jewel-encrusted egg
    {
        "egg",  // id
        "jewel-encrusted egg",  // name
        "",  // description
        "The egg is covered with beautiful jewels that glitter in the light.",  // examineText
        "",  // readText
        "nest",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        6,  // size
        0,  // capacity
        5  // value
    },

    // pile of leaves
    {
        "leave",  // id
        "pile of leaves",  // name
        "There is a pile of leaves here.",  // description
        "The leaves are brown and dry.",  // examineText
        "",  // readText
        "mgrat",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        25,  // size
        0,  // capacity
        0  // value
    },

    // grating
    {
        "grate",  // id
        "grating",  // name
        "There is a grating securely fastened into the ground.",  // description
        "The grating is locked.",  // examineText
        "",  // readText
        "mgrat",  // initialLocation
        OFLAG_VISIBLE | OFLAG_DOOR,  // flags
        0,  // size
        0,  // capacity
        0  // value
    },

    // small pile of coal
    {
        "coal",  // id
        "small pile of coal",  // name
        "There is a small pile of coal here.",  // description
        "It is ordinary coal, suitable for burning.",  // examineText
        "",  // readText
        "coal1",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        20,  // size
        0,  // capacity
        0  // value
    },

    // machine
    {
        "machi",  // id
        "machine",  // name
        "There is a large machine here.",  // description
        "The machine is a coal-operated gadget that looks like it would convert coal into diamonds.",  // examineText
        "",  // readText
        "mmach",  // initialLocation
        OFLAG_VISIBLE | OFLAG_CONTAINER,  // flags
        0,  // size
        50,  // capacity
        0  // value
    },

    // huge diamond
    {
        "diamo",  // id
        "huge diamond",  // name
        "",  // description
        "The diamond is enormous and sparkles brilliantly.",  // examineText
        "",  // readText
        nullptr,  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        6,  // size
        0,  // capacity
        10  // value
    },

    // set of skeleton keys
    {
        "keys",  // id
        "set of skeleton keys",  // name
        "There is a set of skeleton keys here.",  // description
        "The keys are old but still functional.",  // examineText
        "",  // readText
        "maze1",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // platinum bar
    {
        "bar",  // id
        "platinum bar",  // name
        "There is a large platinum bar here.",  // description
        "The bar is stamped 'Frobozz Magic Platinum Company'.",  // examineText
        "",  // readText
        "llrm2",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        20,  // size
        0,  // capacity
        10  // value
    },

    // gold coffin
    {
        "coffi",  // id
        "gold coffin",  // name
        "There is a gold coffin here.",  // description
        "The coffin is beautifully crafted, with gold hieroglyphics.",  // examineText
        "",  // readText
        "egypt",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_CONTAINER | OFLAG_TREASURE,  // flags
        55,  // size
        35,  // capacity
        10  // value
    },

    // ivory torch
    {
        "torch",  // id
        "ivory torch",  // name
        "There is an ivory torch here.",  // description
        "The torch is made of ivory and burns with an eternal flame.",  // examineText
        "",  // readText
        "mtorc",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_LIGHTSOURCE | OFLAG_TREASURE,  // flags
        20,  // size
        0,  // capacity
        10  // value
    },

    // brass bell
    {
        "bell",  // id
        "brass bell",  // name
        "There is a brass bell here.",  // description
        "The bell is made of brass and is very heavy.",  // examineText
        "",  // readText
        "temp1",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // black book
    {
        "book",  // id
        "black book",  // name
        "On the altar is a large black book.",  // description
        "The book is written in a strange language.",  // examineText
        "This ancient tome contains many dark secrets of the Great Underground Empire.",  // readText
        "temp2",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_READABLE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // pair of candles
    {
        "candl",  // id
        "pair of candles",  // name
        "A pair of candles is here.",  // description
        "The candles are white and unlit.",  // examineText
        "",  // readText
        "temp2",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_LIGHTSOURCE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // matchbook
    {
        "match",  // id
        "matchbook",  // name
        "There is a matchbook here.",  // description
        "The matchbook contains several wooden matches.",  // examineText
        "",  // readText
        "damp",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        2,  // size
        0,  // capacity
        0  // value
    },

    // wrench
    {
        "wrenc",  // id
        "wrench",  // name
        "There is a wrench here.",  // description
        "The wrench is made of solid steel.",  // examineText
        "",  // readText
        "maint",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // screwdriver
    {
        "screw",  // id
        "screwdriver",  // name
        "There is a screwdriver here.",  // description
        "The screwdriver has a flat head.",  // examineText
        "",  // readText
        "maint",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // cyclops
    {
        "cyclo",  // id
        "cyclops",  // name
        "A cyclops, who looks extremely hungry, blocks the staircase.",  // description
        "The cyclops has one large eye and very large hands.",  // examineText
        "",  // readText
        "mcycl",  // initialLocation
        OFLAG_VISIBLE | OFLAG_ACTOR | OFLAG_VILLAIN | OFLAG_FIGHTER,  // flags
        0,  // size
        0,  // capacity
        0  // value
    },

    // silver chalice
    {
        "chali",  // id
        "silver chalice",  // name
        "There is a silver chalice here.",  // description
        "The chalice is beautifully crafted with intricate designs.",  // examineText
        "",  // readText
        "treas",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        10,  // size
        0,  // capacity
        10  // value
    },

    // thief
    {
        "thief",  // id
        "thief",  // name
        "There is a seedy-looking individual, wielding a stiletto, here.",  // description
        "The thief is a dangerous character.",  // examineText
        "",  // readText
        "treas",  // initialLocation
        OFLAG_VISIBLE | OFLAG_ACTOR | OFLAG_VILLAIN | OFLAG_FIGHTER,  // flags
        0,  // size
        0,  // capacity
        0  // value
    },

    // stiletto
    {
        "still",  // id
        "stiletto",  // name
        "",  // description
        "The stiletto is thin and deadly.",  // examineText
        "",  // readText
        nullptr,  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_WEAPON,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // pot of gold
    {
        "pot",  // id
        "pot of gold",  // name
        "At the end of the rainbow is a pot of gold.",  // description
        "The pot is full of gold coins.",  // examineText
        "",  // readText
        "pog",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        15,  // size
        0,  // capacity
        10  // value
    },

    // buoy
    {
        "buoy",  // id
        "buoy",  // name
        "There is a red buoy here.",  // description
        "The buoy is made of cork and marked with the words 'Frobozz Magic Buoy Company'.",  // examineText
        "",  // readText
        "sbeac",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_CONTAINER,  // flags
        10,  // size
        20,  // capacity
        0  // value
    },

    // shovel
    {
        "shove",  // id
        "shovel",  // name
        "There is a shovel here.",  // description
        "The shovel is a standard digging implement.",  // examineText
        "",  // readText
        "sbeac",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        15,  // size
        0,  // capacity
        0  // value
    },

    // jeweled scarab
    {
        "scara",  // id
        "jeweled scarab",  // name
        "There is a jeweled scarab here.",  // description
        "The scarab is encrusted with precious gems.",  // examineText
        "",  // readText
        "egypt",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        5,  // size
        0,  // capacity
        5  // value
    },

    // trunk of jewels
    {
        "trunk",  // id
        "trunk of jewels",  // name
        "There is a trunk filled with jewels here.",  // description
        "The trunk overflows with precious stones.",  // examineText
        "",  // readText
        "reser",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_CONTAINER | OFLAG_TREASURE,  // flags
        35,  // size
        35,  // capacity
        15  // value
    },

    // painting
    {
        "paint",  // id
        "painting",  // name
        "A painting is on the wall.",  // description
        "The painting depicts a fiendishly detailed scene of a hellscape.",  // examineText
        "",  // readText
        "galry",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        15,  // size
        0,  // capacity
        4  // value
    },

    // magic boat
    {
        "iboat",  // id
        "magic boat",  // name
        "There is a pile of plastic here which has a small valve on it.",  // description
        "The boat is made of plastic and has 'Frobozz Magic Boat Co.' printed on it.",  // examineText
        "",  // readText
        "dlobb",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        20,  // size
        0,  // capacity
        0  // value
    },

    // hand pump
    {
        "pump",  // id
        "hand pump",  // name
        "There is a hand pump here.",  // description
        "The pump is for inflating something.",  // examineText
        "",  // readText
        "reser",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // crystal statue
    {
        "statu",  // id
        "crystal statue",  // name
        "There is a beautiful crystal statue here.",  // description
        "The statue is made of pure crystal and depicts a warrior.",  // examineText
        "",  // readText
        nullptr,  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        10,  // size
        0,  // capacity
        10  // value
    },

    // tin of spices
    {
        "saffr",  // id
        "tin of spices",  // name
        "There is a tin of rare spices here.",  // description
        "The tin contains valuable saffron.",  // examineText
        "",  // readText
        "pearl",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        5,  // size
        0,  // capacity
        5  // value
    },

    // stack of bills
    {
        "bills",  // id
        "stack of bills",  // name
        "There is a stack of bills here.",  // description
        "The bills are worth a small fortune.",  // examineText
        "",  // readText
        "bkvau",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        5,  // size
        0,  // capacity
        10  // value
    },

    // portrait
    {
        "portr",  // id
        "portrait",  // name
        "There is a portrait here.",  // description
        "The portrait depicts J. Pierpont Flathead.",  // examineText
        "",  // readText
        "bktvw",  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        10,  // size
        0,  // capacity
        10  // value
    },

    // brass bauble
    {
        "baubl",  // id
        "brass bauble",  // name
        "A brass bauble is here.",  // description
        "The bauble is quite ornate.",  // examineText
        "",  // readText
        nullptr,  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        3,  // size
        0,  // capacity
        1  // value
    },

    // golden canary
    {
        "canar",  // id
        "golden canary",  // name
        "A golden clockwork canary is here.",  // description
        "The canary is made of gold and sings when wound.",  // examineText
        "",  // readText
        nullptr,  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE | OFLAG_TREASURE,  // flags
        4,  // size
        0,  // capacity
        6  // value
    },

    // ghost
    {
        "ghost",  // id
        "ghost",  // name
        "The ghost of a spirit haunts this place.",  // description
        "The ghost is pale and translucent.",  // examineText
        "",  // readText
        "hades",  // initialLocation
        OFLAG_VISIBLE | OFLAG_ACTOR,  // flags
        0,  // size
        0,  // capacity
        0  // value
    },

    // skeleton
    {
        "skele",  // id
        "skeleton",  // name
        "There is a skeleton here.",  // description
        "The skeleton appears to be centuries old.",  // examineText
        "",  // readText
        "hades",  // initialLocation
        OFLAG_VISIBLE,  // flags
        50,  // size
        0,  // capacity
        0  // value
    },

    // hot air balloon
    {
        "ballo",  // id
        "hot air balloon",  // name
        "There is a hot air balloon here.",  // description
        "The balloon is large and colorful.",  // examineText
        "",  // readText
        "vlbot",  // initialLocation
        OFLAG_VISIBLE | OFLAG_CONTAINER,  // flags
        0,  // size
        100,  // capacity
        0  // value
    },

    // receptacle
    {
        "recep",  // id
        "receptacle",  // name
        "There is a wire receptacle here.",  // description
        "The receptacle can hold small objects.",  // examineText
        "",  // readText
        "vlbot",  // initialLocation
        OFLAG_VISIBLE | OFLAG_CONTAINER,  // flags
        0,  // size
        20,  // capacity
        0  // value
    },

    // brick
    {
        "brick",  // id
        "brick",  // name
        "There is a brick here.",  // description
        "The brick is red and heavy.",  // examineText
        "",  // readText
        nullptr,  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        10,  // size
        0,  // capacity
        0  // value
    },

    // fuse
    {
        "fuse",  // id
        "fuse",  // name
        "There is a fuse here.",  // description
        "The fuse is short and appears to be functional.",  // examineText
        "",  // readText
        nullptr,  // initialLocation
        OFLAG_VISIBLE | OFLAG_TAKEABLE,  // flags
        2,  // size
        0,  // capacity
        0  // value
    },

};
