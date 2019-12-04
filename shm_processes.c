#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void ClientProcess(int[]);

int main(int argc, char *argv[])
{
	int ShmID;
	int *ShmPTR;
	pid_t pid;
	int status;
	int i;

	if (argc != 11) {
		printf("Use: %s #1 #2 #3 … #10\n", argv[0]);
		exit(1);
	}

	ShmID = shmget(IPC_PRIVATE, 10 * sizeof(int), IPC_CREAT | 0666);
	if (ShmID < 0) {
		printf("*** shmget error (server) ***\n");
		exit(1);
	}
	printf("Server has received a shared memory of ten integers...\n");

	ShmPTR = (int *)shmat(ShmID, NULL, 0);
	if (*ShmPTR == -1) {
		printf("*** shmat error (server) ***\n");
		exit(1);
	}
	printf("Server has attached the shared memory...\n");

	for (i = 0; i < 10; i++)
		ShmPTR[i] = atoi(argv[i + 1]);

	printf("Server has filled ");
	for (i = 0; i < 10; i++)
		printf("%d ", ShmPTR[i]);
	printf("in shared memory...\n");

	printf("Server is about to fork a child process...\n");
	pid = fork();
	if (pid < 0) {
		printf("*** fork error (server) ***\n");
		exit(1);
	} else if (pid == 0) {
		ClientProcess(ShmPTR);
		exit(0);
	}

	wait(&status);
	printf("Server has detected the completion of its child...\n");

	printf("Server found ");
	for (i = 0; i < 10; i++)
		printf("%d ", ShmPTR[i]);
	printf("in shared memory, (from child process)...\n");

	shmdt((void *)ShmPTR);
	printf("Server has detached its shared memory...\n");
	shmctl(ShmID, IPC_RMID, NULL);
	printf("Server has removed its shared memory...\n");
	printf("Server exits...\n");
	exit(0);
}

void ClientProcess(int SharedMem[])
{
	int i;

	printf("   Client process started\n");

	printf("   Client found ");
	for (i = 0; i < 10; i++)
		printf("%d ", SharedMem[i]);
	printf("in shared memory...\n");

	printf("    Client has filled ");
	for (i = 0; i < 10; i++) {
		SharedMem[i] = i;
		printf("%d ", SharedMem[i]);
	}
	printf("in shared memory...\n");

	printf("   Client is about to exit\n");
}
