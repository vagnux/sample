#define DEF_PORT 1234
#define DEF_PATH "mp3"

extern int opt_debug;

extern void checkopt(int argc, char * argv[], struct config * conf);

extern int readrequest (int conn, struct config * conf);


#define	USAGE1 "Usage: %s [OPTION]... [PATH]\n\
\n\
AMPLE - An MP3 LEnder\n\
recursively indexes all MP3 files it finds below PATH and then listens\n\
to a TCP port for incoming connections. Once a connection is made AMPS will\n\
start sending randomly chosen MP3's. Currently AMPS doesn't care which\n\
arguments the client send. This will change in a future version\n\
(allowing a client to choose a subpath below PATH and requiring\n\
authentication for instance).\n\
\n"

#define NOTE "\
NOTE: For security reasons, AMPS changes it's working dir to the PATH given and\n\
      won't follow symlinks once this has been done. It does however send any\n\
      file with a size > 0 ending in .mp3 so don't rename any funny files\n\
      to anything.mp3\n\
\n"

#if HAVE_GETOPT_H
#define USAGE2 "\
  -p, --port=NUMBER           which port to listen to, default %d\n\
  -o, --order                 play MP3 files in alphabetical order\n\
  -h, --help                  display this help and exit\n\
  -d, --debug                 debug messages will be printed\n\
  -v, --version               output version information and exit\n\
\n\
Report bugs to <david@2gen.com>\n"
#else
#define USAGE2 "\
  -p=NUMBER                   which port to listen to, default %d\n\
  -o                          play MP3 files in alphabetical order\n\
  -h                          display this help and exit\n\
  -d                          debug messages will be printed\n\
  -v                          output version information and exit\n\
\n\
Report bugs to <david@2gen.com>\n"
#endif
