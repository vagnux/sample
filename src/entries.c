/*
 * $Id: entries.c,v 1.10 2001/08/07 07:03:45 alphix Exp $
 *
 * This file is part of Ample.
 *
 * Ample is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Ample is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif
#include "database.h"
#include <stdio.h>
#include <mpg123.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#if HAVE_LIMITS_H
#include <limits.h>
#include <pthread.h>
#endif

#include "ample.h"

#include "entries.h"

#define INITIAL_FILES 50
#define INITIAL_DIRS 10

mp3entry *root;

void addentry(mp3entry **rootentry, mp3entry *toadd) {
	mp3entry *tmp = NULL;
	DEBUG(("Inserindo no nó %s \n", toadd->path));
	if (*rootentry == NULL) {
		toadd->next = toadd;
		*rootentry = toadd;
	} else {
		tmp = *rootentry;
		while (tmp->next != *rootentry)
			tmp = tmp->next;
		tmp->next = toadd;
		toadd->next = *rootentry;
	}
}

void removeentry(mp3entry **rootentry, mp3entry *toremove) {
	mp3entry *tmp = *rootentry;

	if (*rootentry == NULL)
		return;

	if (toremove->next == toremove) {
		if (*rootentry == toremove)
			*rootentry = NULL;
		else
			DIE("Inconsistent linked list");

	} else {
		while (tmp->next != toremove)
			tmp = tmp->next;
		tmp->next = toremove->next;
		if (*rootentry == toremove)
			*rootentry = toremove->next;
	}
}

void cleartree(mp3entry *rootentry) {
	mp3entry *tmp;

	while (rootentry != NULL) {
		tmp = rootentry;
		removeentry(&rootentry, rootentry);
		free(tmp->path);
		free(tmp);
	}
}

int countentries(mp3entry *rootentry) {
	mp3entry *tmp = rootentry;
	int i = 0;

	if (tmp != NULL)
		do {
			i++;
			tmp = tmp->next;
		} while (tmp != rootentry);

	return i;
}

void shuffleentries(void) {

}

void listentries(void) {
	int i = 0;
	mp3entry *tmp = root;

	if (tmp == NULL)
		return;

	do {
		i++;
		//DEBUG(("%d - \"%s\"\n",i,tmp->path));
		tmp = tmp->next;
	} while (tmp != root);
}

int psort(const void *arg1, const void *arg2) {
	return strcmp(*(char* const*) arg1, *(char* const*) arg2);
}

void resizearray(char ***array, int *size, int *maxsize) {
	if (*size + 1 == *maxsize) {
		*maxsize = *maxsize * 2;
		*array = realloc((char*) (*array), *maxsize * sizeof(char*));
		if (*array == NULL)
			DIE("realloc");
	}
}

// Function to get the current date and time
char* getCurrentDateTime() {
	time_t rawtime;
	struct tm *timeinfo;
	char *buffer = (char*) malloc(sizeof(char) * 20);

	if (buffer == NULL) {
		fprintf(stderr, "Memory allocation error.\n");
		exit(EXIT_FAILURE);
	}

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 20, "%Y-%m-%d %H:%M:%S", timeinfo);

	return buffer;
}

void updateTrack(struct Music music) {
	char *dateHour = getCurrentDateTime();
	strncpy(music.last_played, dateHour, sizeof(music.last_played) - 1);
	music.last_played[sizeof(music.last_played) - 1] = '\0';
	free(dateHour);
	updateMusic(music);
}

long obterTamanhoArquivo(const char *caminhoArquivo) {
	FILE *arquivo = fopen(caminhoArquivo, "rb");
	if (arquivo == NULL) {
		return -1;
	}

	fseek(arquivo, 0, SEEK_END);
	long tamanho = ftell(arquivo);
	fclose(arquivo);

	return tamanho;
}

int obterDuracaoMP3(const char *filePath) {

	mpg123_handle *mh;
	mpg123_init();
	mh = mpg123_new(NULL, NULL);

	if (mpg123_open(mh, filePath) != MPG123_OK) {
		fprintf(stderr, "Failed to open the MP3 file: %s\n",
				mpg123_strerror(mh));
		mpg123_delete(mh);
		mpg123_exit();
		return -1;  // Return -1 to indicate an error
	}

	off_t *offsets;
	off_t step;
	size_t fill;

	// Get frame indexes
	if (mpg123_index(mh, &offsets, &step, &fill) < 0) {
		fprintf(stderr, "Failed to get frame indexes: %s\n",
				mpg123_strerror(mh));
		mpg123_close(mh);
		mpg123_delete(mh);
		mpg123_exit();
		return -1;  // Return -1 to indicate an error
	}

	// Calculate duration
	off_t firstFrameOffset = offsets[0];
	off_t lastFrameOffset = offsets[fill - 1];
	double averageBitrate = (step * 8) / mpg123_tpf(mh);  // bits per second
	double duration = (lastFrameOffset - firstFrameOffset) / averageBitrate;

	if (duration < 0) {
		duration = duration * -1;
	}

	// Free the allocated memory for frame indexes
	//mpg123_free_index(offsets);

	// Cleanup
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();

	return (int) duration;  // Convert duration to integer seconds
}

void getfilesRecursive(char *path, struct config *conf) {
	DIR *dir;
	struct dirent *entry;

	if (!(dir = opendir(path))) {
		perror("opendir");
		return;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_DIR) {
			if (strcmp(entry->d_name, ".") == 0
					|| strcmp(entry->d_name, "..") == 0)
				continue;

			// Construir o caminho completo do subdiretório
			char subdir[1024];
			snprintf(subdir, sizeof(subdir), "%s/%s", path, entry->d_name);

			// Chamar recursivamente para o subdiretório
			getfilesRecursive(subdir, conf);
		} else if (entry->d_type == DT_REG) {
			// Verificar se o arquivo possui extensão ".mp3"
			const char *extensao = ".mp3";
			size_t tamanhoExtensao = strlen(extensao);
			size_t tamanhoNome = strlen(entry->d_name);

			if (tamanhoNome > tamanhoExtensao
					&& strcmp(entry->d_name + tamanhoNome - tamanhoExtensao,
							extensao) == 0) {
				// Construir o caminho completo do arquivo
				char caminhoArquivo[1024];
				snprintf(caminhoArquivo, sizeof(caminhoArquivo), "%s/%s", path,
						entry->d_name);

				// Obter a duração do arquivo MP3
				//	int duracaoSegundos = obterDuracaoMP3(caminhoArquivo);
				int duracaoSegundos = 0;
				//long bytesArquivo = obterTamanhoArquivo(caminhoArquivo);
				long bytesArquivo = 0;

				//insertMusic(char *musicPath, int duration, char *fileName, int rank, int percent, char *lastPlayed)
				//insertMusic(caminhoArquivo, duracaoSegundos, caminhoArquivo, 0, 1, "0000-00-00 00:00:00", bytesArquivo);

			}
		}
	}

	closedir(dir);
}

void* getfilesWrapper(void *args) {
	struct config *conf = (struct config*) args;
	char *path = conf->path;

	getfilesRecursive(path, conf);

	pthread_exit(NULL);
}

void getfiles(char *path, struct config *conf) {


        int ini = init();

	// Substitua "seu_script.sh" pelo caminho e nome do seu script
	const char *scriptPath = "/source/mp3scan 2> /dev/null &";

	// Executa o script
	int result = system(scriptPath);

	if (result == 0) {
		printf("Script executado com sucesso.\n");
	} else {
		printf("Erro ao executar o script.\n");
	}

	//getfilesRecursive(path, conf);
}

