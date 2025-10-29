#include "templates.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <zlib.h>


#include "cJSON.h"

#include <proto/exec.h>
#include <proto/dos.h>

#include <dos/dos.h>


static sWizTemplate *gFirstTemplate=NULL;

// copy json string to our struct, using AllocVec
static inline void getJsString(char **p, cJSON *jsobj, const char *key )
{
    if(*p != NULL) {
        FreeVec(*p);
        *p = NULL;
    }
    if(!jsobj) return;
    cJSON *pm = cJSON_GetObjectItem(jsobj,key);
    if(!pm) return;
    if( !cJSON_IsString(pm)) return;

    const char *ps = cJSON_GetStringValue(pm);
    if(!ps) return;

    int l = strlen(ps);
    *p = AllocVec(l+1,0);
    if(!(*p)) return;
    strcpy(*p,ps);
    (*p)[l]=0;
}


static int scanTemplates(BPTR lock, struct FileInfoBlock*fib)
{
    char temp[256];
    temp[0] = 0;
    int nb = 0;
    if(!Examine(lock, fib)) return 0;
/*
The allocator used by cJSON_Parse is malloc and free
by default but can be changed (globally) with cJSON_InitHooks.
*/
    if(fib->fib_DirEntryType <= 0) return 0; // if >0, a directory

    while(ExNext(lock, fib))
    {
        if(fib->fib_DirEntryType <= 0)
        {   // is a file.
            // if end with .json
            // fast, no alloc version
            char *p = fib->fib_FileName;
            int l =strlen(p);
            if(l>6 && p[l-5]=='.' &&
                (p[l-4]=='j' || p[l-4]=='J' ) &&
                (p[l-3]=='s' || p[l-4]=='S' ) &&
                (p[l-2]=='o' || p[l-4]=='O' )
                )
            {
                char *pmem = AllocVec(fib->fib_Size+1,0);
                if( pmem )
                {
                    strcat(temp,"PROGDIR:templates/");
                    strcat(temp,fib->fib_FileName);
                    //printf("filesize:%d\n",fib->fib_Size);
                    BPTR fh = Open(temp,MODE_OLDFILE);

 printf("open:%s\n",temp);
                    if(fh)
                    {
                        Read(fh,pmem,fib->fib_Size);
                        pmem[fib->fib_Size]=0;
                        Close(fh);
                    }
                    cJSON *jsroot = cJSON_Parse(pmem); //cJSON_CreateObject();
                    if (jsroot == NULL)
                    {
                        const char *error_ptr = cJSON_GetErrorPtr();
                        if (error_ptr != NULL)
                        {
                            printf("error file: %s\n",fib->fib_FileName);
                            printf("template json error before: %s\n", error_ptr);
                        }
                        FreeVec(pmem);
                        continue;
                    }
                    cJSON *jstemplate = cJSON_GetObjectItemCaseSensitive(jsroot, "template");
                    if(jstemplate)
                    {
                        sWizTemplate *ntmpl = AllocVec(sizeof(sWizTemplate),MEMF_CLEAR);
                        if(ntmpl)
                        {
                            ntmpl->_pNext = gFirstTemplate;
                            gFirstTemplate = ntmpl;

                            getJsString(&(ntmpl->_displayName),jstemplate,"displayname");
                            getJsString(&(ntmpl->_versionstring),jstemplate,"versionstr");
                            getJsString(&(ntmpl->_archivename),jstemplate,"archive");
                            getJsString(&(ntmpl->_defaultname),jstemplate,"defaultname");
                            getJsString(&(ntmpl->_comment),jstemplate,"comment");

        if(ntmpl->_comment) printf(ntmpl->_comment);

                        }
                    } else
                    {
                        FreeVec(pmem);
                        printf("error file: %s\n",fib->fib_FileName);
                        printf("json syntax ok but no template chapter.\n");
                         continue;
                    }



    // resolutions = cJSON_GetObjectItemCaseSensitive(monitor_json, "resolutions");
    // cJSON_ArrayForEach(resolution, resolutions)
    // {
    //     cJSON *width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
    //     cJSON *height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

    //     if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height))
    //     {
    //         status = 0;
    //         goto end;
    //     }

    //     if ((width->valuedouble == 1920) && (height->valuedouble == 1080))
    //     {
    //         status = 1;
    //         goto end;
    //     }
    // }


                    printf("parse ok\n");
                    cJSON_Delete(jsroot);


                    FreeVec(pmem);
                }


                nb++;
            }
        } // end if is file.

    } // end loop per dir file
    return nb;
}

static void closeTemplates()
{
 printf("closeTemplates\n");
    sWizTemplate *pt = gFirstTemplate;
    while(pt)
    {
        sWizTemplate *ptnext = pt->_pNext;
        if(pt->_displayName) FreeVec(pt->_displayName);
        if(pt->_versionstring) FreeVec(pt->_versionstring);
        if(pt->_archivename) FreeVec(pt->_archivename);
        if(pt->_defaultname) FreeVec(pt->_defaultname);
        if(pt->_comment) FreeVec(pt->_comment);

        FreeVec(pt);
        pt = ptnext;
 printf("woot\n");
    }
 printf("end\n");
}

void initTemplates()
{
    atexit(&closeTemplates);

    struct FileInfoBlock *fib;
    fib = (struct FileInfoBlock *)AllocDosObject(DOS_FIB, NULL);
    if(!fib) return 0;


    BPTR lock = Lock( "PROGDIR:templates" , ACCESS_READ);
    if(lock)
    {
        scanTemplates(lock,fib);
        UnLock(lock);
    }

    FreeDosObject(DOS_FIB,fib);


}
int nbTemplates()
{
    return 0;

}
sWizTemplate *getTemplates(int i)
{
    return NULL;
}


int extractTemplate(const char *templateArchive,
                     const char *baseName)
{

}
/*
 *    cJSON *root = NULL;
    cJSON *fmt = NULL;
    cJSON *img = NULL;
    cJSON *thm = NULL;
    cJSON *fld = NULL;
 *
     read expected output
    expected = read_file(expected_path);
    TEST_ASSERT_NOT_NULL_MESSAGE(expected, "Failed to read expected output.");

     read and parse test
    tree = parse_file(test_path);
    TEST_ASSERT_NOT_NULL_MESSAGE(tree, "Failed to read of parse test.");

    if (print_preallocated(root) != 0) {
        cJSON_Delete(root);

static cJSON *parse_file(const char *filename)
{
    cJSON *parsed = NULL;
    char *content = read_file(filename);

    parsed = cJSON_Parse(content);

    if (content != NULL)
    {
        free(content);
    }

    return parsed;
}
*/
