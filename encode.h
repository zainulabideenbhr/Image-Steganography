#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" /*containes user defined types*/
#include<stdio.h>
#define MAX_FILE_SUFFIX 4


typedef struct _EncodeInfo
{
    /*source image info*/
    char *src_image_fname;
    FILE *fptr_src_image;
    uint image_capacity;


    /*secerate file info*/
    char *secret_fname;
    FILE *fptr_secret;
    uint size_secret_file;
    char secret_file_extn[MAX_FILE_SUFFIX];


    /*stego_image info*/
    char *stego_image_fname;
    FILE *fptr_stego_image;



}EncodeInfo;


// Determines if the operation is encoding or decoding based on command-line arguments
OperationType check_operation_type(char *argv[]);

// Validates encoding arguments and populates EncodeInfo structure
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

// Opens the necessary files for encoding
Status open_files(EncodeInfo *encInfo);

// Gets the size of the BMP image file
uint get_image_size_for_bmp(FILE *fptr_image);

// Gets the size of the secret file to be encoded
uint get_file_size(FILE *fptr_secret);

// Checks if the image has enough capacity to hold the secret file
Status check_capacity(EncodeInfo *encInfo);

// Performs the encoding process
Status do_encoding(EncodeInfo *encInfo);

// Copies the BMP header from the source image to the destination image
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

// Encodes the magic string into the image
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

// Encodes a block of data into the image
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

// Encodes the file extension of the secret file into the image
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

// Encodes the size of the secret file's extension into the image
Status encode_size_data_to_image(char *secret_file_extn_size, FILE *fptr_src_image, FILE *fptr_stego_image);

// Encodes the secret file's data into the image
Status encode_secret_file_data_to_image(FILE *fptr_secret, uint secret_file_size, FILE *fptr_src_image, FILE *fptr_stego_image);

// Copies any remaining image data after encoding
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
