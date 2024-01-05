/*
 * Ample - An MP3 lender
 *
 * (c) Copyright - David H�rdeman <david@2gen.com> - 2001
 *
 */

/*
 * $Id: ample.c,v 1.11 2001/08/07 07:03:45 alphix Exp $
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
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "ample.h"
#include "configloader.h"
#include "database.h"
#include "entries.h"
#include "options.h"

struct Config myConfig;

/* FIXME: make this nonstatic */
#define MAX_CHILDREN 10

int opt_debug = 1;
volatile int num_children = 0;
struct Music getMusicInfo(void);

static int copydata(struct Music music, int from, int to) {
  char buf[BUFFSIZE];
  int amount;
  int frame = 65536;
  int sent = 0;
  int pkg = 0;
  char *id = music.id;
  float percent;
  int bytes = music.bytes;
  int duration = music.duration;
  int totalBytes = music.bytes;

  if (duration > 0 && totalBytes > 0) {
    frame = (int)(totalBytes / duration) * 10;
  }

  while ((amount = read(from, buf, sizeof(buf))) > 0) {
    pkg++;

    if (write(to, buf, amount) != amount) {
      perror("write");
      close(from);
      return FALSE;
    } else {
      if ((pkg * amount) > frame) {
        sent += 10;
        pkg = 0;
        percent = ((float)sent / music.duration) * 100;
        music.percent = (int)percent;
        updateTrack(music);
        sleep(9);
      }
    }
  }
  music.percent = 100;
  updateTrack(music);
  if (amount < 0)
    perror("read");

  close(from);
  return TRUE;
}

void playfiles(int conn) {
  int file;
  char *id;
  int bytes;
  int duration;
  struct Music musicInfo;
  int errorlevel = 0;
  do {
    musicInfo = getMusicInfo();
    printf("---- opening file %s with duration %d and size %d\n\n\n\n",
           musicInfo.path_file, duration, bytes);
    if ((file = open(musicInfo.path_file, O_RDONLY)) < 0) {
      perror("open");
      printf("---- Panic to open file\n\n\n\n");
      musicInfo = getMusicInfo();
      errorlevel++;
      if (errorlevel > 5)
        break;
    }

  } while (copydata(musicInfo, file, conn));

  close(file);
}

static int openconn(struct sockaddr_in *address, struct config *conf) {
  int i = 1;
  int sock;

  if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    DIE("socket");

  i = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
  address->sin_family = AF_INET;
  address->sin_port = htons(conf->port);
  memset(&address->sin_addr, 0, sizeof(address->sin_addr));

  if (bind(sock, (struct sockaddr *)address, sizeof(struct sockaddr_in)))
    DIE("bind");

  if (listen(sock, 5))
    DIE("listen");

  return sock;
}

int handleclient(int conn, struct config *conf) {
  int i;
  char buffer[BUFFSIZE];

  i = sprintf(buffer, SERVERMSG);
  write(conn, buffer, i);

  if (readrequest(conn, conf)) {
    DEBUG(("---- requested path %s\n", conf->path));

    printf("---- looking for mp3 files in subdirectory %s\n", conf->path);
    getfiles(conf->path, conf);

    IF_DEBUG(printf("---- listing mp3 files\n"); listentries(););

    if (!conf->order) {
      printf("---- shuffling mp3 files\n");
      shuffleentries();
      IF_DEBUG(printf("---- listing mp3 files\n"); listentries(););
    }
    playfiles(conn);
  }

  cleartree(root);
  close(conn);
  return (0);
}

void handlechild(int signal) {
  int status;
  pid_t pid;

  num_children--;
  printf("Number of children %d\n", num_children);
  pid = wait(&status);
}

int main(int argc, char *argv[]) {
  struct sockaddr_in address;
  struct config conf;
  int conn, sock;
  pid_t pid;
  socklen_t addrlen = sizeof(struct sockaddr_in);
  char *cwd;
  struct sigaction sa;
  sigset_t chld;

  strcpy(myConfig.mysql_host, "");
  myConfig.mysql_port = 0;
  strcpy(myConfig.mysql_user, "");
  strcpy(myConfig.mysql_pass, "");
  strcpy(myConfig.file_path, "");

  if ( loadConfig(&myConfig) != 0) {
        return 0;
  }

  root = NULL;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handlechild;
  sigemptyset(&chld);
  sigaddset(&chld, SIGCHLD);
  if (sigaction(SIGCHLD, &sa, NULL))
    perror("sigaction");

  checkopt(argc, argv, &conf);
  chdir(conf.path);
  printf("---- using %s as root for mp3 files\n", conf.path);
  GETCWD(cwd);

  printf("---- opening socket, port %d\n", conf.port);
  sock = openconn(&address, &conf);

  while ((conn = accept(sock, (struct sockaddr *)&address, &addrlen))) {

    if (conn < 0) {
      if (errno == EINTR)
        continue;
      else
        DIE("accept");
    }

    IF_DEBUG(printf("---- incoming connection\n"););
    sigprocmask(SIG_BLOCK, &chld, NULL);
    pid = fork();

    if (pid < 0) {
      DIE("fork");
    } else if (pid > 0) {
      num_children++;
      IF_DEBUG(printf("---- Number of children %d\n", num_children););
      printf("---- connection from %s:%d handled by child with pid %d\n",
             inet_ntoa(address.sin_addr), address.sin_port, pid);
      sigprocmask(SIG_UNBLOCK, &chld, NULL);

      close(conn);
      while (num_children >= MAX_CHILDREN)
        pause();
      continue;
    } else {
      return (handleclient(conn, &conf));
    }
  }

  close(sock);
  free(cwd);
  return 0;
}
