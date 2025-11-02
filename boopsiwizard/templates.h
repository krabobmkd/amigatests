#ifndef WIZTEMPLATES_H_
#define WIZTEMPLATES_H_

/**
    The software part of the wizard
    (no interface code here, interface should just use it.)

*/
typedef struct _sWTmpl_pref
{
    int type;
    char *_displayname;
} sWTmpl_pref;

typedef struct _sWTmpl_pref_bool
{
    int type;
    char *_displayname;
    char *_rid;

} sWTmpl_pref_bool;


/** one instance per known template, described in json */
typedef struct _sWizTemplate
{
    struct _sWizTemplate *_pNext;
    char *_displayName;
    char *_versionstring;
    char *_archivename;
    char *_defaultname;
    char *_comment;
    // list of prefs
    sWTmpl_pref *_firstPRef;

} sWizTemplate;


typedef void (*template_notifier)(int ilog,const char *log);

void initTemplates(template_notifier n);

sWizTemplate *getTemplates();
int getNbTemplates();

typedef struct _Generation
{
    const char *baseName;

} sGeneration;

int extractTemplate(const char *templateArchive,
                    const char *destDir,
                     sGeneration *pgen);
// if return !=0
const char *extractTextError();

#endif

