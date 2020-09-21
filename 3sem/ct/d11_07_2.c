#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include <stdio.h>
#include <fcntl.h>

#include <errno.h>

struct point{
	float x,y;
};

int main(){

	int fd = open("numb11_07", O_CREAT|O_RDWR, 0775);
	int p = ftruncate(fd, sizeof(struct point)*10);
	void* m = mmap(NULL, sizeof(struct point)*10, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);

	if (m == MAP_FAILED){
		printf("map failed\n");
		return -1;
	}

	struct point* l = (struct point *)m;
	for (int i = 0; i < 10; i++){
                printf("%f %f\n", l[i].x, l[i].y);
        }

	p = munmap(m, sizeof(struct point)*10);

	return 0;
}

