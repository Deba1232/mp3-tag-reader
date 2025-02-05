#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include "main.h"

#define FAILURE 0
#define SUCCESS 1

/**
 * @brief Displays the error message for the MP3 Tag Reader application.
 */
void display_error(const char *message);

/**
 * @brief Checks whether ID3 tag is present in the provided MP3 file.
 * @return SUCCESS on successful validation otherwise FAILURE.
 */
int check_id3_tag_presence(const char *filename);

/**
 * @brief Checks whether the file provided is an MP3 file.
 * @return SUCCESS on successful validation otherwise FAILURE.
 */
int check_extension(const char *filename);

#endif
