/*
** This program is a utility called from the INNO Windows installer
** during Jemboss installations.
** Its purpose is to create the jemboss.properties and runjemboss.bat
** files given the 'win' and 'app' variables from the installer.
** Some simple safeguards are added to deflect any damage caused by users
** trying to invoke it. The syntax is:
**    mkjemfiles Jemboss install_dir win_dir
** where install_dir is e.g. D:\EMBOSS-MS and win_dir is e.g. C:\windows
**
** This should be compiled statically e.g. with bcc rather than vc
*/

#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    char propname[512];
    char batname[512];
    char sr[512];
    char ins[512];
    
    char c;
    
    FILE *batfp;
    FILE *propfp;
    
    int i;
    int j;
    

    if(argc < 2)
    {
	fprintf(stderr,"Error: This program is only to be used by the "
		"EMBOSS installer\n");
	return -1;
    }

    /* Make sure that the first parameter is "Jemboss", otherwise exit  */
    /* A further safety measure in case of grubby paws */
    if(strcmp(argv[1],"Jemboss"))
	return -1;

    /* The syntax is "mkjemfiles Jemboss install_dir system_root" */
    if(argc != 4)
	return -1;


    strcpy(propname,argv[2]);
    strcat(propname,"\\jemboss\\jar\\resources\\jemboss.properties");

    strcpy(batname,argv[2]);    
    strcat(batname,"\\runjemboss.bat");

    if((batfp=fopen(batname,"w")) == NULL)
    {
	fprintf(stderr,"Error: cannot open file %s\n",batname);
	return -1;
    }

    fprintf(batfp,"cd %s\\jemboss\\jar\njava -jar Jemboss-MS.jar local\n",
	    argv[2]);

    fclose(batfp);


    /* Create a SystemRoot from $win by doubling up the backslashes */
    i = 0;
    j = 0;
    while((c=argv[3][i]))
    {
	sr[j++] = c;
	if(c=='\\')
	    sr[j++] = '\\';
	++i;
    }
    sr[j] = '\0';

    /* Create 'ins' from $app by doubling up the backslashes */
    i = 0;
    j = 0;
    while((c=argv[2][i]))
    {
	ins[j++] = c;
	if(c=='\\')
	    ins[j++] = '\\';
	++i;
    }
    ins[j] = '\0';
    

    if((propfp=fopen(propname,"w")) == NULL)
    {
	fprintf(stderr,"Error: cannot open file %s\n",propname);
	return -1;
    }

    fprintf(propfp,"user.auth=false\n");
    fprintf(propfp,"jemboss.server=false\n");
    fprintf(propfp,"plplot=%s\\\\data\\\\\n",ins);
    fprintf(propfp,"embossData=%s\\\\data\\\\\n",ins);
    fprintf(propfp,"embossBin=%s\\\\\n",ins);
    fprintf(propfp,"embossPath=%s\\\\system32\\\\\n",sr);
    fprintf(propfp,"acdDirToParse=%s\\\\acd\\\\\n",ins);
    fprintf(propfp,"embossURL=http://emboss.sourceforge.net/apps/\n");
    fprintf(propfp,"embossEnvironment=SystemRoot=%s EMBOSS_ROOT=%s\\\\ "
	    "EMBOSS_ACDROOT=%s\\\\acd EMBOSS_DATA=%s\\\\data\\\\ "
            "EPLPLOT_LIB=%s\\\\data\\\\\n",
	    sr,ins,ins,ins,ins);

    fclose(propfp);
    
    return 0;
}
