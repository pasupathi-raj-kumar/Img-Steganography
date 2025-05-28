#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    // Stego Image info
    char *stego_image_fname;
    FILE *fptr_stego_image;

        //output file
    char *output_fname;
    FILE *fptr_output;

    //Image Data
    char image_data[8];

    //Magic String
    char magic_string[10];

    //Secret File Extension Size
    long extn_size;

    //Secret File Extension
    char extn_secret_file[10];

    //Secret File Size
    long size_secret_file;


} DecodeInfo; 

/* Decode function prototype */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode Secret File Extension Size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode Secret File Extension */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode Secret File Size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode Secret File Data */
Status decode_secret_file_data(DecodeInfo *decInfo);

char decode_byte_from_lsb(const char *);

long decode_size_from_lsb(const char *);

#endif

