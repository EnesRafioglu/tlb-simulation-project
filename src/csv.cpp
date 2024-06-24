#include "csv.hpp"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
    #include "util.h"
}

/**
 * Counts the number of non empty lines in a given file.
 * 
 *
 * @param filename The name of the file to count lines from.
 * @return The number of lines in the file.
 */
unsigned int count_lines(const char* filename) {
    FILE* file;
    file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Error: Couldn't open input file %s\n", filename);

        return 0;
    }

    unsigned int lines = 0;
    
    char buffer[128];

    while (fgets(buffer, sizeof(buffer), file)) {
        int index = 0;
        while(buffer[index] == ' ') {
            index++;
        }

        if (buffer[index] != '\n') {
            lines++;
        }
    }

    fclose(file);
    return lines;
}



extern "C" struct Request* parse_csv(const char* filename, int* line_count) {
    *line_count = count_lines(filename);

    if (*line_count == 0) {
        return NULL;
    }

    FILE* file;
    file = fopen(filename, "r");

    if (!file) {
        fprintf(stderr, "Error: Couldn't open input file %s\n", filename);
        return NULL;
    }

    struct Request* requests = (struct Request*)malloc(*line_count * sizeof(struct Request));

    if(!requests) {
        fprintf(stderr, "Error: Couldn't allocate memory for requests\n");
        fclose(file);

        return NULL;
    }

    char buffer[256];
    *line_count = 0;


    outer:
    while(fgets(buffer, sizeof(buffer), file)) {
        if (buffer[0] == '\n') {
            continue;
        }

        int token_count = 0;
        

        //erase newline character at end of line, as this would 
        //cause the string to int parsing to fail
        char* new_line = strchr(buffer, '\n');
        if (new_line) {
            *new_line = '\0';
        }


        //detect commas at the beginning and end of file, 
        //as strtok ignores those
        if ((buffer[0] == ',') || buffer[strlen(buffer) - 1] == ',') {
            fprintf(stderr, "Error: ',' must not be at the beginning or end of a line\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n");

            free(requests);
            exit(22);
        }
        
        //using strtok_r to get comma separated tokens
        //and the saveptr to detect if there are multiple commas in a row
        char* saveptr ;
        char* token = strtok_r(buffer, ",", &saveptr);
        struct Request request;

        while(token) {

            //skip whitespace at the beginning of the token
            while(*token == ' ') {
                token++;
            }
            if(token == new_line) {
                goto outer;
            }

            if (*saveptr == ',') {
                fprintf(stderr, "Error: ',' must not be followed by another ','\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n");
                
                free(requests);
                exit(22);
            }


            if (token_count == 0) {
                if (strlen(token) > 1) {
                    fprintf(stderr, "Error: Accesstype must be a single character\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n");  
                    
                    free(requests);
                    exit(22);
                } else {
                    if (token[0] == 'R' || token[0] == 'W' || token[0] == 'r' || token[0] == 'w') {
                        request.we = (token[0] == 'W' || token[0] == 'w') ? 1 : 0;
                    } else {
                        fprintf(stderr, "Error: Accesstype must be either W or R not %s\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);
                        
                        free(requests);
                        exit(22);
                    }
                   
                }

            } else if (token_count == 1) {
                //use parse_string_to_uint to parse the address
                //and check for errors
                enum PARSE_RESULT result = parse_string_to_uint(token, &request.addr);

                
                switch(result) {
                    case PARSE_SUCCESS:
                        break;
                    case PARSE_OUT_OF_RANGE:
                        fprintf(stderr, "Error: Address %s is out of range\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);
                        
                        free(requests);
                        exit(22);
                    case PARSE_NO_DIGITS:
                        fprintf(stderr, "Error: Address %s doesn't contain any digits\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);
                        
                        free(requests);
                        exit(22);
                    case PARSE_CHARACTERS_LEFT:
                        fprintf(stderr, "Error: Address %s contains extra characters\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);
                        
                        free(requests);
                        exit(22);   
                    case PARSE_GENERAL_ERROR:
                        fprintf(stderr, "Error: Couldn't parse address %s\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);
                        
                        free(requests);
                        exit(22);
                        
                } 


            } else if (token_count == 2) {
                //check if we is set
                if (request.we == 1) {
                    enum PARSE_RESULT result = parse_string_to_uint(token, &request.data);

                    switch(result) {
                        case PARSE_SUCCESS:
                            break;
                        case PARSE_OUT_OF_RANGE:
                            fprintf(stderr, "Error: Value %s is out of range\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);
                            
                            free(requests);
                            exit(22);
                        case PARSE_NO_DIGITS:
                            fprintf(stderr, "Error: Value %s doesn't contain any digits\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);
                            
                            free(requests);
                            exit(22);
                        case PARSE_CHARACTERS_LEFT:
                            fprintf(stderr, "Error: Value %s contains extra characters\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);
                            
                            free(requests);
                            exit(22);                 
                        case PARSE_GENERAL_ERROR:
                            fprintf(stderr, "Error: Couldn't parse value %s\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token);

                            free(requests);
                            exit(22);    
                    }
                    
                } else {
                    fprintf(stderr, "Error: Read type shouldn't have data\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n");
                    free(requests);
                    exit(22);
                }
            } else {
                fprintf(stderr, "Error: Too many tokens in line\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n");
                free(requests);
                exit(22);
            }


            token = strtok_r(NULL, ",", &saveptr);
            token_count++;
        }

        if (token_count < 2 || ((request.we == 1) && (token_count < 3))) {
            fprintf(stderr, "Error: Too few tokens in line\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n");
            
            free(requests);
            exit(22);
        }


        requests[*line_count] = request;
        (*line_count)++;


    }

    fclose(file);
    return requests;

}