#ifndef WIZTEMPLATES_H_
#define WIZTEMPLATES_H_


/**
    The softwatre part of the wizard.

    (no interface code here, interface should just just use.)

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

//  "type":"defbool","displayname":"Manage sub-cliping","id":"%USESUBCLIPPING%","def":true},

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


void initTemplates();
sWizTemplate *getTemplates();


#endif

