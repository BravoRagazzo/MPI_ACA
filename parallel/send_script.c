/*
 * this script automatically copies the ./a.out executable file to all the VMs (in GCP)
 * and after that starts the execution of the program
 *
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {

	char s[1000];
	int max = atoi(argv[1]);

	for(int i=2;i<=max;i++) {
		sprintf(s,"scp a.out diego@fc-node-%d:/home/diego/MPI_ACA/parallel",i);
		system(s);
	}

	if(argc==5) {
		sprintf(s,"mpirun --hostfile hostfile -np %d a.out %s %s",atoi(argv[2]), argv[3], argv[4]);
		system(s);
	}


	return 0;

}
