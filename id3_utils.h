#ifndef ID3_UTILS_H
#define ID3_UTILS_H

#include "main.h"

#define TAG_HEADER_SIZE 10
#define FRAME_HEADER_SIZE 10

/**
 * @brief Structure to hold ID3 header data.
 */
typedef struct {
    char *size; /**< Size of the ID3 tag(excluding the ID3 header) */
    char *version; /**< Version of the ID3 tag */
} HeaderData;

/**
 * @brief Structure to hold ID3 tag data.
 */
typedef struct {
    char *title;   /**< Title of the song */
    char *artist;  /**< Artist of the song */
    char *album;   /**< Album name */
    char *track;   /**< Track number or position in the album */
    char *year;    /**< Year of release */
    char *comment; /**< Comment */
    char *genre;   /**< Genre */
    char *album_art;  /**< Album art data */   
} TagData;

/**
 * @brief Decodes a sync-safe integer used in ID3 tags.
 *
 * In ID3 metadata (e.g., in MP3 files), size fields are stored using a sync-safe format.
 * This means each byte uses only 7 bits, and the most significant bit (MSB) is always 0.
 *
 * MP3 files use synchronization signals (typically a series of 1s like 0xFF) to mark the start
 * of audio frames. A regular 32-bit integer might accidentally contain such patterns, which
 * could confuse MP3 players into misinterpreting metadata as audio data. By ensuring the MSB
 * of each byte is 0, sync-safe encoding prevents false frame syncs.
 *
 * This function decodes a 28-bit sync-safe integer (stored across 4 bytes) into a normal integer.
 *
 * @return Decoded 32-bit integer value from sync-safe format.
 */
unsigned int decode_syncsafe(unsigned char[]);

/**
 * @brief Encodes an integer into sync-safe format used in ID3 tag size.
 */
void encode_syncsafe(unsigned int, unsigned char *);

/**
 * @brief Creates a new HeaderData structure.
 * 
 * @return Pointer to the newly created HeaderData structure.
 */
HeaderData *create_header_data();

/**
 * @brief Creates a new TagData structure.
 * 
 * @return Pointer to the newly created TagData structure.
 */
TagData *create_tag_data();

/**
 * @brief Frees the dynamically allocated memory for header data variables
 */
void free_header_data(HeaderData *);

/**
 * @brief Frees the dynamically allocated memory for tag data variables
 */
void free_tag_data(TagData *);

#endif