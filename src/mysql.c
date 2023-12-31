#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>
#include "mysql.h"
#include "md5.h"

MYSQL* openMySQLConnection(const char *hostname, const char *username,
                           const char *password, const char *database) {
    MYSQL *conn = mysql_init(NULL);

    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return NULL;
    }

    if (mysql_real_connect(conn, hostname, username, password, database, 0,
                           NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    // Optionally, you can check the server version
    printf("Connected to MySQL server (version %s)\n", mysql_get_server_info(conn));

    return conn;
}


void closeMySQLConnection(MYSQL *conn) {
	if (conn != NULL) {
		mysql_close(conn);
		printf("Connection closed\n");
	}
}

int initMysql(MYSQL *mysqlConn) {
	const char *create_table_query = "CREATE TABLE IF NOT EXISTS musics ("
			"id varchar(255) NOT NULL,"
			"bitrate int(11) DEFAULT NULL,"
			"duration int(11) DEFAULT NULL,"
			"bytes int(11) DEFAULT NULL,"
			"file_name varchar(255) DEFAULT NULL,"
			"last_played datetime(6) DEFAULT NULL,"
			"path_file varchar(255) DEFAULT NULL,"
			"points int(11) DEFAULT NULL,"
			"percent int(11) DEFAULT NULL,"
			"PRIMARY KEY (id)"
			") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;";

	// Execute the SQL query
	if (executeQuery(mysqlConn, create_table_query) != 0) {
		fprintf(stderr, "Error creating table: %s\n", mysql_error(mysqlConn));
		return -1;
	} else {
		fprintf(stdout, "Table 'musics' created successfully.\n");
	}

	return 0;
}

MYSQL_RES* executeQuery(MYSQL *mysqlConn, const char *query) {
	if (mysql_query(mysqlConn, query) != 0) {
		fprintf(stderr, "Query execution failed: %s\n", mysql_error(mysqlConn));
		return NULL;
	}

	MYSQL_RES *result = mysql_store_result(mysqlConn);

	if (result == NULL) {
		fprintf(stderr, "Result set retrieval failed: %s\n",
				mysql_error(mysqlConn));
		return NULL;
	}

	return result;
}

int insertMusicMysql(MYSQL *mysqlConn, char *musicPath, int duration,
		char *fileName, int rank, int percent, char *lastPlayed,
		long bytesArquivo) {
	const char *insert_query =
			"INSERT INTO musics (id, path_file, duration, file_name, points, percent, last_played, bytes) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

	// Prepare the statement
	MYSQL_STMT *stmt = mysql_stmt_init(mysqlConn);
	if (stmt == NULL) {
		fprintf(stderr, "mysql_stmt_init() failed\n");
		return -1;
	}

	if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query)) != 0) {
		fprintf(stderr, "Error preparing SQL statement: %s\n",
				mysql_stmt_error(stmt));
		mysql_stmt_close(stmt);
		return -1;
	}

	char *id = genHash(musicPath);
	printf("ID %s musica %s.\n", id, musicPath);

	MYSQL_BIND bind_params[8];
	memset(bind_params, 0, sizeof(bind_params));

	// Bind parameter values
	bind_params[0].buffer_type = MYSQL_TYPE_STRING;
	bind_params[0].buffer = id;
	bind_params[0].buffer_length = strlen(id);

	// Repeat this for the other parameters...

	// Bind the parameters to the statement
	if (mysql_stmt_bind_param(stmt, bind_params) != 0) {
		fprintf(stderr, "Error binding parameters: %s\n",
				mysql_stmt_error(stmt));
		free(id);
		mysql_stmt_close(stmt);
		return -1;
	}

	// Execute the statement
	if (mysql_stmt_execute(stmt) != 0) {
		fprintf(stderr, "Error executing SQL statement: %s\n",
				mysql_stmt_error(stmt));
		free(id);
		mysql_stmt_close(stmt);
		return -1;
	}

	// Finalize the statement
	mysql_stmt_close(stmt);
	free(id);

	return 0;
}

struct Music getMusicInfoMysql(MYSQL *mysqlConn) {
	struct Music music;

	// Execute the SQL query
	const char *sql_query = "SELECT  id, bytes, duration, file_name, last_played, path_file, points, percent FROM musics ORDER BY RAND() LIMIT 1;";

	// Prepare the statement
	MYSQL_STMT *stmt = mysql_stmt_init(mysqlConn);
	if (stmt == NULL) {
		fprintf(stderr, "mysql_stmt_init() failed\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_stmt_prepare(stmt, sql_query, strlen(sql_query)) != 0) {
		fprintf(stderr, "Failed to prepare the query: %s\n",
				mysql_stmt_error(stmt));
		exit(EXIT_FAILURE);
	}

	// Execute the query and populate the struct
	if (mysql_stmt_execute(stmt) == 0) {
		MYSQL_BIND bind_result[8];
		memset(bind_result, 0, sizeof(bind_result));

		bind_result[0].buffer_type = MYSQL_TYPE_STRING;
		bind_result[0].buffer = music.id;
		bind_result[0].buffer_length = sizeof(music.id) - 1;

		bind_result[1].buffer_type = MYSQL_TYPE_LONGLONG;
		bind_result[1].buffer = &(music.bytes);

		bind_result[2].buffer_type = MYSQL_TYPE_LONG;
		bind_result[2].buffer = &(music.duration);

		bind_result[3].buffer_type = MYSQL_TYPE_STRING;
		bind_result[3].buffer = music.file_name;
		bind_result[3].buffer_length = sizeof(music.file_name) - 1;

		bind_result[4].buffer_type = MYSQL_TYPE_STRING;
		bind_result[4].buffer = music.last_played;
		bind_result[4].buffer_length = sizeof(music.last_played) - 1;

		bind_result[5].buffer_type = MYSQL_TYPE_STRING;
		bind_result[5].buffer = music.path_file;
		bind_result[5].buffer_length = sizeof(music.path_file) - 1;

		bind_result[6].buffer_type = MYSQL_TYPE_LONG;
		bind_result[6].buffer = &(music.rank);

		bind_result[7].buffer_type = MYSQL_TYPE_LONG;
		bind_result[7].buffer = &(music.percent);

		if (mysql_stmt_bind_result(stmt, bind_result) != 0) {
			fprintf(stderr, "Error binding result: %s\n",
					mysql_stmt_error(stmt));
			exit(EXIT_FAILURE);
		}

		if (mysql_stmt_fetch(stmt) != MYSQL_NO_DATA) {
			// Data fetched successfully
		} else {
			// No result found, set default values
			strcpy(music.last_played, "0000-00-00 00:00:00");
			// Set other default values as needed
		}
	} else {
		fprintf(stderr, "Error executing SQL statement: %s\n",
				mysql_stmt_error(stmt));
		exit(EXIT_FAILURE);
	}

	// Finalize the statement
	mysql_stmt_close(stmt);

	return music;
}

void updateMusicMysql(MYSQL *mysqlConn, struct Music music) {
	// Execute the update query
	const char *update_query =
			"UPDATE musics SET last_played = ? WHERE path_file = ?;";

	// Prepare the statement
	MYSQL_STMT *stmt = mysql_stmt_init(mysqlConn);
	if (stmt == NULL) {
		fprintf(stderr, "mysql_stmt_init() failed\n");
		exit(EXIT_FAILURE);
	}

	if (mysql_stmt_prepare(stmt, update_query, strlen(update_query)) != 0) {
		fprintf(stderr, "Failed to prepare the query: %s\n",
				mysql_stmt_error(stmt));
		exit(EXIT_FAILURE);
	}

	// Bind parameters to the update query
	MYSQL_BIND bind_params[2];
	memset(bind_params, 0, sizeof(bind_params));

	bind_params[0].buffer_type = MYSQL_TYPE_STRING;
	bind_params[0].buffer = music.last_played;
	bind_params[0].buffer_length = strlen(music.last_played);

	bind_params[1].buffer_type = MYSQL_TYPE_STRING;
	bind_params[1].buffer = music.path_file;
	bind_params[1].buffer_length = strlen(music.path_file);

	if (mysql_stmt_bind_param(stmt, bind_params) != 0) {
		fprintf(stderr, "Error binding parameters: %s\n",
				mysql_stmt_error(stmt));
		exit(EXIT_FAILURE);
	}

	// Execute the update query
	if (mysql_stmt_execute(stmt) != 0) {
		fprintf(stderr, "Failed to execute the update query: %s\n",
				mysql_stmt_error(stmt));
	} else {
		printf("\nLast Played updated successfully.\n %s  - %s -> %s \n",
				update_query, music.last_played, music.path_file);
	}

	// Finalize the statement
	mysql_stmt_close(stmt);
}
