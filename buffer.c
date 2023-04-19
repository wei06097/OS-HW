#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* ======================================== */
sem_t full, empty;
pthread_mutex_t mutex;

void *producer(void *param);
void *consumer(void *param);

int in = 0, out = 0, counter = 0;
buffer_item buffer[BUFFER_SIZE];
int insert_item(buffer_item item);
int remove_item(buffer_item *item);

/* ======================================== */
int main(int argc, char *argv[]) {
    srand(time(0));

    /* 1. Get command line arguments argv[1], argv[2], argv[3] */
    if (argc != 4) {
        fprintf(stderr, "參數數量錯誤\n");
        return -1;
	}
    int sleepTime = atoi(argv[1]);
    int producerNumber = atoi(argv[2]);
    int consumerNumber = atoi(argv[3]);

    /* 2. Initialize buffer */
    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    /* 3. Create producer thread(s) */
    for (int i=0; i<producerNumber; i++){
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, producer, NULL);
    }

    /* 4. Create consumer thread(s) */
    for (int i=0; i<consumerNumber; i++){
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, consumer, NULL);
    }

    /* 5. Sleep */
    sleep(sleepTime);

    /* 6. Exit */
    return 0;
}

/* ======================================== */
void *producer(void *param) {
    buffer_item item;
    while (1) {
        /* sleep for a random period of time */
        sleep(rand() % 5 + 1);
        /* generate a random number */
        item = rand() % 100;
        /* insert to queue */
        if (insert_item(item) == -1) {
            fprintf(stderr, "report error condition\n");
        } else {
            printf("producer produced %d\n", item);
        }
    }
}

void *consumer(void *param) {
    buffer_item item;
    while (1) {
        /* sleep for a random period of time */
        sleep(rand() % 5 + 1);
        /* remove from queue */
        if (remove_item(&item) == -1) {
            fprintf(stderr, "report error condition\n");
        } else {
            printf("consumer consumed %d\n", item);
        }
    }
}

/* ======================================== */
int insert_item(buffer_item item) {
    int successful = 0;
    /* insert item into buffer */
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    if (counter != BUFFER_SIZE) {
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        counter++;
    } else {
	    successful = -1;
	}
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    return successful;	
}

int remove_item(buffer_item *item) {
    int successful = 0;
    /* remove an object from buffer */
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    if (counter != 0) {
        *item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        counter--;
    } else {
        successful = -1;
    }
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return successful;
}
