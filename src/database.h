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



// Função para inicializar o banco de dados SQLite
// Retorna 0 em caso de sucesso, -1 em caso de erro
int init();

// Função para inserir informações sobre uma música no banco de dados
// Parâmetros:
//   - musicPath: caminho do arquivo da música
//   - duration: duração da música em segundos
//   - fileName: nome do arquivo da música
//   - rank: classificação da música
//   - percent: porcentagem de conclusão da música
//   - lastPlayed: data e hora da última reprodução da música
// Retorna 0 em caso de sucesso, -1 em caso de erro

int init();

int insertMusic(char *musicPath, int duration, char *fileName, int rank, int percent, char *lastPlayed, long bytesArquivo);

struct Music getMusicInfo();

void updateMusic(struct Music music);

struct Music getMusicInfo();

#endif // SQLITE_H
