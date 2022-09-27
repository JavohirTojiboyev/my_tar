#include "my_tar.h"
#include <stdint.h>
#include <stdio.h>  //#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Conversions and supporting functions section */
char* my_itoa(char *res, unsigned int num, int size, int base)
{
    const char symbols[] = "0123456789ABCDEF";
    memset(res, '0', size - 1);
    res[size - 1] = '\0';
    size--;

    while(num)
    {
        res[--size] = symbols[num % base];
        num /= base;
    }

    return res;
}

int convertDecimalToOctal(int decimalNumber)
{
    int octalNumber = 0, i = 1;

    while (decimalNumber != 0)
    {
        octalNumber += (decimalNumber % 8) * i;
        decimalNumber /= 8;
        i *= 10;
    }

    return octalNumber;
}

int     ft_strlen(char *str)
{
        int i = 0;
        while (str[i] != '\0')
                i++;
        return (i);
}

int     absolute_value(int nbr)
{
    if (nbr < 0)
        return (-nbr);
    return (nbr);
}
 
int     get_len(int nbr)
{
    int len = 0;
    if (nbr <= 0)
        ++len;
    while (nbr != 0)
    {
        ++len;
        nbr = nbr / 10;
    }
    return (len);
}
 
char    *ft_itoa(int nbr)
{
    char *result;
    int len;
 
    len = get_len(nbr);
    result = malloc(sizeof(char) * (len + 1));
    result[len] = '\0';
 
    if (nbr < 0)
        result[0] = '-';
    else if (nbr == 0)
        result[0] = '0';
 
    while (nbr != 0)
    {
        --len;
        result[len] = absolute_value(nbr % 10) + '0';
        nbr = nbr / 10;
    }
    return (result);
}

//calculates; return value = num^pow
long int ft_power(int num, int pow)
{
    long int result = num;
    int i = 1;
    if (pow == 0)
        return (1);
    while (i < pow)
    {
        result *= num;
        i++;
    }
    return (result);
}

//take an octal in string form and coverts it decimal long int
//previous function I used was giving incorrect results for time last modified
long int ft_atoi_octal_to_decimal(char *str)
{
    int i = 0;
    int len = ft_strlen(str);
    long int result = 0;
    int j;
    long int tmp;
    while (i < len)
    {
        j = len - (i + 1);
        tmp = ft_power(8, j) * ((int)str[i] - '0');
        result += tmp;
        i++;
    }
    return (result);
}

long char_to_long(char *arg) {
    long result = 0;
    for (int i = 0; arg[i] != '\0'; ++i) {
        result = result * 10 + arg[i] - '0'; 
    }
    return result;
}

/* Options section */
int is_arguments(arguments *args)
{
    if(args->name != NULL) {return 1;}
    return 0;
}

arguments *init_argument()
{
    arguments *args = malloc(sizeof(arguments));
    bzero(args, sizeof(arguments));
    args->name = NULL;
    return args;
}

static tar_opts *init_tar_opts(void)
{
    tar_opts *options = malloc(sizeof(tar_opts));
    options->create = false;
    options->extract = false;
    options->append = false;
    options->list = false;
    options->update = false;
    options->args = init_argument();
    options->archive_name = NULL;

    return options;
}

static tar_opts *handle_options(tar_opts *options, char *argument, bool *is_archive_name)
{
    int i = 0;
    while(argument[i])
    {
        if(argument[i] == 'c') {options->create = true;}
        else if(argument[i] == 'x') {options->extract = true;}
        else if(argument[i] == 'f') {*is_archive_name = true;}
        else if(argument[i] == 'r') {options->append = true;}
        else if(argument[i] == 't') {options->list = true;}
        else if(argument[i] == 'u') {options->update = true;}
        i++;
    }

    return options;
}

static bool validate_options(tar_opts *options)
{
    int check = 0;
    if(!options->create && !options->extract && !options->list && !options->append && !options->update)
    {
        printf("tar: Must specify one of -c, -r, -t, -u, -x\n");
        return false;
    }

    if(options->append) {check++;}
    if(options->create) {check++;}
    if(options->list) {check++;}
    if(options->update) {check++;}
    if(options->extract) {check++;}

    if(check > 1)
    {
        printf("tar: Must specify only one of -c, -r, -t, -u, -x\n");
        return false;
    }

    if(options->archive_name == NULL) {
        printf("tar: Must specify a filename\n");
        return false;
    }

    return true;
}

void add_to_list(arguments **arg, char *av)
{
    arguments *new = init_argument();
    new->next = *arg;
    new->name = (char*)malloc(sizeof(char) * ((strlen(av)) + 1));
    strcpy(new->name, av);
    *arg = new;
}

tar_opts *get_options(int ac, char **av)
{
    if(ac < 2) {printf("%s: No arguments specified", av[0]); return NULL;}

    tar_opts *options = init_tar_opts();
    int ptr = 1;
    bool is_archive_name = false;

    while(av[ptr])
    {
        if(av[ptr][0] == '-') {handle_options(options, av[ptr], &is_archive_name);}
        else if(is_archive_name == true) {
            options->archive_name = strdup(av[ptr]);
            is_archive_name = false;
        }
        ptr++;
    }

    while(!is_same(options->archive_name, av[ac-1]))
    {
        add_to_list(&(options)->args, av[ac-1]);
        ac--;
    }

    if(validate_options(options) != true)
    {
        delete_options(options);
        return NULL;
    }

    return options;
}

void delete_options(tar_opts *options)
{
    if(options->archive_name != NULL) {free(options->archive_name);}
    delete_argument_list(&(options)->args);
    free(options);
    return;
}

void delete_argument_list(arguments **args)
{
    arguments* current = *args; 
    arguments* next; 
    while (current != NULL)
    { 
       next = current->next;
       free(current->name);
       free(current); 
       current = next; 
    } 
   *args = NULL;
}

/* File manipulation section */
int open_archive_file(tar_opts *options)
{
    int fd_archive, flags;
    const mode_t DEFAULT_CREAT_MODE = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    if(options->create) {flags = O_CREAT | O_WRONLY | O_TRUNC;}
    else if(options->extract) {flags = O_RDONLY;}
    else {flags = O_RDWR;}

    fd_archive = open(options->archive_name, flags, DEFAULT_CREAT_MODE);
    return fd_archive;
}

/* Create archive section */
tar_header *init_tar_header()
{
    tar_header *header = malloc(BLOCKSIZE);
    if(header != NULL) {
        bzero(header, BLOCKSIZE);
    }
    return header;
}

char get_typeflag(int mode)
{
    switch (mode & S_IFMT)
    {
        case S_IFREG: return REGTYPE;
        case S_IFLNK: return SYMTYPE;
        case S_IFCHR: return CHRTYPE;
        case S_IFBLK: return BLKTYPE;
        case S_IFDIR: return DIRTYPE;
        case S_IFIFO: return FIFOTYPE;
        default: return REGTYPE;
    }
}

int add_to_chksum(char *str)
{
    int i = 0;
    int result = 0;
    while (str[i])
    {
        result += (int)str[i];
        i++;
    }

	return (result);
}

tar_header *get_metadata(char *filename)
{
    struct stat info;
    struct group *grp;
    struct passwd *p;
    int checkValue = 0;
    if(stat(filename, &info) < 0) {printf("tar: %s: Cannot stat: No such file or directory\n", filename); return NULL;}

    tar_header *header = init_tar_header();
    strncpy(header->name, filename, 100);
    checkValue += add_to_chksum(header->name);
    my_itoa(header->mode, info.st_mode, 8, OCTAL_BASE);
    checkValue += add_to_chksum(header->mode);
    my_itoa(header->uid, info.st_uid, 8, OCTAL_BASE);
    checkValue += add_to_chksum(header->uid);
    my_itoa(header->gid, info.st_gid, 8, OCTAL_BASE);
    checkValue += add_to_chksum(header->gid);
    my_itoa(header->size, info.st_size, 12, OCTAL_BASE);
    checkValue += add_to_chksum(header->size);
    my_itoa(header->mtime, info.st_mtime, 12, OCTAL_BASE);
    checkValue += add_to_chksum(header->mtime);
    checkValue += add_to_chksum("       ");// 7 ' ' space values substituted for chksum
    header->typeflag = get_typeflag(info.st_mode);
    checkValue += (int)header->typeflag;
    strncpy(header->magic, TMAGIC, TMAGLEN);
    checkValue += add_to_chksum(header->magic);
    //strncpy(header->version, TVERSION, TVERSLEN);//Use this line if your comment out "checkValue += (int)header->typeflag;"
    strncpy(header->version, " ", 2); //-Wall -Werror -Wextra throws error warnings for 334, but you could ignore them and not use line 335 here
    checkValue += add_to_chksum("00");

    if (readlink(filename, header->linkname, 100) > 0)
    {
        header->typeflag = '2';
        checkValue += add_to_chksum(header->linkname);
    }

    p = getpwuid(info.st_uid);
    strncpy(header->uname, p->pw_name, 32);
    checkValue += add_to_chksum(header->uname);
    grp = getgrgid(info.st_gid);
    strncpy(header->gname, grp->gr_name, 32);
    checkValue += add_to_chksum(header->gname);

    if (header->typeflag == '5')
    {
        my_itoa(header->devmajor, MAJOR(info.st_rdev), sizeof(header->devmajor), OCTAL_BASE);
        checkValue += add_to_chksum(header->devmajor);
	    my_itoa(header->devminor, MINOR(info.st_rdev), sizeof(header->devminor), OCTAL_BASE);
        checkValue += add_to_chksum(header->devminor);
    }

    char *checkStr = ft_itoa(convertDecimalToOctal(checkValue));
    char tmp[8];
    int i = 0;
    int j = 0;

    while (i + ft_strlen(checkStr) < 6)
    {
        tmp[i] = '0';
        i++;
    }

    while (i < 6)
    {
        tmp[i] = checkStr[j];
        i++;
        j++;
    }

    tmp[6] = ' ';
    tmp[7] = '\0';
    strncpy(header->chksum, tmp, 8);
    free(checkStr);
    return header;
}

int write_file_content(int fd_archive, char *filename, int fsize)
{
    int res = 0;
    int fd_file = open(filename, O_RDONLY);

    char content[BLOCKSIZE];
    bzero(content, BLOCKSIZE);
    
    while (fsize > BLOCKSIZE)
    {
        read(fd_file, content, BLOCKSIZE);
        write(fd_archive, content, BLOCKSIZE);
        fsize -= BLOCKSIZE;
        res += BLOCKSIZE;
    }

    if (fsize > 0)
    {
        bzero(content, BLOCKSIZE);
        read(fd_file, content, fsize);
        write(fd_archive, content, BLOCKSIZE);
        res += BLOCKSIZE;
    }

    close(fd_file);
    return res;
}

void end_of_archive(int fd)
{
    char end[BLOCKSIZE];
    bzero(end, BLOCKSIZE);
    write(fd, end, BLOCKSIZE);
    write(fd, end, BLOCKSIZE);
    return;
}

int create_archive(int fd_archive, tar_opts *options)
{
    int res = 0;
    arguments *current_arg = options->args;

    while(is_arguments(current_arg))
    {
        tar_header *header;
        header = get_metadata(current_arg->name);
        if(header != NULL)
        {
            write(fd_archive, header, BLOCKSIZE);
            if (header->typeflag != '5' && header->typeflag != '2')
                write_file_content(fd_archive, current_arg->name, (ft_atoi_octal_to_decimal(header->size) + 1));
            free(header);
        }
        else{res++;}
        current_arg = current_arg->next;
    }

    end_of_archive(fd_archive);
    return res;
}

/* Extract archive section */
int get_archive_size(int fd_archive)
{
    int res = 0, ptr = 0;
    char buf[BLOCKSIZE];
    while((ptr = read(fd_archive, buf, BLOCKSIZE)) > 0) {
        res += ptr;
    }
    return res;
}

char *get_archive_content(int fd_archive, char *dst)
{
    struct stat info;
    if(fstat(fd_archive, &info) < 0) {printf("Error opening file\n"); return 0;}
    int size = info.st_size;
    lseek(fd_archive, 0, SEEK_SET);

    read(fd_archive, dst, size);
    return dst;
}

int is_header(tar_header *header)
{
    int fd = open(header->name, O_RDONLY);
    if(fd >= 0) {
        close(fd);
        return 1;
    }
    else {
        return 0;
    }
}

//ft_strseccpy grabs a particular section of *src(a string/block) and sends it to *dest 
//dest has to have already been maaloc or declare (example dst[512])
char *ft_strseccpy(char *dest, char *src, unsigned int i, unsigned int n)
{
    unsigned int j = 0;
	while (src[i] != '\0' && (i < n))
	{
		dest[j] = src[i];
		i++;
        j++;
	}
	dest[j] = '\0';
	return (dest);
}

char *isNullBlock(char *block)
{
    int i = 0;
    while (block[i] == '\0' && i < 512)
        i++;
    if (i == 512)
        return ("1");
    else
        return ("0");
}

int older_exist(char* fname)
{
    struct stat info;
    if(stat(fname, &info) < 0)
        return (0);
    else {
        return (1);
    }
}

int more_rescent_exist(char* fname, char* sname)
{
    struct stat info;
    if(stat(fname, &info) < 0)
        return (0);
    char* existing_file_time = malloc(sizeof(char) * 12);
    my_itoa(existing_file_time, info.st_mtime, 12, OCTAL_BASE);
    int result = strcmp(sname, existing_file_time);
    free(existing_file_time);
    if (result < 0)
    {
        return (1);
    }
    else
        return (0);
}

char* getCommand(char* Cchar, char* fname)
{
    char* result = (char*)malloc(sizeof(char) * (ft_strlen(Cchar) + ft_strlen(fname) + 1));
    int i = 0;
    int j = 0;
    while (Cchar[j] != '\0')
    {
        result[i] = Cchar[j];
        i++;
        j++;
    }
    j = 0;
    while (fname[j] != '\0')
    {
        result[i] = fname[j];
        i++;
        j++;
    }
    result[i] = '\0';
    return (result);
}

//this is the function that extract the files from the archive
int extract_archive(int fd_archive)
{
    char *content = (char*)malloc(sizeof(char) * 512);
    bzero(content, 512);
    read(fd_archive, content, 512);
    char fname[512];//the array size has to be declared in this line
    int fd;
    int i;
    int fsize;
    char sname[512];//the array size has to be declared in this line
    int fmode;
    while (content[0] != '\0')
    {
        int len = ft_strlen(content);
        i = 0;
        while (i < len)
        {
            fname[i] = content[i];
            i++;
        }
        fname[i] = '\0';
        ft_strseccpy(sname, content, 136, 148);
        if (more_rescent_exist(fname, sname) == 1)
        {
            ft_strseccpy(sname, content, 124, 136);
            fsize = ft_atoi_octal_to_decimal(sname);
            while (fsize > 0)
            {
                read(fd_archive, content, 512);
                fsize -= 512;
            }
        }
        else
        {
            if (older_exist(fname))
            {
                char *command = getCommand("rm -rf ", fname);
                system(command);
                free (command);
            }
            ft_strseccpy(sname, content, 100, 107);
            fmode = ft_atoi_octal_to_decimal(sname);
            if (content[156] == '5')
            {
                mkdir(fname, fmode);
            }
            else if (content[156] == '2')
            {
                ft_strseccpy(sname, content, 157, 257);
                symlink(sname, fname);
            }
            else
            {
                fd = open(fname, O_RDWR | O_CREAT | O_APPEND, fmode);
                struct utimbuf ut;
                ft_strseccpy(sname, content, 136, 148);
                ut.actime = ft_atoi_octal_to_decimal(sname);
                ut.modtime = ft_atoi_octal_to_decimal(sname);
                ft_strseccpy(sname, content, 124, 136);
                fsize = ft_atoi_octal_to_decimal(sname);
                while (fsize > 512 )
                {
                    read(fd_archive, content, 512);
                    write(fd, content, 512);
                    fsize -= 512;
                }
                read(fd_archive, content, 512);
                write(fd, content, fsize);
                if( utime( fname, &ut ) == -1 )
                    printf( "utime failed\n" );
            }
            close(fd);
        }
        read(fd_archive, content, 512);
    }
    if (content[0] != '\0')
        printf("A NullBlock was not detected %d\n", i);
    read(fd_archive, content, 512);
    if (content[0] != '\0')
        printf("A NullBlock was not detected %d\n", i);
    free(content);
    return (0);
}

/* List archive contents section */
int list_archive_contents(int fd_archive)
{
    int res = 0;
    char *content = (char*)malloc(sizeof(char) * 512);
    char sname[512];
    int fsize;
    bzero(content, 512);
    read(fd_archive, content, 512);
    while (content[0] != '\0')
    {
        printf("%s\n", content);
        ft_strseccpy(sname, content, 136, 148);
        if ((content[156] != '5') && (content[156] != '2'))
        {
            ft_strseccpy(sname, content, 124, 136);
            fsize = ft_atoi_octal_to_decimal(sname);
            while (fsize > 512)
            {
                read(fd_archive, content, 512);
                fsize -= 512;
            }
            read(fd_archive, content, 512);
        }
        read(fd_archive, content, 512);
    }
    free(content);
    return res;
}

/* Append to the archive section */
int append_to_archive(int fd_archive, tar_opts *options) {
    struct stat info;
    if(fstat(fd_archive, &info) < 0) {printf("Error opening file\n"); return 0;}
    int archive_size = info.st_size;
    lseek(fd_archive, archive_size - 2*BLOCKSIZE, SEEK_SET);

    int res = 0;
    arguments *current_arg = options->args;
    while(is_arguments(current_arg))
    {
        tar_header *header;
        header = get_metadata(current_arg->name);
        if(header != NULL)
        {
            write(fd_archive, header, BLOCKSIZE);
            if (header->typeflag != '5' && header->typeflag != '2')
                write_file_content(fd_archive, current_arg->name, ft_atoi_octal_to_decimal(header->size) + 1);
            free(header);
        }
        else{res++;}
        current_arg = current_arg->next;
    }

    end_of_archive(fd_archive);
    return res;
}

/* Update section */
long get_time(char *arg)
{
    struct stat this_file;
    char mtime[12];
    if(lstat(arg, &this_file) == 0)
    {
        my_itoa(mtime, this_file.st_mtime, 12, OCTAL_BASE);
        return char_to_long(mtime);
    }

    return -1;
}

bool is_same(char *one, char *two)
{
    while(*one == 32) {one++;}
    while(*two == 32) {two++;}
    if(strcmp(one,two) == 0) {return true;}
    else {return false;}
}

long is_old_time(int fd_archive, char *filename, char* time)
{
    struct stat info;
    if(fstat(fd_archive, &info) < 0) {printf("Error opening file\n"); return 0;}
    int archive_size = info.st_size;
    lseek(fd_archive, 0, SEEK_SET);
    char *archive_content = (char*)malloc(archive_size);
    get_archive_content(fd_archive, archive_content);
    tar_header *header;
    int i = 0;

    while(i < archive_size)
    {
        header = (tar_header*)&archive_content[i];
        if(is_header(header)) {
            if(is_same(header->name, filename)) {
                time = strcpy(time, header->mtime);
                free(archive_content);
                return char_to_long(time);
            }
        }
        i++;
    }
    free(archive_content);
    return 0;
}

int append_file(int fd_archive, char *filename)
{
    struct stat info;
    if(fstat(fd_archive, &info) < 0) {printf("Error opening file\n"); return 0;}
    int archive_size = info.st_size;
    lseek(fd_archive, archive_size - (2 * BLOCKSIZE), SEEK_SET);

    int res = 0;
    tar_header *header;
    header = get_metadata(filename);

    if(header != NULL)
    {
        write(fd_archive, header, BLOCKSIZE);
        if (header->typeflag != '5' && header->typeflag != '2')
                write_file_content(fd_archive, filename, ft_atoi_octal_to_decimal(header->size) + 1);
        free(header);
    }

    else{res++;}
    end_of_archive(fd_archive);
    return res;
}

int update_archive(int fd_archive, tar_opts *options)
{
    int res = 0;
    long time = 0;
    arguments *current_arg = options->args;

    while(is_arguments(current_arg))
    {
        char *buff = malloc(12);
        lseek(fd_archive, 0, SEEK_SET);
        time = is_old_time(fd_archive, current_arg->name, buff);
        if(time)
        {
            if(time == get_time(current_arg->name))
            {
                current_arg = current_arg->next;
            }
            else
            {
                res += append_file(fd_archive, current_arg->name);
                current_arg = current_arg->next;
            }
        }
        else
        {
            res += append_file(fd_archive, current_arg->name);
            current_arg = current_arg->next;
        }

        free(buff);
    }
    return res;
}

/* Main section */
int main(int ac, char **av) {
    int res = 0, fd_archive;
    tar_opts *options = get_options(ac, av);

    if(options == NULL)
    {
        printf("Error: options weren't pass correctly\n");
        return 1;
    }

    fd_archive = open_archive_file(options);

    if(fd_archive < 0)
    {
        delete_options(options);
        printf("Error: archive couldn't be open\n");
        return 2;
    }

    if(options->create == true) {res += create_archive(fd_archive, options);}
    else if(options->extract) {res += extract_archive(fd_archive);}
    else if(options->list == true) {res += list_archive_contents(fd_archive);}
    else if(options->append == true) {res += append_to_archive(fd_archive, options);}
    else if(options->update == true) {res += update_archive(fd_archive, options);}

    close(fd_archive);
    delete_options(options);
    return res;
}
