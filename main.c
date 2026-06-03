/* 
Name : Gurukiran Angadi
Date : 25/02/2026 
project name: LSb image stegnography.
description : LSB (Least Significant Bit) image steganography is a technique used to hide secret data inside an image by modifying the least significant bits of pixel values.
Since LSB changes cause minimal variation in pixel intensity, the visual appearance of the image remains almost unchanged.
*/
#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "Decode.h"
#include "types.h"

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

int main(int argc, char *argv[])
{
    if(argc<4){
        printf("Provided input is not sufficient.\n");
        printf("For Encoding part : ./a.out -e beautiful.bmp secret.txt [optional output file name]\n");
        printf("For Decoding part : ./a.out -d stego.bmp secret.txt [optional output file name]\n");
        return 2;
    }
    //res = check_operationType(argv);
    int res = check_operation_type(argv);

    //res -> e_encode -> read_and_validate_encodde_arg(); -> do_encoding();
    if(res == e_encode){ 
        printf("Encoding.....\n");
        EncodeInfo encInfo;
        res = read_and_validate_encode_args(argv,&encInfo);

        if(res == e_success){
            printf("The data entered in a structure\n");
            if(do_encoding (&encInfo)== e_failure){
                return 1;
            }
            else if(res == e_failure){
                printf("The Data entry failed\n");
            }
        }
    }

    //res -> e_decode -> read_and_validate_decode_arg();  -> do_decoding();
    else if(res == e_decode){
        printf("Decoding......\n");
        DecodeInfo decInfo;
        res=read_and_validate_decode_args(argv,&decInfo);
        if(res == e_success){
            printf("Decode data entered into a structure\n");
            if(do_decoding(&decInfo) == e_failure){
                return 1;
            }
            else if(res == e_failure){
                printf("The Decode data entry failed\n");
                return 1;
            }
        }

    }

    //res -> e_usupported -> error -> terminate
    else if(res == e_unsupported){
        printf("Invalid input\n");
        return 0;
    }
    return 0;
}
