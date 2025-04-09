#ifndef ID3_READER_H
#define ID3_READER_H

#include "main.h"
#include "id3_utils.h"

/**
 * @brief Reads the ID3 tags from the MP3 file
 * @return HeaderData Structure
 */
HeaderData *read_id3_header(FILE *, unsigned int *);

/**
 * @brief Reads the ID3 tags from the MP3 file
 * @return TagData Structure
 */
TagData *read_id3_tag(FILE *, unsigned int *);

/**
 * @brief Displays the MP3 details
 */
void display_metadata(const HeaderData *, const TagData *);

/**
 * @brief View the MP3 tag
 */
void view_tags(const char *);

#endif // ID3_READER_H