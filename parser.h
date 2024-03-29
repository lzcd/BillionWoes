#pragma once

#include <stdio.h>
#include "memory_arena.h"
#include "character_tree.h"

typedef struct CityInfo
{
    int record_count;
    float record_total;
    float min;
    float max;
} CityInfo;

void parse_file(char *input_path, MemoryArena *city_name_tree_arena, MemoryArena *city_infos_arena)
{
    enum parse_mode
    {
        parse_mode_pre_content,
        parse_mode_comment,
        parse_mode_city,
        parse_mode_temperature
    };

    FILE *input_file = fopen(input_path, "r");
    char character;
    enum parse_mode mode = parse_mode_pre_content;

    char city_buffer[200];
    int city_buffer_index = 0;
    char temperature_buffer[100];
    int temperature_buffer_index = 0;
    int city_info_index;

    while ((character = fgetc(input_file)) != EOF)
    {
        switch (mode)
        {
        case parse_mode_pre_content:
            switch (character)
            {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                break;
            case '#':
                mode = parse_mode_comment;
                break;
            default:
                mode = parse_mode_city;
                city_buffer_index = 0;
                city_buffer[city_buffer_index] = character;
                city_buffer_index++;
                break;
            }
            break;
        case parse_mode_comment:
            switch (character)
            {
            case '\r':
            case '\n':
                mode = parse_mode_pre_content;
                break;
            }
            break;
        case parse_mode_city:
            switch (character)
            {
            case ';':
            {
                city_buffer[city_buffer_index] = 0;
                // printf("city: %s\n", city_buffer);
                int city_leaf_node_index = find_or_create_leaf_node_index(city_name_tree_arena, 0, city_buffer, -1);

                assert(city_leaf_node_index >= 0);
                CharacterNode *city_name_nodes = (CharacterNode *)(city_name_tree_arena->data);
                CharacterNode *city_name_node = &city_name_nodes[city_leaf_node_index];
                city_info_index = city_name_node->terminal_index;
                if (city_info_index < 0)
                {
                    CityInfo *new_info = arena_append(city_infos_arena, sizeof(CityInfo));
                    new_info->record_total = 0;
                    new_info->record_count = 0;
                    new_info->max = -9999;
                    new_info->min = 9999;
                    int new_info_index = city_infos_arena->count - 1;
                    city_name_node->terminal_index = new_info_index;
                    city_info_index = new_info_index;
                }
                assert(city_info_index >= 0);
                mode = parse_mode_temperature;
                temperature_buffer_index = 0;
            }
            break;
            default:
                city_buffer[city_buffer_index] = character;
                city_buffer_index++;
                break;
            }
            break;
        case parse_mode_temperature:
            switch (character)
            {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
            {
                temperature_buffer[temperature_buffer_index] = 0;
                // printf("temp: %s\n", temp_buffer);
                float temperature = atof(temperature_buffer);
                CityInfo *city_infos = (CityInfo *)(city_infos_arena->data);
                assert(city_info_index >= 0);
                CityInfo *city_info = &city_infos[city_info_index];
                city_info->record_total += temperature;
                city_info->record_count += 1;
                if (temperature > city_info->max)
                {
                    city_info->max = temperature;
                }
                if (temperature < city_info->min)
                {
                    city_info->min = temperature;
                }
                mode = parse_mode_pre_content;
            }
            break;
            case '#':
            {
                temperature_buffer[temperature_buffer_index] = 0;
                // printf("temp: %s\n", temp_buffer);
                float temperature = atof(temperature_buffer);
                CityInfo *city_infos = (CityInfo *)(city_infos_arena->data);
                assert(city_info_index >= 0);
                CityInfo *city_info = &city_infos[city_info_index];
                city_info->record_total += temperature;
                city_info->record_total += 1;
                if (temperature > city_info->max)
                {
                    city_info->max = temperature;
                }
                if (temperature < city_info->min)
                {
                    city_info->min = temperature;
                }
                mode = parse_mode_comment;
            }
            break;
            default:
                temperature_buffer[temperature_buffer_index] = character;
                temperature_buffer_index++;
                break;
            }
            break;
        }
    }

    fclose(input_file);
    input_file = NULL;
}
