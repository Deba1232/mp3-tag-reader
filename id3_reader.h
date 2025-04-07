#ifndef ID3_READER_H
#define ID3_READER_H

#include "main.h"
#include "id3_utils.h"

/**
TODO: Add documention as sample given
 */
HeaderData *read_id3_header(FILE *);

/**
TODO: Add documention as sample given
 */
TagData *read_id3_tag(FILE *);

/**
TODO: Add documention as sample given 
 */
void display_metadata(const HeaderData *, const TagData *);

/**
TODO: Add documention as sample given
 */
void view_tags(const char *);

#endif // ID3_READER_H
