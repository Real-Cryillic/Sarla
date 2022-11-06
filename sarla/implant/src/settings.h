struct job_registration {
    CHAR hostname[MAX_PATH];
    CHAR username[MAX_PATH];
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