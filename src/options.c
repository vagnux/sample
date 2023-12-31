/*
 * $Id: options.c,v 1.6 2001/08/09 08:53:25 alphix Exp $
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
#if HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "ample.h"
#include "options.h"


static void usage(int status, struct config * conf)
{
	if(status) {
		fprintf(stderr, "Try `%s --help' for more information.\n", conf->program_name);
		exit(1);
	} else {
		/* This is several printf's to please ISO C89 */
		printf(USAGE1, conf->program_name);
		printf(NOTE);
		printf(USAGE2, DEF_PORT);
		exit(0);
	}
}

/* This function is lame right now, clean up later */
int readrequest (int conn, struct config * conf) {
	ssize_t readsize;
	int begin=0,end;
	char buffer[BUFFSIZE];
	const char *get = "GET ";

        readsize = read(conn,&buffer,BUFFSIZE-1);
	if(readsize == BUFFSIZE-1) {
		printf("---- invalid reply\n");
		return FALSE;
	} else {
		buffer[readsize] = '\0';
		IF_DEBUG(printf("---- reply:\n%s\n",buffer);
			 printf("---- parsing reply\n"););
		
		if(strncasecmp(&(buffer[0]),get,(strlen(get) - 1))) {
			printf("---- weird request, ignoring\n");
			return FALSE;
		}

		begin = end = 4;

		while(buffer[end] != '\r' && buffer[end] != '\0')
			end++;

		if(buffer[end] == '\0' || buffer[end-1] == ' ') {
			printf("---- weird request, ignoring\n");
			return FALSE;
		}
		
		while(buffer[end] != ' ')
			end--;

		if(begin == end) {
			printf("---- weird request, ignoring\n");
			return FALSE;
		}
		
		buffer[end] = '\0';
		if(buffer[begin] == '/' && strlen(&buffer[begin]) > 1)
			conf->path = (char *)strdup(&(buffer[begin+1]));
		else if(buffer[begin] == '/')
			conf->path = ".";
		else
			conf->path = (char *)strdup(&(buffer[begin]));
		
		return TRUE;
	}
}


void checkopt(int argc, char * argv[], struct config * conf)
{
	int c,i;
#if HAVE_GETOPT_H
	static struct option const longopts[] =
	{
		{"port", required_argument, NULL, 'p'},
		{"order", no_argument, NULL, 'o'},
		{"help", no_argument, NULL, 'h'},
	        {"debug", no_argument, NULL, 'd'},
	      	{"version", no_argument, NULL, 'v'},
	       	{NULL, 0, NULL, 0}
        };
#endif
	
	memset(conf, 0, sizeof(struct config));
	conf->program_name = argv[0];
	conf->port = DEF_PORT;
	conf->path = DEF_PATH;
 
#if HAVE_GETOPT_H	
	while((c = getopt_long(argc, argv, "p:ohdv", longopts, &i)) != -1) {
#else
	while((c = getopt(argc, argv, "p:ohdv")) != -1) {
#endif
		switch(c) {
		case 'p':
			if(atoi(optarg) > 0)
				conf->port = atoi(optarg);
			break;
		case 'o':
			conf->order = TRUE;
			break;
		case 'h':
			usage(FALSE, conf);
			break;
		case 'd':
			opt_debug = TRUE;
			break;
		case 'v':
			printf("Ample version %s\n",AMPLE_VERSION);
			exit(0);
		default:
			usage(TRUE, conf);
		}
	}

	if(optind < argc) {
		DEBUG(("---- user given path %s\n",argv[optind]));
		conf->path = argv[optind];
	}
}



