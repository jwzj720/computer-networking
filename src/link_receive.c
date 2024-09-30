
#include "link_receive.h"



int read_to_file()
{
    char* result = read_bits(GPIO_SEND, GPIO_RECEIVE);
    printf("Binary received: %s\n", result);
    char* final = binary_to_text7(result);
    printf( "Message Received: %s\n", final); //comment
    free(final);
    free(result);

    return 0;
}
