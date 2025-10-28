#ifndef WIZTEMPLATES_H_
#define WIZTEMPLATES_H_

typedef struct _sWizTemplate
{
    const char *_displayName;
    const char *_id;

} sWizTemplate;


void initTemplates();
int nbTemplates();
sWizTemplate *getTemplates(int i);


#endif

