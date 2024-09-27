#include <stdlib.h>
#include <stdio.h>
#include <pigpiod_if2.h>
#include <string.h>
#include <inttypes.h>

#include "read.h"
#include "send.h"
#include "text_6bit.h"
#include "text_7bit.h"


int main()
{
    char input[MAX_INPUT_LENGTH + 1];
    printf("Enter text to send: ");
    fgets(input,sizeof(input),stdin);

    size_t input_len = strlen(input)
    if (input[input_len - 1]=='\n'){
        input[input_len - 1] = '\0';
    }
    
    char* binary = text_to_binary7(input);
    printf("Binary %s\n", binary)

    send_bits(binary);
    return 0;
}