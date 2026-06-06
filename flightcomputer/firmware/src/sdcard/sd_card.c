#include "sd_card.h"

#include "datastructures.h"
#include "peripheral/port/plib_port.h"

#define SD_CARD_TESTING 0
#define MAX_TRIES 10

/*

 The SD-Card translation unit handles the core operations needed for direct interaction between the SD-Card and the MicroController.
 It has functions for mounting, unmounting, opening a file, reading from a file and writing to a file. It has flexible debugging, using
 the LEDs to indicate a failure. To toggle it, adjust the #define at the top of the translation unit.

 All functions returns -1 for failure, and most often 1 for success. The write/read functions returns the amount of bytes written/read.

 All functions rely on MPLABs SYS_FS Functions, that has provided an abstraction layer right above the driver layer. If you look at MCC,
 you can see the options for the file system, and additional settings for the SD-Card. Note that we have ENABLED AUTO MOUNTING, so DO NOT
 mount manually.

*/

// Warning! do not use this function when using automounting through MPLab, which COMPASS does.
// Attempts to mount SD-Card. Returns -1 on failure, 1 on success.
// If in Debug/Testing mode, turns on blue GREEN LED for success(BATTERY_LED), red for failure/error
int sdMount() {
    int mountResult = 0;

    for (int tries = 0; tries < MAX_TRIES; tries++) {
        if (SYS_FS_Mount("/dev/mmcblka1", "/mnt/myDrive1", FAT, 0, NULL) == SYS_FS_RES_SUCCESS) {
            mountResult = 1;
            break;
        }
    }

    if (mountResult == 1 && SD_CARD_TESTING) {
        BATT_LED_Set();
    }

    if (mountResult == 0 && SD_CARD_TESTING) {
        ERR_LED_Set();
    }

    return mountResult;  // Return the result (1 for success, 0 for fai
}

// Function takes in a filehandle genereated by SYS, and attempts to open the file. Tries MAX_TRIES times.
//  If in Debug/Testing mode, turns on blue GREEN LED for success(BATTERY_LED), red for failure/error
int sdFileOpen(SYS_FS_HANDLE* fileHandle, const char* name, int read, int write) {
    // We can only be in either read or write mode, and naturally it does not make sense to be in neither
    if (write && read) return -1;
    if (!write && !read) return -1;

    int fileOpenResult = -1;

    // SYS_FS_CurrentDriveSet("/mnt/myDrive1");

    if (read) {
        for (int tries = 0; tries < MAX_TRIES; tries++) {
            *fileHandle = SYS_FS_FileOpen(name, SYS_FS_FILE_OPEN_READ_PLUS);
            if (*fileHandle != SYS_FS_HANDLE_INVALID) {
                fileOpenResult = 1;
                return fileOpenResult;
            }
        }
    }

    if (write) {
        for (int tries = 0; tries < MAX_TRIES; tries++) {
            *fileHandle = SYS_FS_FileOpen(name, SYS_FS_FILE_OPEN_APPEND_PLUS);  // Append to not overwrite old info
            if (*fileHandle != SYS_FS_HANDLE_INVALID) {
                fileOpenResult = 1;
                return fileOpenResult;
            }
        }
    }

    if (fileOpenResult == 1 && SD_CARD_TESTING) {
        BATT_LED_Set();
    }

    if (fileOpenResult == 0 && SD_CARD_TESTING) {
        ERR_LED_Set();
    }

    return fileOpenResult;
}

// Closes file, returns -1 on failure, 1 on success.
int sdFileClose(SYS_FS_HANDLE* fileHandle) {
    int closeResult = -1;
    if (*fileHandle == SYS_FS_HANDLE_INVALID) return -1;

    for (int tries = 0; tries < MAX_TRIES; tries++) {
        if (SYS_FS_FileClose(*fileHandle) == SYS_FS_RES_SUCCESS) {
            closeResult = 1;
            break;
        }
    }

    if (closeResult == 1 && SD_CARD_TESTING) {
        BATT_LED_Set();
    }

    if (closeResult == 0 && SD_CARD_TESTING) {
        ERR_LED_Set();
    }
    return closeResult;
}

// Return 1 for successfull full write, -1 if none are written. It takes in a filehandle, a void pointer to the data and the size of the data in bytes.
// Important to take note that you pass in the name of the file you want to open, no need to open the file before you call this function
int sdWrite(SYS_FS_HANDLE* fileHandle, const char* name, void* data, int sizeOfData) {
    // if (*fileHandle == SYS_FS_HANDLE_INVALID)
    // {
    //   if (sdFileOpen(fileHandle, name, 0, 1) != 1)
    //   {
    //      BATT_LED_Set();
    //     if (SD_CARD_TESTING) ERR_LED_Set();
    //     return -1;
    //   }
    // }

    size_t bytesWritten = SYS_FS_FileWrite(*fileHandle, data, sizeOfData);

    if (bytesWritten < sizeOfData) {
        if (SD_CARD_TESTING) ERR_LED_Set();
        return -1;
    }
    return 1;
}

int sdFlush(SYS_FS_HANDLE* fileHandle) {
    int success = -1;

    for (int tries = 0; tries < MAX_TRIES; tries++) {
        if (SYS_FS_FileSync(*fileHandle) == SYS_FS_RES_SUCCESS) {
            return 1;
        }
    }

    if (SD_CARD_TESTING && (success != 1)) {
        ERR_LED_Set();
    }

    return success;
}

int sdReadSensorDataFromCard(SYS_FS_HANDLE* fileHandle, const char* name, SensorData* data) {
    SensorData tempData;

    if (sdFileOpen(fileHandle, name, 1, 0) != 1) return -1;

    size_t bytesRead;

    for (int tries = 0; tries < MAX_TRIES; tries++) {
        bytesRead = SYS_FS_FileRead(*fileHandle, &tempData, sizeof(*data));
        if (bytesRead == sizeof(*data)) {
            *data = tempData;
            if (SD_CARD_TESTING) BATT_LED_Set();
            return 1;
        }
    }

    if (SD_CARD_TESTING) ERR_LED_Set();
    return -1;
}

int sdUnmount() {
    int success = -1;

    for (int tries = 0; tries < MAX_TRIES; tries++) {
        if (SYS_FS_Unmount("/mnt/myDrive1") == SYS_FS_RES_SUCCESS) {
            success = 1;
            break;
        }
    }

    if (SD_CARD_TESTING && (success != 1)) {
        ERR_LED_Set();
    }

    return success;
}
