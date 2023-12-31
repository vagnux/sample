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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#include "ample.h"
#include "entries.h"
#include "options.h"
#include "mysql.h"


/* FIXME: make this nonstatic */
#define MAX_CHILDREN 10

int opt_debug = 1;
volatile int num_children = 0;
struct Music getMusicInfo(void);


static int copydata(char *id, int totalBytes, int duration, int from, int to) {
    char buf[BUFFSIZE];
    int amount;
    int frame = 65536;
    int sent = 0;
    int pkg = 0;

    if (duration > 0 && totalBytes > 0) {
        frame = (int)(totalBytes / duration) * 10;
    }

    clock_t start_time, end_time;
    double elapsed_time;

    start_time = clock();
    while ((amount = read(from, buf, sizeof(buf))) > 0) {
        sent = sent + amount;
        pkg++;
        if (write(to, buf, amount) != amount) {
            perror("write");
            close(from);
            return FALSE;
        } else {
            if ((pkg * amount) > frame) {
                pkg = 0;
                sleep(9);
            }
        }
    }
    end_time = clock();
    elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Print the elapsed time
    printf("Time elapsed: %f seconds\n", elapsed_time);
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

    do {
        struct Music musicInfo = getMusicInfo();
        id = musicInfo.id;
        bytes = musicInfo.bytes;
        duration = musicInfo.duration;
        updateTrack(musicInfo);
        printf("---- opening file %s with duration %d and size %d\n\n\n\n",
               musicInfo.path_file, duration, bytes);
        if ((file = open(musicInfo.path_file, O_RDONLY)) < 0) {
            perror("open");
            break;
        }

    } while (copydata(id, bytes, duration, file, conn));

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

	if (bind(sock, (struct sockaddr*) address, sizeof(struct sockaddr_in)))
		DIE("bind");

	if (listen(sock, 5))
		DIE("listen");

	return sock;
}

int handleclient(int conn, struct config *conf) {
	int i;
	char buffer[BUFFSIZE];

	DEBUG(("---- sending headers\n"));
	i = sprintf(buffer, SERVERMSG);
	write(conn, buffer, i);

	DEBUG(("---- reading request\n"));
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

	while ((conn = accept(sock, (struct sockaddr*) &address, &addrlen))) {

		if (conn < 0) {
			if (errno == EINTR)
				continue;
			else
				DIE("accept");
		}

		IF_DEBUG(printf("---- incoming connection\n")
		;
		);
		sigprocmask(SIG_BLOCK, &chld, NULL);
		pid = fork();

		if (pid < 0) {
			DIE("fork");
		} else if (pid > 0) {
			num_children++;
			IF_DEBUG(printf("---- Number of children %d\n", num_children)
			;
			);
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