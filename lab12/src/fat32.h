#ifndef FAT32_H
#define FAT32_H
#include <stdint.h>
// FAT32 parameters index
#define CLUSTER_NUMBER_BEGIN    0X0D
#define SECTOR_SIZE_BEGIN       0X0B
#define RES_SECOTR_SIZE_BEGIN   0X0E
#define FAT_NUMBER_BEGIN        0X10
#define FAT_SIZE_BEGIN          0X24

// File properties
#define READ_WRITE              0X0
#define READ_ONLY               0X1
#define HIDDEN                  0X2
#define SYSTEM                  0X4
#define VOLUM                   0X8
#define CHILD_DIR               0X10
#define GUI_DANG                0X20

// FAT table magic number
#define END_OF_FILE             0X0FFFFFFF

//File entry magic number
#define FILE_ENTRY_SIZE         0X20
#define SHORT_FILE_NAME         0X00
#define SHORT_FILE_NAME_SIZE    0X0B
#define LONG_FILE_NAME_SIZE     0X1A / 2
#define LONG_NAME_OFFSET1       0X01
#define LONG_NAME_SIZE1         0X0A / 2
#define LONG_NAME_OFFSET2       0X0E
#define LONG_NAME_SIZE2         0X0C / 2
#define LONG_NAME_OFFSET3       0X1C
#define LONG_NAME_SIZE3         0X04 / 2
#define FILE_PROPERTIES         0X0B
#define LOW_CLUSTER_ADDRESS     0X1A
#define HIGH_CLUSTER_ADDRESS    0X14
#define FILE_LENGTH             0X1C
#define LONG_ENTRY              0X0F

#endif
