#ifndef ID3_WRITER_H
#define ID3_WRITER_H

/**
 * @brief Copies the ID3 tag header.
 * 
 * @param FilePointer1 Pointer to the original MP3 file structure.
 * @param FilePointer2 Pointer to the temporary file structure.
 * @param tagheader Stores the tag header data.
 */
void copy_tag_header(FILE *, FILE *, unsigned char *);

/**
 * @brief Copies the ID3 tag header.
 * 
 * @param FilePointer1 Pointer to the original MP3 file structure.
 * @param FilePointer2 Pointer to the temporary file structure.
 * @param tagsize Size of the ID3 tag (excluding the ID3 header, but includes extended header and padding).
 * @param option Frame ID edited.
 * @param data Pointer to the TagData structure containing the ID3 tags.
 * @return ID3 tag size after edit on success, -1 on failure.
 */
unsigned int copy_tag_frames(FILE *, FILE *, const unsigned int *, const char *, const TagData *);

/**
 * @brief Copies the remaining audio data.
 * 
 * @param FilePointer1 Pointer to the original MP3 file structure.
 * @param FilePointer2 Pointer to the temporary file structure.
 */
void copy_remaining_data(FILE *, FILE *);

/**
 * @brief Copies the whole data from temporary file to original MP3 file.
 * 
 * @param FileName1 Filename of the original MP3 file.
 * @param FileName12 Filename of the temporary file.
 */
void copy_to_original_file(const char *, const char *);

/**
 * @brief Writes the ID3 tag to an MP3 file.
 * 
 * @param filename The name of the MP3 file.
 * @param data Pointer to the TagData structure containing the ID3 tags.
 * @param tagsize Size of the ID3 tag (excluding the ID3 header, but includes extended header and padding).
 * @param option Frame ID edited.
 * @return 0 on success, non-zero on failure.
 */
int write_id3_tag(const char *, const TagData *, const unsigned int *, const char *);

/**
 * @brief Edit the ID3 tag.
 * 
 * @param filename The name of the MP3 file.
 * @param option Frame ID to edit.
 * @param value New value of the frame content.
 * 
 * @return 0 on success, non-zero on failure.
 */
int edit_tag(const char *, const char *, const char *);

#endif // ID3_WRITER_H
