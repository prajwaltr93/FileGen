//filename:filegen.c
//author : Prajwal T R
//data last modified : 11/02/2020
//comments : generate files with header documentation

#include<stdio.h>
#include<stdlib.h>
#include<argp.h>
#include<string.h>
#include<sys/utsname.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include<utmpx.h>
#include<paths.h>

//constants
#define MAXEXTSIZE 7 //exetension for file max length
#define STRUCT_ELE sizeof(comments)/sizeof(comments[0])
#define PROP_SIZE  sizeof(prop)/sizeof(prop[0])
#define MAXLINELEN 400
//function prototypes for filegen.c
void *getextension(char *filename);
void *getcomment(char *extension);
void writeline(int fd_open,char *comment,char *prop,char *value,char *filename);
//comment struct for each extension
struct
{
        char *ext;
        char *cmnt;
} comments[] =
{
        {"py","#"},
        {"c","//"},
        {"java","//"}
};
//messages for struct argp
const char *argp_program_version = "filegen 1.0";
const char *argp_program_bug_version = "github/prajwaltr93/FileGen/";
static char doc[] = "file header generation command";
static char args_doc[] = "filename";
//key short-options flags docs
static struct argp_option options[] =
{
{ "silent",'s',0,0,"Don't open default editor Only file generation with header information"},
{ "comment",'c',"COM",0,"add comment section of file header"},
{ "author",'a',"AUTHR",0,"Specify author name otherthan username specified by operating system"},
{0},//skip filename argument in help message
};
//store values parsed argp_parse
struct arguments
{
    char *argv[1];
    int silent;
    char *comment,*author;
};
//argp parser ,parse each key
static error_t parse_opt (int key,char *arg,struct argp_state *state)
{
    struct arguments *arguments = state->input;

    switch(key)
    {
        case 's':
            arguments->silent = 1;
            break;
        case 'c':
            arguments->comment = arg;
            break;
        case 'a':
            arguments->author = arg;
            break;
        case ARGP_KEY_ARG:
            if(state->arg_num >= 1)
            {
                    argp_usage(state); //too many arguments
            }
            arguments->argv[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if(state->arg_num < 1)
            {
                    argp_usage(state); //less arguments
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0; // return 0 to enusre no error has occured : imp

}

//argp parser
static struct argp argp = {options,parse_opt,args_doc,doc};
//file gen program
int main(int argc,char **argv)
{
    // values to write to files
    char *prop[] = {"filename : ","author : ","date last modified : ","comments : "};
    //rows properties to write to files
    char *values[PROP_SIZE];
    //struct for argument parsing
    struct arguments arguments;
    //date variables
    time_t rawtime;
    struct tm *info;
    time(&rawtime); //populate rawtime
    info = localtime(&rawtime); //get all date attributes by passing seconds epoch
    char *filename,*ext,*comment,*date,*comments;
    char author[100]; //replace constant size with sys defined max length
    void *temp;
    //file descriptor for file to open
    int fd_open;
    //file descriptor for utmp file
    int fd_utmp;
    //utmp struct to refer while reading from utmp file
    struct utmpx utmp_data;
    //intialise struct values with defaults
    arguments.silent = 0;
    arguments.comment = " ";
    arguments.author = "";
    //parse all arguments and initialise variables
    argp_parse(&argp,argc,argv,0,0,&arguments);
    //write to file
    filename = arguments.argv[0];
    temp = getextension(filename);
    ext = temp ? (char *)temp : " ";
    temp = getcomment(ext);
    comment = temp ? (char *)temp : "//";
    strcpy(author,arguments.author); //get author name from command line arguments
    comments = arguments.comment; //get comments about file from arguments
    //get uname of node if author value not specified
    if(strlen(author) == 0) //author not specified in command line argument
    {
      fd_utmp = open(_PATH_UTMP,O_RDONLY); //_PATH_UTMP /var/run/utmp
      if(fd_utmp != -1)
      {
        //this logic works on assumption that username is the last entry in utmp file , bad logic : replace
        while(read(fd_utmp,&utmp_data,sizeof(struct utmpx)) != 0) //till end of file
        {
          strcpy(author,utmp_data.ut_user);
        }
      }//else no author value is written ex : " "
    }
    //get current date
    date = asctime(info); //get date string
    date[strlen(date)-1] = '\0'; //remove newline at end
    //initiate value array todo: rewrite logic
    *(values) = filename;
    *(values+1) = author;
    *(values+2) = date;
    *(values+3) = comments;
    //open file and write values
    fd_open = open(filename,O_WRONLY | O_CREAT | O_EXCL,0666); //todo : Get file create permission from system
    if(fd_open == -1)
    {
      perror(filename); //file already exists
      exit(1);
    }
    for(int i=0;i<PROP_SIZE;++i)
    {
			writeline(fd_open,comment,prop[i],values[i],filename);
    }
    close(fd_open);
    //todo : fork and execute editor specified default : vi
    return 0;
}
//get file extension from filename specified ex : .py
void *getextension(char *filename)
{
    int limit = strlen(filename);
    static char ext[MAXEXTSIZE];
    int i,k;
    for(i=0;i<limit;++i)
    {
        if(*(filename+i) == '.')
        {
            for(i=i+1,k=0;i<limit;++k,++i)
            {
                *(ext+k) = *(filename+i); //get everything after '.'
            }

        break;
        }
    }
    if(strlen(ext) == 0)
    {
        return NULL;
    }
    else
    {
        return ext;
    }
}
void *getcomment(char *ext)
{
    int i;
    //perform linear search replace with hashtable
    for(i=0;i<STRUCT_ELE;++i)
    {
            if(strcmp(ext,comments[i].ext) == 0)
            {
                    return comments[i].cmnt;
            }
    }
    return NULL;
}
void writeline(int fd_open,char *comment,char *prop,char *value,char *filename)
{
	char wrtstrng[MAXLINELEN];
  int res;
	strcpy(wrtstrng,comment);
	strcat(wrtstrng,prop);
	strcat(wrtstrng,value);
  strcat(wrtstrng,"\n");
	res = write(fd_open,wrtstrng,strlen(wrtstrng)*sizeof(char));
  if(res == -1)
  {
    //remove file not required
    close(fd_open);
    remove(filename);
    //error during writing to a file
    perror("filegen");
    exit(1);
  }
}
