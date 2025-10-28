#include "templates.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <zlib.h>


#include "cJSON.h"

#include <proto/exec.h>
#include <proto/dos.h>

#include <dos/dos.h>

//char* read_file(const char *filename);
//char* read_file(const char *filename) {
//    FILE *file = NULL;
//    long length = 0;
//    char *content = NULL;
//    size_t read_chars = 0;

//    /* open in read binary mode */
//    file = fopen(filename, "rb");
//    if (file == NULL)
//    {
//        goto cleanup;
//    }

//    /* get the length */
//    if (fseek(file, 0, SEEK_END) != 0)
//    {
//        goto cleanup;
//    }
//    length = ftell(file);
//    if (length < 0)
//    {
//        goto cleanup;
//    }
//    if (fseek(file, 0, SEEK_SET) != 0)
//    {
//        goto cleanup;
//    }

//    /* allocate content buffer */
//    content = (char*)malloc((size_t)length + sizeof(""));
//    if (content == NULL)
//    {
//        goto cleanup;
//    }

//    /* read the file into memory */
//    read_chars = fread(content, sizeof(char), (size_t)length, file);
//    if ((long)read_chars != length)
//    {
//        free(content);
//        content = NULL;
//        goto cleanup;
//    }
//    content[read_chars] = '\0';


//cleanup:
//    if (file != NULL)
//    {
//        fclose(file);
//    }

//    return content;
//}
static int scanTemplates(BPTR lock, FileInfoBlock*fib)
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
                     // if (monitor_json == NULL)
                     //    {
                     //        const char *error_ptr = cJSON_GetErrorPtr();
                     //        if (error_ptr != NULL)
                     //        {
                     //            fprintf(stderr, "Error before: %s\n", error_ptr);
                     //        }
                     //        status = 0;
                     //        goto end;
                     //    }



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

                    if(jsroot)
                    {
                        printf("parse ok\n");
                        cJSON_Delete(jsroot);
                    }

                    FreeVec(pmem);
                }


                nb++;
            }
        } // end if is file.

    } // end loop per dir file
    return nb;
}

void initTemplates()
{
    struct FileInfoBlock *fib;
    fib = (struct FileInfoBlock *)AllocDosObject(DOS_FIB, NULL);
    if(!fib) return 0;


    BPTR lock = Lock( "PROGDIR:templates" , ACCESS_READ);
    if(lock)
    {
        scanTemplates(temp, lock,fib);
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
