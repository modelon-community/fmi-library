/*
   minizip.c
   Version 1.1, February 14h, 2010
   sample part of the MiniZip project - ( http://www.winimage.com/zLibDll/minizip.html )

         Copyright (C) 1998-2010 Gilles Vollant (minizip) ( http://www.winimage.com/zLibDll/minizip.html )

         Modifications of Unzip for Zip64
         Copyright (C) 2007-2008 Even Rouault

         Modifications for Zip64 support on both zip and unzip
         Copyright (C) 2009-2010 Mathias Svensson ( http://result42.com )

    Modifications for FMI-Library:
        1. Changed all uses of printf to a new function minizip_printf
        2. Renamed file and main function to fmi_minizip
        3. Removed code not related to just zipping
        4. Replaced localtime with localtime_r on POSIX systems
*/


#if (!defined(_WIN32)) && (!defined(WIN32)) && (!defined(__APPLE__))
        #ifndef __USE_FILE_OFFSET64
                #define __USE_FILE_OFFSET64
        #endif
        #ifndef __USE_LARGEFILE64
                #define __USE_LARGEFILE64
        #endif
        #ifndef _LARGEFILE64_SOURCE
                #define _LARGEFILE64_SOURCE
        #endif
        #ifndef _FILE_OFFSET_BIT
                #define _FILE_OFFSET_BIT 64
        #endif
#endif

#if defined(__APPLE__) || defined(__HAIKU__) || defined(MINIZIP_FOPEN_NO_64)
// In darwin and perhaps other BSD variants off_t is a 64 bit value, hence no need for specific 64 bit functions
#define FOPEN_FUNC(filename, mode) fopen(filename, mode)
#define FTELLO_FUNC(stream) ftello(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko(stream, offset, origin)
#else
#define FOPEN_FUNC(filename, mode) fopen64(filename, mode)
#define FTELLO_FUNC(stream) ftello64(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)
#endif

#if defined(__unix__) || defined(__APPLE__)
/* For localtime_r */
#define _POSIX_C_SOURCE 200112L
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef _WIN32
# include <direct.h>
# include <io.h>
#else
# include <unistd.h>
# include <utime.h>
# include <sys/types.h>
# include <sys/stat.h>
#endif

#include "zip.h"

#ifdef _WIN32
/* MODIFICATION: To allow smoother integration with system minizip-ng, don't use the original minizip's iowin32 API */
#include <windows.h>
#endif



#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

/**
 * MODIFICATION: All calls to printf have been replaced with this function
 * which just suppresses the output.
 *
 * FIXME: A lot of calls to this function are error messages. These messages
 * should forwarded to the FMI logger and proper errors should be given.
 */ 
static int minizip_printf(const char * format, ...) {
    return 0;
}

#ifdef _WIN32
/* f: name of file to get info on, tmzip: return value: access,
   modification and creation times, dt: dostime */
static int filetime(const char *f, tm_zip *tmzip, uLong *dt) {
  int ret = 0;
  {
      FILETIME ftLocal;
      HANDLE hFind;
      WIN32_FIND_DATAA ff32;

      hFind = FindFirstFileA(f,&ff32);
      if (hFind != INVALID_HANDLE_VALUE)
      {
        FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
        FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
        FindClose(hFind);
        ret = 1;
      }
  }
  return ret;
}
#else
#if defined(__unix__) || defined(__APPLE__)
/* f: name of file to get info on, tmzip: return value: access,
   modification and creation times, dt: dostime */
static int filetime(const char *f, tm_zip *tmzip, uLong *dt) {
  (void)dt;
  int ret=0;
  struct stat s;        /* results of stat() */
  struct tm filedate;
  struct tm* filedatePtr;
  time_t tm_t=0;

  if (strcmp(f,"-")!=0)
  {
    char name[MAXFILENAME+1];
    size_t len = strlen(f);
    if (len > MAXFILENAME)
      len = MAXFILENAME;

    strncpy(name, f,MAXFILENAME-1);
    /* strncpy doesn't append the trailing NULL, of the string is too long. */
    name[ MAXFILENAME ] = '\0';

    if (name[len - 1] == '/')
      name[len - 1] = '\0';
    /* not all systems allow stat'ing a file with / appended */
    if (stat(name,&s)==0)
    {
      tm_t = s.st_mtime;
      ret = 1;
    }
  }
  filedatePtr = localtime_r(&tm_t, &filedate);

  tmzip->tm_sec  = filedatePtr->tm_sec;
  tmzip->tm_min  = filedatePtr->tm_min;
  tmzip->tm_hour = filedatePtr->tm_hour;
  tmzip->tm_mday = filedatePtr->tm_mday;
  tmzip->tm_mon  = filedatePtr->tm_mon ;
  tmzip->tm_year = filedatePtr->tm_year;

  return ret;
}
#else
/* f: name of file to get info on, tmzip: return value: access,
   modification and creation times, dt: dostime */
static int filetime(const char *f, tm_zip *tmzip, uLong *dt) {
    (void)f;
    (void)tmzip;
    (void)dt;
    return 0;
}
#endif
#endif




static int check_exist_file(const char* filename) {
    FILE* ftestexist;
    int ret = 1;
    ftestexist = FOPEN_FUNC(filename,"rb");
    if (ftestexist==NULL)
        ret = 0;
    else
        fclose(ftestexist);
    return ret;
}

static int isLargeFile(const char* filename) {
  int largeFile = 0;
  ZPOS64_T pos = 0;
  FILE* pFile = FOPEN_FUNC(filename, "rb");

  if(pFile != NULL)
  {
    FSEEKO_FUNC(pFile, 0, SEEK_END);
    pos = (ZPOS64_T)FTELLO_FUNC(pFile);

                minizip_printf("File : %s is %llu bytes\n", filename, pos);

    if(pos >= 0xffffffff)
     largeFile = 1;

                fclose(pFile);
  }

 return largeFile;
}

int fmi_minizip(const char* zip_file_path, int n_files_to_zip, const char** files_to_zip) {
    int i;
    int opt_overwrite=1;
    int opt_compress_level=1;
    char filename_try[MAXFILENAME+16];
    int zipok = 1;
    int err=0;
    size_t size_buf=0;
    void* buf=NULL;
    const char* password=NULL;

    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf==NULL)
    {
        minizip_printf("Error allocating memory\n");
        return ZIP_INTERNALERROR;
    }


    int len;
    int dot_found=0;

    strncpy(filename_try, zip_file_path, MAXFILENAME-1);
    /* strncpy doesn't append the trailing NULL, of the string is too long. */
    filename_try[ MAXFILENAME ] = '\0';

    len=(int)strlen(filename_try);
    for (i=0;i<len;i++)
        if (filename_try[i]=='.')
            dot_found=1;

    if (dot_found==0)
        strcat(filename_try,".zip");

    if (opt_overwrite==2)
    {
        /* if the file don't exist, we not append file */
        if (check_exist_file(filename_try)==0)
            opt_overwrite=1;
    }
    else
    if (opt_overwrite==0)
        if (check_exist_file(filename_try)!=0)
        {
            char rep=0;
            do
            {
                char answer[128];
                int ret;
                minizip_printf("The file %s exists. Overwrite ? [y]es, [n]o, [a]ppend : ",filename_try);
                ret = scanf("%1s",answer);
                if (ret != 1)
                {
                    exit(EXIT_FAILURE);
                }
                rep = answer[0] ;
                if ((rep>='a') && (rep<='z'))
                    rep -= 0x20;
            }
            while ((rep!='Y') && (rep!='N') && (rep!='A'));
            if (rep=='N')
                zipok = 0;
            if (rep=='A')
                opt_overwrite = 2;
        }

    if (zipok==1)
    {
        zipFile zf;
        int errclose;
#        ifdef USEWIN32IOAPI
        zlib_filefunc64_def ffunc;
        fill_win32_filefunc64A(&ffunc);
        zf = zipOpen2_64(filename_try,(opt_overwrite==2) ? 2 : 0,NULL,&ffunc);
#        else
        zf = zipOpen64(filename_try,(opt_overwrite==2) ? 2 : 0);
#        endif

        if (zf == NULL)
        {
            minizip_printf("error opening %s\n",filename_try);
            err= ZIP_ERRNO;
        }
        else
            minizip_printf("creating %s\n",filename_try);

        for (i=0;(i<n_files_to_zip) && (err==ZIP_OK);i++)
        {
            {
                FILE * fin = NULL;
                size_t size_read;
                const char* filenameinzip = files_to_zip[i];
                const char *savefilenameinzip;
                zip_fileinfo zi;
                unsigned long crcFile=0;
                int zip64 = 0;

                zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
                zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
                zi.dosDate = 0;
                zi.internal_fa = 0;
                zi.external_fa = 0;
                filetime(filenameinzip,&zi.tmz_date,&zi.dosDate);

/*
                err = zipOpenNewFileInZip(zf,filenameinzip,&zi,
                                 NULL,0,NULL,0,NULL / * comment * /,
                                 (opt_compress_level != 0) ? Z_DEFLATED : 0,
                                 opt_compress_level);
*/

                zip64 = isLargeFile(filenameinzip);

                                                         /* The path name saved, should not include a leading slash. */
               /*if it did, windows/xp and dynazip couldn't read the zip file. */
                 savefilenameinzip = filenameinzip;
                 while( savefilenameinzip[0] == '\\' || savefilenameinzip[0] == '/' )
                 {
                     savefilenameinzip++;
                 }

                 /**/
                err = zipOpenNewFileInZip3_64(zf,savefilenameinzip,&zi,
                                 NULL,0,NULL,0,NULL /* comment*/,
                                 (opt_compress_level != 0) ? Z_DEFLATED : 0,
                                 opt_compress_level,0,
                                 /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
                                 -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                                 password,crcFile, zip64);

                if (err != ZIP_OK)
                    minizip_printf("error in opening %s in zipfile\n",filenameinzip);
                else
                {
                    fin = FOPEN_FUNC(filenameinzip,"rb");
                    if (fin==NULL)
                    {
                        err=ZIP_ERRNO;
                        minizip_printf("error in opening %s for reading\n",filenameinzip);
                    }
                }

                if (err == ZIP_OK)
                    do
                    {
                        err = ZIP_OK;
                        size_read = fread(buf,1,size_buf,fin);
                        if (size_read < size_buf)
                            if (feof(fin)==0)
                        {
                            minizip_printf("error in reading %s\n",filenameinzip);
                            err = ZIP_ERRNO;
                        }

                        if (size_read>0)
                        {
                            err = zipWriteInFileInZip (zf,buf,(unsigned)size_read);
                            if (err<0)
                            {
                                minizip_printf("error in writing %s in the zipfile\n",
                                                 filenameinzip);
                            }

                        }
                    } while ((err == ZIP_OK) && (size_read>0));

                if (fin)
                    fclose(fin);

                if (err<0)
                    err=ZIP_ERRNO;
                else
                {
                    err = zipCloseFileInZip(zf);
                    if (err!=ZIP_OK)
                        minizip_printf("error in closing %s in the zipfile\n",
                                    filenameinzip);
                }
            }
        }
        errclose = zipClose(zf,NULL);
        if (errclose != ZIP_OK)
            minizip_printf("error in closing %s\n",filename_try);
    }


    free(buf);
    return 0;
}
