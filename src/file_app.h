// FileTransferApp.h

#ifndef FILE_APP_H
#define FILE_APP_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hamming.h"
#include "build_packet.h"
#include "send.h"
#include "read.h"

#define MAX_PAYLOAD_SIZE 40  

typedef struct {
    int pinit;
    uint8_t device_addr;
    uint8_t receiver_addr;
    int out_pin;
    int in_pin;
} FileTransferConfig;

typedef struct {
    FileTransferConfig config;
    // Additional fields if needed
} FileTransferApp;

void FileTransferApp_init(FileTransferApp* app, FileTransferConfig config);
void FileTransferApp_sendFile(FileTransferApp* app);
void FileTransferApp_receivePacket(FileTransferApp* app, uint8_t* packet_data, size_t packet_len);

#endif // FILE_TRANSFER_APP_H
