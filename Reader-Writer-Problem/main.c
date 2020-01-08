#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<string.h>

sem_t mutex,writeblock;
volatile int data[6];
volatile int arr_counter=0;
volatile int rcount = 0;

void *reader(void *arg)
{
	// sleep(1);
	int temp = ((intptr_t)arg)+1;
	printf("\nReader %d is trying to enter into database to read the data\n",temp);
	sem_wait(&mutex);
	rcount = rcount + 1;

	if(rcount==1)
		{
			while(sem_wait(&writeblock)!=0)
			{
				printf("Ooops! The Reader is currently blocked by another Thread! Waiting for turn...\n");
			}
			printf("\nReader %d is reading the database\n",temp); 
			int checker=0;
			for (int i = 0; i < 6; ++i)
			{

				if (data[i]==0)
				{
					checker+=1;
				}
			}

			if(checker==6)
			{
				printf("Error: No Data has been written by any Writer!\n");
			}
			else
			{
				printf("Data read by the reader-%d is: [" ,temp);
				for (int i = 0; i < 6; ++i)
				{
					if (data[i]!=0)
					{
						printf(" %d ", data[i]);
					}
				}
				printf("]\n");
			}
			// printf("\nReader dequeuing last element...");
			
			// for (int i = 6; i >= 0; i--)
			// {
			//  if (data[i]!=0)
			//  {
			//      // printf(" LOL VAL:    %d ", data[i]);
			//      data[i]=0;
			//      break;
			//  }
			// }
			// printf("New Data read by the reader-%d is: [" ,temp);

			// for (int i = 0; i < 6; ++i)
			// {
			//  if (data[i]!=0)
			//  {
			//      printf(" %d ", data[i]);
			//  }
			// }
			// printf("]");
			// sleep(3);
		}
	sem_post(&mutex);
	sem_wait(&mutex);
	rcount = rcount - 1;
	if(rcount==0)
		{
			printf("Reader %d is leaving the database\n",temp); 
			sem_post(&writeblock);
		}
	sem_post(&mutex);

}

void *writer(void *arg)
{
	// sleep(1);
	int temp = ((intptr_t) arg)+1;
	int value;
	printf("\nWriter %d is trying to enter into database for modifying the data\n",temp);
	while(sem_wait(&writeblock)!=0)
		{
			printf("Ooops! The Writer is currently blocked by another Thread! Waiting for turn...\n");
		}

	arr_counter++;
	data[arr_counter]=arr_counter;
	printf("Data written by the writer-%d is %d\n",temp,data[arr_counter]);
	// sleep(3);
	printf("Writer Done Writing!\n");
	sem_post(&writeblock);
}

int main()
{
	int i=0,j=0, ReaderThreads=0,WriterThreads=0; 
	sem_init(&mutex,0,1);
	sem_init(&writeblock,0,1);

	pthread_t Reader_Thread[100],Writer_Thread[100];
	printf("\nEnter number of Readers thread(MAX 5): ");
	if (scanf("%d", &ReaderThreads) <= 0 || ReaderThreads< 1 || ReaderThreads > 5)
		{
			printf("Scanf error: Only numbers between 0-5 expected!\n");
			return 0;
		}
	printf("\nEnter number of Writers thread(MAX 5): ");
	if (scanf("%d", &WriterThreads) <= 0 || WriterThreads< 1 || WriterThreads > 5)
		{
			printf("Scanf error: Only numbers between 0-5 expected!\n");
			return 0;
		}

	while(i<WriterThreads || j<ReaderThreads)
	{
	if(j<ReaderThreads)
		{
			if((pthread_create(&Reader_Thread[j],NULL,reader,(void *)(intptr_t)j))!=0)
			{
				printf("\033[1;31m");
				printf("Error in Thread Creation!");
				printf("\033[0m");
				return 0;
			}
			
			j++;
		}
	if(i<WriterThreads)
		{
			if((pthread_create(&Writer_Thread[i],NULL,writer,(void *)(intptr_t)i))!=0)
			{
				printf("\033[1;31m");
				printf("Error in Thread Creation!");
				printf("\033[0m");
				return 0;
			}
			i++;
		}
	}

	for(i = 0; i < WriterThreads; i++)
			pthread_join(Writer_Thread[i], NULL);

	for(i = 0; i < ReaderThreads; i++)
			pthread_join(Reader_Thread[i], NULL);

	sem_destroy(&mutex);
	sem_destroy(&writeblock);

	return 0;
}