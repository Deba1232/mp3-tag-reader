/**
 * @file main.c
 * @brief Main entry point for the MP3 Tag Reader application.
 */
#include "main.h"
#include "id3_reader.h"
#include "id3_writer.h"
#include "error_handling.h"

/**
 * @brief Displays the help message for the MP3 Tag Reader application.
 */
void display_help() {
    printf("Usage: ./mp3tag [OPTION] filename.mp3\n");
    printf("       ./mp3tag -e [TAGOPTION] <value> filename\n");
    printf("Options:\n");
    printf("  -v               View song info\n");
    printf("  -h, --help              Display this help\n");
    printf("  -e               Edit tags\n");
    printf("Edit Tag Options:\n");
    printf("      -t           Modifies Title tag\n      -T           Modifies Track tag\n      -a           Modifies Artist tag\n      -A           Modifies Album tag\n      -y           Modifies Year tag\n      -c           Modifies Comment tag\n      -g           Modifies Genre tag\n");
}

/**
 * @brief Main function to handle command-line arguments and execute appropriate actions.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 */
int main(int argc, char *argv[]){
    if (argc < 2) {
        display_help();
    }
    else{

        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            display_help();
        }
        else if (strcmp(argv[1], "-v") == 0 && argc == 3) {

            if(check_extension(argv[2])){

                if(check_id3_tag_presence(argv[2])){
                    view_tags(argv[2]);
                }
                else{
                    display_error("This MP3 file doesn't follow ID3v2 standard.");
                }  
            }
            else{
                display_error("Please provide an MP3 file.");
            }
        } 
        // else if (strcmp(argv[1], "-e") == 0 && argc == 5) {
        //     char *tag = argv[2];
        //     char *filename = argv[3];
        //     char *value = argv[4];
        //     if (edit_tag(filename, tag, value) != 0) {
        //         // display_error("Failed to edit tag.");
        //         return 1;
        //     }
        //     printf("Tag edited successfully.\n");
        // } 
        else {
            display_help();
        }
    }
    
    return 0;
}
