#ifndef DECODE_H
#define DECODE_H

#include "types.h"               // Contains user defined types
#include<stdio.h>
typedef struct _DecodeInfo
{
    /*image info */

    char *image_fname;
    FILE *fptr_image_fname;

    /*secret file info*/
    uint sec_file_ext_size;
    uint sec_file_data_size;
    char *secret_dec_fname;
    char arr[12] ;
    FILE *fptr_secret_dec_fname;



}DecodeInfo;

 // Validates decoding arguments and populates DecodeInfo structure
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

// Opens the required files for decoding
Status dec_open_files(DecodeInfo *decInfo);

// Decodes the magic string to verify hidden data in the image
Status decode_magic_string(FILE *fptr_image);

// Decodes the size of the secret file's extension
uint decode_op_file_extn_size(FILE *fptr_image);

// Decodes the secret file's extension from the image
Status decode_op_file_extn(char *secret_dec_fname, FILE *fptr_image, uint sec_file_ext_size);

// Opens the file where the decoded secret data will be stored
Status dec_open_sec_file(DecodeInfo *decInfo);

// Decodes the size of the secret file data
uint decode_op_file_data_size(FILE *fptr_secret_dec_fname);

// Copies the decoded data from the image to the secret file
Status copy_data_to_sec_file(FILE *fptr_image, FILE *fptr_sec, uint sec_file_data_size);

#endif
