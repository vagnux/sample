#ifndef CONFIG_LOADER_H
#define CONFIG_LOADER_H

#define MAX_LINE_LENGTH 100

// Define a struct to store configuration values
struct Config {
    char mysql_host[20];
    int mysql_port;
    char mysql_user[20];
    char mysql_pass[20];
    char mysql_database[20];
    char file_path[50];
};

// Function prototypes
int loadConfig(struct Config *config);

#endif // CONFIG_LOADER_H