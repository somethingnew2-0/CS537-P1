#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE (32)

void
usage(char *prog) 
{
    fprintf(stderr, "Usage: %s  <-i input file> <-o output file>\n", prog);
    exit(1);
}

int recordcmp(const void *v1, const void *v2)
{
      return ((*(rec_t *)v1).key) - ((*(rec_t *)v2).key);
}

int
main(int argc, char *argv[])
{
    // program assumes a 4-byte key
    assert(sizeof(rec_t) == 4096);
	
    // arguments
    char *inFile   = NULL;
    char *outFile   = NULL;

	// sotrting variables
	int numRecords = 0;
	rec_t *records = NULL;

	if(argc != 5) {
		usage(argv[0]);
	}

    // input params
    int c;
    opterr = 0;
    while ((c = getopt(argc, argv, "i:o:")) != -1) {
		switch (c) {
			case 'i':
				inFile = strdup(optarg);
				break;
			case 'o':
				outFile = strdup(optarg);
				break;
			default:
				usage(argv[0]);
		}
    }

	if(inFile == NULL || outFile == NULL) {
		usage(argv[0]);
	}

    // open and read input file
    int fd = open(inFile, O_RDONLY);
    if (fd < 0) {
    	fprintf(stderr, "Error: Cannot open file %s\n", inFile);
		exit(1);
    }
   
	rec_t r; 
    while (1) {	
		int rc = read(fd, &r, sizeof(rec_t));
		if (rc == 0) // 0 indicates EOF
			break;
		if (rc < 0) {
	    	fprintf(stderr, "Error: Problem reading file %s\n", inFile);
			exit(1);
		}
		records = (rec_t*)realloc(records, (numRecords+1)*sizeof(rec_t));
		if(records == NULL) {
	    	fprintf(stderr, "Error: Cannot reallooate memory\n");
			exit(1);
		}
		records[numRecords++] = r;
    }
    
    // close the input file
    (void) close(fd);
   
    qsort(records, numRecords, sizeof(rec_t), recordcmp);
    
    // open and create output file
    fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (fd < 0) {
    	fprintf(stderr, "Error: Cannot open file %s\n", outFile);
		exit(1);
    }
	
    int i;
    for (i = 0; i < numRecords; i++) {
		r = records[i];
		//printf("key:%9d \n", r.key);
		int rc = write(fd, &r, sizeof(rec_t));
		if (rc != sizeof(rec_t)) {
	    	fprintf(stderr, "Error: Problem writing file %s\n", outFile);
			exit(1);
			// should probably remove file here but ...
		}
    }
    (void) close(fd);
	free(records);
    return 0;
}
