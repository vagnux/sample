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
#include <dirent.h>
#include <errno.h>
#include <mpg123.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "ample.h"

#include "entries.h"

#define INITIAL_FILES 50
#define INITIAL_DIRS 10

mp3entry *root;

// Function to get the current date and time
char *getCurrentDateTime() {
  time_t rawtime;
  struct tm *timeinfo;
  char *buffer = (char *)malloc(sizeof(char) * 20);

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

void getfilesRecursive(char *path, struct config *conf) {
  DIR *dir;
  struct dirent *entry;

  if (!(dir = opendir(path))) {
    perror("opendir");
    return;
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;

      char subdir[1024];
      snprintf(subdir, sizeof(subdir), "%s/%s", path, entry->d_name);

      getfilesRecursive(subdir, conf);
    } else if (entry->d_type == DT_REG) {

      const char *extensao = ".mp3";
      size_t tamanhoExtensao = strlen(extensao);
      size_t tamanhoNome = strlen(entry->d_name);

      if (tamanhoNome > tamanhoExtensao &&
          strcmp(entry->d_name + tamanhoNome - tamanhoExtensao, extensao) ==
              0) {

        char caminhoArquivo[1024];
        snprintf(caminhoArquivo, sizeof(caminhoArquivo), "%s/%s", path,
                 entry->d_name);

        int duracaoSegundos = 0;

        long bytesArquivo = 0;
      }
    }
  }

  closedir(dir);
}

void *getfilesWrapper(void *args) {
  struct config *conf = (struct config *)args;
  char *path = conf->path;

  getfilesRecursive(path, conf);
}

void getfiles(char *path, struct config *conf) {

  int ini = init();
  const char *scriptPath = "/usr/bin/mp3scan 2> /dev/null &";
  int result = system(scriptPath);
}
