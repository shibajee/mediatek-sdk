#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv) {
    int pipefd[2];
    int nread;
    int nwrite;
    FILE *in_file;
    FILE *out_file;
    struct stat st;
    int size_left=0;
    int count=0;

    if(pipe(pipefd)) {
	    printf("pipe create fail!\n");
	    return 0;
    }

    in_file = fopen(argv[1], "rb");
    out_file = fopen(argv[2], "wb");
    
    stat(argv[1], &st);
    size_left = st.st_size;
    printf("file size=%d\n",size_left);

    while(size_left != 0) {
	    if(size_left >= 16384) {
		    printf("%d.", count++);
		    nread = splice(fileno(in_file), NULL, pipefd[1], NULL, 16384, SPLICE_F_MORE | SPLICE_F_MOVE);
		    printf("nread=%d (errno=%d), ", nread, errno);

		    nwrite = splice(pipefd[0], NULL, fileno(out_file), NULL, nread, SPLICE_F_MORE | SPLICE_F_MOVE);
		    printf("nwrite=%d (errno=%d)\n", nwrite, errno);

		    size_left -= nwrite;
	    }else {
		    printf("%d.", count++);
		    nread = splice(fileno(in_file), NULL, pipefd[1], NULL, size_left, SPLICE_F_MORE | SPLICE_F_MOVE);
		    printf("nread=%d (errno=%d), ", nread, errno);

		    nwrite = splice(pipefd[0], NULL, fileno(out_file), NULL, nread, SPLICE_F_MORE | SPLICE_F_MOVE);
		    printf("nwrite=%d (errno=%d)\n", nwrite, errno);

		    size_left -= nwrite;
	    }
    }

    if (nread == -1 || nwrite == -1)
        printf("%d - %s\n", errno, strerror(errno));

    close(pipefd[0]);
    close(pipefd[1]);
    fclose(in_file);
    fclose(out_file);

    return 0;
}
