// This SECTION CONSISTS OF HEADERS THAT NEED TO BE DEFINED ON HOST OS
//
//
//

#include <syscall.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

struct linux_dirent {
	long int d_ino;
	off_t d_off;
	unsigned short d_reclen;
	char d_name[];
};

struct date
{
	int date,mon,year,wday;
	int h,m,s,yday;
};

// THIS SECTION CONSISTS OF LIBRARY FUNCTIONS
//
//
//

// Included strcmp

int strcmp (const char *p1, const char *p2)
{
	const unsigned char *s1 = (const unsigned char *) p1;
	const unsigned char *s2 = (const unsigned char *) p2;
	unsigned char c1, c2;

	do
	{
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == 0)
			return c1 - c2;
	}
	while (c1 == c2);

	return c1 - c2;
}

// Included strlen

size_t strlen (const char *str)
{
	size_t c = 0;
	unsigned char *cc = (unsigned char *) str;
	while (*cc++)
		c++;

	return c + 1;
}

// Included strcpy

char *strcpy(char *dest, const char *src)
{
	char c;
	char *s = (char *) src;
	const ptrdiff_t off = dest - s - 1;

	do
	{
		c = *s++;
		s[off] = c;
	}
	while (c != 0);

	return dest;
}

// Included strcat

char *strcat (char *dest, const char *src)
{
	char *s1 = dest;
	const char *s2 = src;
	char c;

	/* Find the end of the string.  */

	do
		c = *s1++;
	while (c != 0);

	s1 -= 2;

	do
	{
		c = *s2++;
		*++s1 = c;
	}
	while (c != 0);

	return dest;
}

// Included strchr

char *strchr (const char *s, int c_in)
{
	unsigned char *c = (unsigned char *)s;
	int x = (int) c_in;

	while (c++ && *c)
		if (*c == x)
			return (char *) c;

	return NULL;
}

// Included strrev

void strrev(char *p)
{
	char *q = p;
	while(q && *q) ++q;
	for(--q; p < q; ++p, --q)
		*p = *p ^ *q,
			*q = *p ^ *q,
			*p = *p ^ *q;
}

// Included own itoa (base 10 only)

char *itoa(unsigned long long int x , char *buffer)
{
	if (!x)
		return "0";
	char *c = buffer;
	while (x)
	{
		*c = x % 10 + '0';
		x = x / 10;
		c++;
	}
	*c = 0;
	strrev(buffer);
	return buffer;
}

char *itoa_date(unsigned long long int x , char *buffer) //lazy hack
{
	char *c = buffer;

	if (x < 10)
	{
		switch (x)
		{
			case 0:
				return "00";
			case 1:
				return "01";
			case 2:
				return "02";
			case 3:
				return "03";
			case 4:
				return "04";
			case 5:
				return "05";
			case 6:
				return "06";
			case 7:
				return "07";
			case 8:
				return "08";
			case 9:
				return "09";
		}
	}

	while (x)
	{
		*c = x % 10 + '0';
		x = x / 10;
		c++;
	}

	*c = 0;

	strrev(buffer);

	return buffer;
}

// Included own atoll

unsigned long long int atoll (char *s)
{
	char *ss=s;
	unsigned long long int num = 0;
	while(*ss)
	{
		num = num * 10;
		num = num + (int)(*ss - '0');
		ss++;
	}
	return num;
}

// Included own qsort (strings only)

void QSort(char List[][1024], int Left, int Right)
{
	int i, j;
	char *x;
	char TEMP[1024];

	i = Left;
	j = Right;
	x = List[(Left+Right)/2];

	do {
		while((strcmp(List[i],x) < 0) && (i < Right)) {
			i++;
		}
		while((strcmp(List[j],x) > 0) && (j > Left)) {
			j--;
		}
		if(i <= j) {
			strcpy(TEMP, List[i]);
			strcpy(List[i], List[j]);
			strcpy(List[j], TEMP);
			i++;
			j--;
		}
	} while(i <= j);

	if(Left < j) {
		QSort(List, Left, j);
	}
	if(i < Right) {
		QSort(List, i, Right);
	}
}

// THIS SECTION HAS #define-s THAT I THOUGHT WERE USEFUL
//
//
//

#define _write_wrapper(string) syscall(SYS_write , STDOUT_FILENO , string , strlen(string))
#define endl _write_wrapper("\n\0")
#define tabspace _write_wrapper("\t\0")

#define LS_L 0x01
#define LS_A 0x02
#define LS_H 0x04

#define BUFFER_SIZE 4096

#define YEAR0           1900
#define EPOCH_YR        1970
#define SECS_DAY        (24L * 60L * 60L)
#define LEAPYEAR(year)  (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)  (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)
#define TIME_MAX        ULONG_MAX
#define ABB_LEN         3

#define HOUR_OFFSET 5
#define MINUTE_OFFSET 30
#define SECOND_OFFSET 0

const int _ytab[2][12] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};


/*
   -a, --all
   -h, --human-readable
   with -l, print sizes in human readable format (e.g., 1K 234M 2G)
   -l     use a long listing format

*/

// PROGRAM SPECIFIC FUNCTIONS
//
//
//

struct date getDate(unsigned long long t)
{
	int year = EPOCH_YR;
	struct date tp;
	unsigned long dayclock, dayno;
	dayclock = t % 86400;
	dayno = t / 86400;
	tp.s = (int) dayclock % 60;
	tp.m = ((int) dayclock % 3600) / 60;
	tp.h= (int) dayclock / 3600;
	while (dayno >= YEARSIZE(year))
	{
		dayno = dayno - YEARSIZE(year);
		year++;
	}
	tp.year = year;
	tp.yday = dayno;
	tp.mon = 1;
	while(dayno >=  _ytab[ LEAPYEAR(year)][ tp.mon ] )
	{
		dayno = dayno - _ytab[ LEAPYEAR(year)][ tp.mon ];
		tp.mon++;
	}
	tp.date = dayno;
	return tp;
}

int findAndPrint(char * l , int ID )
{
	char *line = l;
	int flag = 0;
	char temp[10];
	int i = 0;
	int j = 0;

	char user[20];

	while(flag < 3)
	{
		if(*line == ':')
			flag++;

		if(flag == 2 && *line != ':')
			temp[i++] = *line;
		else
			if(flag == 0 && *line != ':')
				user[j++] = *line;

		line++;
	}
	user[ j ] = 0;
	temp[ i ] = 0;
	long long chuid = atoll(temp);
	if(chuid == ID) {
		_write_wrapper(user);
		tabspace;
		return 1;
	}
	return 0;
}

void printID( const int ID  , char *path)
{
	char readBuffer[ 1048576 ]; // Read All in one pass. Fuck loops.
	int i;

	int fd = syscall (SYS_open , path , O_RDONLY );

	syscall( SYS_read , fd , readBuffer , sizeof(readBuffer) );

	char *poof = readBuffer;

	while (*poof && poof)
	{
		i = 0;
		char temp_line[1000];
		while (*poof && *poof != '\n')
		{
			temp_line[i] = *poof;
			poof++;
			i++;
		}
		temp_line[i] = '\n';
		temp_line[i + 1] = 0;

		if (findAndPrint(temp_line , ID))
			break;
		poof++;
	}

	syscall (SYS_close , fd);
}

void iambored_sizeconvert(unsigned long long int size , char *buffer)
{
	const char *units[] = {"  ", "K ", "M ", "GB", "TB", "PB"};

	int i = 0;

	if (size < 0x400)
	{
		_write_wrapper(itoa(size,buffer));
		_write_wrapper(units[i]);
		return;
	}

	while (size > 0x400)
	{
		size = size >> 10;
		i++;
	}

	_write_wrapper(itoa(size,buffer));
	_write_wrapper(units[i]);
}

int printFSObjectInfo( const char * path , const char * name , int switcher)
{
	char integerPrintBuffer[100];
	struct stat statbuf;
	char type[] = "?";
	char owner[] = "---", group[] = "---", others[] = "---";

	syscall (SYS_lstat , path , &statbuf);

	if( S_ISREG(statbuf.st_mode) ) type[0] = '-'; // regular file
	if( S_ISDIR(statbuf.st_mode) ) type[0] = 'd'; // directory
	if( S_ISBLK(statbuf.st_mode) ) type[0] = 'b'; // block special
	if( S_ISCHR(statbuf.st_mode) ) type[0] = 'c'; // character special
	if( S_ISFIFO(statbuf.st_mode) ) type[0] = 'f'; // pipe or FIFO
	if( S_ISLNK(statbuf.st_mode) ) type[0] = 'l'; // symbolic link
	if( S_ISSOCK(statbuf.st_mode) ) type[0] = 's'; // socket

	if( S_IRUSR & statbuf.st_mode ) owner[0] = 'r';
	if( S_IWUSR & statbuf.st_mode ) owner[1] = 'w';
	if( S_IXUSR & statbuf.st_mode ) owner[2] = 'x';

	if( S_IRGRP & statbuf.st_mode ) group[0] = 'r';
	if( S_IWGRP & statbuf.st_mode ) group[1] = 'w';
	if( S_IXGRP & statbuf.st_mode ) group[2] = 'x';

	if( S_IROTH & statbuf.st_mode ) others[0] = 'r';
	if( S_IWOTH & statbuf.st_mode ) others[1] = 'w';
	if( S_IXOTH & statbuf.st_mode ) others[2] = 'x';

	_write_wrapper(type);
	_write_wrapper(owner);
	_write_wrapper(group);
	_write_wrapper(others);
	_write_wrapper(" ");

	_write_wrapper(itoa(statbuf.st_nlink , integerPrintBuffer) );
	tabspace;

	printID( statbuf.st_uid , "/etc/passwd" );
	printID( statbuf.st_gid , "/etc/group" );

	if (switcher & LS_H)
		iambored_sizeconvert(statbuf.st_size , integerPrintBuffer);
	else
		_write_wrapper(itoa(statbuf.st_size , integerPrintBuffer) );
	tabspace;

	struct date d = getDate(statbuf.st_mtime);

	_write_wrapper(itoa_date((d.date + (d.h + HOUR_OFFSET) / 24) , integerPrintBuffer));
	_write_wrapper(":");
	_write_wrapper(itoa_date(d.mon , integerPrintBuffer));
	_write_wrapper(":");
	_write_wrapper(itoa_date(d.year , integerPrintBuffer));
	tabspace;

	_write_wrapper(itoa_date( (d.h + HOUR_OFFSET + ((d.m + MINUTE_OFFSET) / 60)) % 24 , integerPrintBuffer));
	_write_wrapper(":");
	_write_wrapper(itoa_date( (d.m + MINUTE_OFFSET) % 60 , integerPrintBuffer));
	_write_wrapper(":");
	_write_wrapper(itoa_date( d.s + SECOND_OFFSET , integerPrintBuffer));
	tabspace;

	_write_wrapper(name);

	if( S_ISLNK(statbuf.st_mode) )
	{
		_write_wrapper(" -> ");
		int goo = syscall (SYS_readlink , path , integerPrintBuffer , sizeof(integerPrintBuffer));
		integerPrintBuffer[goo] = 0;
		_write_wrapper(integerPrintBuffer);
	}

	endl;
	return 0;
}

int getArgs (const int argc , const char ** argv)
{
	int switcher = 0;
	int i;
	for (i = 1 ; i < argc ; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strchr(argv[i] , 'l'))
				switcher |= LS_L;
			if (strchr(argv[i] , 'a'))
				switcher |= LS_A;
			if (strchr(argv[i] , 'h'))
				switcher |= LS_H;
		}
		else
			continue;
	}

	return switcher;
}

const char * getDir( const int argc , const char ** argv)
{
	int i;
	for (i = 1 ; i < argc ; i++)
	{
		if (argv[i][0] == '-')
			continue;
		else
			return argv[i];
	}
	return ".";
}

int main ( const int argc , const char ** argv)
{
	int switcher = getArgs( argc , argv );
	const char * directory = getDir( argc , argv );
	char fileList[1024][1024];
	int nread;
	char fileDirectoryBuffer[BUFFER_SIZE];
	int bpos;
	struct linux_dirent * d;
	int i = 0;

	int fd = syscall( SYS_open , directory , O_RDONLY | O_DIRECTORY);

	if (fd == -1)
	{
		if (syscall(SYS_access , directory , F_OK) != 0)
		{
			_write_wrapper("No Such File Or Directory");
			endl;
			return fd;
		}
		else
		{
			if (switcher & LS_L)
				printFSObjectInfo(directory , directory , switcher);
			else
			{
				_write_wrapper(directory);
				endl;
			}
		}
	}

	nread = syscall(SYS_getdents, fd, fileDirectoryBuffer, BUFFER_SIZE);
	for (bpos = 0; bpos < nread;) {
		d = (struct linux_dirent *) (fileDirectoryBuffer + bpos);

		if (switcher & LS_A)
		{
			strcpy(fileList[i] , d->d_name);
			i = i + 1;
		}
		else
			if (d->d_name[0] != '.')
			{
				strcpy(fileList[i] , d->d_name);
				i = i + 1;
			}

		bpos += d->d_reclen;
	}

	QSort(fileList , 0 , i - 1);

	int n = i;
	char dirTemp[100];

	for (i = 0 ; i < n ; i++)
	{
		strcpy(dirTemp , directory);
		strcat(dirTemp , "/");
		if (switcher & LS_L)
			printFSObjectInfo(strcat(dirTemp,fileList[i]) , fileList[i] , switcher);
		else
		{
			_write_wrapper(fileList[i]);
			endl;
		}
	}

	syscall (SYS_close , fd);

	return 0;
}
