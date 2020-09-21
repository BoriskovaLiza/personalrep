#include "sys/types.h"
#include "unistd.h"
#include "stdio.h"
#include "sys/wait.h"
#include "signal.h"
#include "stdlib.h"
int n, num = 0; //счетчик рабочих процессов

void runsim(int signum) //
{
	int status = 0;
	while(waitpid(-1, &status,0) > 0){
		num--;
	}
}

int main(int m,char* command[]) //вводим количество нужных процессов и имена файлов
{
	printf("Program start\n"); //
	char list[256]; //запишем перечень имен программ в лист
	pid_t p;
	sscanf(command[1], "%d", &n); //из строки считываем количество
	printf("n=%d\n",n);
	signal(SIGCHLD, runsim); //работает вне зависимости от программы, будет передавать код завершения дочернего процесса в функцию runsim
	printf("Inicialization  complete\n");
	while(scanf("%s", list)!= EOF) //читаем строку листа пока не кончится
	{
		printf("while process start\n");
		if(num < n) // если запущено меньше чем максимум процессов
		{
			num++; //то прибавим 1
			p = fork();
			if(p == 0)
			{
				printf("program %d start\n",num); //выводим какой номер команды запустился
				execl(list, list, NULL);
				perror("execl");
				exit(-1);
			}
		}
		else
		{
		printf(" Too many processes! \n");
		continue;
		}
	}
	return 0;
}
