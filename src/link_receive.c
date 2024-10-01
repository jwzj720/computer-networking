#include "link_receive.h"
#include "hamming.h"

int read_to_file()
{
    char* result = read_bits(GPIO_SEND, GPIO_RECEIVE);
    printf("Binary received: %s\n", result);
    char* hamming_decode = hamming_decode_full(result);
    char* final = binary_to_text7(hamming_decode);
    printf( "Results: %s\n", final);
    return 0;
}
