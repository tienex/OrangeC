/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include "compiler.h"
#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <deque>
#include "PreProcessor.h"
#include "Utils.h"

#include "../../util/Utils.h"
#ifdef MSIL
#    include "../version.h"
#else
#    include "../../version.h"
#endif
#if defined(_MSC_VER) || defined(BORLAND) || defined(__ORANGEC__)
#    include <io.h>
#endif

#ifdef __CCDL__
int _stklen = 100 * 1024;
#    ifdef MSDOS
int __dtabuflen = 32 * 1024;
#    endif
#endif

#if defined(WIN32) || defined(MICROSOFT)
extern "C"
{
    char* __stdcall GetModuleFileNameA(void* handle, char* buf, int size);
}
#endif

extern COMPILER_PARAMS cparams;
extern int total_errors;
extern ARCH_ASM* chosenAssembler;
extern int diagcount;
extern NAMESPACEVALUELIST* globalNameSpace;
extern char infile[];
extern PreProcessor* preProcessor;

FILE* outputFile;
FILE* listFile;
char outfile[256];
char* prm_searchpath = 0;
char* sys_searchpath = 0;
char* prm_libpath = 0;
char version[256];
char copyright[256];
LIST* clist = 0;
int showBanner = true;
int showVersion = false;

struct DefValue
{
    std::string name;
    bool undef;
};
std::deque<DefValue> defines;

static bool has_output_file;
static char** set_searchpath = &prm_searchpath;
static char** set_libpath = &prm_libpath;
void fatal(const char* fmt, ...)
{
    va_list argptr;

    va_start(argptr, fmt);
    fprintf(stderr, "Fatal error: ");
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);
    extern void Cleanup();
    Cleanup();
    exit(1);
}
void banner(const char* fmt, ...)
{
    va_list argptr;

    va_start(argptr, fmt);
    vfprintf(stderr, fmt, argptr);
    va_end(argptr);

    putc('\n', stderr);
    putc('\n', stderr);
}

/* Print usage info */
void usage(char* prog_name)
{
    char* short_name;
    char* extension;

    short_name = strrchr(prog_name, '\\');
    if (short_name == nullptr)
        short_name = strrchr(prog_name, '/');
    if (short_name == nullptr)
        short_name = strrchr(prog_name, ':');
    if (short_name)
        short_name++;
    else
        short_name = prog_name;

    extension = strrchr(short_name, '.');
    if (extension != nullptr)
        *extension = '\0';
    fprintf(stderr, "Usage: %s %s", short_name, getUsageText());
#    ifndef USE_LONGLONG
    fprintf(stderr, "   long long not supported");
#    endif

    exit(1);
}
/*
 * If no extension, add the one specified
 */
void AddExt(char* buffer, const char* ext)
{
    char* pos = strrchr(buffer, '.');
    if (!pos || (*(pos - 1) == '.') || (*(pos + 1) == '\\'))
        strcat(buffer, ext);
}

/*
 * Strip extension, if it has one
 */
void StripExt(char* buffer)
{
    char* pos = strrchr(buffer, '.');
    if (pos && (*(pos - 1) != '.'))
        *pos = 0;
}

/*
 * Return path of EXE file
 */
void EXEPath(char* buffer, char* filename)
{
    char* temp;
    strcpy(buffer, filename);
    if ((temp = strrchr(buffer, '\\')) != 0)
        *(temp + 1) = 0;
    else
        buffer[0] = 0;
}

/*-------------------------------------------------------------------------*/

int HasExt(char* buffer, const char* ext)
{
    int l = strlen(buffer), l1 = strlen(ext);
    if (l1 < l)
    {
        return Utils::iequal(buffer + l - l1, ext);
    }
    return 0;
}
/*
 * Pull the next path off the path search list
 */
static const char* parsepath(const char* path, char* buffer)
{
    const char* pos = path;

    /* Quit if hit a ';' */
    while (*pos)
    {
        if (*pos == ';')
        {
            pos++;
            break;
        }
        *buffer++ = *pos++;
    }
    *buffer = 0;

    /* Return a null pointer if no more data */
    if (*pos)
        return (pos);

    return (0);
}

extern CMDLIST* ArgList;
/*int parseParam(int bool, char *string); */

static int use_case; /* Gets set for case sensitivity */

/*
 * Function that unlinks the argument from che argv[] chain
 */
static void remove_arg(int pos, int* count, char* list[])
{
    int i;

    /* Decrement length of list */
    (*count)--;

    /* move items down */
    for (i = pos; i < *count; i++)
        list[i] = list[i + 1];
}

/*
 * ompare two characters, ignoring case if necessary
 */
static int cmatch(char t1, char t2)
{
    if (use_case)
        return (t1 == t2);

    return (toupper(t1) == toupper(t2));
}

/* Routine scans a string to see if any of the characters match
 *  the arguments, then dispatches to the action routine if so.
 */
/* Callbacks of the form
 *   void boolcallback( char selectchar, int value)
 *   void switchcallback( char selectchar, int value)  ;; value always true
 *   void stringcallback( char selectchar, char *string)
 */
static int scan_args(char* string, int index, char* arg)
{
    int i = -1;
    bool legacyArguments = !!getenv("OCC_LEGACY_OPTIONS");
    while (ArgList[++i].id)
    {
        switch (ArgList[i].mode)
        {
            case ARG_SWITCHSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], &string[index]);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_SWITCH:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], (char*)true);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_BOOL:
                if (cmatch(string[index], ArgList[i].id))
                {
                    if (!legacyArguments || string[0] == ARG_SEPtrue || string[0] == '/')
                        (*ArgList[i].routine)(string[index], (char*)true);
                    else
                        (*ArgList[i].routine)(string[index], (char*)false);
                    return (ARG_NEXTCHAR);
                }
                break;
            case ARG_CONCATSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    (*ArgList[i].routine)(string[index], string + index + 1);
                    return (ARG_NEXTARG);
                }
                break;
            case ARG_NOCONCATSTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    if (!arg)
                        return (ARG_NOARG);
                    (*ArgList[i].routine)(string[index], arg);
                    return (ARG_NEXTNOCAT);
                }
                break;
            case ARG_COMBINESTRING:
                if (cmatch(string[index], ArgList[i].id))
                {
                    if (string[index + 1])
                    {
                        (*ArgList[i].routine)(string[index], string + index + 1);
                        return (ARG_NEXTARG);
                    }
                    else
                    {
                        if (!arg)
                            return (ARG_NEXTARG);
                        (*ArgList[i].routine)(string[index], arg);
                        return (ARG_NEXTNOCAT);
                    }
                }
                break;
        }
    }
    return (ARG_NOMATCH);
}

/*
 * Main parse routine.  Scans for '-', then scan for arguments and
 * delete from the argv[] array if so.
 */
bool parse_args(int* argc, char* argv[], bool case_sensitive)
{

    int pos = 0;
    bool retval = true;
    use_case = case_sensitive;

    while (++pos < *argc)
    {
        if ((argv[pos][0] == ARG_SEPSWITCH) || (argv[pos][0] == ARG_SEPfalse) || (argv[pos][0] == ARG_SEPtrue))
        {
            if (argv[pos][1] == '!' || !strcmp(argv[pos], "--nologo"))
            {
                // skip the silence arg
            }
            else if (argv[pos][0] == ARG_SEPfalse && !argv[pos][1])
            {
                continue;
            }
            else
            {
                int argmode;
                int index = 1;
                bool done = false;
                do
                {
                    /* Scan the present arg */
                    if (pos < *argc - 1)
                        argmode = scan_args(argv[pos], index, argv[pos + 1]);
                    else
                        argmode = scan_args(argv[pos], index, 0);

                    switch (argmode)
                    {
                        case ARG_NEXTCHAR:
                            /* If it was a char, go to the next one */
                            if (!argv[pos][++index])
                                done = true;
                            break;
                        case ARG_NEXTNOCAT:
                            /* Otherwise if it was a nocat, remove the extra arg */
                            remove_arg(pos, argc, argv);
                            /* Fall through to NEXTARG */
                        case ARG_NEXTARG:
                            /* Just a next arg, go do it */
                            done = true;
                            break;
                        case ARG_NOMATCH:
                            /* No such arg, spit an error  */
                                    fprintf(stderr, "Invalid Arg: %s\n", argv[pos]);
                                    retval = false;
                                    done = true;
                            break;
                        case ARG_NOARG:
                            /* Missing the arg for a CONCAT type, spit the error */
                            fprintf(stderr, "Missing string for Arg %s\n", argv[pos]);
                            done = true;
                            retval = false;
                            break;
                    };

                } while (!done);
            }
            /* We'll always get rid of the present arg
             * And back up one
             */
            remove_arg(pos--, argc, argv);
        }
    }
    return (retval);
}
/*-------------------------------------------------------------------------*/

void err_setup(char select, char* string)
/*
 * activation for the max errs argument
 */
{
    int n;
    (void)select;
    if (*string == '+')
    {
        cparams.prm_extwarning = true;
        string++;
    }
    else if (*string == '-')
    {
        cparams.prm_warning = false;
        string++;
    }
    n = atoi(string);
    if (n > 0)
        cparams.prm_maxerr = n;
    DisableTrivialWarnings();
}
void warning_setup(char select, char* string)
{
    if (string[0] == 0)
        AllWarningsDisable();
    else
        switch (string[0])
        {
            case '+':
                cparams.prm_extwarning = true;
                DisableTrivialWarnings();
                break;
            case 'd':
                DisableWarning(atoi(string + 1));
                break;
            case 'o':
                WarningOnlyOnce(atoi(string + 1));
                break;
            case 'x':
                AllWarningsAsError();
                break;
            case 'e':
                if (!strcmp(string, "error"))
                    AllWarningsAsError();
                else
                    WarningAsError(atoi(string + 1));
                break;
            default:
                EnableWarning(atoi(string));
                break;
        }
}

/*-------------------------------------------------------------------------*/

void sysincl_setup(char select, char* string)
{
    (void)select;
    if (sys_searchpath)
    {
        sys_searchpath = (char*)realloc(sys_searchpath, strlen(string) + strlen(sys_searchpath) + 2);
        strcat(sys_searchpath, ";");
    }
    else
    {
        sys_searchpath = (char*)malloc(strlen(string) + 1);
        sys_searchpath[0] = 0;
    }
    fflush(stdout);
    strcat(sys_searchpath, string);
}
void incl_setup(char select, char* string)
/*
 * activation for include paths
 */
{
    (void)select;
    if (*set_searchpath)
    {
        *set_searchpath = (char*)realloc(*set_searchpath, strlen(string) + strlen(*set_searchpath) + 2);
        strcat(*set_searchpath, ";");
    }
    else
    {
        *set_searchpath = (char*)malloc(strlen(string) + 1);
        *set_searchpath[0] = 0;
    }
    fflush(stdout);
    strcat(*set_searchpath, string);
}
void libpath_setup(char select, char* string)
{
    (void)select;
    if (*set_libpath)
    {
        *set_libpath = (char*)realloc(*set_libpath, strlen(string) + strlen(*set_libpath) + 2);
        strcat(*set_libpath, ";");
    }
    else
    {
        *set_libpath = (char*)malloc(strlen(string) + 1);
        *set_libpath[0] = 0;
    }
    fflush(stdout);
    strcat(*set_libpath, string);
}
void tool_setup(char select, char* string)
{
    char buf[2048];
    buf[0] = '$';
    strcpy(buf + 1, string);
    InsertAnyFile(buf, 0, -1, false);
}
/*-------------------------------------------------------------------------*/

void def_setup(char select, char* string)
/*
 * activation for command line #defines
 */
{
    defines.push_back(DefValue{string, 0});
}

void undef_setup(char select, char* string) { defines.push_back(DefValue{string, 1}); }

/*-------------------------------------------------------------------------*/

void output_setup(char select, char* string)
{
    (void)select;
    strcpy(outfile, string);
    has_output_file = true;
}

/*-------------------------------------------------------------------------*/

void setglbdefs(void)
/*
 * function declares any global defines from the command line and also
 * declares a couple of other macros so we can tell what the compiler is
 * doing
 */
{
    char buf[256];
    int major, temp, minor, build;
    ARCH_DEFINES* a = chosenAssembler->defines;
    sscanf(STRING_VERSION, "%d.%d.%d.%d", &major, &temp, &minor, &build);
    my_sprintf(buf, "%d", major * 100 + minor);
    preProcessor->Define("__ORANGEC__", buf);
    my_sprintf(buf, "%d", major);
    preProcessor->Define("__ORANGEC_MAJOR__", buf);
    my_sprintf(buf, "%d", minor);
    preProcessor->Define("__ORANGEC_MINOR__", buf);
    my_sprintf(buf, "%d", build);
    preProcessor->Define("__ORANGEC_PATCHLEVEL__", buf);
    sprintf(buf, "\"%s\"", STRING_VERSION);
    preProcessor->Define("__VERSION__", buf);
    preProcessor->Define("__CHAR_BIT__", "8");
    if (cparams.prm_cplusplus)
    {
        preProcessor->Define("__cplusplus", "201402");
        if (cparams.prm_xcept)
            preProcessor->Define("__RTTI__", "1");
    }
    preProcessor->Define("__STDC__", "1");

    if (cparams.prm_c99 || cparams.prm_c1x)
    {
        preProcessor->Define("__STDC_HOSTED__", chosenAssembler->hosted);  // hosted compiler, not embedded
    }
    if (cparams.prm_c1x)
    {
        preProcessor->Define("__STDC_VERSION__", "201112L");
    }
    else if (cparams.prm_c99)
    {
        preProcessor->Define("__STDC_VERSION__", "199901L");
    }
    /*   preProcessor->Define("__STDC_IEC_599__","1");*/
    /*   preProcessor->Define("__STDC_IEC_599_COMPLEX__","1");*/
    /*   preProcessor->Define("__STDC_ISO_10646__","199712L");*/
/*    preProcessor->Define(preProcessor->Define, "");*/
    if (a)
    {
        while (a->define)
        {
            if (a->respect)
            {
                preProcessor->Define(a->define, a->value);
            }
            a++;
        }
    }
    for (auto d : defines)
    {
        size_t n = d.name.find_first_of("=");
        std::string name, val;
        if (n != std::string::npos)
        {
            name = d.name.substr(0, n);
            if (n != d.name.size() - 1)
                val = d.name.substr(n + 1);
        }
        else
        {
            name = d.name;
            val = "1";
        }
        if (d.undef)
            preProcessor->Undefine(name);
        else
            preProcessor->Define(name.c_str(), val);
    }
}

/*-------------------------------------------------------------------------*/

void InsertOneFile(char* filename, char* path, int drive, bool primary)
/*
 * Insert a file name onto the list of files to process
 */

{
    char a = 0;
    char *newbuffer, buffer[260], *p = buffer;
    bool inserted;
    LIST **r = &clist, *s;

    if (drive != -1)
    {
        *p++ = (char)(drive + 'A');
        *p++ = ':';
    }
    if (path)
    {
        strcpy(p, path);
        //        strcat(p, "\\");
    }
    else
        *p = 0;
    /* Allocate buffer and make .C if no extension */
    strcat(buffer, filename);
    if (buffer[0] == '-')
    {
        a = buffer[0];
        buffer[0] = 'a';
    }
    inserted = chosenAssembler->insert_noncompile_file && chosenAssembler->insert_noncompile_file(buffer, primary);
    if (a)
        buffer[0] = a;
    if (!inserted)
    {
        AddExt(buffer, ".c");
        newbuffer = (char*)malloc(strlen(buffer) + 1);
        if (!newbuffer)
            return;
        strcpy(newbuffer, buffer);

        while ((*r))
            r = &(*r)->next;
        (*r) = (LIST*)malloc(sizeof(LIST));
        s = (*r);
        if (!s)
            return;
        s->next = 0;
        s->data = newbuffer;
    }
}
void InsertAnyFile(char* filename, char* path, int drive, bool primary)
{
    char drv[256], dir[256], name[256], ext[256];
#if defined(_MSC_VER) || defined(BORLAND) || defined(__ORANGEC__)
    struct _finddata_t findbuf;
    size_t n;
    _splitpath(filename, drv, dir, name, ext);
    n = _findfirst(filename, &findbuf);
    if (n != -1)
    {
        do
        {
            InsertOneFile(findbuf.name, dir[0] ? dir : 0, drv[0] ? tolower(drv[0]) - 'a' : -1, primary);
        } while (_findnext(n, &findbuf) != -1);
        _findclose(n);
    }
    else
    {
        InsertOneFile(filename, path, drive, primary);
    }
#else
    InsertOneFile(filename, path, drive, primary);
#endif
}
/*-------------------------------------------------------------------------*/

void dumperrs(FILE* file);
void setfile(char* buf, const char* orgbuf, const char* ext)
/*
 * Get rid of a file path an add an extension to the file name
 */
{
    const char* p = strrchr(orgbuf, '\\');
    const char* p1 = strrchr(orgbuf, '/');
    if (p1 > p)
        p = p1;
    else if (!p)
        p = p1;
    if (!p)
        p = orgbuf;
    else
        p++;
    strcpy(buf, p);
    StripExt(buf);
    strcat(buf, ext);
}

/*-------------------------------------------------------------------------*/

void outputfile(char* buf, const char* orgbuf, const char* ext)
{

    if (buf[strlen(buf) - 1] == '\\')
    {
        const char* p = strrchr(orgbuf, '\\');
        if (p)
            p++;
        else
            p = orgbuf;
        strcat(buf, p);
        StripExt(buf);
        AddExt(buf, ext);
    }
    else if (has_output_file)
    {
        AddExt(buf, ext);
    }
    else
    {
        setfile(buf, orgbuf, ext);
    }
}

/*-------------------------------------------------------------------------*/

void scan_env(char* output, char* string)
{
    char name[256], *p;
    while (*string)
    {
        if (*string == '%')
        {
            p = name;
            string++;
            while (*string && *string != '%')
                *p++ = *string++;
            if (*string)
                string++;
            *p = 0;
            p = getenv(name);
            if (p)
            {
                strcpy(output, p);
                output += strlen(output);
            }
        }
        else
            *output++ = *string++;
    }
    *output = 0;
}

/*-------------------------------------------------------------------------*/

int parse_arbitrary(char* string)
/*
 * take a C string and and convert it to ARGC, ARGV format and then run
 * it through the argument parser
 */
{
    char* argv[40];
    char output[1024];
    int rv, i;
    int argc = 1;
    if (!string || !*string)
        return 1;
    scan_env(output, string);
    string = output;
    while (1)
    {
        int quoted = ' ';
        while (*string == ' ')
            string++;
        if (!*string)
            break;
        if (*string == '\"')
            quoted = *string++;
        argv[argc++] = string;
        while (*string && *string != quoted)
            string++;
        if (!*string)
            break;
        *string = 0;
        string++;
    }
    rv = parse_args(&argc, argv, true);
    for (i = 1; i < argc; i++)
        InsertAnyFile(argv[i], 0, -1, true);
    return rv;
}

/*-------------------------------------------------------------------------*/

void parsefile(char select, char* string)
/*
 * parse arguments from an input file
 */
{
    FILE* temp = fopen(string, "r");
    (void)select;
    if (!temp)
        fatal("Response file not found");
    while (!feof(temp))
    {
        char buf[256];
        buf[0] = 0;
        fgets(buf, 256, temp);
        if (buf[strlen(buf) - 1] == '\n')
            buf[strlen(buf) - 1] = 0;
        if (!parse_arbitrary(buf))
            break;
    }
    fclose(temp);
}

/*-------------------------------------------------------------------------*/

void addinclude(void)
/*
 * Look up the INCLUDE environment variable and append it to the
 * search path
 */
{
#ifdef COLDFIRE
    char* string = getenv("cfccincl");
#else
    char* string = getenv("CCINCL");
#endif
    if (string && string[0])
    {
        char temp[1000];
        strcpy(temp, string);
        if (*set_searchpath)
        {
            strcat(temp, ";");
            strcat(temp, *set_searchpath);
            free(*set_searchpath);
        }
        *set_searchpath = (char*)malloc(strlen(temp) + 1);
        strcpy(*set_searchpath, temp);
    }
    string = getenv("CPATH");
    if (string && string[0])
    {
        char temp[1000];
        strcpy(temp, string);
        if (*set_searchpath)
        {
            strcat(temp, ";");
            strcat(temp, *set_searchpath);
            free(*set_searchpath);
        }
        *set_searchpath = (char*)(strlen(temp) + 1);
        strcpy(*set_searchpath, temp);
    }
}

/*-------------------------------------------------------------------------*/

int parseenv(const char* name)
/*
 * Parse the environment argument string
 */
{
    char* string = getenv(name);
    return parse_arbitrary(string);
}

/*-------------------------------------------------------------------------*/

int parseconfigfile(char* name)
{
    char buf[256], *p;
    if (!chosenAssembler->cfgname)
        return 0;
    strcpy(buf, name);
    p = strrchr(buf, '\\');
    if (p)
    {
        FILE* temp;
        strcpy(p + 1, chosenAssembler->cfgname);
        strcat(p, ".CFG");
        temp = fopen(buf, "r");
        if (!temp)
            return 0;
        set_searchpath = &sys_searchpath;
        while (!feof(temp))
        {
            buf[0] = 0;
            fgets(buf, 256, temp);
            if (buf[strlen(buf) - 1] == '\n')
                buf[strlen(buf) - 1] = 0;
            if (!parse_arbitrary(buf))
                break;
        }
        set_searchpath = &prm_searchpath;
        fclose(temp);
    }
    return 0;
}

/*-------------------------------------------------------------------------*/

void dumperrs(FILE* file)
{
    if (cparams.prm_listfile)
    {
        fprintf(listFile, "******** Global Symbols ********\n");
        list_table(globalNameSpace->valueData->syms, 0);
        fprintf(listFile, "******** Global Tags ********\n");
        list_table(globalNameSpace->valueData->tags, 0);
    }
    if (diagcount && !total_errors)
        fprintf(file, "%d Diagnostics\n", diagcount);
    if (total_errors)
        fprintf(file, "%d Errors\n", total_errors);
}

/*-------------------------------------------------------------------------*/

void ctrlchandler(int aa)
{
    fprintf(stderr, "^C");
    extern void Cleanup();
    Cleanup();
    exit(1);
}

/*-------------------------------------------------------------------------*/

void internalError(int a)
{
    (void)a;
    extern void Cleanup();
    Cleanup();
    fprintf(stderr, "Internal Error - Aborting compile");
    exit(1);
}

/*-------------------------------------------------------------------------*/
void ccinit(int argc, char* argv[])
{
    char buffer[260];
    char* p;
    int rv;
    int i;

    strcpy(copyright, COPYRIGHT);
    strcpy(version, STRING_VERSION);

    outfile[0] = 0;
    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-' || argv[i][0] == '/')
        {
            if (argv[i][1] == '!' || !strcmp(argv[i], "--nologo"))
            {
                showBanner = false;
            }
            else if ((argv[i][1] == 'V' && argv[i][2] == 0) || !strcmp(argv[i], "--version"))
            {
                showVersion = true;
            }
        }

    if (showBanner || showVersion)
    {
        banner("%s Version %s %s", chosenAssembler->progname, version, copyright);
    }
    if (showVersion)
    {
        fprintf(stderr, "Compile date: " __DATE__ ", time: " __TIME__ "\n");
        exit(0);
    }
#if defined(WIN32) || defined(MICROSOFT)
    GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
#else
    strcpy(buffer, argv[0]);
#endif

    if (!getenv("ORANGEC"))
    {
        char* p = strrchr(buffer, '\\');
        if (p)
        {
            *p = 0;
            char* q = strrchr(buffer, '\\');
            if (q)
            {
                *q = 0;
                char* buf1 = (char*)calloc(1, strlen("ORANGEC") + strlen(buffer) + 2);
                strcpy(buf1, "ORANGEC");
                strcat(buf1, "=");
                strcat(buf1, buffer);
                putenv(buf1);
                *q = '\\';
            }
            *p = '\\';
        }
    }
    DisableTrivialWarnings();
    /* parse the environment and command line */
    if (chosenAssembler->envname && !parseenv(chosenAssembler->envname))
        usage(argv[0]);

    parseconfigfile(buffer);
    if (!parse_args(&argc, argv, true) || (!clist && argc == 1))
        usage(argv[0]);

    /* tack the environment includes in */
    addinclude();

    /* Scan the command line for file names or response files */
    {
        int i;
        for (i = 1; i < argc; i++)
            if (argv[i][0] == '@')
                parsefile(0, argv[i] + 1);
            else
                InsertAnyFile(argv[i], 0, -1, true);
    }

#ifndef PARSER_ONLY

    if (has_output_file)
    {
        if (chosenAssembler->insert_output_file)
            chosenAssembler->insert_output_file(outfile);
        if (!cparams.prm_compileonly)
        {
            has_output_file = false;
        }
        else
        {
            if (clist && clist->next && outfile[strlen(outfile) - 1] != '\\')
                fatal("Cannot specify output file for multiple input files\n");
        }
    }
#else
    {
        LIST* t = clist;
        while (t)
        {
            t->data = litlate(Utils::FullQualify((char*)t->data));
            t = t->next;
        }
    }
#endif

    /* Set up a ctrl-C handler so we can exit the prog with cleanup */
    signal(SIGINT, ctrlchandler);
    //    signal(SIGSEGV, internalError);
}
