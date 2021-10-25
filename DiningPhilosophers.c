#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h> //for I/O
#include <stdlib.h> //for rand/strand
#include <windows.h> //for Win32 API
#include <time.h> //for using the time as the seed to strand!

//5 Dining Philosophers
#define N 5
//Each Philosopher is Philosophising 5 rounds
#define MAX_PHILOSOPHY_ROUNDS 5
//Here You should define RIGHT and LEFT MACROS (exactly like in Lecture
//Notes)
#define RIGHT (philID + 1)%N
#define LEFT (philID + N - 1)%N
//Every Philosopher waits random time between 1 and 500 msec
//before it takes forks and before it puts forks
#define MAX_SLEEP_TIME 500 //Miliseconds (1/2 second)
#define True 1
#define False 0

//The possible states of a Philosopher (enumeration)
enum PHIL_STATES { THINKING, HUNGRY, EATING };

///Global State Data:
//state array, holds current state of each Philosopher in the System
int state[N];

//single mutex to protect access to the global state data. Recall, like in
//lecture notes, global state data must be accessed exclusively by each
//Philosopher!
HANDLE mutex;
//Array of Scheduling Constraints Semaphores - one for Each Philosopher
HANDLE sem[N];

//Function to Initialise and clean global data (mainly for creating the
//Semaphore Haldles before all Threads start and closing them properly after
//all Threads finish!).
int initGlobalData();
void cleanupGlobalData();

//Thread function for each Philosopher
DWORD WINAPI Philosopher(LPVOID Param);

//better to write a generic function to randomise a Sleep time between 1 and
//MAX_SLEEP_TIME msec
int calcSleepTime();

//Functions to manipulate Philosopher State - as defined in lecture notes
void take_forks(int philID);
void put_forks(int philID);
void test(int philID);

int main(int argc, char* argv[])
{
	//Define Local Variables (e.g. Handle Array for all Philosophers,
	//perhaps Array of unique IDs for Philosophers so that each ID is
	//send exclusively to the corresponding Philosopher (to prevent
	//race condition in initialisation
	srand(time(NULL));
	int philosopherID[N];
	HANDLE philosophersHandler[N];
	DWORD threadId;

	// initialise Global Data (using initGlobalData function)
	if (initGlobalData() == False)
	{
		return 1;
		printf("main::Unexpected Error - initGlobalData()\n");
	}

	// Create all Philosopher Threads. Report if Error occurred!
	for (int i = 0; i < N; i++)
	{
		philosopherID[i] = i;
		philosophersHandler[i] = CreateThread(NULL, 0, Philosopher, &philosopherID[i], 0, &threadId);

		if (philosophersHandler[i] == NULL)
		{
			printf("Main::Unexpected Error - In Philosopher Thread %d Creation\n", philosopherID[i]);
			return 1;
		}
	}
	 
	// Wait for ALL Philosophers to finish
	WaitForMultipleObjects(N, philosophersHandler, True, INFINITE);

	// Report Completion
	printf("All Philosophers are Completed!\n");

	// Close all Thread Handles and perform cleanup of Global Data,
	// using cleanupGlobalData function
	cleanupGlobalData();

	for (int i = 0; i < N; i++)
	{
		CloseHandle(philosophersHandler[i]);
	}

	return 0;
}

int initGlobalData()
{
	mutex = CreateMutex(NULL, False, NULL);

	if (mutex == NULL)
	{
		return False;
	}

	for (int i = 0; i < N; i++)
	{
		state[i] = THINKING;
		sem[i] = CreateSemaphore(NULL, 0, 1, NULL);

		if (sem[i] == NULL)
		{
			return False;
		}
	}

	return True;
}

void cleanupGlobalData()
{
	CloseHandle(mutex);

	for (int i = 0; i < N; i++)
	{
		CloseHandle(sem[i]);
	}
}

DWORD WINAPI Philosopher(LPVOID Param)
{
	//Get the Unique ID from Param and keep it in a local variable
	int philID = *(int*)Param;

	for (int i = 0; i < MAX_PHILOSOPHY_ROUNDS; i++)
	{
		// Sleep Random time between 1 and MAX_SLEEP_TIME
		// (you may wish to implement and use the function
		// calcSleepTime() to calculate and return to you this
		// random time!)
		Sleep(calcSleepTime());

		// take_forks - HUNGRY
		take_forks(philID);

		// Sleep Random time between 1 and MAX_SLEEP_TIME
		Sleep(calcSleepTime());

		// put_forks - THINKING
		put_forks(philID);
	}

	return 0;
}

int calcSleepTime()
{
	return rand() % MAX_SLEEP_TIME + 1;
}

void take_forks(int philID)
{	
	// mutex.P()
	WaitForSingleObject(mutex, INFINITE);

	state[philID] = HUNGRY;
	printf("Phil %d: HUNGRY\n", philID);

	// test - EATING
	test(philID);

	// mutex.V()
	if (!ReleaseMutex(mutex))
	{
		printf("take_forks::Unexpected Error - mutex.V()\n");
	}

	// sem[philID].P()
	WaitForSingleObject(sem[philID], INFINITE);
}

void put_forks(int philID)
{
	// mutex.P()
	WaitForSingleObject(mutex, INFINITE);

	state[philID] = THINKING;
	printf("Phil %d: THINKING\n", philID);

	// test - EATING
	test(LEFT);
	test(RIGHT);

	// mutex.V()
	if (!ReleaseMutex(mutex))
	{
		printf("put_forks::Unexpected Error - mutex.V()\n");
	}
}

void test(int philID)
{
	// Only eat if LEFT and RIGHT neibours aren't eating
	if (state[philID] == HUNGRY &&
		state[LEFT] != EATING &&
		state[RIGHT] != EATING)
	{
		state[philID] = EATING;
		printf("Phil %d: EATING\n", philID);

		// sem[philID].V()
		if (!ReleaseSemaphore(sem[philID], 1, NULL))
		{
			printf("test::Unexpected Error - sem[%d].V()\n", philID);
		}
	}
}