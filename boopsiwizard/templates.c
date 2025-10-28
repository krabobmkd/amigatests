#include "templates.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <zlib.h>


#include "cJSON.h"

#include <proto/exec.h>
#include <proto/dos.h>

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



void initTemplates()
{


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
