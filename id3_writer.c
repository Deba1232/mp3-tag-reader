#include "main.h"
#include "id3_utils.h"
#include "id3_reader.h"
#include "id3_writer.h"
#include "error_handling.h"

void copy_tag_header(FILE *original_file, FILE *tmp_file, unsigned char *tag_header){
    //read 10 bytes header from the original MP3 file
    fread(tag_header, TAG_HEADER_SIZE, 1, original_file);
    //now write these 10 bytes to the temporary file
    fwrite(tag_header, TAG_HEADER_SIZE, 1, tmp_file);
}

unsigned int copy_tag_frames(FILE *original_file, FILE *tmp_file, const unsigned int *tag_size, const char *option, const TagData *data){
    // This variable stores the total size of all frames written
    unsigned int total_written_frame_size = 0;
    unsigned int new_frame_size = 0;

    unsigned int remaining_frames = *tag_size;

    // Loop through each frame to identify and update the relevant tag frame
    // Ensure at least 10 bytes remain for reading a complete frame header
    while(remaining_frames > FRAME_HEADER_SIZE){
        unsigned char frame_header[FRAME_HEADER_SIZE];
        // Read 10 bytes frame header from the original MP3 file
        if(fread(frame_header, FRAME_HEADER_SIZE, 1, original_file) != 1){
            display_error("Unexpected end of file or read error while reading frame header.\n");
            break;
        }

        // Check if padding is reached (frame ID starts with 0)
        if (frame_header[0] == 0) {
            // Move the file pointer back by 10 bytes to position it at the start of padding
            fseek(original_file, -FRAME_HEADER_SIZE, SEEK_CUR);
            break;  // Padding reached
        }

        // The first 4 bytes of frame header has the frame ID
        unsigned char frame_id[5];
        memcpy(frame_id, frame_header, 4);
        frame_id[4] = '\0';

        // The next 4 bytes of the frame header contain the frame content size
        unsigned int original_frame_size = (frame_header[4] << 24) | (frame_header[5] << 16) | (frame_header[6] << 8)  | frame_header[7];

        // Variables to store edited content and its length
        char *edited_content = NULL;
        unsigned int edited_length = 0;

        // Compare frame ID with known tag identifiers and check if the current option matches, also assign corresponding data
        if(strcmp(frame_id, "TIT2") == 0 && strcmp(option, "-t") == 0){
            edited_content = data->title;  
        }
        else if(strcmp(frame_id, "TRCK") == 0 && strcmp(option, "-T") == 0){
            edited_content = data->track; 
        }
        else if(strcmp(frame_id, "TPE1") == 0 && strcmp(option, "-a") == 0){
            edited_content = data->artist; 
        }
        else if(strcmp(frame_id, "TALB") == 0 && strcmp(option, "-A") == 0){
            edited_content = data->album;  
        }
        else if(strcmp(frame_id, "TYER") == 0 && strcmp(option, "-y") == 0){
            edited_content = data->year;
        }
        else if(strcmp(frame_id, "COMM") == 0 && strcmp(option, "-c") == 0){ 
            edited_content = data->comment;  
        }
        else if(strcmp(frame_id, "TCON") == 0 && strcmp(option, "-g") == 0){
            edited_content = data->genre; 
        }

        // If the current frame is the one being edited
        if (edited_content) {
            edited_length = strlen(edited_content);
            // 1 added to account for the text encoding byte at the start of the frame content
            new_frame_size = edited_length + 1;
            // Convert the new frame size to big-endian format (as per ID3v2.3 spec) and store it in edited_frame_size
            for (int i = 0; i < 4; i++) {
                frame_header[i + 4] = (new_frame_size >> (24 - (8 * i))) & 0xFF;
            }
        } 

        // Write the 10 bytes frame header to the temporary file
        fwrite(frame_header, FRAME_HEADER_SIZE, 1, tmp_file);

        // Read the frame content (including text encoding byte) from the original file and write it to the temp file
        char *frame_content_buf = (char *)calloc(1, original_frame_size);
        if (!frame_content_buf) {
            perror("Memory allocation failed\n");
            return -1;
        }

        fread(frame_content_buf, original_frame_size, 1, original_file);

        if(edited_length){
            // Write encoding byte (first byte of original content)
            fwrite(frame_content_buf, 1, 1, tmp_file);
            // Write the new content
            fwrite(edited_content, edited_length, 1, tmp_file);

            total_written_frame_size = total_written_frame_size + (new_frame_size + FRAME_HEADER_SIZE);
        }
        else{
            fwrite(frame_content_buf, original_frame_size, 1, tmp_file);
            
            total_written_frame_size = total_written_frame_size + (original_frame_size + FRAME_HEADER_SIZE);
        }

        free(frame_content_buf);
        
        // Deduct total size of frame (header + content)
        remaining_frames = remaining_frames - (original_frame_size + FRAME_HEADER_SIZE);
    }

    // Skipping the padding data (if any) here as it'll be handled in write_id3_tag function
    fseek(original_file, remaining_frames, SEEK_CUR);

    return total_written_frame_size;
}

void copy_remaining_data(FILE *original_file, FILE *tmp_file){
    unsigned char remaining_data_buf[BUFSIZ];
    size_t bytes;

    while(bytes = (fread(remaining_data_buf, sizeof(remaining_data_buf), 1,original_file)) > 0){
        fwrite(remaining_data_buf, sizeof(remaining_data_buf), 1, tmp_file);
    }
}

void copy_to_original_file(const char *original_filename, const char *tmp_filename){
    FILE *original_file = fopen(original_filename, "wb");
    if(!original_file){
        perror("Failed to open file");
        return;
    }

    FILE *tmp_file = fopen(tmp_filename, "rb");
    if(!tmp_file){
        perror("Failed to open file");
        return;
    }

    unsigned char tmp_data_buf[BUFSIZ];
    size_t bytes;

    while((bytes = fread(tmp_data_buf, sizeof(tmp_data_buf), 1, tmp_file)) > 0){
        fwrite(tmp_data_buf, sizeof(tmp_data_buf), 1, original_file);
    }

    fclose(original_file);
    fclose(tmp_file);
    
    // Remove the temp file
    remove(tmp_filename);
}

int write_id3_tag(const char *filename, const TagData *data, const unsigned int *tag_size, const char *option) {
    FILE *original_file = fopen(filename, "rb");
    if(!original_file){
        perror("Failed to open file");
        return 1;
    }

    // Create a temporary file to hold updated MP3 data
    FILE *tmp_file = fopen("tmp.mp3", "wb");
    if(!tmp_file){
        perror("Failed to open file");
        return 1;
    }

    // Buffer to hold the 10-byte ID3 tag header
    unsigned char tag_header[TAG_HEADER_SIZE] = {0};
    // Copy the tag header from original to temporary file and keep a copy in tag_header
    copy_tag_header(original_file, tmp_file, tag_header);

    unsigned int frames_written = copy_tag_frames(original_file, tmp_file, tag_size, option, data);

    // Adjust padding or header size depending on edited frame size
    if(frames_written < *tag_size){
        // Case 1: New total frame size is smaller than original tag size after edit, keep padding the same size as original
        unsigned int padding_size = *tag_size - frames_written;

        char *padding_buf = (char *)calloc(1, padding_size);
        if(!padding_buf){
            perror("Memory allocation failed\n");
            return -1;
        }

        //Write the padding data to temp file
        fwrite(padding_buf, padding_size, 1, tmp_file);
    }
    else if (frames_written > *tag_size) {
        // Case 2: New total frame size exceeds original tag size after edit, update header with new tag size
        // In this case, no explicit padding is written because the new tag size exceeds the original tag size, which means there’s no extra room to pad, and the tag grows.

        // Update the tag size in the header using synchsafe integer format
        encode_syncsafe(frames_written, &tag_header[6]); // Encode into header bytes 6-9
        // Rewind the temp file to the beginning to overwrite the old header with the updated size
        rewind(tmp_file);
        fwrite(tag_header, TAG_HEADER_SIZE, 1, tmp_file); 

        // Skip the whole header and frame section to the beginning of the audio part
        fseek(original_file, FRAME_HEADER_SIZE + *tag_size, SEEK_SET);
    }

    copy_remaining_data(original_file, tmp_file);

    fclose(original_file);
    fclose(tmp_file);

    copy_to_original_file(filename, "tmp.mp3");

    return 0;
}

int edit_tag(const char *filename, const char *option, const char *value) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }

    //Size of the ID3 tag (excluding the ID3 header, but includes extended header and padding)
    unsigned int tag_size = 0;

    char *option_string;
    
    read_id3_header(file, &tag_size);
    TagData *data = read_id3_tag(file, &tag_size);
    if (!data) {
        return 1;
    }

    if(strcmp(option, "-t") == 0){
        strcpy(data->title, value);
        option_string = "Title";
    }
    else if(strcmp(option, "-T") == 0){
        strcpy(data->track, value);
        option_string = "Track";
    }
    else if(strcmp(option, "-a") == 0){
        strcpy(data->artist, value);
        option_string = "Artist";
    }
    else if(strcmp(option, "-A") == 0){
        strcpy(data->album, value);
        option_string = "Album";
    }
    else if(strcmp(option, "-y") == 0){
        strcpy(data->year, value);
        option_string = "Year";
    }
    else if(strcmp(option, "-c") == 0){
        strcpy(data->comment, value);
        option_string = "Comment";
    }
    else if(strcmp(option, "-g") == 0){
        strcpy(data->genre, value);
        option_string = "Genre";
    }
    
    write_id3_tag(filename, data, &tag_size, option);

    printf("--------------- Select Edit Option ------------------------\n");

    printf("------------- Selected \"%s\" change option ------------------\n", option_string);
    printf("%s\t:\t%s\n", option_string, value);

    printf("------------- %s changed successfully ------------------\n", option_string);

    fclose(file);

    return 0;
}
