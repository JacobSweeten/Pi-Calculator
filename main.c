#include <stdio.h>
#include <tgmath.h>
#include <stdlib.h>
#include <pthread.h>

// For safe multithreading
int done;
pthread_mutex_t lock;

// f(x)=sqrt(1-x^2)
long double f(long double x)
{
	return sqrtl(1.0L - (x * x));
}

typedef struct threadInfo_t {
	pthread_t thread;
	int index;
	long long int slices;
	long double start;
	long double width;
	long double sum;
} threadInfo_t;

// Thread worker
void* t(void* arg)
{
	threadInfo_t* info = (threadInfo_t*)arg;

	for(long long int i = 0; i < info->slices; i++)
	{
		info->sum += f(info->start + ((long double)i * info->width)) * info->width;
	}

	pthread_mutex_lock(&lock);
	done--;
	pthread_mutex_unlock(&lock);
}

int main(int argc, char** argv)
{
	// Process args
	if(argc != 3)
	{
		printf("I need a number of threads and slices m8\n");
		return 1;
	}

	int numThreads = atoi(argv[1]);
	long long int numSlices = atoll(argv[2]);
	
	// Setup safe multithreading
	done = numThreads;
	pthread_mutex_init(&lock, NULL);

	// Begin making threads
	threadInfo_t* threads;

	threads = (threadInfo_t*)malloc(sizeof(threadInfo_t) * numThreads);

	// Calculate shares of work
	long long int slicesPerThread = numSlices / numThreads;
	long long int lastThreadSlices = numSlices - (slicesPerThread * (numThreads - 1));

	long double width = 1.0L / (long double)numSlices;
	long double lastX = 0;

	// Spawn threads
	for(int i = 0; i < numThreads; i++)
	{
		threads[i].sum = 0.0L;
		threads[i].index = i;
		threads[i].width = width;
		threads[i].start = lastX;
		if(i == numThreads - 1)
			threads[i].slices = lastThreadSlices;
		else
			threads[i].slices = slicesPerThread;
		pthread_create(&threads[i].thread, NULL, t, &threads[i]);

		lastX += width * slicesPerThread;
	}

	// Wait for threads to finish
	while(done != 0) {}

	// Sum up
	long double total = 0.0L;

	for(int i = 0; i < numThreads; i++)
	{
		total += threads[i].sum;
	}

	// Output
	printf("%.50Le\n", total * 4.0L);

	free(threads);

	return 0;
}