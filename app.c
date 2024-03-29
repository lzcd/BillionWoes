#include <stdio.h>
#include <limits.h>
#include <time.h>

#include "memory_arena.h"
#include "character_tree.h"
#include "parser.h"

void print_node(
    CharacterNode *city_name_nodes,
    int node_index,
    char *print_buffer,
    int character_index,
    CityInfo *city_infos,
    FILE *output_file)
{


    char previous_minimum_character = CHAR_MIN;
    int minimum_character_node_index = 0;
    bool minimum_character_found = true;
    while (minimum_character_found)
    {
        int search_node_index = node_index;
        char minimum_character = CHAR_MAX;
        minimum_character_found = false;
        do
        {
            CharacterNode search_node = city_name_nodes[search_node_index];
            if (search_node.test_character > previous_minimum_character &&
                search_node.test_character < minimum_character)
            {
                minimum_character = search_node.test_character;
                minimum_character_node_index = search_node_index;
                minimum_character_found = true;
            }
            search_node_index = search_node.on_match_failure_index;
        } while (search_node_index > 0);

        if (minimum_character_found)
        {
            previous_minimum_character = minimum_character;

            CharacterNode selected_node = city_name_nodes[minimum_character_node_index];
            print_buffer[character_index] = selected_node.test_character;
            if (selected_node.on_match_success_index > 0)
            {
                print_node(
                    city_name_nodes, 
                    selected_node.on_match_success_index, 
                    print_buffer, 
                    character_index + 1, 
                    city_infos,
                    output_file);
            }

            if (selected_node.terminal_index >= 0)
            {
                print_buffer[character_index] = selected_node.test_character;
                print_buffer[character_index + 1] = 0;
                fprintf(output_file, print_buffer);
                fprintf(output_file, "=");
                CityInfo city_info = city_infos[selected_node.terminal_index];
                float min = city_info.min;
                float max = city_info.max;
                float mean = city_info.record_total / city_info.record_count;
                fprintf(output_file, "%.1f/%.1f/%.1f ", min, mean, max);
            }
        }
    }
}


int main(int argc, char const *argv[])
{
    clock_t start_time = clock();

    MemoryArena city_name_tree_arena = arena_create();
    MemoryArena city_infos_arena = arena_create();

    char *input_path = "/home/luke/Projects/1brc/data/measurements.txt";

    parse_file(input_path, &city_name_tree_arena, &city_infos_arena);

    char *output_path = "output.txt";
    FILE *output_file = fopen(output_path, "w");
    fprintf(output_file, "{");

    char print_buffer[200];
    CharacterNode *city_name_nodes = (CharacterNode *)(city_name_tree_arena.data);
    CityInfo *city_infos = (CityInfo *)(city_infos_arena.data);
    print_node(city_name_nodes, 0, print_buffer, 0, city_infos, output_file);

    fprintf(output_file, "}");
    fclose(output_file);
    output_file = NULL;

    clock_t end_time = clock();
    double time_taken = ((double)(end_time -start_time))/CLOCKS_PER_SEC;
    printf("The program took %f seconds to execute", time_taken);

    return 0;
}
