#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include<string.h>

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
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
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

//checking operation
OperationType check_operation_type(char *argv[])
{
    if(argv[1] == NULL)            
        return e_unsupported;

    if(strcmp(argv[1], "-e") == 0) //encode
        return e_encode;
        
    if(strcmp(argv[1], "-d") == 0) //decode   
        return e_decode;

    return e_unsupported;    
}

//read and validate
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] == NULL)
    {
        printf("bmp file has not been passed\n"); 
        return e_failure;
    }
    //check argv[2] is bmp file
    if(strstr(argv[2], ".bmp") == NULL)   
    {
        printf("Invalid bmp file\n");
        return e_failure;
    }
    //storing argv[2] bmp file name to structure
    encInfo->src_image_fname = argv[2];         

    if(argv[3] == NULL)
    {
        printf("secret file is not present\n");  
        return e_failure;
    } 
    if(strchr(argv[3], '.') == NULL)            
    {
        printf("Invalid secret file\n");
        return e_failure;
    }
    //store argv[3] to struct
    encInfo->secret_fname = argv[3];
    if(argv[4] == NULL)
    {
    encInfo->stego_image_fname = "stego.bmp";  //output file name
    }
    else
    {
        if(strstr(argv[4], ".bmp") == NULL)
        
        {
            printf("Invalid output file name\n");  
            return e_failure;
        }
        encInfo->stego_image_fname = argv[4];
    }
    return e_success;
} 

Status do_encoding(EncodeInfo *encInfo)
{

    //open files
    int ret = open_files(encInfo);   
    if(ret == e_failure)
    {
        printf("open files failed");
        return e_failure;
    }

    if(check_capacity(encInfo) == e_failure)  
    {
        printf("check capacity is failed\n");
        return e_failure;
    }
    
    //rewind src_image
    rewind(encInfo->fptr_src_image);    

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("bmp copy header is failed\n");              
        return e_failure;
    }

    printf("[debug] header copy offset: %ld\n",ftell(encInfo->fptr_src_image));
    


    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
       printf("magic string is failed\n");                
       return e_failure;
    }

    //display secret file size
    printf("INFO: secret file size = %ld bytes\n", encInfo->size_secret_file);  

    printf("[debug] encode copy offset: %ld\n",ftell(encInfo->fptr_src_image));

     if(encode_extn_file_size(encInfo->extn_size, encInfo) == e_failure)
    {
        printf("extn file size is failed\n");                    
        return e_failure;
    }

    //display extension size
    printf("[ENCODE ] encoded extension size: %u\n", encInfo->extn_size);    

    printf("[debug] encode copy offset: %ld\n",ftell(encInfo->fptr_src_image));


    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        printf("secret file extn is failed\n");             
        return e_failure;
    }

    printf("[debug] encode copy offset: %ld\n",ftell(encInfo->fptr_src_image));

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("secret file size is failed\n");             
        return e_failure;
    }  

    printf("[debug] encode copy offset: %ld\n",ftell(encInfo->fptr_src_image));

    //Display sec file data
    printf("INFO: Encoding Secret file data\n");              


    if(encode_secret_file_data(encInfo) == e_failure)
    {
        printf("secret data is failed\n");                     
        return e_failure;
    }

    printf("[debug] encode copy offset: %ld\n",ftell(encInfo->fptr_src_image));

    //Display secret data 
    printf("INFO: secret data encoded successfully\n");         

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("copy remaining data is failed\n");              
        return e_failure;
    }


    printf("[debug] encode copy offset: %ld\n",ftell(encInfo->fptr_src_image));

    //encode to stego_image_fname
    printf("INFO:Done\n");                                                 
    printf("SUCCESS: Encoded %ld bytes of secret data into %s\n", encInfo->size_secret_file, encInfo->stego_image_fname);

   //close all files
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_stego_image);
    fclose(encInfo->fptr_secret);

    return e_success;


}



Status check_capacity(EncodeInfo *encInfo) 
{
    //get sec file extrn
    strcpy(encInfo->extn_secret_file, strchr(encInfo->secret_fname, '.'));

    //get length of extn  
    encInfo->extn_size = strlen(encInfo->extn_secret_file);

    //get sec file size
    fseek(encInfo->fptr_secret, 0, SEEK_END);                
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);
    

    //reset src file back to first byte
    rewind(encInfo->fptr_secret);                            
   
    int total_bytes_needed = 54 + 16 + 32 + (encInfo->extn_size * 8) + 32 + (encInfo->size_secret_file * 8);

    int bmp_file_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    rewind(encInfo->fptr_src_image);
    

    if(total_bytes_needed <= bmp_file_size)
        return e_success;
    else
        return e_failure;
    
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char buffer[54];

    //read 54 bytes from fptr_src_image
    if(fread(buffer, 1, 54, fptr_src_image) != 54)         
        return e_failure;

    //write 54 bytes to fptr_dest_image
    if(fwrite(buffer, 1, 54, fptr_dest_image) != 54)        
        return e_failure;

    return e_success;
}

//generic function
Status encode_size_to_lsb(int data, char *image_buffer)
{
   //image buffer size in 32 bytes
   
   int n = 31;
   for(int i = 0; i < 32; i++)
    {
        int mask = 1 << n;
        int bit = (data >> n) & 1; 
        

        image_buffer[i] = image_buffer[i] & 0xfe;
        image_buffer[i] = image_buffer[i] | bit;
        n--;


    }
    return e_success;
    
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //image buffer arr size in 8 bytes
    
    int n = 7;
    for(int i = 0; i < 8; i++)
   {      
        char mask = 1 << n;
        char bit = (data >> n) & 1;
        

        image_buffer[i] = image_buffer[i] & 0xfe;
        image_buffer[i] = image_buffer[i] | bit;
        n--;


    }
    return e_success;
    
}


Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    
    for(int i = 0; i < 2; i++)
    {
        char buffer[8];
        //read 8 bytes of data buffer from src get_file_size

        if(fread(buffer, 1, 8, encInfo->fptr_src_image)!= 8)   
            return e_failure;

        encode_byte_to_lsb(magic_string[i], buffer);            

        //write 8 bytes buffer to stego files

        if(fwrite(buffer, 1, 8, encInfo->fptr_stego_image) != 8)   
            return e_failure;
    } 
    return e_success;
}

Status encode_extn_file_size(int extn_size, EncodeInfo *encInfo)
{
    char buffer[32];

    //read 32 bytes from src file
    if(fread(buffer, 1, 32, encInfo->fptr_src_image) != 32)      
        return e_failure;

    encode_size_to_lsb(extn_size, buffer);                   
    
    //write 32 bytes buffer to stego file
    if(fwrite(buffer, 1, 32, encInfo->fptr_stego_image) != 32)  
        return e_failure;
 
    return e_success;    
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    for(int i = 0; file_extn[i] != '\0'; i++)               
    {
        char buffer[8];
        
        //read 8 bytes from src file and store to buffer
        if(fread(buffer, 1, 8, encInfo->fptr_src_image) != 8)  
            return e_failure;

        encode_byte_to_lsb(file_extn[i], buffer);              
        
        //write 8 bytes buffer to stego file
        if(fwrite(buffer, 1, 8, encInfo->fptr_stego_image) != 8)  
            return e_failure;
    
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];

    //read 32 bytes from src image to store buffer
    if(fread(buffer, 1, 32, encInfo->fptr_src_image) != 32)   
        return e_failure;
    
    encode_size_to_lsb(file_size, buffer);                   
 
    //fwrite 32 bytes buffer to stego file
    if(fwrite(buffer, 1, 32, encInfo->fptr_stego_image) != 32) 
        return e_failure;
        
    return e_success;    
    
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //bytes encoded
    int bytes_encoded = 0;                             
    unsigned char buffer[8];
    char ch;

    //seek set 0 to sec data
    fseek(encInfo->fptr_secret, 0, SEEK_SET);  
                 
    //read 1 byte from sec
    while(fread(&ch, 1, 1,encInfo->fptr_secret) == 1)  
    {

        //read 8 byte from src image    
        if(fread(buffer, 1, 8, encInfo->fptr_src_image) != 8) 
            return e_failure;

        encode_byte_to_lsb(ch, buffer);                       

        //write buffer to stego file
        if(fwrite(buffer,1, 8, encInfo->fptr_stego_image) != 8)
            return e_failure;

        bytes_encoded++;                                         
        
    }
    printf("INFO: Total bytes encoded from secret file: %d\n", bytes_encoded);  
    return e_success; 
}


Status copy_remaining_img_data(FILE *fptr_src ,FILE *fptr_dest)
{
    char ch;

     //read 1 byte from src file
    while(fread(&ch, 1, 1, fptr_src) == 1)    
    {
       
        //write 1 byte to dest file
        if(fwrite(&ch, 1, 1, fptr_dest) != 1)   
            return e_failure;
    }
    return e_success;
}



  