#include "md5.h"
#include "sqlite.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <unistd.h>
#include "sqlite.h"

int fileExistSqlite(const char *fileName) {
	if (access(fileName, F_OK) != -1) {
		return 1;
	} else {
		return 0;
	}
}

int initSqlite() {
	sqlite3 *db;
	char *error_message = 0;

	// Abre (ou cria) o banco de dados
	int rc = sqlite3_open("playlist.db", &db);

	if (rc) {
		fprintf(stderr, "Não foi possível abrir o banco de dados: %s\n",
				sqlite3_errmsg(db));
		return rc;
	}
	// Cria a musics "users"
	const char *create_table_query = "CREATE TABLE IF NOT EXISTS musics ("
			"id TEXT PRIMARY KEY,"
			"bytes INTEGER,"
			"duration INTEGER,"
			"file_name TEXT,"
			"last_played DATETIME,"
			"path_file TEXT,"
			"rank INTEGER,"
			"percent INTEGER"
			");";

	rc = sqlite3_exec(db, create_table_query, 0, 0, &error_message);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Erro ao criar a tabela: %s\n", error_message);
		sqlite3_free(error_message);
		return rc;
	} else {
		fprintf(stdout, "Tabela 'musics' criada com sucesso.\n");
	}

	sqlite3_close(db);

	return 0;
}

int insertMusicSqlite(char *musicPath, int duration, char *fileName, int rank,
		int percent, char *lastPlayed, long bytesArquivo) {
	if (!fileExistSqlite("playlist.db")) {
		initSqlite();
	}

	sqlite3 *db;
	char *error_message = 0;

	// Abre (ou cria) o banco de dados
	int rc = sqlite3_open("playlist.db", &db);

	if (rc) {
		fprintf(stderr, "Não foi possível abrir o banco de dados: %s\n",
				sqlite3_errmsg(db));
		return rc;
	}



	// Cria a instrução SQL de inserção com parâmetros
	const char *insert_query =
			"INSERT INTO musics (id, path_file, duration, file_name, rank, percent, last_played, bytes) VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

	// Prepara a instrução SQL
	sqlite3_stmt *stmt;
	rc = sqlite3_prepare_v2(db, insert_query, -1, &stmt, 0);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "Erro ao preparar a instrução SQL: %s\n",
				sqlite3_errmsg(db));
		return rc;
	}

	char *id = genHash(musicPath);
	printf("ID %s musica %s.\n", id, musicPath);

	// Atribui os valores aos parâmetros
	sqlite3_bind_text(stmt, 1, id, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, musicPath, -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 3, duration);
	sqlite3_bind_text(stmt, 4, fileName, -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 5, rank);
	sqlite3_bind_int(stmt, 6, percent);
	sqlite3_bind_text(stmt, 7, "1979-08-06 00:00:00", -1, SQLITE_STATIC);
	sqlite3_bind_int(stmt, 8, bytesArquivo);
	free(id);

	// Verifica se a string de data e hora está no formato correto (YYYY-MM-DD HH:mm:ss)
	if (sqlite3_bind_text(stmt, 7, lastPlayed, -1, SQLITE_STATIC)
			!= SQLITE_OK) {
		fprintf(stderr, "Erro ao vincular a string de data e hora.\n");
		return -1;
	}

	// Executa a instrução SQL
	rc = sqlite3_step(stmt);

	if (rc != SQLITE_DONE) {
		fprintf(stderr, "Erro ao executar a instrução SQL: %s\n",
				sqlite3_errmsg(db));
		return rc;
	}

	// Finaliza a instrução SQL
	sqlite3_finalize(stmt);

	// Fecha o banco de dados
	sqlite3_close(db);

	return 0;
}

// Function to execute the SQL query and return the result as a struct
struct Music getMusicInfoSqlite() {
	struct Music music;
	sqlite3 *db;
	char *error_message = 0;

	// Open the database
	int rc = sqlite3_open("playlist.db", &db);

	if (rc) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}

	// Execute the SQL query
	const char *sql_query =
			"SELECT id, bytes, duration, file_name, last_played, path_file, rank, percent FROM musics WHERE (datetime(last_played) <= datetime('now', '-30 minutes')) or (last_played = '0000-00-00 00:00:00') order by percent DESC limit 1;";

	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, sql_query, -1, &stmt, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare the query: %s\n",
				sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}

	// Execute the query and populate the struct
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		strcpy(music.id, (const char*) sqlite3_column_text(stmt, 0));
		music.bytes = sqlite3_column_int64(stmt, 1);
		music.duration = sqlite3_column_int(stmt, 2);
		strcpy(music.file_name, (const char*) sqlite3_column_text(stmt, 3));
		strcpy(music.last_played, (const char*) sqlite3_column_text(stmt, 4));
		strcpy(music.path_file, (const char*) sqlite3_column_text(stmt, 5));
		music.rank = sqlite3_column_int(stmt, 6);
		music.percent = sqlite3_column_int(stmt, 7);
	} else {
		// No result found, set default values
		strcpy(music.last_played, "0000-00-00 00:00:00");
		// Set other default values as needed
	}

	// Finalize the statement and close the database
	sqlite3_finalize(stmt);
	sqlite3_close(db);

	return music;
}

// Function to update the last_played field in the musics table
void updateMusicSqlite(struct Music music) {
	sqlite3 *db;
	char *error_message = 0;

	// Open the database
	int rc = sqlite3_open("playlist.db", &db);

	if (rc) {
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}

	// Execute the update query
	const char *update_query =
			"UPDATE musics SET last_played = ? WHERE path_file = ?;";

	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db, update_query, -1, &stmt, 0) != SQLITE_OK) {
		fprintf(stderr, "Failed to prepare the query: %s\n",
				sqlite3_errmsg(db));
		exit(EXIT_FAILURE);
	}

	// Bind parameters to the update query
	sqlite3_bind_text(stmt, 1, music.last_played, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 2, music.path_file, -1, SQLITE_STATIC);

	// Execute the update query
	if (sqlite3_step(stmt) != SQLITE_DONE) {
		fprintf(stderr, "Failed to execute the update query: %s\n",
				sqlite3_errmsg(db));
		//exit(EXIT_FAILURE);
	} else {
		printf("\nLast Played updated successfully.\n %s  - %s -> %s \n",
				update_query, music.last_played, music.path_file);
	}

	// Finalize the statement and close the database
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}
