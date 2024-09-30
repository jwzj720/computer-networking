#include "link_send.h"

int send_to_file()
{
    char input[MAX_INPUT_LENGTH + 1];
    printf("Enter text to send: ");
    fgets(input,sizeof(input),stdin);

    size_t input_len = strlen(input);
    if (input[input_len - 1]=='\n'){
        input[input_len - 1] = '\0';
    }
    
    char* binary = text_to_binary7(input);
    printf("Binary %s\n", binary);

    send_bits(binary);
    free(binary);
    return 0;
}
