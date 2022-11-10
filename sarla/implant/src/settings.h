struct job_registration {
    CHAR hostname[256];
    CHAR username[256];
    DWORD process_id;
    DWORD version;
}job;

struct agent_information {
    CHAR *address;
    CHAR *user_agent;
    INT port;
    CHAR *path;
    CHAR *key;
    CHAR *identifier;
}agent;