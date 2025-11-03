
#include "templates.h"
#include "zlib.h"
#include "unzip.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>

#include <dos/dos.h>

/**
    almost same thing as unzipping...
    almost: we replace names also.
*/
static const char *errorstring=NULL;
static const char *errorstringmem="No mem";
static unzFile zF=NULL;

static void extractClose()
{
    if(zF) unzClose(zF);
    zF = NULL;
}
static int exitclset=0;

const char *extractTextError()
{
    return errorstring;
}


static char lowName[32];
static char upName[32];
static void makeSecureNames(const char *pn)
{
    int iln=0;
    int uln=0;
    while(*pn != 0 && iln<31 && uln<31)
    {
        char c = *pn++;
        if(c == " "){
            continue;
        }
        if(c>='A' && c<='Z')
        {
            upName[uln] = c;
            uln++;
            lowName[iln] = c + ((int)'a'-(int)'A');
            iln++;
        } else
        if(c>='a' && c<='z')
        {
            upName[uln] = c;
            uln++;
            lowName[iln] = c ;
            iln++;
        }
    }
    upName[uln] = 0;
    lowName[iln] = 0;

}

int get_path_info(const char *fullpath)
{
    BPTR hdl = Open(fullpath, MODE_OLDFILE);
    if(!hdl)
    {
        return PATH_NOT_FOUND;
    }
    struct FileInfoBlock *fib = (struct FileInfoBlock *)AllocDosObject(DOS_FIB, NULL);
    if(!fib){
        Close(hdl);
        return PATH_NOT_FOUND;
    }
    ExamineFH(hdl,fib);
    int res = ((fib->fib_DirEntryType<0)?PATH_IS_FILE:PATH_IS_DIRECTORY);
    FreeDosObject(DOS_FIB,fib);
    Close(hdl);
    return res;
}

// would create directory recusively, return if ok.
static int assumeDirectory(const char *fullpath)
{
    int dirtype = get_path_info(fullpath);
    if(dirtype == PATH_IS_DIRECTORY) return PATH_IS_DIRECTORY;

// n o need recurse  for the moment
    // // if not...
    // std::string sfullpath(fullpath);
    // size_t i = sfullpath.rfind("/");
    // if(i != string::npos)
    // {
    //     string sparent =sfullpath.substr(0,i);
    //     int iparent = assumeDirectory(sparent.c_str());
    //     if(iparent != PATH_IS_DIRECTORY) return PATH_NOT_FOUND;
    // }
    BPTR l = CreateDir(fullpath);
    if(l) {
        UnLock(l);
        return PATH_IS_DIRECTORY;
    }
    return PATH_NOT_FOUND;
}

typedef struct {
 const char *key;
 const char *repl;
} sReplace;

// this is the whole effective part:
// file names and uppercase/lower cases are all replaced while writting
int replaceWrite(const char *originalbin, ULONG origbsize,
                 const char *origfilepath, const char *destbase)
{
    // assume base dir
    ULONG fullbase_l = strlen(destbase)+2+strlen(upName);
    char *fulldestpath = AllocVec(fullbase_l,0);
    if(!fulldestpath)
    {
     errorstring = errorstringmem;
     return 1;
    }
    *fulldestpath=0;
    strcat(fulldestpath,destbase);
    // if dvice: no need to /
    if(destbase[strlen(destbase)-1]!= ':')  strcat(fulldestpath,"/");
    strcat(fulldestpath,upName);
 printf("dirpath:%s\n",fulldestpath);

    int dirtype = assumeDirectory(fulldestpath);
    if(dirtype != PATH_IS_DIRECTORY)
    {
      errorstring = "can't create project directory";
     return 1;
    }
    // - - - -
    sReplace replacers[]={
        {"basename",lowName},
        {"BaseName",upName},
        {NULL,NULL}
    };
    // got to remap file name
    // origfilepath

    // TODO replace/ write engine.

    // - - - -
    FreeVec(fulldestpath);
    return 0;
}

int extractTemplate(const char *templateArchive,
                    const char *destDir,
                     sGeneration *pgen)
{
//    printf("extractTemplate:%s %s %s\n",templateArchive,destDir,pgen->baseName);
    if(exitclset==0) {
        atexit(extractClose);
        exitclset=1;
    }
    char btemp[128];
    if(!templateArchive || !destDir || !pgen || !pgen->baseName)
    {
        errorstring = "archive name or extract dir name uncompliant";
        return 1;
    }
    makeSecureNames(pgen->baseName);

    extractClose(); // in case of previous call.

    btemp[0]=0;
    strcat(btemp,"PROGDIR:templates/");
    strcat(btemp,templateArchive);
    zF = unzOpen(btemp);
    if(!zF)
    {
        errorstring = "can't open template zip file";
        return 1;
    }

    printf("unzip ok\n");

    int r = unzGoToFirstFile(zF);
    int rr=0;
    while(r == UNZ_OK)
    {
    //uLong compressed_size;      /* compressed size                 4 bytes */
    //uLong uncompressed_size;    /* uncompressed size               4 bytes */
    //uLong size_filename;        /* filename length                 2 bytes */

        unz_file_info zfinfo;
        char tfilename[256];

        /*int*/  unzGetCurrentFileInfo(zF, // unzFile file,
                                    &zfinfo,//     unz_file_info *pfile_info,
                                         tfilename,//char *szFileName,
                                         255,//uLong fileNameBufferSize,
                                         NULL, //void *extraField,
                                         0, //uLong extraFieldBufferSize,
                                         NULL,//char *szComment,
                                         0 //uLong commentBufferSize
                                         );
        printf("f:%s\n",tfilename);
        // to read a file in zip, need open/read close,
        int zferr = unzOpenCurrentFile(zF);
        if(zferr == UNZ_OK)
        {
            char *f = AllocVec(zfinfo.uncompressed_size,0);
            if(f)
            {
                int nbdone = unzReadCurrentFile(zF,f,zfinfo.uncompressed_size);

                if(nbdone == zfinfo.uncompressed_size)
                {
                    // extracted !
                    printf("looks extracted ok!\n");
                    rr |= replaceWrite(f, nbdone,tfilename,destDir); // !=0 if any error

                }
                FreeVec(f);
            }
            unzCloseCurrentFile(zF);
        } // if inner file zopened ok.

        r = unzGoToNextFile(zF);
    }

    extractClose();
    return rr;
}
