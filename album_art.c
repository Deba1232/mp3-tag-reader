#include "main.h"
#include "id3_utils.h"
#include "album_art.h"
#include "error_handling.h"

void extract_album_art(FILE *file, unsigned int frame_size, char *img_file_name){
    // Allocate memory to store the APIC frame content
    char *apic_content = (char *)calloc(1, frame_size);
    if (!apic_content) {
        perror("Memory allocation failed");
        return;
    }
    
    /*
     * APIC frame format in ID3v2:
     * ---------------------------------------------------------
     * Byte 0       : Text encoding (1 byte)
     * Byte 1 - n   : Image format (null-terminated string)
     * Next byte    : Picture type (1 byte)
     * Next bytes   : Description (null-terminated string)
     * Remaining    : Actual image data (JPEG/PNG binary data)
     * ---------------------------------------------------------
    */

    // Skip the text encoding byte
    fseek(file, 1, SEEK_CUR);

    fread(apic_content, frame_size - 1, 1, file);
    int idx = 0;

    // Get the image format
    unsigned char image_format[20];
    unsigned int image_format_len = 0;

    while(apic_content[idx] != '\0'){
        image_format[image_format_len++] = apic_content[idx++];
    }
    image_format[image_format_len] = '\0';
    idx += 2; // Skip the null byte after the image format string and the picture type byte

    // Skip the image description string
    while(apic_content[idx] != '\0'){
        idx++;
    }
    idx++; // Skip the null byte after the description string

    char *extension = strcmp(image_format, "image/png") ? "jpg" : "png";

    sprintf(img_file_name, "album_art.%s", extension);
    
    // Write the image data to the file
    FILE *img_file = fopen(img_file_name, "wb");
    if(!img_file){
        perror("File");
        return;
    }

    fwrite(&apic_content[idx], frame_size - idx, 1, img_file);

    fclose(img_file);
    free(apic_content);
}