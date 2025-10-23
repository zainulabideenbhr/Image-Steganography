#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "common.h"
#include "encode.h"
#include "decode.h"

/*ARGUMENT CHECK*/
OperationType check_operation_type(char *argv[]) {
    if (strcmp(argv[1], "-e") == 0) return e_encode;
    else if (strcmp(argv[1], "-d") == 0) return e_decode;
    else return e_unsupported;
}

/*READ AND VALIDATE THE ENCODE ARGUMENTS*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo) {
    if (strstr(argv[2], ".bmp") == NULL) {
        return e_failure;    
    }
    encInfo->src_image_fname = argv[2];

    if (argv[3] == NULL) {
        return e_failure;
    }
    encInfo->secret_fname = argv[3];

    if (argv[4] == NULL) {
        encInfo->stego_image_fname = "default.bmp";
    } else {
        encInfo->stego_image_fname = argv[4];
    }

    return e_success;
}

/*OPEN ALL THE REQUIRED FILES*/
Status open_files(EncodeInfo *encInfo) {
    /* Open src image file */
    printf("Opening source image file: %s\n", encInfo->src_image_fname);
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL) {
        perror("fopen");
        fprintf(stderr, "Error: Couldn't open source image file: %s\n", encInfo->src_image_fname);
        return e_failure;    
    }
    printf("Source image file opened: %s\n", encInfo->src_image_fname);

    /* Open secret file */
    printf("Opening secret file: %s\n", encInfo->secret_fname);
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    if (encInfo->fptr_secret == NULL) {
        perror("fopen");
        fprintf(stderr, "Error: Couldn't open secret file: %s\n", encInfo->secret_fname);
        return e_failure;    
    }
    printf("Secret file opened: %s\n", encInfo->secret_fname);

    /* Open stego image file */
    printf("Opening/Creating stego image file: %s\n", encInfo->stego_image_fname);
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL) {
        perror("fopen");
        fprintf(stderr, "Error: Couldn't open or create stego image file: %s\n", encInfo->stego_image_fname);
        return e_failure;    
    }
    printf("Stego image file opened/created: %s\n", encInfo->stego_image_fname);
    printf("Files are ready. Proceeding with encoding.\n");
    return e_success;
}

/*GET THE SIZE OF BMP IMAGE*/
uint get_image_size_for_bmp(FILE *fptr_image) {
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(uint), 1, fptr_image); 
    fread(&height, sizeof(uint), 1, fptr_image);
    return width * height * 3;
}

/*GET THE SIZE OF THE SECRET FILE*/
uint get_file_size(FILE *fptr_secret) {
    fseek(fptr_secret, 0L, SEEK_END);
    return ftell(fptr_secret);
}

/*CHECK THE CAPACITY i.e., DATA IMAGE CAN HOLD*/
Status check_capacity(EncodeInfo *encInfo) {
    uint size = 54 + MAGIC_STRING_SIZE + 32 + 32 + 32 + encInfo->size_secret_file * 8;
    return (encInfo->image_capacity >= size) ? e_success : e_failure;
}

/*DO ENCODING*/
Status do_encoding(EncodeInfo *encInfo) {
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) {
        printf("BMP header copied.\n");
        printf("Encoding magic string...\n");
        if (encode_magic_string(MAGIC_STRING, encInfo) == e_success) {
            printf("Magic string encoded.\n");
            printf("Encoding file extension: %s\n", encInfo->secret_fname);
            if (encode_secret_file_extn(encInfo->secret_fname, encInfo) == e_success) {
                int secret_file_extn_size = strlen(encInfo->secret_file_extn); 
                char *ptr_sec_file_extn_size = (char *)&secret_file_extn_size;
                printf("File extension encoded.\n");

                if (encode_size_data_to_image(ptr_sec_file_extn_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) {
                    printf("Encoding file extension size.\n");
                    if (encode_data_to_image(encInfo->secret_file_extn, strlen(encInfo->secret_file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) {
                        printf("File extension size encoded.\n");
                        int secret_file_size = encInfo->size_secret_file;
                        char *ptr_secret_file_size = (char *)&secret_file_size;
                        printf("Encoding secret file size: %d\n", secret_file_size);
                        if (encode_size_data_to_image(ptr_secret_file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) {
                            printf("Secret file size encoded.\n");
                            printf("Encoding secret file data...\n");
                            if (encode_secret_file_data_to_image(encInfo->fptr_secret, secret_file_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) {
                                printf("Secret file data encoded.\n");
                                printf("Copying remaining image data...\n");
                                if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) {
                                    printf("Remaining image data copied.\n");
                                    return e_success;
                                }
                            }
                        }
                    }
                }
            } else {
                printf("Error: Failed to encode magic string.\n");
            }
        }
    }
    return e_failure;
}

/*COPY BMP HEADER TO THE OUTPUT FILE*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image) {
    char buff[54];
    rewind(fptr_src_image);
    fread(buff, 54, 1, fptr_src_image);
    fwrite(buff, 54, 1, fptr_dest_image);
    return e_success;
}

/*ENCODE MAGIC STRING*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo) {
    return encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

/*ENCODE DATA TO THE IMAGE*/
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 8; j++) {
            int mask = (data[i] >> j) & 1;
            char buff;
            fread(&buff, 1, 1, fptr_src_image);
            buff = (buff & 0xFE) | mask;                         // Clear LSB and set it to 'mask'
            fwrite(&buff, 1, 1, fptr_stego_image);
        }
    }
    return e_success;
}

/*ENCODE SECRET FILE EXTENSION TO THE IMAGE*/
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo) {
    char *token = strtok((char *)file_extn, ".");
    token = strtok(NULL, ".");
    strcpy(encInfo->secret_file_extn, ".");
    strcat(encInfo->secret_file_extn, token);
    return e_success;
}

/*ENCODE SIZE OF DATA TO THE IMAGE*/
Status encode_size_data_to_image(char *data_size, FILE *fptr_src_image, FILE *fptr_stego_image) {
    for (int i = 0; i < sizeof(int); i++) {
        for (int j = 0; j < 8; j++) {
            int mask = (data_size[i] >> j) & 1;
            char buff;
            fread(&buff, 1, 1, fptr_src_image);
            buff = (buff & 0xFE) | mask;
            fwrite(&buff, 1, 1, fptr_stego_image);
        }
    }
    return e_success;
}

/*ENCODE SECRET FILE DATA TO IMAGE */
Status encode_secret_file_data_to_image(FILE *fptr_secret, uint secret_file_size, FILE *fptr_src_image, FILE *fptr_stego_image) {
    rewind(fptr_secret);
    for (int i = 0; i < secret_file_size; i++) {
        char secret_file_data_buff;
        fread(&secret_file_data_buff, 1, 1, fptr_secret);
        for (int j = 0; j < 8; j++) {
            int mask = (secret_file_data_buff >> j) & 1;
            char buff;
            fread(&buff, 1, 1, fptr_src_image);
            buff = (buff & 0xFE) | mask;
            fwrite(&buff, 1, 1, fptr_stego_image);
        }
    }
    return e_success;
}

/*COPY REMAINING IMAGE DATA TO STEGO IMAGE AFTER ENCODING*/
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_dest_image) {
    char buff;
    while (fread(&buff, 1, 1, fptr_src_image) > 0) {
        fwrite(&buff, 1, 1, fptr_dest_image);
    }
    return e_success;
}
