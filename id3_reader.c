/**
 * @file id3_reader.c
 * @brief Implementation of functions for reading ID3 tags from MP3 files.
 */
#include "id3_reader.h"
#include "error_handling.h"
#include "id3_utils.h"

//Size of the ID3 tag (excluding the ID3 header, but includes extended header and padding)
unsigned int tag_size; 

/**
 * @brief Reads the ID3 tags from the MP3 file
 * @return HeaderData Structure
 */
HeaderData *read_id3_header(FILE *file){
    HeaderData *header_data = create_header_data();
    //skip the ID3 tag identifier from header
    fseek(file, 3, SEEK_SET);
    //version is only 2 bytes, so read 2 bytes after tag identifier
    fread(header_data->version, 2, 1, file);
    //again skip 1 byte for ID3 header flag 
    fseek(file, 1, SEEK_CUR);
    //get the size (4 bytes) of the total ID3 tag(excluding the header)
    fread(header_data->size, 4, 1, file);

    //get the actual integer value of the size
    tag_size = decode_syncsafe(header_data->size);

    return header_data;
}

/**
 * @brief Reads the ID3 tags from the MP3 file
 * @return TagData Structure
 */
TagData *read_id3_tag(FILE *file){
    TagData *data = create_tag_data();

    printf("Tag size (excluding header): %u bytes\n", tag_size);

    //To determine how many ID3 frames are there
    unsigned int remaining_frames = tag_size;

    // Loop through each frame to extract tag names and content
    // A frame header is 10 bytes, so ensure at least 10 bytes remain before reading the next frame
    // This prevents reading into padding, past the end of the tag, or into incomplete/corrupted tag
    while(remaining_frames > 10){ 
        unsigned char frame_header[10];
        if(fread(frame_header, 10, 1, file) != 1){
            display_error("Unexpected end of file or read error while reading frame header.\n");
            break;
        }

        // Check if we've reached padding: According to the ID3v2 spec, padding bytes must be $00.
        // We only check the first few bytes since a valid frame ID must start with non-zero ASCII characters.
        if (frame_header[0] == 0 || memcmp(frame_header, "\0\0\0\0", 4) == 0) {
            break;  // Padding or empty frame detected
        }
        
        //The first 4 bytes of frame header has the frame ID
        char frame_id[5];
        memcpy(frame_id, frame_header, 4);
        frame_id[4] = '\0';

        // Validate frame ID: should be ASCII value
        for (int i = 0; i < 4; i++) {
            if (frame_id[i] < 0x20 || frame_id[i] > 0x7E) {
                fprintf(stderr, "Invalid frame ID detected. Possibly corrupted tag.\n");
                break;
            }
        }

        //The next 4 bytes of frame header contain the frame content size, which is in big endian
        unsigned int frame_size = (frame_header[4] << 24) |
                              (frame_header[5] << 16) |
                              (frame_header[6] << 8)  |
                               frame_header[7];

        // Validate frame size against remaining bytes
        if (frame_size > (remaining_frames - 10)) {
            display_error("Frame size exceeds remaining tag size. Aborting.\n");
            break;
        }

        //Skipping the text encoding byte(which is the first byte) in the frame content
        fseek(file, 1, SEEK_CUR);

        char *content = (char *)calloc(1, frame_size);
        if (content == NULL) {
            perror("Memory allocation failed");
            return NULL;
        }

        fread(content, frame_size - 1, 1, file);
        content[frame_size - 1] = '\0';

        // printf("Frame ID: %s, Size: %u bytes, Content: %s\n", frame_id, frame_size, content);

        if(strcmp(frame_id, "TIT2") == 0){
            data->title = (char *)calloc(1, strlen(content));
            if(!data->title){
                perror("Memory allocation failed");
                return NULL;
            }

            strcpy(data->title, content);
        }
        else if(strcmp(frame_id, "TPE1") == 0){
            data->artist = (char *)calloc(1, strlen(content));
            if(!data->artist){
                perror("Memory allocation failed");
                return NULL;
            }
            
            strcpy(data->artist, content);
        }
        else if(strcmp(frame_id, "TALB") == 0){
            data->album = (char *)calloc(1, strlen(content));
            if(!data->album){
                perror("Memory allocation failed");
                return NULL;
            }
            
            strcpy(data->album, content);
        }
        else if(strcmp(frame_id, "TYER") == 0){
            data->year = (char *)calloc(1, strlen(content));
            if(!data->year){
                perror("Memory allocation failed");
                return NULL;
            }
            
            strcpy(data->year, content);
        }
        else if(strcmp(frame_id, "TCON") == 0){
            data->genre = (char *)calloc(1, strlen(content));
            if(!data->genre){
                perror("Memory allocation failed");
                return NULL;
            }
            
            strcpy(data->genre, content);
        }
        else if(strcmp(frame_id, "COMM") == 0){
            data->comment = (char *)calloc(1, strlen(content));
            if(!data->comment){
                perror("Memory allocation failed");
                return NULL;
            }
            
            strcpy(data->comment, content);
        }
        else if(strcmp(frame_id, "APIC") == 0){
            printf("%s\n", content);
        }

        // Deduct total size of frame (header + content)
        remaining_frames = remaining_frames - (frame_size + 10);

        free(content);
        content = NULL;
    }

    return data;
}

/**
 * @brief Displays the MP3 metadata
 */
void display_metadata(const HeaderData *header_data, const TagData *data) {
    printf("Version is 2.%hhx.%hhx\n", header_data->version[0], header_data->version[1]);
    printf("Title: %s\n", data->title);
    printf("Artist: %s\n", data->artist);
    printf("Album: %s\n", data->album);
    printf("Year: %s\n", data->year);
    printf("Genre: %s\n", data->genre);
    printf("Comment: %s\n", data->comment);
}

/**
TODO: Add documention as sample given
 */
void view_tags(const char *filename){
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    HeaderData *header_data = read_id3_header(file);
    if (!header_data) {
        display_error("Failed to read ID3 header.");
        return;
    }
    TagData *data = read_id3_tag(file);
    if (!data) {
        display_error("Failed to read ID3 frame.");
        return;
    }

    display_metadata(header_data, data);

    fclose(file);
    free_header_data(header_data);
    free_tag_data(data);
}
