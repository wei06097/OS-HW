#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

/* ===================================== */
typedef struct
{
    int starting;
    int ending;
} Parameters;

int *sorted_list, list[] = {7, 12, 19, 3, 18, 4, 2, 6, 15, 8};
void *result1, *result2;
int length1, length2;

/* ===================================== */
void showList(int *array, int length) {
    for (int i=0; i<length; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}
void *merge() {
    int *arr1 = result1;
    int *arr2 = result2;
    /* merge array */
    int length = length1 + length2;
    sorted_list = (int *) malloc(sizeof(int) * length);
    for (int i=0; i<length; i++) {
        if (i < length1) sorted_list[i] = arr1[i];
        else sorted_list[i] = arr2[i - length1];
    }
    /* bubble sort */
    for (int i=0; i<length; i++) {
        for (int j=0; j<i; j++) {
            if (sorted_list[j] > sorted_list[i]) {
                int temp = sorted_list[j];
                sorted_list[j] = sorted_list[i];
                sorted_list[i] = temp;
            }
        }
    }
}
void *sort(void *params) {
    Parameters *data = params;
    /* make new list */
    int length = data->ending - data->starting + 1;
    int *newList = (int *) malloc(sizeof(int) * length);
    /* copy form list to newList */
    for (int i=0; i<length; i++) {
        newList[i] = list[data->starting + i];
    }
    /* bubble sort */
    for (int i=0; i<length; i++) {
        for (int j=0; j<i; j++) {
            if (newList[j] > newList[i]) {
                int temp = newList[j];
                newList[j] = newList[i];
                newList[i] = temp;
            }
        }
    }
    return newList;
}

/* ===================================== */
int main() {
    int length = sizeof(list)/sizeof(int);
    int half = floor((length - 1) / 2);
    if (length <= 0) {
        printf("list is invalid\n");
        return 0;
    }
    printf("List: ");
    showList(list, length);
    Parameters *data1 = (Parameters *) malloc(sizeof(Parameters));
    Parameters *data2 = (Parameters *) malloc(sizeof(Parameters));
    data1->starting = 0;
    data1->ending = half;
    data2->starting = half + 1;
    data2->ending = length - 1;
    length1 = data1->ending - data1->starting + 1;
    length2 = data2->ending - data2->starting + 1;

    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, sort, data1);
    pthread_create(&tid2, NULL, sort, data2);
    pthread_join(tid1, &result1);
    pthread_join(tid2, &result2);
    printf("Sorting Thread 0: ");
    showList(result1, length1);
    printf("Sorting Thread 1: ");
    showList(result2, length2);
    
    pthread_t tid3;
    pthread_create(&tid3, NULL, merge, NULL);
    pthread_join(tid3, NULL);
    printf("Merge Thread: ");
    showList(sorted_list, length);
    return 0;
}
