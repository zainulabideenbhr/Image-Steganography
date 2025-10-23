/*

Name          : Zainul Abideen
Date          : 08-08-2024
Description   : Steganography
Sample Input  :
Sample Output :

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "common.h"
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;                             //structure variable of encoding
    DecodeInfo decInfo;                             //structure variable of decoding
    int ret = check_operation_type(argv);         //it will check if the operation is encode or decode

/*if operation is encoding*/  
    if (ret == e_encode)                            
    {
        ret = read_and_validate_encode_args(argv, &encInfo);        //function call to validate encoding arguments

        if (ret == e_failure)
        {
            printf("ERROR: Pass proper arguments\n");
            return e_failure;
        }

        if (open_files(&encInfo) == e_failure)                      //function call for opening files                
        {
            printf("ERROR: Failed to open files\n");
            return e_failure;
        }
        else
        {
            encInfo.image_capacity = get_image_size_for_bmp(encInfo.fptr_src_image);        //to store image capacity
            encInfo.size_secret_file = get_file_size(encInfo.fptr_secret);                  //to store secret file size
            printf("Checking size of %s\n", encInfo.secret_fname);

            if (encInfo.size_secret_file == 0)                                              //check if the secret file is empty
            {
                printf("ERROR: %s is empty\n", encInfo.secret_fname);
                return e_failure;
            }

            printf("File is not empty\n");
            printf("Checking if %s has enough capacity to hold the secret file\n", encInfo.src_image_fname);

            if (check_capacity(&encInfo) == e_success)                                      // Check if the image has enough space to hide the secret file
            {
                printf("Capacity check passed\n");
                printf("\n## Encoding Procedure Started ##\n");

                if (do_encoding(&encInfo) == e_success)                                     //function call to start encoding
                {
                    printf("Encoding completed successfully\n");
                }
                else
                {
                    printf("ERROR: Encoding failed\n");
                    return e_failure;
                }
            }
            else
            {
                printf("ERROR: Not enough storage capacity to store the secret file\n");
            }
        }
    }
    
/*if operation is decoding*/
    else if (ret == e_decode)
{
    printf("Starting decode process\n");

    if (read_and_validate_decode_args(argv, &decInfo) == d_failure)                             //function call to validate decoding arguments
    {
        printf("ERROR: Pass proper arguments\n");
        return e_failure;
    }

    printf("\n## Decoding Procedure Started ##\n");
    printf("Opening required files\n");

    if (dec_open_files(&decInfo) == d_failure)                                                  //function call for opening files
    {
        printf("ERROR: Failed to open files\n");
        return e_failure;
    }

    if (decode_magic_string(decInfo.fptr_image_fname) == d_success)                             //function to decode magic string
    {
        printf("Magic string decoded successfully\n");
        decInfo.sec_file_ext_size = decode_op_file_extn_size(decInfo.fptr_image_fname);         //secret file extension size

        if (decode_op_file_extn(decInfo.secret_dec_fname, decInfo.fptr_image_fname, decInfo.sec_file_ext_size) == d_success) //secret file's extension
        {
            printf("File extension decoded successfully\n");

            if (dec_open_sec_file(&decInfo) == d_success)                                      //open the necessary files for decoding
            {
                printf("Decoding file size for %s\n", decInfo.secret_dec_fname);
                decInfo.sec_file_data_size = decode_op_file_data_size(decInfo.fptr_image_fname);    
                printf("File size decoded successfully\n");
                printf("Decoding file data for %s\n", decInfo.secret_dec_fname);

                /* Decode and copy the secret file data from the image to the secret file */                
                copy_data_to_sec_file(decInfo.fptr_image_fname, decInfo.fptr_secret_dec_fname, decInfo.sec_file_data_size);
                printf("File data decoded successfully\n");
            }
        }
        else
        {
            printf("ERROR: Failed to decode file extension\n");
        }
    }
    else
    {
        printf("No secret data to decode\n");
    }

    printf("## Decoding Completed Successfully ##\n");
}
return 0;
}

