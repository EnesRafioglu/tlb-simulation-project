#include "csv.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"


/**
 * Counts the number of non empty lines in a given file.
 * This is used to determine the size of the array that will store the requests.
 * Therefore only one memory allocation is needed.
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
    
    char buffer[35];

    while (fgets(buffer, sizeof(buffer), file)) {
        
        if (buffer[0] != '\n' && buffer[0] != '\r') {
            lines++;
        }
    }

    if(feof(file)) {
        fclose(file);
        return lines;
    } else {
        fprintf(stderr, "Some error occurred while reading the file\n");
        fclose(file);
        return 0;
    }
    
}


/**
 * Tokenizes a string into individual tokens.
 * Works similarly to strtok, but doesn't skip empty tokens
 * and the delimiter is always a comma.
 * 
 *
 * @param str The string to be tokenized.
 * @return A pointer to the first token in the string.
 */
char* tokenize_string (char* str) {

    //using static buffer to keep the state of the tokenizer
    //between calls
    static char* buffer = NULL;
    char* token;

    if (str) {
        buffer = str;
    }

    if (buffer == NULL) {
        return NULL;
    }


    token = buffer;

    //find the next comma. If there is no comma,
    //buffer will be set to NULL and the next call
    //will return NULL
    buffer = strchr(buffer, ',');

    if (buffer) {
        *buffer = '\0';
        buffer++;
    }


    return token;
}


struct Request* parse_csv(const char* filename, int* line_count) {
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

    //longest possible line is 33 characters long
    // "W",0037777777777,0037777777777\r\n
    //extra 2 characters for the null terminator 
    //and to check if the line is too long
    char buffer[35];
    *line_count = 0;


    
    while(fgets(buffer, sizeof(buffer), file)) {
        if (buffer[0] == '\n' || buffer[0] == '\r'	) {
            continue;
        }

        //check if the line is too long
        if (strlen(buffer) > 33) {
            fprintf(stderr, "Error: Too many characters encountered. Could be unnecessary white spaces\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n");
            
            fclose(file);
            free(requests);
            exit(22);
        }

        int token_count = 0;
        

        //erase newline character at end of line, as this would 
        //cause the string to int parsing to fail
        char* new_line = strchr(buffer, '\n');
        if (new_line) {
            *new_line = '\0';
        }

        new_line = strchr(buffer, '\r');
        if (new_line) {
            *new_line = '\0';
        }
        

        //tokenize the string
        char* token = tokenize_string(buffer);
        struct Request request;

        while(token) {

            //check for empty tokens
            if (strlen(token) == 0) {
                fprintf(stderr, "Error: Multiple commas in a row or a comma at the beginning/end of a line are not allowed\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n");
                
                fclose(file);
                free(requests);
                exit(22);
            }


            if (token_count == 0) {
                
                if (strlen(token) == 1){

                    if (token[0] == 'R' || token[0] == 'W' || token[0] == 'r' || token[0] == 'w') {
                        request.we = token[0] == 'W' || token[0] == 'w';
                    } else {
                        fprintf(stderr, "Error: Accesstype must be either W or R not '%c'\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n", token[0]);
                        
                        fclose(file);
                        free(requests);
                        exit(22);
                    }

                } else if (strlen(token) == 3){

                    if (token[0] == '\"' && token[2] == '\"') {
                        if (token[1] == 'R' || token[1] == 'W' || token[1] == 'r' || token[1] == 'w') {
                            request.we = token[1] == 'W' || token[1] == 'w';
                        } else {
                            fprintf(stderr, "Error: Accesstype must be either W or R not '%c'\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token[1]);
                            
                            fclose(file);
                            free(requests);
                            exit(22);
                        }
                    }
                    
                } else{
                    fprintf(stderr, "Error: Accesstype must be a single character with or without quotes\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n");  
                    
                    fclose(file);
                    free(requests);
                    exit(22);
                }

            } else if (token_count == 1) {
                //use parse_string_to_uint to parse the address
                //and check for errors

                enum PARSE_RESULT result = parse_string_to_uint32(token, &request.addr);

                
                switch(result) {
                    case PARSE_SUCCESS:
                        break;
                    case PARSE_OUT_OF_RANGE:
                        fprintf(stderr, "Error: Address %s is out of range\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token);
                        
                        fclose(file);
                        free(requests);
                        exit(22);
                    case PARSE_NO_DIGITS:
                        fprintf(stderr, "Error: Address %s doesn't contain any digits\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token);
                        
                        fclose(file);
                        free(requests);
                        exit(22);
                    case PARSE_CHARACTERS_LEFT:
                        fprintf(stderr, "Error: Address %s contains extra characters\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token);
                        
                        fclose(file);
                        free(requests);
                        exit(22);   
                    case PARSE_GENERAL_ERROR:
                        fprintf(stderr, "Error: Couldn't parse address %s\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token);
                        
                        fclose(file);
                        free(requests);
                        exit(22);
                        
                } 


            } else if (token_count == 2) {
                //check if we is set
                if (request.we == 1) {
                    
                    enum PARSE_RESULT result = parse_string_to_uint32(token, &request.data);

                    switch(result) {
                        case PARSE_SUCCESS:
                            break;
                        case PARSE_OUT_OF_RANGE:
                            fprintf(stderr, "Error: Value %s is out of range\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token);
                            
                            fclose(file);
                            free(requests);
                            exit(22);
                        case PARSE_NO_DIGITS:
                            fprintf(stderr, "Error: Value %s doesn't contain any digits\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token);
                            
                            fclose(file);
                            free(requests);
                            exit(22);
                        case PARSE_CHARACTERS_LEFT:
                            fprintf(stderr, "Error: Value %s contains extra characters\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token);
                            
                            fclose(file);
                            free(requests);
                            exit(22);                 
                        case PARSE_GENERAL_ERROR:
                            fprintf(stderr, "Error: Couldn't parse value %s\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n", token);

                            fclose(file);
                            free(requests);
                            exit(22);    
                    }
                    
                } else {
                    fprintf(stderr, "Error: Read type shouldn't have data\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n");
                    
                    fclose(file);
                    free(requests);
                    exit(22);
                }
            } else {
                fprintf(stderr, "Error: Too many tokens in line\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n");

                fclose(file);
                free(requests);
                exit(22);
            }


            token = tokenize_string(NULL);
            token_count++;
        }

        if (token_count < 2 || ((request.we == 1) && (token_count < 3))) {
            fprintf(stderr, "Error: Too few tokens in line\n\texample inputs: R,0x00000000\n\t\t\tW,1,0x1337\n\t\t\t\"W\",00377,0x433c7\n");
            
            fclose(file);
            free(requests);
            exit(22);
        }


        requests[*line_count] = request;
        (*line_count)++;

    }

    fclose(file);
    return requests;
}