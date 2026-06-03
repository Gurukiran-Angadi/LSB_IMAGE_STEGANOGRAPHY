#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

//#define MAGIC_STRING "#*"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // argv[2] -> .bmp -> store argv[2] in structure encInfo->src_image_fname = argv[2];
    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("Invalid picture file format.\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];

    // argv[3] -> .txt -> encInfo->secret_fname = argv[3] -> strcpy(encInfo->extn_secret_file, strstr(argv[3], "."))
    if (strstr(argv[3], ".txt") == NULL)
    {
        printf("Invalid file format(it must be (.txt))\n");
        return e_failure;
    }
    strcpy(encInfo->extn_secret_file, strstr(argv[3], "."));
    encInfo->secret_fname = argv[3];

    // argv[4] -> NULL-> store "stego.bmp" in structure
    //    -> no NULL -> .bmp -> store argv[4] in structure
    if (argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);

    return ftell(fptr);
}

Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    if ((encInfo->image_capacity - 54) > (strlen(MAGIC_STRING) + sizeof(int) + strlen(encInfo->extn_secret_file) + sizeof(int) + encInfo->size_secret_file) * 8)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];
    for (int i = 0; i < size; i++)
    {
        if(fread(arr, 8, 1, fptr_src_image)!=1){
            return e_failure;
        }

        encode_byte_to_lsb(data[i], arr);

        if(fwrite(arr, 8, 1, fptr_stego_image)!=1){
            return e_failure;
        }
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    return encode_data_to_image(MAGIC_STRING, strlen(MAGIC_STRING), encInfo->fptr_src_image, encInfo->fptr_stego_image);
}

Status do_encoding(EncodeInfo *encInfo)
{
    // open_files() == e_failure -> error -> return e_failure
    if (open_files(encInfo) == e_failure){

        printf("The File opening part is failed.\n");
        return e_failure;

    }
    else{

        printf("The File opening part completed.\n");

    }

    // check_capacity() == e_failure -> error -> return e_failure

    if (check_capacity(encInfo) == e_failure){

        printf("Check capcity is failed.\n");
        return e_failure;

    }
    else{

        printf("Check capacity part is completed.\n");

    }

    // copy_bmp_header() == e_failure -> error -> return e_failure

    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){

        printf("Copy of bmp header is failed.\n");
        return e_failure;

    }
    else{

        printf("Copy of bmp headder is successfull.\n");

    }

    // encode_magic_string() == e_failure -> error -> return e_failure

    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure){

        printf("Encode magic string is failed.\n");
        return e_failure;

    }
    else{

        printf("Encode magic string is completed.\n");

    }

    // Encode secreat file extension size

    if (encode_secret_file_extension_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure){

        printf("The Encode secret file extention size failed.\n");
        return e_failure;

    }
    else{
        printf("Encoding of secret file extention size completed.\n");
    }

    // Encode secret file extension

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {

        printf("Encoding of secret file extension is failed.\n");
        return e_failure;

    }
    else{

        printf("Encoding secret file extension is successfull.\n");

    }

    // Encode secret file size

    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure){

        printf("Encoding of secret file size is failed.\n");
        return e_failure;

    }
    else{

        printf("Encoding of secret file size is successfull.\n");

    }

    // Encode of secrete file data

    if (encode_secret_file_data(encInfo) == e_failure){

        printf("Encode of secret file data is failed.\n");
        return e_failure;

    }
    else{

        printf("Encode of secret file data is successfull.\n");

    }

    // Copy remaining image bytes from src to stego image after encoding

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure){

        printf("Copy remaining data is failed.\n");
        return e_failure;

    }
    else{

        printf("Encoding of remaining data is completed.\n");

    }
    printf("Encoding successfull.\n");
    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    char buffer[54];

    fseek(fptr_src_image, 0, SEEK_SET);

    if (fread(buffer, 1, 54, fptr_src_image) != 54){

        return e_failure;

    }

    if (fwrite(buffer, 1, 54, fptr_dest_image) != 54){

        return e_failure;

    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++){

        image_buffer[i] &= 0xFE;
        int bit = (data >> i) & 1;
        image_buffer[i] |= bit;
    }

    return e_success;
}

Status encode_integer_to_lsb(int data,char *image_buffer)
{
    for (int i = 0; i < 32; i++){

        image_buffer[i] &= 0xFE;
        int bit = (data >> i) & 1;
        image_buffer[i] |= bit;
    }

    return e_success;
}

Status encode_secret_file_extension_size(long file_size,EncodeInfo *encInfo)
{
    char buffer[32];

    if (fread(buffer, 32, 1,encInfo->fptr_src_image) != 1){

        return e_failure;

    }

    encode_integer_to_lsb(file_size, buffer);

    if (fwrite(buffer, 32, 1,encInfo->fptr_stego_image) != 1){

        return e_failure;

    }
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn,EncodeInfo *encInfo)
{
    return encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,
                                                            encInfo->fptr_stego_image);
}

Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{
    char buffer[32];

    if (fread(buffer, 32, 1,encInfo->fptr_src_image) != 1){

        return e_failure;

    }

    encode_integer_to_lsb(file_size, buffer);

    if (fwrite(buffer, 32, 1,encInfo->fptr_stego_image) != 1)
        return e_failure;

    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);

    char ch;

    while (fread(&ch, 1, 1,encInfo->fptr_secret) == 1)
    {
        if (encode_data_to_image(&ch, 1,encInfo->fptr_src_image,
                                        encInfo->fptr_stego_image) == e_failure){

            return e_failure;
        }
    }

    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    char ch;

    while (fread(&ch, 1, 1, fptr_src) == 1)
    {
        if (fwrite(&ch, 1, 1, fptr_dest) != 1){
            return e_failure;
        }
    }

    return e_success;
}