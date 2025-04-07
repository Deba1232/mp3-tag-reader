#include "id3_utils.h"

/**
 * @brief Decodes a sync-safe integer used in ID3 tag size.
 *
 * In ID3 metadata (e.g., in MP3 files), header size field is stored using a sync-safe format.
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
unsigned int decode_syncsafe(unsigned char bytes[4]){
    return (bytes[0] << 21) | (bytes[1] << 14) | (bytes[2] << 7) | bytes[3];
}

/**
 * @brief Initializes each field to be displayed for the MP3 file
 * @return Pointer to the TagData structure 
 */
HeaderData *create_header_data() {
    HeaderData *header_data = (HeaderData *)calloc(1, sizeof(HeaderData));
    if (header_data) {
        header_data->version = (char *)calloc(2, sizeof(char));
        if(!header_data->version){
            perror("Memory allocation failed");
            return NULL;
        }
        header_data->size = (char *)calloc(4, sizeof(char));
        if(!header_data->size){
            perror("Memory allocation failed");
            return NULL;
        }
    }
    return header_data;
}

/**
 * @brief Initializes each field to be displayed for the MP3 file
 * @return Pointer to the TagData structure 
 */
TagData* create_tag_data() {
    TagData *data = (TagData *)calloc(1, sizeof(TagData));
    if (data) {
        data->title = NULL;
        data->artist = NULL;
        data->album = NULL;
        data->year = NULL;
        data->comment = NULL;
        data->genre = NULL;
        // Initialize other fields as needed
    }
    return data;
}

/**
TODO: Add documention as sample given
 */
void free_header_data(HeaderData *header_data) {
    if (header_data) {
        free(header_data->size);
        free(header_data->version);
        free(header_data);
    }
}

/**
TODO: Add documention as sample given
 */
void free_tag_data(TagData *data) {
    if (data) {
        free(data->title);
        free(data->artist);
        free(data->album);
        free(data->year);
        free(data->comment);
        free(data->genre);
        // Free other fields as needed
        free(data);
    }
}
