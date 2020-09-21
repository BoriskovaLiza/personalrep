#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <dirent.h>

void copy_files(char* dir_src, char* dir_dst){
	char conc_s[256];
	char conc_d[256];
	char conc_d_gz[256];

	DIR *dir_s = opendir(dir_src);
	struct dirent* e;
	struct stat info;
	struct stat info_d;
	while ( (e = readdir(dir_s)) != NULL ){
		if (strcmp(e->d_name, ".")==0 || strcmp(e->d_name, "..")==0){
			continue;
		}
		sprintf(conc_s, "%s/%s", dir_src, e->d_name);
		sprintf(conc_d, "%s/%s", dir_dst, e->d_name);

		stat(conc_s, &info);

		if ( S_ISDIR(info.st_mode) ){
			mkdir(conc_d, 0775);
			copy_files(conc_s, conc_d);
		}

		if (S_ISREG(info.st_mode) )
		{ 
			sprintf(conc_d_gz, "%s.gz", conc_d);
			if( stat(conc_d_gz, &info_d) == 0 ) // destination file exists
			{
				if (info_d.st_mtim.tv_sec > info.st_mtim.tv_sec){
					unlink(conc_d_gz);
				}
				else
					continue;
			}
			int status = 0;
			pid_t p = fork();
			if (p == 0){
				execlp("cp", "cp", conc_s, conc_d, NULL);
                                perror("cp");
				exit(-1);
			}
			wait(&status);
			if( WIFSIGNALED(status) || WEXITSTATUS(status)!=0 )
			{
				printf("Error copying file: %s -> %s\n", conc_s, conc_d);
				continue;
			}
			p = fork();
                        if (p == 0){
                                execlp("gzip", "gzip", "-f", conc_d, NULL);
                                perror("gzip");
				exit(-1);
                        }
			wait(&status);
			if( WIFSIGNALED(status) || WEXITSTATUS(status)!=0 )
			{
				printf("Error ziping file %s\n", conc_d);
				continue;
			}
		}
	}
	closedir(dir_s);
}

int main(int argc, char* argv[]){
	int status;
	char* s = argv[1];
	char* d = argv[2];
	mkdir( d, 0775 );
	copy_files(s, d);
	wait(&status);
	return 0;
}
