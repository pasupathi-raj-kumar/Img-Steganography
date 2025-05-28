#include <stdio.h>
#include "decode.h"
#include "common.h"
#include "types.h"
#include <string.h>

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check for NULL pointers
    if (argv[2] == NULL || argv[3] == NULL)
    {
        return e_failure;
    }

    if (strstr(argv[2], ".bmp") == NULL)
    {
        return e_failure;
    }

    decInfo->stego_image_fname = argv[2];

    if (argv[3] == NULL)
    {
        decInfo->output_fname = "My_secret";
    }
    else
    {
        char *ch = strchr(argv[3], '.');
        if (ch != NULL)
            *ch = '\0';

        decInfo->output_fname = argv[3];
    }

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    // Open Stego Image
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "  ❌ ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

char decode_byte_from_lsb(const char *image_buffer)
{
    char data = 0;
    for (int i = 0; i < 8; i++)
    {
        data = (data << 1) | (image_buffer[i] & 1);
    }
    return data;
}

long decode_size_from_lsb(const char *image_buffer)
{
    long data = 0;
    for (int i = 0; i < 32; i++)
    {
        data = (data << 1) | (image_buffer[i] & 1);
    }
    //  printf("    Decoded Size = %ld\n", data);
    return data;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    // Read Magic String
    char data;
    // printf("    Magic String = ");
    for (int i = 0; i < strlen(MAGIC_STRING); i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);
        data = decode_byte_from_lsb(decInfo->image_data);
        // printf("%c", data);
        if (data != MAGIC_STRING[i])
        {
            fprintf(stderr, "  ❌ ERROR: Magic String not found\n");
            return e_failure;
        }
    }

    // printf("\n");

    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    // Read Secret File Extension Size

    decInfo->extn_size = 0;
    char data[32];

    fread(data, 32, 1, decInfo->fptr_stego_image);
    decInfo->extn_size = decode_size_from_lsb(data);

    // printf("    Secret File Extension Size = %ld\n", decInfo->extn_size);

    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{

    // Read Secret File Extension
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);
        decInfo->extn_secret_file[i] = decode_byte_from_lsb(decInfo->image_data);
    }
    decInfo->extn_secret_file[decInfo->extn_size] = '\0';

    // printf("    Secret File Extension = %s\n", decInfo->extn_secret_file);

    strcat(decInfo->output_fname, decInfo->extn_secret_file);

    // printf("    Secret file name = %s\n", decInfo->output_fname);

    decInfo->fptr_output = fopen(decInfo->output_fname, "w");

    return e_success;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    // Read Secret File Size
    char data[32];

    // printf("   secret file ptr : %ld\n", ftell(decInfo->fptr_stego_image));
    fread(data, 32, 1, decInfo->fptr_stego_image);

    decInfo->size_secret_file = decode_size_from_lsb(data);

    // printf("    Secret File Size => %ld\n", decInfo->size_secret_file);

    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // Read Secret File Data
    char data;
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(decInfo->image_data, 8, 1, decInfo->fptr_stego_image);
        data = decode_byte_from_lsb(decInfo->image_data);
        fwrite(&data, 1, 1, decInfo->fptr_output);
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{

    // Open files
    if (open_decode_files(decInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to open files\n\n");
        return e_failure;
    }
    printf("  ✅ Open Files Success\n\n");

    // Decode Magic String
    if (decode_magic_string(decInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to decode Magic String\n\n");
        return e_failure;
    }
    printf("  ✅ Decode Magic String Success\n\n");

    // Decode Secret File Extension Size
    if (decode_secret_file_extn_size(decInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to decode Secret File Extension Size\n\n");
        return e_failure;
    }
    printf("  ✅ Decode Secret File Extension Size Success\n\n");

    // Decode Secret File Extension
    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to decode Secret File Extension\n\n");
        return e_failure;
    }
    printf("  ✅ Decode Secret File Extension Success\n\n");

    // Decode Secret File Size
    if (decode_secret_file_size(decInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to decode Secret File Size\n\n");
        return e_failure;
    }
    printf("  ✅ Decode Secret File Size Success\n\n");

    // Decode Secret File Data
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to decode Secret File Data\n\n");
        return e_failure;
    }
    printf("  ✅ Decode Secret File Data Success\n\n");

    return e_success;
}