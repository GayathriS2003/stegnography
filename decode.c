#include<stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>

Status read_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[2] == NULL)
    {
        printf("bmp file has not been passed\n");
        return e_failure;
    }

    //check argv[2] is bmp file
    if(strstr(argv[2], ".bmp") == NULL)    //check .bmp
    {
        printf("Invalid bmp file\n");
        return e_failure;
    }

    //storing argv[2] stego file name to structure
    decInfo->stego_fname = argv[2];

    if (argv[3] == NULL)                
    {
        strcpy(decInfo->output_fname, "output");  //file name
    }
    else
    {
        //store user given output file name
        strcpy(decInfo->output_fname, argv[3]);
    }

}



Status do_decoding(DecodeInfo *decInfo)
{
    printf("-----------DECODED STARTED--------------\n"); 
    int ret = open_file(decInfo);
    if(ret == e_failure) 
    {
        printf("File opening failed\n");
        return e_failure;
    }
    printf("Decoded file is opened successfully\n");

    if(decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        printf("decoded magic string is not copied\n");
        return e_failure;
    }
    printf("After magic string, ftell = %ld\n",ftell(decInfo->fptr_stego));
    
    
    if(decode_extn_size(decInfo) == e_failure)
    {
        printf("The decoded extn file size failed\n");
        return e_failure;
    }

    if(decode_extn(decInfo) == e_failure)
    {
        printf("The decoded file extension is failed\n");
        return e_failure;
    }

    if(decode_file_size(decInfo) == e_failure)
    {
        printf("The decoded file size is failed\n");
        return e_failure;
    }


    if(decode_file_data(decInfo) == e_failure)
    {
        printf("The encoded secret data is not copied to decoded output file\n");
        return e_failure;

    }
    
    printf("The data is decoded to output file successfully!!\n");
    return e_success;


}
    

 //open files
Status open_file(DecodeInfo *decInfo)
{
    
    decInfo->fptr_stego = fopen(decInfo->stego_fname,"rb"); 
    if(decInfo->fptr_stego == NULL)
    {
        printf(" decoded file not opened\n");  
        return e_failure;
    }
    return e_success;
}

//generic function
Status decode_lsb_to_size(char *image_buffer)
    {
        int size = 0;
        int n = 31;
        char buffer[32];
        for(int i = 0; i < 32; i++)
        {
            //get lsb
            int bit = image_buffer[i] & 1;

            //left shift bit n times
            bit = bit << n;

            //write bit variable to ch
            size = size | bit; 

            n--;
        }
        return size;
        
    }
Status decode_lsb_to_bytes(char *image_buffer)
{
    char ch = 0;
    int n = 7;
    char buffer[8];
    for(int i = 0; i < 8; i++)
    {    
            //get lsb
            char bit = image_buffer[i] & 1;

            //left shift bit n times
            bit = bit << n;

            //write bit variable to ch
            ch = ch | bit;

            n--;
        }
        return ch;
    }


Status decode_magic_string(const char *magic_str, DecodeInfo *decInfo)
{
    //skip 54 bytes
    fseek(decInfo->fptr_stego, 54, SEEK_SET);

    char magic_string[10];
    int i;

    for(i = 0; i < 2; i++)
    {
        char buffer[8];
        
        //read 8 bytes of data buffer from stego
        if(fread(buffer, 1, 8, decInfo->fptr_stego)!= 8)
        {
            printf("Error:decoded failed to read magic string\n");
            return e_failure;
        }

        magic_string[i] = decode_lsb_to_bytes(buffer);
    }
        magic_string[i] = '\0';  //null terminator

        char str[20];
        printf("Enter your magic string:");  // user inputs magic string
        scanf("%[^\n]",str);
    
        if(strcmp(str, MAGIC_STRING) == 0)  //compare both
        {
            printf("The magic string is correct, The person is authorized!!\n");
            return e_success;
        }
        else
        {
            printf("The magic string is wrong\n");
            return e_failure;
        } 
}


Status decode_extn_size(DecodeInfo *decInfo)
{

    char buffer[32];

    //read 32 bytes from stego file
    if(fread(buffer, 1, 32, decInfo->fptr_stego) != 32)
        return e_failure;

    decInfo->extn_size = decode_lsb_to_size(buffer);

    printf("Decoded extension size: %d\n", decInfo->extn_size);

    return e_success;
}

Status decode_extn(DecodeInfo *decInfo)
{
     for(int i = 0; i < decInfo->extn_size; i++)
    {
        char buffer[8];
        
        //read 8 bytes from stego file and store to buffer
        if(fread(buffer, 1, 8, decInfo->fptr_stego) != 8)
            return e_failure;

        decInfo->extn[i] = decode_lsb_to_bytes(buffer);

    }

    decInfo->extn[decInfo->extn_size] = '\0';  //null terminator

    
    printf("Decoded extension: %s\n", decInfo->extn);

    //merged output file name with decoded extn
    strcpy(decInfo->output_fname, "output");
    strcat(decInfo->output_fname, decInfo->extn);


    decInfo->output = fopen(decInfo->output_fname,"wb");   // open output file
    if(decInfo->output == NULL)
    {
       printf("output decoded file not opened\n");
       return e_failure;
    }
    printf("Output file opened\n");
    return e_success;
    
}


Status decode_file_size(DecodeInfo *decInfo)
{
    char buffer[32];

    //read 32 bytes from stego to store buffer
    if(fread(buffer, 1, 32, decInfo->fptr_stego) != 32)
        return e_failure;
    
    decInfo->file_size = decode_lsb_to_size(buffer);

    printf("Decode sec file size: %d\n", decInfo->file_size);
    return e_success;

}

Status decode_file_data(DecodeInfo *decInfo)
{
    char image_buffer[8];
    char ch;

    for(int i = 0; i < decInfo->file_size; i++)
    {
        if(fread(image_buffer, 1, 8, decInfo->fptr_stego) != 8)
            return e_failure;

        ch = decode_lsb_to_bytes(image_buffer);
        fputc(ch, decInfo->output);    
    }
    
    fclose(decInfo->output);      //close all files
    printf("All the data are copied to output file successfully!!\n");
    return e_success;


}

















