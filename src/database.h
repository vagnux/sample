// sqlite.h

#ifndef SQLITE_H
#define SQLITE_H

#include <mysql/mysql.h>

struct Music {
    char id[50];
    long bytes;
    int duration;
    char file_name[100];
    char last_played[20];
    char path_file[100];
    int rank;
    int percent;
};

int init();

int insertMusic(char *musicPath, int duration, char *fileName, int rank, int percent, char *lastPlayed, long bytesArquivo);

struct Music getMusicInfo();

void updateMusic(struct Music music);

struct Music getMusicInfo();

#endif // SQLITE_H
