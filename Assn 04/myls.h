/*
 * File:    myls.h
 * Author:  Matthew Morgan
 * Date:    6 April 2018
 * Description:
 * This file contains macros, inclusion statements, a structure definition,
 * and function prototypes for definition in the myls.c file. They help in
 * determining file permissions and storing file information.
 */

#ifndef MYLS
#define MYLS

  // Inclusion statements
  #include <sys/stat.h>
  #include <sys/types.h>
  #include <time.h>
  #include <dirent.h>
  #include <grp.h>
  #include <pwd.h>
  #include <stdbool.h>

  // Macros
  #define USE_CAN_R(m) (m & S_IRUSR) // User RWX permissions
  #define USE_CAN_W(m) (m & S_IWUSR)
  #define USE_CAN_X(m) (m & S_IXUSR)
  #define GRP_CAN_R(m) (m & S_IRGRP) // Group RWX permissions
  #define GRP_CAN_W(m) (m & S_IWGRP)
  #define GRP_CAN_X(m) (m & S_IXGRP)
  #define OTH_CAN_R(m) (m & S_IROTH) // Other user RWX permissions
  #define OTH_CAN_W(m) (m & S_IWOTH)
  #define OTH_CAN_X(m) (m & S_IXOTH)

  /*
   * DFILE is a structure that stores information about a single file
   * in the directory. It contains variables for the permissions, user
   * group and filenames, modification date, hard links, and file size,
   * as well as a pointer to the next DFILE in the list.
   */
  struct dfile {
    char *permissions, *timeMod,
         *user, *group, *name;
    long long size, links;
    struct dfile *next;
  } dfile;

  // Type definitions
  typedef struct stat   STAT;
  typedef struct dirent DIRENT;
  typedef struct tm     TIME;
  typedef struct group  GROUP;
  typedef struct passwd USER;
  typedef struct dfile  DFILE;

  // Function prototypes
  int     readDirectory(char* path);
  void    readFile(const char* dir, char* f, bool useFullPath);
  void    filePrint(DFILE* Arr[], int cnt);
  DFILE** fileSort(int cnt);
  int     fileCompare(const void* fa, const void* fb);
  bool    verifyFile(const char* name);
  char*   filetime(STAT* file);
  char*   permissions(int mode);
  DFILE*  genFile(char* perm, char* user, char* group, char* time, char* name,
    int link, long long size);

#endif
