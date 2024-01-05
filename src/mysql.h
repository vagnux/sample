// sqlite.h
#include <mysql/mysql.h>

#ifndef MYSQL_H
#define MYSQL_H



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

MYSQL* openMySQLConnection(const char *hostname, const char *username,
		const char *password, const char *database);

void closeMySQLConnection(MYSQL *conn);

int initMysql(MYSQL *mysqlConn);

MYSQL_RES* executeQuery(MYSQL *mysqlConn, const char *query);

int insertMusicMysql(MYSQL *mysqlConn, char *musicPath, int duration,
		char *fileName, int rank, int percent, char *lastPlayed,
		long bytesArquivo);

struct Music getMusicInfoMysql(MYSQL *mysqlConn);

void updateMusicMysql(MYSQL *mysqlConn, struct Music music);

#endif // MYSQL_H
