#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;

    DecodeInfo decInfo;

    //check operation type(encode / decode)
    int ret = check_operation_type(argv);

    //check the condition
    if(ret == e_encode)
    {
        //encoding
        int ret = read_and_validate_encode_args(argv, &encInfo);
        if(ret == e_failure)
        {
            printf("validation is Invalid.\n");
            return 0;
        }
        ret = do_encoding(&encInfo);
        if(ret == e_failure)
        {
            printf("encoding is failed\n");
            return 0;
        }
        printf("encoding is successfull....\n");
        return 0;
    }
    
    else if(ret == e_decode)
    {
        //decoding
        if(read_validate_decode_args(argv, &decInfo) == e_failure)
        {
        printf("validation is failed\n");
        return 0;
        }

        if(do_decoding(&decInfo) == e_failure)
        {
            printf("decoding is failed\n");
            return 0; 
        }
        printf("decoding is successful!!\n");
        return 0;
    }
    else
    {
        //unsupported
        printf("Invalid operation type\n");
        return 0;
    }
}



    
