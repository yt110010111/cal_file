#include "apue.h"
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>		
/* function type that is called for each filename */
typedef int Myfunc(const char *, const struct stat *, int);
static Myfunc myfunc;
static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);
static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;
          /*regular file block special char apecial socket total*/
off_t totalsize=0;
int vl=0,ivl=0;
int main(int argc, char *argv[])
{       
	    
		int ret;
		if (argc != 2)
				err_quit("usage: ftw <starting-pathname>");
		ret = myftw(argv[1], myfunc); /* does it all */
		ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
		

        /*
		struct stat sb;
		if (stat(filename, &sb) == -1) {
        perror("stat");
        exit(EXIT_FAILURE);
        }*/

		/* 
		int vl,ivl

		if(access(file, F_OK) == 0) 
		vl++
		else
		ivl++
		*/
		if (ntot == 0)
				ntot = 1; /* avoid divide by 0; print 0 for all counts */
		printf("regular files\t= %7ld, %5.2f %%\n", nreg,
						nreg*100.0/ntot);
		printf("directories\t= %7ld, %5.2f %%\n", ndir,
						ndir*100.0/ntot);
		printf("block special\t= %7ld, %5.2f %%\n", nblk,
						nblk*100.0/ntot);
		printf("char special\t= %7ld, %5.2f %%\n", nchr, nchr*100.0/ntot);
		printf("FIFOs\t\t= %7ld, %5.2f %%\n", nfifo,
						nfifo*100.0/ntot);
		printf("symbolic links\t= %7ld, %5.2f %%\n", nslink,
						nslink*100.0/ntot);
		printf("sockets\t\t= %7ld, %5.2f %%\n", nsock,
						nsock*100.0/ntot);
        printf("Total file size\t\t= %7ld byte \n",totalsize); 
		printf("validlink=%d\n",vl);
		printf("invalidlink=%d\n",ivl);             
		exit(ret);
}
/*
 * Descend through the hierarchy, starting at "pathname".
 * The caller's func() is called for every file.
 */
#define FTW_F 1 /* file other than directory */
#define FTW_D 2 /* directory */
#define FTW_NS 4 /* file that we can't stat */
static char *fullpath; /* contains full pathname for every file */
static int myftw(char *pathname, Myfunc *func)/* we return whatever func() returns */
{
		int len;
		fullpath = path_alloc(&len); /* malloc's for PATH_MAX+1 bytes */
		/* (Figure 2.15) */
		strncpy(fullpath, pathname, len); /* protect against */
		fullpath[len-1] = 0; /* buffer overrun */
		return(dopath(func));
}
/*
 * Descend through the hierarchy, starting at "fullpath".
 * If "fullpath" is anything other than a directory, we lstat() it,
 * call func(), and return. For a directory, we call ourself
 * recursively for each name in the directory.
 */
static int dopath(Myfunc* func)/* we return whatever func() returns */
{
		struct stat statbuf;
		struct dirent *dirp;
		DIR *dp;
		int ret;
		char *ptr;
		char buf[256];
		
		
		if (lstat(fullpath, &statbuf) < 0) /* stat error */
				return(func(fullpath, &statbuf, FTW_NS));
		else{

		 	totalsize+=statbuf.st_size;
            
			if(readlink(fullpath, buf,  256) > 0) //symbolic link
				
			   if(access(fullpath, F_OK) == 0){
				   
		       		vl++;
			   }
			   else{
			   		ivl++;
			   }
        }    
		if (S_ISDIR(statbuf.st_mode) == 0) /* not a directory */ return(func(fullpath, &statbuf, FTW_F));
		/*
		 * It's a directory. First call func() for the directory,
		 * then process each filename in the directory.
		 */
		if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
				return(ret);
		ptr = fullpath + strlen(fullpath); /* point to end of fullpath */
		*ptr++ = '/';
		*ptr = 0;
		if ((dp = opendir(fullpath)) == NULL) /* can't read directory */
				chmod(fullpath, S_IRWXU);                                                /**/	
                dp = opendir(fullpath);

		while ((dirp = readdir(dp)) != NULL) {
				if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
						continue; /* ignore dot and dot-dot */
				strcpy(ptr, dirp->d_name); /* append name after slash */
				if ((ret = dopath(func)) != 0) /* recursive */
						break; /* time to leave */
		}
		ptr[-1] = 0; /* erase everything from slash onwards */
		if (closedir(dp) < 0)
				err_ret("can't close directory %s", fullpath);
		return(ret);
}
static int myfunc(const char *pathname, const struct stat *statptr, int type)
{
		switch (type) {
				case FTW_F:
						switch (statptr->st_mode & S_IFMT) {
								case S_IFREG: nreg++; break;
								case S_IFBLK: nblk++; break;
								case S_IFCHR: nchr++; break;
								case S_IFIFO: nfifo++; break;
								case S_IFLNK: nslink++; break;
								case S_IFSOCK: nsock++; break;
								case S_IFDIR:
											   err_dump("for S_IFDIR for %s", pathname);
											   /* directories should have type = FTW_D */
						}
						break;
				case FTW_D:
						ndir++;
						break;
				case FTW_NS:
						err_ret("stat error for %s", pathname);
						break;
				default:
						err_dump("unknown type %d for pathname %s", type, pathname);
		}
		return(0);
}
