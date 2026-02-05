#pragma once

#include "GameEngine.h"

/**
 * GameData - Room and object definitions for Zork
 *
 * Note: This is a subset for initial testing. Full room/object data
 * will be added in Phase 4 of development.
 */

// Number of rooms defined
#define ROOM_COUNT 8

// Room data - initial subset around the white house
static const Room ROOMS[ROOM_COUNT] = {
    // West of House (starting location)
    {
        "whous",
        "West of House",
        "You are standing in an open field west of a white house, with a boarded front door.",
        "West of House",
        RFLAG_LIGHT | RFLAG_LAND,
        {
            { DIR_NORTH, "nhous", false, nullptr },     // north -> North of House
            { DIR_SOUTH, "shous", false, nullptr },     // south -> South of House
            { DIR_WEST, "fore1", false, nullptr },      // west -> Forest
            { DIR_EAST, nullptr, true, "The door is boarded and you can't remove the boards." },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, "fore1", false, nullptr }, // northwest -> Forest
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, "fore1", false, nullptr }, // southwest -> Forest
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr }
        },
        4 // numExits
    },

    // North of House
    {
        "nhous",
        "North of House",
        "You are facing the north side of a white house. There is no door here, and all the windows are boarded up. To the north a narrow path winds through the trees.",
        "North of House",
        RFLAG_LIGHT | RFLAG_LAND,
        {
            { DIR_NORTH, "fore3", false, nullptr },     // north -> Forest Path
            { DIR_SOUTH, "whous", false, nullptr },     // south -> West of House
            { DIR_EAST, "ehous", false, nullptr },      // east -> Behind House
            { DIR_WEST, "fore1", false, nullptr },      // west -> Forest
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr }
        },
        4
    },

    // South of House
    {
        "shous",
        "South of House",
        "You are facing the south side of a white house. There is no door here, and all the windows are boarded.",
        "South of House",
        RFLAG_LIGHT | RFLAG_LAND,
        {
            { DIR_NORTH, "whous", false, nullptr },     // north -> West of House
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "ehous", false, nullptr },      // east -> Behind House
            { DIR_WEST, "fore1", false, nullptr },      // west -> Forest
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr }
        },
        3
    },

    // Behind House (East of House)
    {
        "ehous",
        "Behind House",
        "You are behind the white house. A path leads into the forest to the east. In one corner of the house there is a small window which is slightly ajar.",
        "Behind House",
        RFLAG_LIGHT | RFLAG_LAND,
        {
            { DIR_NORTH, "nhous", false, nullptr },     // north -> North of House
            { DIR_SOUTH, "shous", false, nullptr },     // south -> South of House
            { DIR_EAST, "clear", false, nullptr },      // east -> Clearing
            { DIR_WEST, nullptr, true, "The windows are all boarded." },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, "kitch", false, nullptr },        // in -> Kitchen (through window)
            { DIR_OUT, nullptr, false, nullptr }
        },
        4
    },

    // Kitchen
    {
        "kitch",
        "Kitchen",
        "You are in the kitchen of the white house. A table seems to have been used recently for the preparation of food. A passage leads to the west and a dark staircase can be seen leading upward. A dark chimney leads down and to the east is a small window which is open.",
        "Kitchen",
        RFLAG_LIGHT | RFLAG_LAND,
        {
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "ehous", false, nullptr },      // east -> Behind House (through window)
            { DIR_WEST, "lroom", false, nullptr },      // west -> Living Room
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "attic", false, nullptr },        // up -> Attic
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, "ehous", false, nullptr }        // out -> Behind House
        },
        4
    },

    // Living Room
    {
        "lroom",
        "Living Room",
        "You are in the living room. There is a doorway to the east, a wooden door with strange gothic lettering to the west, which appears to be nailed shut, a trophy case, and a large oriental rug in the center of the room.",
        "Living Room",
        RFLAG_LIGHT | RFLAG_LAND,
        {
            { DIR_NORTH, nullptr, false, nullptr },
            { DIR_SOUTH, nullptr, false, nullptr },
            { DIR_EAST, "kitch", false, nullptr },      // east -> Kitchen
            { DIR_WEST, nullptr, true, "The door is nailed shut." },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr }
        },
        1
    },

    // Forest
    {
        "fore1",
        "Forest",
        "This is a forest, with trees in all directions. To the east, there appears to be sunlight.",
        "Forest",
        RFLAG_LIGHT | RFLAG_LAND,
        {
            { DIR_NORTH, "fore1", false, nullptr },     // Wandering in forest
            { DIR_SOUTH, "fore1", false, nullptr },
            { DIR_EAST, "whous", false, nullptr },      // east -> West of House
            { DIR_WEST, "fore1", false, nullptr },
            { DIR_NORTHEAST, "fore1", false, nullptr },
            { DIR_NORTHWEST, "fore1", false, nullptr },
            { DIR_SOUTHEAST, "fore1", false, nullptr },
            { DIR_SOUTHWEST, "fore1", false, nullptr },
            { DIR_UP, nullptr, false, nullptr },
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr }
        },
        8
    },

    // Forest Path
    {
        "fore3",
        "Forest Path",
        "This is a path winding through a dimly lit forest. The path heads north-south here. One particularly large tree with some low branches stands at the edge of the path.",
        "Forest Path",
        RFLAG_LIGHT | RFLAG_LAND,
        {
            { DIR_NORTH, "clear", false, nullptr },     // north -> Clearing
            { DIR_SOUTH, "nhous", false, nullptr },     // south -> North of House
            { DIR_EAST, nullptr, false, nullptr },
            { DIR_WEST, nullptr, false, nullptr },
            { DIR_NORTHEAST, nullptr, false, nullptr },
            { DIR_NORTHWEST, nullptr, false, nullptr },
            { DIR_SOUTHEAST, nullptr, false, nullptr },
            { DIR_SOUTHWEST, nullptr, false, nullptr },
            { DIR_UP, "tree", false, nullptr },         // up -> Up a Tree
            { DIR_DOWN, nullptr, false, nullptr },
            { DIR_IN, nullptr, false, nullptr },
            { DIR_OUT, nullptr, false, nullptr }
        },
        3
    }
};

// Number of objects defined
#define OBJECT_COUNT 0

// Object data - TODO: Add in Phase 4
// static const GameObject OBJECTS[OBJECT_COUNT] = {
// };
