#include "configloader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

// Function to load configuration from file
int loadConfig(struct Config *config) {

  const char *filename = "";

  // Specify the path to your config file
  const char *configFileNameLocal = "~/.config/sample/sample.cfg";
  const char *configFileNameRoot = "/etc/sample/sample.cfg";

  // Attempt to open the local config file
  FILE *file = fopen(configFileNameLocal, "r");
  if (file == NULL) {
    printf("Error opening config file %s \n", configFileNameLocal);

    // Attempt to open the root config file if the local one fails
    file = fopen(configFileNameRoot, "r");
    if (file == NULL) {
      printf("Error opening config file %s \n", configFileNameRoot);
      return 1; // Return an error code
    }
  }

  char line[MAX_LINE_LENGTH];
  while (fgets(line, sizeof(line), file) != NULL) {
    // Parse lines with the format key=value
    char *key = strtok(line, "=");
    char *value = strtok(NULL, "\n");

    if (key != NULL && value != NULL) {
      if (strcmp(key, "mysql_host") == 0) {
        strncpy(config->mysql_host, value, sizeof(config->mysql_host));
      } else if (strcmp(key, "mysql_port") == 0) {
        config->mysql_port = atoi(value);
      } else if (strcmp(key, "mysql_user") == 0) {
        strncpy(config->mysql_user, value, sizeof(config->mysql_user));
      } else if (strcmp(key, "mysql_pass") == 0) {
        strncpy(config->mysql_pass, value, sizeof(config->mysql_pass));
     } else if (strcmp(key, "mysql_database") == 0) {
        strncpy(config->mysql_database, value, sizeof(config->mysql_database));
      } else if (strcmp(key, "file_path") == 0) {
        strncpy(config->file_path, value, sizeof(config->file_path));
      }
    }
  }

  fclose(file);
  return 0; // Return success code
}