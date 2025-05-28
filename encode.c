#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("    width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("    height = %u\n", height);

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

    printf("    Opening Files :- \n");
    printf("    Beautiful file :   %s\n", encInfo->src_image_fname);
    printf("    Secret file    :   %s\n", encInfo->secret_fname);
    printf("    Stego file     :   %s\n", encInfo->stego_image_fname);

    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "  ❌ ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "  ❌ ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "  ❌ ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    printf("\n");
    // No failure return e_success
    return e_success;
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
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

    int i = 0, dot = 0;
    while (argv[3][i] != '\0')
    {
        if (argv[3][i] == '.')
        {
            dot++;
        }
        i++;
    }

    if (dot != 1)
    {
        return e_failure;
    }

    encInfo->src_image_fname = argv[2];
    encInfo->secret_fname = argv[3];

    if (argv[4] == NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    else
    {

        char *ch = strchr(argv[4], '.');
        if (ch != NULL)
        {
            *ch = '\0';
        }
        
        encInfo->stego_image_fname = argv[4];

        strcat(encInfo->stego_image_fname, ".bmp");
    }

    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // Get image size
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // Get secret file size
    fseek(encInfo->fptr_secret, 0L, SEEK_END);
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);

    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname, '.'));

    // printf("secret file extn size = %ld, extention : %s\n", strlen(encInfo->extn_secret_file), encInfo->extn_secret_file);

    // Check if image capacity is sufficient

    // printf("    Image Capacity = %u\n", encInfo->image_capacity);
    // printf("    Encoded Data Size = %lu\n", (strlen(MAGIC_STRING) + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file) * 8);
    
    if (encInfo->image_capacity < ((strlen(MAGIC_STRING) + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file) * 8))
    {
        fprintf(stderr, "ERROR: Insufficient Image Capacity\n");
        return e_failure;
    }

    return e_success;
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    // Copy 54 bytes from src to dest
    char data[54];

    // Seek to start of file
    fseek(fptr_src_image, 0L, SEEK_SET);

    // Read 54 bytes
    fread(data, 54, 1, fptr_src_image);
    // Write 54 bytes
    fwrite(data, 54, 1, fptr_dest_image);

    return e_success;
}

void encode_byte_to_lsb(char data, char *image_buffer)
{

    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> 7 - i) & 1);
    }
}

void encode_size_to_lsb(long data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> 31 - i) & 1);
        // printf("%ld", (data >> 31 - i) & 1);
        // printf("%d ", image_buffer[i]&1);
    }
    // printf("\n");
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{

    // printf("    Magic string = %s\n", magic_string);
    // Copy magic string to image data
    for (int i = 0; i < strlen(magic_string); i++)
    {
        fread(encInfo->image_data, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(MAGIC_STRING[i], encInfo->image_data);
        fwrite(encInfo->image_data, 8, 1, encInfo->fptr_stego_image);
    }

    // Return e_success
    return e_success;
}

Status encode_secret_file_extn_size(long file_size, EncodeInfo *encInfo)
{
    // Copy secret file size to image data
    char data[32];

    // printf("    Extention size = %ld\n", file_size);

    fread(data, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, data);

    fwrite(data, 32, 1, encInfo->fptr_stego_image);

    // Return e_success
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{

    // printf("    File extn = %s\n", file_extn);
    // Copy secret file extension to image data
    for (int i = 0; i < strlen(file_extn); i++)
    {
        fread(encInfo->image_data, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], encInfo->image_data);
        fwrite(encInfo->image_data, 8, 1, encInfo->fptr_stego_image);
    }

    // Return e_success
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    // printf("    file size => %ld\n", ftell(encInfo->fptr_src_image));
    // Copy secret file size to image data
    char data[32];

    fread(data, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_size, data);
    fwrite(data, 32, 1, encInfo->fptr_stego_image);

    // Return e_success
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0L, SEEK_SET);

    // printf("    Secret file data :- \n");

    // printf("+-------------------------------------------------------------------+\n");
    // Copy secret file data to image data
    char data;
    while (fread(&data, 1, 1, encInfo->fptr_secret))
    {
        // printf("%c", data);

        fread(encInfo->image_data, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(data, encInfo->image_data);
        fwrite(encInfo->image_data, 8, 1, encInfo->fptr_stego_image);
    }

    // printf("\n");

    // printf("+------------------- Secret data displayed -------------------------+\n");

    // Return e_success
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char data;
    while (fread(&data, 1, 1, fptr_src))
    {
        fwrite(&data, 1, 1, fptr_dest);
    }

    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    // Open files
    if (open_files(encInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to open files\n\n");
        return e_failure;
    }
    printf("  ✅ Open Files Success\n\n");

    // Check capacity
    if (check_capacity(encInfo) == e_failure)
    {
        printf("  ❌ ERROR: Insufficient Image Capacity\n\n");
        return e_failure;
    }
    printf("  ✅ Check Capacity Success\n\n");

    // Copy BMP Header
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("  ❌ ERROR: Unable to copy BMP Header\n\n");
        return e_failure;
    }
    printf("  ✅ Copy BMP Header Success\n\n");

    // Encode Magic String
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to encode Magic String\n\n");
        return e_failure;
    }
    printf("  ✅ Encode Magic String Success\n\n");

    // Encode secret file extension size
    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to encode Secret File Extension Size\n\n");
        return e_failure;
    }
    printf("  ✅ Encode Secret File Extension Size Success\n\n");

    // Encode Secret File Extension
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to encode Secret File Extension\n\n");
        return e_failure;
    }
    printf("  ✅ Encode Secret File Extension Success\n\n");

    // // Encode secret file size
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to encode Secret File Size\n\n");
        return e_failure;
    }
    printf("  ✅ Encode Secret File Size Success\n\n");

    // // Encode secret file data
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("  ❌ ERROR: Unable to encode Secret File Data\n\n");
        return e_failure;
    }
    printf("  ✅ Encode Secret File Data Success\n\n");

    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("  ❌ ERROR: Unable to copy remaining image data\n\n");
        return e_failure;
    }
    printf("  ✅ Copy Remaining Image Data Success\n\n");

    // Close all files
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);

    return e_success;
}