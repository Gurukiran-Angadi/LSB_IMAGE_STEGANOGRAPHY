#include <stdio.h>
#include<string.h>
#include "common.h"
#include "decode.h"
#include "types.h"

//read and validate files
Status read_and_validate_decode_args(char* argv[],DecodeInfo *decInfo)
{
   if(strstr(argv[2],".bmp")==NULL)
    {
        printf("Invalid file format\n");
        return e_failure;
    }
    decInfo->stego_image_fname=argv[2];

    if(argv[3] != NULL)
    {
        strcpy(decInfo->secret_file_name, argv[3]);
    }
    else
    {
        strcpy(decInfo->secret_file_name, "output.txt");
    }

    return e_success;
}
// Open  files 
Status open_files_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"rb");
    if(decInfo->fptr_stego_image==NULL)
    {
        fprintf(stderr,"ERROR: unable to open file\n");
        return e_failure;
    }
    return e_success;

}
// Decoding a byte from lsb
int decode_byte_from_lsb(char *arr)
{
    int result=0;
    for(int i=0;i<8;i++)
    {
        char res=arr[i] &1;
        result= result | (res<<i);
    }
    return result;
}
// Decoding data from image
Status decode_data_from_image(const char *data, int size, FILE *fptr_stego_image)
{
    char magic_string[size+1];
    char arr[8];
    for(int i=0;i<size;i++)
    {
        if(fread(arr,8,1,fptr_stego_image)!=1)
            return e_failure;
       magic_string[i]= decode_byte_from_lsb(arr);
    }
    magic_string[size]='\0';
    if(strcmp(magic_string,data)==0)
    {
        printf("decoded magic string-->%s\n",magic_string);
        return e_success;
    }
    return e_failure;
}

// Decoding magic string
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    return decode_data_from_image(magic_string,strlen(magic_string),decInfo->fptr_stego_image);
}

// Decoding secret file extension size
Status decode_secret_file_extension_size(DecodeInfo *decInfo)
{
    char arr[32];
    fread(arr,32,1,decInfo->fptr_stego_image);
    decInfo->secret_file_extn_size=decode_integer_from_lsb(arr);
    printf("secret file ext size\n");
    return e_success;
}
// Decoding an integer from lsb
int decode_integer_from_lsb(char *arr)
{
    int result=0;
    for(int i=0;i<32;i++)
    {
        int res=arr[i] &1;
        result= result | (res<<i);
    }
    return result;

}
// Decoding secret file extension
Status decode_secret_file_extn( DecodeInfo *decInfo)
{
    char secret_extn[decInfo->secret_file_extn_size+1];
    char arr[8];
    for(int i=0;i<decInfo->secret_file_extn_size;i++)
    {
        if(fread(arr,8,1,decInfo->fptr_stego_image)!=1)
            return e_failure;
        secret_extn[i]=decode_byte_from_lsb(arr);
    }
    secret_extn[decInfo->secret_file_extn_size]='\0';
    //strcat(decInfo->secret_file_name,secret_extn);
    printf("secret file name-->%s\n",decInfo->secret_file_name);
    return e_success;
}
// Decoding size of the secret message
Status decode_data_size(DecodeInfo *decInfo)
{
    char arr[32];
    if(fread(arr,32,1,decInfo->fptr_stego_image)!=1)
        return e_failure;
    decInfo->size_secret_data=decode_integer_from_lsb(arr);
    printf("size of secret data-->%d\n",decInfo->size_secret_data);
    return e_success;

}
// Decoding the secret message
Status decode_string_from_lsb(DecodeInfo *decInfo)
{
    char arr[8];
    char ch;
    for(int i=0;i<decInfo->size_secret_data;i++)
    {
        if(fread(arr,8,1,decInfo->fptr_stego_image)!=1)
            return e_failure;
        ch=decode_byte_from_lsb(arr);
        if(fwrite(&ch,1,1,decInfo->fptr_secret)!=1)
            return e_failure;
    }
    return e_success;
    
}

// Decoding part
Status do_decoding(DecodeInfo *decInfo)
{
    if(open_files_decode(decInfo)==e_failure)
    {
        printf("opening file unsuccessful\n");
        return e_failure;
    }
    printf("opening files completed\n");

    fseek(decInfo->fptr_stego_image,54,SEEK_SET);

    if(decode_magic_string(MAGIC_STRING,decInfo)==e_failure)
    {
        printf("Error in decoding magic string\n");
        return e_failure;
    }
    printf("Decoding magic string completed\n");

    if(decode_secret_file_extension_size(decInfo)==e_failure)
    {
        printf("Error in decoding secret file extn size\n");
        return e_failure;
    }
    printf("Decoding of secret file ext size completed\n");

    if(decode_secret_file_extn(decInfo)==e_failure)
    {
        printf("Error in decoding secret file extn \n");
        return e_failure;
    }
    printf("Decoding of secret file ext  completed\n");

    if((decInfo->fptr_secret=fopen(decInfo->secret_file_name,"wb"))==NULL)
    {
        printf("Error in opening file in write mode\n");
        return e_failure;
    }
    printf("opening successful\n");

    if(decode_data_size(decInfo)==e_failure)
    {
        printf("Error in decoding data size\n");
        return e_failure;
    }
    printf("Decoding of data size completed\n");

    if(decode_string_from_lsb(decInfo)==e_failure)
    {
        printf("Error in decoding data string \n");
        return e_failure;
    }
    printf("Decoding of data string completed\n");
    printf("Decoding Successfull.\n");
    return e_success;

}