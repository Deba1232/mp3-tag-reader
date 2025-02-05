/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */
#include "id3_reader.h"
#include "error_handling.h"

/**
 * @brief Reads the ID3 tags from the MP3 file
 * @return Reads the ID3 tags from the MP3 file
 */
TagData* read_id3_tags(const char *filename) {
    // Implementation for reading ID3 tags
    return NULL;
}

/**
TODO: Add documention as sample given
 */
void display_metadata(const TagData *data) {
    // Implementation for displaying metadata
}

/**
TODO: Add documention as sample given
 */
void view_tags(const char *filename) {
    char *version = (char *)calloc(2, sizeof(short));
    if(!version){
        fprintf(stderr, "Memory couldn't be allocated\n");
        exit(EXIT_FAILURE);
    }

    TagData *data = read_id3_tags(filename);
    if (!data) {
        display_error("Failed to read ID3 tags.");
        return;
    }

    display_metadata(data);
    
    free(version);
    free_tag_data(data);
}
