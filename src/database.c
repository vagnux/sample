#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "md5.h"
#include "mysql.h"

int init() {
     const char *hostname = "mysql";
        const char *username = "default";
        const char *password = "123";
        const char *database = "mp3";


        MYSQL *mysqlConn = openMySQLConnection(hostname, username, password, database);
        return initMysql(mysqlConn);
}

int insertMusic(char *musicPath, int duration, char *fileName, int rank,
                int percent, char *lastPlayed, long bytesArquivo) {

        const char *hostname = "mysql";
        const char *username = "default";
        const char *password = "123";
        const char *database = "mp3";


        MYSQL *mysqlConn = openMySQLConnection(hostname, username, password,
                        database);

        // Perform MySQL operations here...

        return insertMusicMysql(mysqlConn, musicPath, duration, fileName, rank,
                        percent, lastPlayed, bytesArquivo);
        closeMySQLConnection(mysqlConn);
}

void updateMusic(struct Music music) {

        const char *hostname = "mysql";
        const char *username = "default";
        const char *password = "123";
        const char *database = "mp3";

        MYSQL *mysqlConn = openMySQLConnection(hostname, username, password,
                        database);

        return updateMusicMysql(mysqlConn, music);

        closeMySQLConnection(mysqlConn);
}

struct Music getMusicInfo(void) {

        const char *hostname = "mysql";
        const char *username = "default";
        const char *password = "123";
        const char *database = "mp3";


        MYSQL *mysqlConn = openMySQLConnection(hostname, username, password,
                        database);

        return getMusicInfoMysql(mysqlConn);
        closeMySQLConnection(mysqlConn);

}

