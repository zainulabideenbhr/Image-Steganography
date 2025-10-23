#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "common.h"
#include "encode.h"
#include "decode.h"

/* READ AND VALIDATE DECODE ARGUMENTS */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) {
    if (strstr(argv[2], ".bmp") == NULL) {
        return d_failure;
    }
    decInfo->image_fname = argv[2];

    if (argv[3] == NULL) {
        // Set default filename if none is provided
        strcpy(decInfo->arr, "default");
        decInfo->secret_dec_fname = decInfo->arr;
    } else {
        decInfo->secret_dec_fname = argv[3];
    }

    return d_success;
}

/* OPEN REQUIRED FILES */
Status dec_open_files(DecodeInfo *decInfo) {
    printf("Opening %s\n", decInfo->image_fname);
    decInfo->fptr_image_fname = fopen(decInfo->image_fname, "rb");

    if (decInfo->fptr_image_fname == NULL) {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->image_fname);
        return d_failure;
    }
    return d_success;
}

/* DECODE MAGIC STRING SIGNATURE */
Status decode_magic_string(FILE *fptr_image) {
    printf("Decoding Magic String Signature\n");
    fseek(fptr_image, 54, SEEK_SET);                     // Skip BMP header
    unsigned char buff;
    unsigned char magic_str[sizeof(MAGIC_STRING)] = {0};

    for (int i = 0; i < sizeof(MAGIC_STRING) - 1; i++) {
        for (int j = 0; j < 8; j++) {
            fread(&buff, 1, 1, fptr_image);
            magic_str[i] |= ((buff & 0x01) << j);
        }
    }

    return (strcmp((char *)magic_str, MAGIC_STRING) == 0) ? d_success : d_failure;
}

/* DECODE OUTPUT FILE EXTENSION SIZE */
uint decode_op_file_extn_size(FILE *fptr_image) {
    unsigned char buff;
    uint size = 0;

    for (int i = 0; i < sizeof(uint); i++) {
        for (int j = 0; j < 8; j++) {
            fread(&buff, 1, 1, fptr_image);
            size |= ((buff & 0x01) << j);
        }
    }

    return size;
}

/* DECODE OUTPUT FILE EXTENSION */
Status decode_op_file_extn(char *secret_dec_fname, FILE *fptr_image, uint sec_file_ext_size) {
    printf("Decoding Output File Extension\n");
    unsigned char buff;
    char ext[sec_file_ext_size + 1];

    // Initialize the ext array to zero
    for (int i = 0; i < sec_file_ext_size + 1; i++) {
        ext[i] = '\0';
    }

    for (int i = 0; i < sec_file_ext_size; i++) {
        for (int j = 0; j < 8; j++) {
            fread(&buff, 1, 1, fptr_image);
            ext[i] |= ((buff & 0x01) << j);
        }
    }

    // Concatenate the decoded extension to the secret file name
    strcat(secret_dec_fname, ext);

    return d_success;
}


/* OPEN SECRET FILE */
Status dec_open_sec_file(DecodeInfo *decInfo) {
    printf("Opening %s\n", decInfo->secret_dec_fname);
    decInfo->fptr_secret_dec_fname = fopen(decInfo->secret_dec_fname, "wb");

    if (decInfo->fptr_secret_dec_fname == NULL) {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_dec_fname);
        return d_failure;
    }
    printf("Files opened successfully.\n");
    return d_success;
}

/* DECODE OUTPUT FILE DATA SIZE */
uint decode_op_file_data_size(FILE *fptr_image) {
    unsigned char buff;
    uint size = 0;
    char *ptr_size = (char*)&size;

    for (int i = 0; i < sizeof(uint); i++) {
        for (int j = 0; j < 8; j++) {
            fread(&buff, 1, 1, fptr_image);
            ptr_size[i] |= ((buff & 0x01) << j);
        }
    }
    return size;
}

/* COPY DATA TO SECRET FILE */
Status copy_data_to_sec_file(FILE *fptr_image, FILE *fptr_sec, uint sec_file_data_size) {
    unsigned char buff;
    unsigned char byte = 0;

    for (int i = 0; i < sec_file_data_size; i++) {
        byte = 0;
        for (int j = 0; j < 8; j++) {
            fread(&buff, 1, 1, fptr_image);
            byte |= ((buff & 0x01) << j);
        }
        fwrite(&byte, 1, 1, fptr_sec);
    }

    return d_success;
}
