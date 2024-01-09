#include "configloader.h"
#include "mysql.h"
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern struct Config myConfig;

int init() {
  const char *hostname = myConfig.mysql_host;
  const char *username = myConfig.mysql_user;
  const char *password = myConfig.mysql_pass;
  const char *database = myConfig.mysql_database;

  MYSQL *mysqlConn =
      openMySQLConnection(hostname, username, password, database);
  return initMysql(mysqlConn);
}

int insertMusic(char *musicPath, int duration, char *fileName, int rank,
                int percent, char *lastPlayed, long bytesArquivo) {

  const char *hostname = myConfig.mysql_host;
  const char *username = myConfig.mysql_user;
  const char *password = myConfig.mysql_pass;
  const char *database = myConfig.mysql_database;

  MYSQL *mysqlConn =
      openMySQLConnection(hostname, username, password, database);

  
  return insertMusicMysql(mysqlConn, musicPath, duration, fileName, rank,
                          percent, lastPlayed, bytesArquivo);
  closeMySQLConnection(mysqlConn);
}

void updateMusic(struct Music music) {

  const char *hostname = myConfig.mysql_host;
  const char *username = myConfig.mysql_user;
  const char *password = myConfig.mysql_pass;
  const char *database = myConfig.mysql_database;

  MYSQL *mysqlConn =
      openMySQLConnection(hostname, username, password, database);

  return updateMusicMysql(mysqlConn, music);

  closeMySQLConnection(mysqlConn);
}

struct Music getMusicInfo(void) {

  const char *hostname = myConfig.mysql_host;
  const char *username = myConfig.mysql_user;
  const char *password = myConfig.mysql_pass;
  const char *database = myConfig.mysql_database;

  MYSQL *mysqlConn =
      openMySQLConnection(hostname, username, password, database);

  return getMusicInfoMysql(mysqlConn);
  closeMySQLConnection(mysqlConn);
}
