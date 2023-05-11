#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define AMOUNT 15

void fifo_algo(int frame_limit, int *coming_page, int amount);
void lru_algo(int frame_limit, int *coming_page, int amount);
void optimal_algo(int frame_limit, int *coming_page, int amount);

/* ======================================== */
int main(int argc, char *argv[]) {
    srand(time(0));
    int frame_limit;
    int coming_page[AMOUNT];
    for (int i=0; i<AMOUNT; i++) {
        coming_page[i] = rand() % 10;
    }
    printf("請設定 page frames 大小 (1 ~ 7): ");
    scanf("%d", &frame_limit);
    while (frame_limit > 7 || frame_limit < 1) {
        int c;
        while((c=getchar())!='\n' && c!=EOF);
        printf("輸入有誤請重新輸入: ");
        scanf("%d", &frame_limit);
    }
    printf("page frames 數量: %d (element = -1 代表是空的)\n", frame_limit);
    printf("page 依序為: ");
    for (int i=0; i<AMOUNT; i++) {
        printf("%d ", coming_page[i]);
    }
    printf("\n");
    
    fifo_algo(frame_limit, coming_page, AMOUNT);
    lru_algo(frame_limit, coming_page, AMOUNT);
    optimal_algo(frame_limit, coming_page, AMOUNT);
    return 0;
}

/* ======================================== */
void optimal_algo(int frame_limit, int *coming_page, int amount) {
    printf("\n=== Optimal Algorithm ===\n");
    // LRU 設定初值為 -1
    int pagefault_counter = 0;
    int queue[frame_limit];
    for (int i=0; i<frame_limit; i++) {
        queue[i] = -1;
    }
    // page 依序進來
    for (int i=0; i<amount; i++) {
        bool isPagefault = true;
        bool isCompleted = false;
        for (int j=0; j<frame_limit; j++) {
            if (queue[j] == coming_page[i]) {
                // 已經存在 => 不用做事
                isPagefault = false;
                isCompleted = true;
                break;
            } else if (queue[j] == -1) {
                // 不存在但有空位 => page 放進來
                queue[j] = coming_page[i];
                isCompleted = true;
                break;
            }
        }
        // 沒空位放 page
        if (!isCompleted) {
            // 找到離下次使用最久的 page
            int counter = 0;
            int candidate = 0;
            bool flag[frame_limit];
            for (int j=0; j<frame_limit; j++) {
                flag[j] = false;
            }
            // 遍歷未來的 page
            for (int j=i+1; j<amount; j++) {
                // 跟 queue 的每個 page number 進行比較
                for (int k=0; k<frame_limit; k++) {
                    if (queue[k] == coming_page[j] && !flag[k]) {
                        counter++;
                        candidate = k;
                        flag[candidate] = true;
                        break;
                    }
                }
                // 最晚使用到的 page 已經確定了
                if (counter == frame_limit) break;
            }
            // 沒有最晚使用到的 page ==> 挑一個之後不會用到的
            if (counter < frame_limit) {
                for (int j=0; j<frame_limit; j++) {
                    if (!flag[j]) {
                        candidate = j;
                        break;
                    }
                }
            }
            // 替換成新進來的 page
            queue[candidate] = coming_page[i];
        }
        // 統計 page fault 並顯示本次結果
        if (isPagefault) pagefault_counter++;
        printf("page #%d 進入 => page frames: ", coming_page[i]);
        for (int j=0; j<frame_limit; j++) printf("%d ", queue[j]);
        printf(isPagefault? "(page fault)\n": "\n");
    }
    printf("\n總共 page fault %d 次\n", pagefault_counter);
    printf("=========================\n");
}

void lru_algo(int frame_limit, int *coming_page, int amount) {
    printf("\n===== LRU Algorithm =====\n");
    // LRU 設定初值為 -1
    int pagefault_counter = 0;
    int queue[frame_limit];
    int latest_time[frame_limit];
    for (int i=0; i<frame_limit; i++) {
        queue[i] = -1;
        latest_time[i] = -1;
    }
    // page 依序進來
    for (int i=0; i<amount; i++) {
        bool isPagefault = true;
        bool isCompleted = false;
        for (int j=0; j<frame_limit; j++) {
            if (queue[j] == coming_page[i]) {
                // 已經存在 => 不用做事，但記錄時間(用counter取代)
                latest_time[j] = i;
                isPagefault = false;
                isCompleted = true;
                break;
            } else if (queue[j] == -1) {
                // 不存在但有空位 => page 放進來，並記錄時間(用counter取代)
                queue[j] = coming_page[i];
                latest_time[j] = i;
                isCompleted = true;
                break;
            }
        }
        // 沒空位放 page
        if (!isCompleted) {
            // 找到最久未使用的 page
            int min_time = latest_time[0];
            for (int j=1; j<frame_limit; j++) {
                if (min_time > latest_time[j]) min_time = latest_time[j];
            }
            // 替換成新進來的 page，並記錄時間(用counter取代)
            for (int j=0; j<frame_limit; j++) {
                if (min_time == latest_time[j]) {
                    queue[j] = coming_page[i];
                    latest_time[j] = i;
                    break;
                }
            }
        }
        // 統計 page fault 並顯示本次結果
        if (isPagefault) pagefault_counter++;
        printf("page #%d 進入 => page frames: ", coming_page[i]);
        for (int j=0; j<frame_limit; j++) printf("%d ", queue[j]);
        printf(isPagefault? "(page fault)\n": "\n");
    }
    printf("\n總共 page fault %d 次\n", pagefault_counter);
    printf("=========================\n");
}

void fifo_algo(int frame_limit, int *coming_page, int amount) {
    printf("\n===== FIFO Algorithm =====\n");
    // FIFO 設定初值為 -1
    int pagefault_counter = 0;
    int queue[frame_limit];
    int loading_time[frame_limit];
    for (int i=0; i<frame_limit; i++) {
        queue[i] = -1;
        loading_time[i] = -1;
    }
    // page 依序進來
    for (int i=0; i<amount; i++) {
        bool isPagefault = true;
        bool isCompleted = false;
        for (int j=0; j<frame_limit; j++) {
            if (queue[j] == coming_page[i]) {
                // 已經存在 => 不用做事
                isPagefault = false;
                isCompleted = true;
                break;
            } else if (queue[j] == -1) {
                // 不存在但有空位 => page 放進來，並記錄時間(用counter取代)
                queue[j] = coming_page[i];
                loading_time[j] = i;
                isCompleted = true;
                break;
            }
        }
        // 沒空位放 page
        if (!isCompleted) {
            // 找到最早進入 queue 的 page
            int min_time = loading_time[0];
            for (int j=1; j<frame_limit; j++) {
                if (min_time > loading_time[j]) min_time = loading_time[j];
            }
            // 替換成新進來的 page，並記錄時間(用counter取代)
            for (int j=0; j<frame_limit; j++) {
                if (min_time == loading_time[j]) {
                    queue[j] = coming_page[i];
                    loading_time[j] = i;
                    break;
                }
            }
        }
        // 統計 page fault 並顯示本次結果
        if (isPagefault) pagefault_counter++;
        printf("page #%d 進入 => page frames: ", coming_page[i]);
        for (int j=0; j<frame_limit; j++) printf("%d ", queue[j]);
        printf(isPagefault? "(page fault)\n": "\n");
    }
    printf("\n總共 page fault %d 次\n", pagefault_counter);
    printf("=========================\n");
}
