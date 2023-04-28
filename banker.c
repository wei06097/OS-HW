#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

/* sleep 1~5s */
#define SLEEP_TIME 5
/* these may be any values >= 0 */
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3
/* the available amount of each resource */
/* the maximum demand of each customer */
/* the amount currently allocated to each customer */
/* the remaining need of each customer */
int available[NUMBER_OF_RESOURCES];
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

pthread_mutex_t lock;
void showResources();
void *runner(void *arg);
bool isSafe();
int request_resources(int customer_num, int request[]);
int release_resources(int customer_num, int release[]);

/* ======================================== */
int main(int argc, char *argv[]) {
    srand(time(0));
    
    /* 初始化資源 */
    if (argc-1 != NUMBER_OF_RESOURCES) {
        fprintf(stderr, "參數數量錯誤\n");
        return -1;
    }
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        available[i] = atoi(argv[i+1]);
        for (int j=0; j<NUMBER_OF_CUSTOMERS; j++) {
            maximum[j][i] = rand() % (available[i] + 1);
            allocation[j][i] = 0;
            need[j][i] = maximum[j][i];
        }
    }

    /* 顯示資源 */
    showResources();

    /* 建立 customer threads */
    pthread_t tid[NUMBER_OF_CUSTOMERS];
    int customer_numer[NUMBER_OF_CUSTOMERS];
    for (int i=0; i<NUMBER_OF_CUSTOMERS; i++) {
        customer_numer[i] = i;
        pthread_create(&(tid[i]), NULL, runner, &customer_numer[i]);
    }
    for (int i=0; i<NUMBER_OF_CUSTOMERS; i++) {
        pthread_join(tid[i], NULL);
    }

    /* 退出 */
    return 0;
}

/* ======================================== */
void showResources() {
    printf(" maximum     allocation     need     available: ");
    for (int i=0; i<NUMBER_OF_RESOURCES; i++)
        printf("%2d ", available[i]);
    printf("\n");
    for (int i=0; i<NUMBER_OF_CUSTOMERS; i++) {
        for (int j=0; j<NUMBER_OF_RESOURCES; j++)
            printf("%2d ", maximum[i][j]);
        printf("    ");
        for (int j=0; j<NUMBER_OF_RESOURCES; j++)
            printf("%2d ", allocation[i][j]);
        printf("    ");
        for (int j=0; j<NUMBER_OF_RESOURCES; j++)
            printf("%2d ", need[i][j]);
        printf("\n");
    }
    printf("\n");
}

void *runner(void *arg) {
    int customer_num = *(int *)arg;
    while (1) {
        sleep(rand() % SLEEP_TIME + 1);
        /* 亂數產生 request 數量 */
        int request[NUMBER_OF_RESOURCES];
        for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
            request[i] = rand() % (need[customer_num][i] + 1);
        }
        /* 發出 request */
        if (request_resources(customer_num, request) != -1) {
            printf("customer #%d requset: ", customer_num);
            for (int i=0; i<NUMBER_OF_RESOURCES; i++) printf("%d ", request[i]);
            printf("=> 成功\n");
            showResources();
        } else {
            printf("customer #%d requset: ", customer_num);
            for (int i=0; i<NUMBER_OF_RESOURCES; i++) printf("%d ", request[i]);
            printf("=> 失敗\n");
            showResources();
            continue;
        }

        sleep(rand() % SLEEP_TIME + 1);
        /*　亂數產生 release 數量 */
        int release[NUMBER_OF_RESOURCES];
        for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
            release[i] = rand() % (allocation[customer_num][i] + 1);
        }
        /* release */
        if (release_resources(customer_num, release) != -1) {
            printf("customer #%d release: ", customer_num);
            for (int i=0; i<NUMBER_OF_RESOURCES; i++) printf("%d ", release[i]);
            printf("=> 成功\n");
            showResources();
        } else {
            printf("customer #%d release: ", customer_num);
            for (int i=0; i<NUMBER_OF_RESOURCES; i++) printf("%d ", release[i]);
            printf("=> 失敗\n");
            showResources();
            continue;
        }
    }
}

bool isSafe() {
    bool safe = true;
    int work[NUMBER_OF_RESOURCES];
    bool finish[NUMBER_OF_CUSTOMERS];

    // 1. 設定初始値
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        work[i] = available[i];
    }
    for (int i=0; i<NUMBER_OF_CUSTOMERS; i++) {
        finish[i] = false;
    }

    while (1) {
        // 2. 找出一個Process i, 滿足 Need i <= Work, Finish[i] = False, 找到去3沒找到去4
        int candidate = -1;
        for (int customer=0; customer<NUMBER_OF_CUSTOMERS; customer++) {
            bool flag = false;
            for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
                if (finish[customer]) flag = true;
                if (need[customer][i] > work[i]) flag = true;
                if (flag) break;
            }
            if (!flag) {
                candidate = customer;
                break;
            }
        }

        // 3. 設定 Finish[i] = True, Work = Work + Allocation i, 回去2
        if (candidate != -1) {
            finish[candidate] = true;
            for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
                work[i] += allocation[candidate][i];
            }
        } else {
        // 4. finish 全部 True => Safe State, 否則 => Unsafe State
            for (int i=0; i<NUMBER_OF_CUSTOMERS; i++) {
                if (!finish[i]) safe = false;
            }
            break;
        }
    }

    return safe;
}

/* ======================================== */
int release_resources(int customer_num, int release[]) {
    int granted = 0;
    pthread_mutex_lock(&lock);

    /* 檢查 release 是否大於 allocation (不合法) */
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        if (release[i] - allocation[customer_num][i] > 0) granted = -1;
        if (granted == -1) break;
    }

    /* 合法 => release recollection */
    if (granted == 0) {
        for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
            allocation[customer_num][i] -= release[i];
            need[customer_num][i] += release[i];
            available[i] += release[i];
        }
    }

    pthread_mutex_unlock(&lock);
    return granted;
}

int request_resources(int customer_num, int request[]) {
    int granted = 0;
    pthread_mutex_lock(&lock);

    /* 檢查 request 是否大於 available 和 need (不合法) */
    for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
        if (request[i] - available[i] > 0) granted = -1;
        if (request[i] - need[customer_num][i] > 0) granted = -1;
        if (granted == -1) break;
    }

    /* 合法 => safety algorithm */
    if (granted == 0) {
        /* 假設性試算(假設同意) */
        for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
            available[i] -= request[i];
            need[customer_num][i] -= request[i];
            allocation[customer_num][i] += request[i];
        }
        /* Unsafe => 改回來 */
        if (!isSafe()) {
            granted = -1;
            for (int i=0; i<NUMBER_OF_RESOURCES; i++) {
                available[i] += request[i];
                need[customer_num][i] += request[i];
                allocation[customer_num][i] -= request[i];
            }
        }
    }

    pthread_mutex_unlock(&lock);
    return granted;
}
