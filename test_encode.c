#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>


int main(int argc, char *argv[])
{
    int ret = check_operation_type(argv);
    
    if (ret == e_encode)
    {
        if (argc < 4)   
        {
            printf("  ❌ Insufficient Arguments\n");
            return 0;
        }
        
        EncodeInfo encInfo;
        printf("+-------------------------------------------------------------------+\n");
        printf("|               ...Welcome to Steganography encoding...             |\n");
        printf("+-------------------------------------------------------------------+\n\n");

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("  ✅ Read and Validate Encode Args Success\n\n");
            if (do_encoding(&encInfo) == e_success)
            {
                printf("  🎉 Encoding Success\n");
            }
            else
            {
                printf("  😣 Encoding Failure\n");
            }

        }
        else
        {
            printf("  ❌ Read and Validate Encode Args Failure\n");
            return 0;
        }
        printf("+-------------------------------------------------------------------+\n");

    }
    else if (ret == e_decode)
    {

            if(argc < 4)
        {
            printf("  ❌ Insufficient Arguments\n");
            return 0;
        }
        DecodeInfo decInfo;
        printf("+-------------------------------------------------------------------+\n");
        printf("|               ...Welcome to Steganography decoding...             |\n");
        printf("+-------------------------------------------------------------------+\n\n");

        if(read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("  ✅ Read and Validate Decode Args Success\n\n");
            if(do_decoding(&decInfo) == e_success)
            {
                printf("  🎉 Decoding Success\n");
            }
            else
            {
                printf("  😣 Decoding Failure\n");
            }
        }
        else
        {
            printf("  ❌ Read and Validate Decode Args Failure\n");
            return 0;
        }
        printf("+-------------------------------------------------------------------+\n");

    }
    else
    {
        printf("Unsupported Operation\n");
    }

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if (argv[1] != NULL)
    {
        if (strcmp(argv[1], "-e") == 0)
        {
            return e_encode;
        }
        else if (strcmp(argv[1], "-d") == 0)
        {
            return e_decode;
        }
        else
        {
            return e_unsupported;
        }
    }
    else
    {
        return e_unsupported;
    }
}