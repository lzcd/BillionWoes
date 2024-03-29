#pragma once
#include <string.h>
#include "memory_arena.h"

typedef struct CharacterNode
{
    char test_character;
    int on_match_success_index;
    int on_match_failure_index;
    int terminal_index;
} CharacterNode;

CharacterNode retrieve_node(MemoryArena *arena, int node_index)
{
    CharacterNode *nodes = (CharacterNode *)(arena->data);
    CharacterNode node = nodes[node_index];
    return node;
}

int find_or_create_leaf_node_index(MemoryArena *arena, int start_index, char *text, int new_node_terminal_index)
{
    // following existing matching nodes
    int node_index = start_index;
    int character_index = 0;
    while (text[character_index] != 0)
    {
        char character = text[character_index];
        bool match_found = false;
        if (arena->count > 0)
        {
            while (true)
            {
                CharacterNode *nodes = (CharacterNode *)(arena->data);
                CharacterNode node = nodes[node_index];
                if (character == node.test_character)
                {
                    match_found = true;
                    break;
                }
                else
                {
                    if (node.on_match_failure_index != 0)
                    {
                        node_index = node.on_match_failure_index;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        if (!match_found)
        {
            break;
        }
        CharacterNode *nodes = (CharacterNode *)(arena->data);
        CharacterNode node = nodes[node_index];
        int next_node_index = node.on_match_success_index;
        if (next_node_index == 0)
        {
            next_node_index = node_index;
        }
        node_index = next_node_index;

        character_index++;
    }

    // append new path of nodes any unmatched characters
    while (text[character_index] != 0)
    {
        char character = text[character_index];

        bool previous_node_exists = arena->count > 0;
        CharacterNode *new_node = arena_append(arena, sizeof(CharacterNode));
        size_t new_node_index = arena->count - 1;

        new_node->test_character = character;
        new_node->on_match_success_index = 0;
        new_node->on_match_failure_index = 0;
        new_node->terminal_index = new_node_terminal_index;
        if (previous_node_exists)
        {
            CharacterNode *nodes = (CharacterNode *)(arena->data);
            CharacterNode *node = &nodes[node_index];
            if (node->on_match_success_index == 0)
            {
                node->on_match_success_index = new_node_index;
            }
            else
            {
                node->on_match_failure_index = new_node_index;
            }
        }
        node_index = new_node_index;

        character_index++;
    }

    return node_index;
}