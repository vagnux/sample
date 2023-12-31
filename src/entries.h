#ifndef PATH_MAX
#  if defined (_POSIX_PATH_MAX)
#    define PATH_MAX _POSIX_PATH_MAX
#  elif defined (MAXPATHLEN)
#    define PATH_MAX MAXPATHLEN
#  elif defined (_PC_PATH_MAX)
#    define PATH_MAX _PC_PATH_MAX
#  else
#    define PATH_MAX 1024
#  endif
#endif
#include "sqlite.h"

struct node {
	char * path;
	struct node * next;
};

typedef struct node mp3entry;

/* Current entry in our circular linked list */
extern mp3entry * root;

/* Adds an entry to the end of the list pointed to by rootentry */
extern void addentry(mp3entry ** rootentry, mp3entry * toadd);

/* Removes an entry from the list pointed to by rootentry */
extern void removeentry(mp3entry ** rootentry, mp3entry * toremove);

/* Counts the entries in the list pointed to by rootentry */
extern int countentries(mp3entry * rootentry);

/* Shuffles the entries */
extern void shuffleentries(void);

/* Lists all entries to stdout */
extern void listentries(void);

/* Recursively finds all MP3's below path and adds them to list */
extern void getfiles(char * path, struct config *conf);

/* Clears and frees a tree of MP3 files FIXME: should be clearlist */
extern void cleartree(mp3entry *rootentry);


void updateTrack(struct Music music);
