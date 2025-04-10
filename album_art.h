#ifndef ALBUM_ART_H
#define ALBUM_ART_H

#include "id3_utils.h"

/**
 * @brief Extracts the album art from the MP3 file
 */
void extract_album_art(FILE *, unsigned int, char *);

#endif