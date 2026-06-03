#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"     // Contains Status definition

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

/* Structure to store decoding information */
typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;     // Stego image file name
    FILE *fptr_stego_image;      // Stego image file pointer

    /* Secret File Info */
    char secret_file_name[50];   // Output file name
    FILE *fptr_secret;           // Output file pointer

    char secret_file_extn[10];   // Secret file extension
    int  secret_file_extn_size;  // Size of extension

    int size_secret_data;        // Size of actual secret data

} DecodeInfo;


/* Read and validate command line arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Open required files for decoding */
Status open_files_decode(DecodeInfo *decInfo);

/* Decode magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode extension size */
Status decode_secret_file_extension_size(DecodeInfo *decInfo);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_data_size(DecodeInfo *decInfo);

/* Decode actual secret string */
Status decode_string_from_lsb(DecodeInfo *decInfo);

/* decoding function */
Status do_decoding(DecodeInfo *decInfo);

/* Helper functions */

/* Decode one byte from LSB */
int decode_byte_from_lsb(char *arr);

/* Decode integer (32 bits) from LSB */
int decode_integer_from_lsb(char *arr);

/* Decode data from image */
Status decode_data_from_image(const char *data, int size, FILE *fptr_stego_image);

#endif