#include "error_handling.h"

/**
 * @brief Displays the error message for the MP3 Tag Reader application.
 */
void display_error(const char *message){
    fprintf(stderr, "\033[0;31m%s\033[0m\n", message);
}

/**
 * @brief Checks whether the file provided is an MP3 file.
 * @return SUCCESS on successful validation otherwise FAILURE.
 */
int check_extension(const char *filename){
    if(strstr(filename, ".mp3") && *(strstr(filename, ".mp3") + 4) == '\0'){
        return SUCCESS;
    }

    return FAILURE;
}

/**
 * @brief Checks whether ID3 tag is present in the provided MP3 file.
 * @return SUCCESS on successful validation otherwise FAILURE.
 */
int check_id3_tag_presence(const char *filename){
    FILE *file;
    if(!(file = fopen(filename, "r"))){
        display_error("File not available, unable to proceed further\n");
        exit(EXIT_FAILURE);
    }

    char *tag = (char *)calloc(3, sizeof(char));
    if(!tag){
        fprintf(stderr, "Memory couldn't be allocated\n");
        exit(EXIT_FAILURE);
    }

    fread(tag, 3, 1, file);
    
    if(strcmp(tag, "ID3") == 0){
        free(tag);
        fclose(file);

        return SUCCESS;
    }

    free(tag);
    fclose(file);

    return FAILURE;
}
    