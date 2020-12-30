#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

struct elem 
{
	char id[16];
	char program_name[512];
};

static int open_data(char *path)
{
	int fd = open(path,O_RDONLY);
	if(fd > 0)
	{
		return fd;
	}
	else
	{
		return -1;
	}
}
#define ELEM_CNT 100
#define BUFF_LEN (ELEM_CNT * 512)
#define UNIQUE_ID "uniqueId"
#define PROGRAM_NAME "programName"
static void parse_data(int fd, struct elem **result, int *cnt)
{
	char* buf = NULL;
	const char *s = "{";
	char *line = NULL;
	char *sub_str = NULL;
	char *sub_str_end = NULL;
	int i = 0;
	struct elem *list;
	
	list = *result = calloc(1,sizeof(struct elem) * ELEM_CNT);
	buf = calloc(1,BUFF_LEN);
	if(buf == NULL || result == NULL)
	{
		printf("alloc memory failed\n");
		return;
	}
	read(fd,buf,BUFF_LEN);
	close(fd);	
	
	line = strtok(buf,s);
	
	while(line != NULL)
	{
		sub_str = strstr(line,UNIQUE_ID);	
		if(sub_str != NULL)
		{
			sub_str += strlen("uniqueId\":");
			sub_str_end = strstr(sub_str,",");
			memcpy(list[i].id,sub_str,sub_str_end - sub_str);
			sub_str = strstr(sub_str,PROGRAM_NAME);
			if(sub_str != NULL)
			{
				sub_str += strlen("programName\":\"");
				sub_str_end = strstr(sub_str,",") - 1;
				memcpy(list[i].program_name,sub_str,sub_str_end - sub_str);
				sprintf(list[i].program_name,"%s.aac",list[i].program_name);
			}
			i++;
		}
		line = strtok(NULL,s);
	}	
	*cnt = i;
 	return;	
}

static void process_file(struct elem *list, int cnt)
{
	DIR *dir = NULL;
	struct dirent *entry;
	int i = 0;

	if(list == NULL)
	{
		printf("list is NULL\n");
		return;
	}
	
	if((dir = opendir("./")) == NULL)
	{
		printf("opendir failed\n");	
		return;
	}
	else
	{
		while(entry = readdir(dir))
		{
			for(i = 0; i < cnt; i++)
			{
				printf("%s %s\n",list[i].id,list[i].program_name);
				if(0 == strcmp(entry->d_name,list[i].id))
				{
					rename(entry->d_name,list[i].program_name);
				}
			}
		}
		closedir(dir);
	}
}

int main()
{
	int fd;
	struct elem *ret;
	int cnt;
	fd = open_data("./download.dat");
	if(fd == -1)
	{
		printf("open file failed\n");
		return -1;
	}
	parse_data(fd,&ret,&cnt);
	process_file(ret,cnt);
	return 0;
	
}

