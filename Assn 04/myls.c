/*
 * Operating Systems, Assn 4
 * File:   myls.c
 * Author: Matthew Morgan
 * Date:   5 April 2018
 * Description:
 * myls is a simulation of the ls -l command, allowing a user to print
 * the contents of a directory on their system, showing various properties
 * of the different files. It also allows a user to print the properties of
 * a single file.
 *
 * Compilation:
 * gcc -std=c99 -o myls myls.c myls.h
 */

// Inclusion statements
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "myls.h"

// MONTH_NAME is used for storing month names
// FILE_NAME_MAX is the maximum number of characters allowed for a file's name
// CUR_YEAR stores the current year of program execution
const char* MONTH_NAME[12] =
  {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const short FILE_NAME_MAX = 256;
int CUR_YEAR;

/* longest stores:
 * [0] : The longest length of the usernames that own the files in the directory
 * [1] : The largest filesize discovered amongst the files of the directory
 * [2] : The longest length of the group names
 * [3] : The largest number of links amongst the files of the directory */
// list is a list of all the files' data that exist in the directory
long long longest[4] = {0, 0, 0, 0};
DFILE* list = NULL;

int main(int argc, const char *argv[]) {
  // int is the number of files there are
  // path is the directory or file being looked at
  // Arr is a reference to the sorted array of file information
  int cnt;
  char* path;
  DFILE** Arr;

  time_t cTime = time(NULL);
  CUR_YEAR = localtime(&cTime)->tm_year;
  
  // Three methods of executing the program:
  // - The user specifies a directory to be scanned
  // - The user specifies a specific file
  // - The user specifies nothing (assume "./")
  if (argc == 2)
    path = (char*)argv[1];
  else if (argc == 1) {
    path = "./";
  }

  cnt = readDirectory(path);
  Arr = fileSort(cnt);
  filePrint(Arr, cnt);
  
  exit(0);
}

/* Reads all of the files of a directory, storing their information into DFILEs
 * @param path The directory to be read
 * @return The number of files counted to be in the directory */
int readDirectory(char* path) {
  // dir stores a reference to the directory stream
  // entry stores a reference to a directory entry
  // cnt is used for counting the number of files in the directory
  // fPath is the filepath used for reading files
  /* newPath is used to append a final slash, if needed, to a dir name */
  DIR *dir = opendir(path);
  DIRENT *entry;
  int cnt=0;
  char* fPath = path, *newPath = NULL;

  if (dir != NULL) {
    // Update the directory string if the final character isn't a
    // final slash
    if (path[strlen(path)-1] != '/') {
      newPath = malloc(sizeof(char)*(strlen(path)+2));
      sprintf(newPath, "%s/", path);
      fPath = newPath;
    }

    // While there are still entries in the directory, read them
    while((entry = readdir(dir)) != NULL) {
      if (verifyFile(entry->d_name)) {
        readFile(fPath, entry->d_name, false);
        cnt++;
      }
    }

    if (newPath != NULL) { free(newPath); }
    closedir(dir);
  }
  else {
    // A file was specified, so we only read it
    readFile(path, "", true);
    cnt++;
  }

  return cnt;
}

/* Reads in a file's information, generating a DFILE structure and
 * appending it to the list of files read.
 * @param dir The directory of the file
 * @param f The name of the file to be read
 * @param useFullPath Whether or not to store the file's full path or not */
void readFile(const char* dir, char* f, bool useFullPath) {
  // file stores information about the file
  // grp and usr store group/user information
  // tmp is a temporary reference to the DFILE generated
  // nLen is temporary storage for lengths of user/group names
  // path is the full pathname to the file
  STAT *file = malloc(sizeof(*file));
  GROUP *grp;
  USER *usr;
  DFILE *tmp;
  int nLen;
  char* path = malloc(sizeof(char)*(1+FILE_NAME_MAX+(int)strlen(dir)));

  // Construct pathname and load in file information
  sprintf(path, "%s%s", dir, f);
  if (stat(path, file) == -1) {
    printf("myls: cannot access '%s': No such file or directory\n", path);
    //printf("ERR: Failure to fetch file data for '%s'!\n", path);
    exit(1);
  }

  // Generate the DFILE structure after fetching group/user info
  grp = getgrgid(file->st_gid);
  usr = getpwuid(file->st_uid);
  tmp = genFile(
    permissions(file->st_mode), usr->pw_name, grp->gr_name,
    filetime(file), (useFullPath ? path : f),
    (int)file->st_nlink, file->st_size
  );

  // Update the DFILE list and deallocate structures
  tmp->next = list;
  list = tmp;
  free(path);
  free(file);

  // Update the longest names and largest links/sizes
  if ((nLen = strlen(list->user)) > (int)longest[0])
    longest[0] = nLen;
  if ((nLen = strlen(list->group)) > (int)longest[2])
    longest[2] = nLen;
  if (list->size > longest[1])
    longest[1] = list->size;
  if (list->links > longest[3])
    longest[3] = list->links;
}

/* Compares two DFILEs and returns which comes first in ascending order.
 * Order is determined by the DFILE names.
 * @param fa A pointer to the first DFILE
 * @param fb A pointer to the second DFILE
 * @return 1 if fa comes first, -1 if fb comes first, or 0 if fa=fb. */
int fileCompare(const void* fa, const void* fb) {
  return strcasecmp((*((DFILE* const*)fa))->name, (*((DFILE* const*)fb))->name);
}

/* Sorts the list of files that was generated by scanning the directory,
 * sorting them by name in ascending order.
 * @param cnt The number of DFILE structures in the list
 * @return An array with the DFILEs sorted by filename */
DFILE** fileSort(int cnt) {
  // Arr is the array where the DFILE structures will be sorted
  // tmp is a counter for placing all the DFILE structs into Arr
  DFILE** Arr = malloc(sizeof(DFILE*)*cnt);
  int tmp = 0;

  for(DFILE* L=list; L != NULL; L=L->next) {
    Arr[tmp] = L;
    tmp++;
  }
  qsort(Arr, cnt, sizeof(DFILE*), fileCompare);

  return Arr;
}

/* Prints all the properties of the various files found in the directory.
 * @param Arr An array of DFILE structures to print information of
 * @param cnt The number of items in Arr */
void filePrint(DFILE* Arr[], int cnt) {
  // format is the final format string that'll be used to print file information
  // spcUser is used for formatting usernames
  // spcGroup is used for formatting group names
  // spcSize is used for formatting file sizes
  // spcLink is used for formatting link counts
  // sizeTmp will be used to write the longest filesize to a string
  // linkTmp will be used to write the largest number of links to a string
  char format[18+longest[0]],
    *spcUser = malloc(sizeof(char)*(longest[0]+4)),
    *spcGroup = malloc(sizeof(char)*(longest[2]+4)),
    *spcSize = malloc(sizeof(char)*25), *sizeTmp = malloc(sizeof(char)*20),
    *spcLink = malloc(sizeof(char)*25), *linkTmp = malloc(sizeof(char)*20);

  // Generate the format for user/group/size before generating final format
  // To generate the size's, first write the largest size to a string such that
  // its length can be computed
  sprintf(sizeTmp, "%lld", longest[1]);
  sprintf(linkTmp, "%lld", longest[3]);
  sprintf(spcUser, "%%-%ds ", (int)longest[0]);
  sprintf(spcGroup, "%%-%ds ", (int)longest[2]);
  sprintf(spcSize, "%%%dlld ", (int)strlen(sizeTmp));
  sprintf(spcLink, "%%%dd ", (int)strlen(linkTmp));
  sprintf(format, "%s%s%s%s%s%s%s", "%s ", spcLink, spcUser, spcGroup, spcSize, "%s ", "%s\n");

  // Iterate through all of the files, printing their properties
  for(int i=0; i<cnt; i++) {
    printf(format, Arr[i]->permissions, Arr[i]->links, Arr[i]->user,
      Arr[i]->group, Arr[i]->size, Arr[i]->timeMod, Arr[i]->name);
  }

  free(spcUser);
  free(spcGroup);
  free(spcSize);
  free(sizeTmp);
  free(spcLink);
  free(linkTmp);
}

/* Verifies if a filename is valid for printing in ls -l simulation.
 * These cases are ignored:
 * - The filename is 0 characters long
 * - The filename being looked at is "." or ".."
 * - The filename begins with a period
 * @param name The name of the file being looked at
 * @return True if the file's data should be listed, or false if it
 *   violates one of the above cases */
bool verifyFile(const char *name) {
  return (strlen(name)>0) &&
    (strcmp(name, ".") != 0) &&
    (strcmp(name, "..") != 0) &&
    (name[0] != '.');
}

/* Generates an 11 character null-terminated string for the permissions of a file
 * based on the mode provided.
 * @param mode The mode of a file, derived from its DIRENT structure
 * @return The file's permissions, as a string */
char* permissions(int mode) {
  char *p = malloc(sizeof(char)*11);

  sprintf(p, "%c%c%c%c%c%c%c%c%c%c",
    S_ISREG(mode) ? '-' : 'd',
    USE_CAN_R(mode) ? 'r' : '-', USE_CAN_W(mode) ? 'w' : '-', USE_CAN_X(mode) ? 'x' : '-',
    GRP_CAN_R(mode) ? 'r' : '-', GRP_CAN_W(mode) ? 'w' : '-', GRP_CAN_X(mode) ? 'x' : '-',
    OTH_CAN_R(mode) ? 'r' : '-', OTH_CAN_W(mode) ? 'w' : '-', OTH_CAN_X(mode) ? 'x' : '-');

  return p;
}

/* Returns the modified time of the file as it would display in ls -l
 * @param file The file's stats
 * @return A string representing the time's modified date */
char* filetime(STAT *file) {
  // fTime is the most recent modification time of the file
  // ftm is the resulting string as it would be printed by ls -l
  // tmp is a temporray buffer
  TIME* fTime = localtime(&file->st_mtime);
  char* ftm = malloc(sizeof(char)*13), tmp[5];

  if (fTime->tm_year < CUR_YEAR)
    sprintf(tmp, "%5d", fTime->tm_year+1900);
  else
    sprintf(tmp, "%02d:%02d", fTime->tm_hour, fTime->tm_min);

  sprintf(ftm, "%3s %2d %5s", MONTH_NAME[fTime->tm_mon], fTime->tm_mday, tmp);

  return ftm;
}

/* Generates a DFILE and returns it. This function assumes that the group and
 * user names are all dynamically allocated, and thus creates copies for
 * local storage in the structure.
 *
 * @param perm The permissions, as a string, for the file
 * @param user The name of the owner
 * @param group The group of the file
 * @param time The time of modification for the file
 * @param name The name of the file itself
 * @param link The number of links to the file (hard links)
 * @param size The size of the file in bytes
 * @return The final DFILE structure's reference */
DFILE* genFile(char* perm, char* user, char* group, char* time, char* name, int link, long long size) {
  // u and g are both used to copy over user and group names to prevent memory corruption
  char *u = malloc(sizeof(char)*strlen(user)),
    *g = malloc(sizeof(char)*strlen(group));
  strcpy(u, user);
  strcpy(g, group);

  // Create the DFILE and add its properties
  DFILE* f = malloc(sizeof(DFILE));
  f->name = malloc(sizeof(char)*(strlen(name)+1));
  strcpy(f->name, name);
  f->permissions = perm;
  f->user = u;
  f->group = g;
  f->timeMod = time;
  f->links = link;
  f->size = size;
  f->next = NULL;

  // Return the pointer to the new DFILE
  return f;
}
