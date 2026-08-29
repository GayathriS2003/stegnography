#ifndef DECODE_H
#define DECODE_H

#include"types.h"

typedef struct _DecodeInfo
{
    //stego file name
    char *stego_fname;   //stego file
    FILE *fptr_stego;  //opened


    //info
    int extn_size;
    char extn[10];  //".txt"
    int file_size;  //25
    char file_data;
    

    //output file info
    char output_fname[30];  //"output.txt"
    FILE *output;  //opened

}DecodeInfo;

/* Decoding function prototype */

/* Read and validate decode args from argv */
Status read_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* open files*/
Status open_file(DecodeInfo *decInfo);

/* Decode magic string */
Status decode_magic_string(const char *magic_str, DecodeInfo *decInfo);

/* Decode lsb to size */
Status decode_lsb_to_size(char *image_buffer);

/* Decode lsb to bytes */
Status decode_lsb_to_bytes(char *image_buffer);

/* Decode secret file extn size*/
Status decode_extn_size(DecodeInfo *decInfo);

/* Decode secret  file extn */
Status decode_extn(DecodeInfo *decInfo);

/* Decode secret file size */ 
Status decode_file_size(DecodeInfo *decInfo);

/* Decode secret file data */
Status decode_file_data(DecodeInfo *decInfo);


#endif
 