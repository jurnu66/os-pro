#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_SIZE 5 // ขนาด buffer สูงสุด

int buffer[MAX_SIZE]; // buffer สำหรับเก็บข้อมูล
int count = 0;        // จำนวนข้อมูลใน buffer
pthread_mutex_t mutex;        // mutex สำหรับ mutual exclusion
pthread_cond_t notFull;       // condition เมื่อ buffer ไม่เต็ม
pthread_cond_t notEmpty;      // condition เมื่อ buffer ไม่ว่าง

// ฟังก์ชันสำหรับ Producer
void* producer(void* arg) {
    for (int i = 1; i <= 10; i++) {
        pthread_mutex_lock(&mutex); // เข้าสู่ critical section

        // รอจน buffer มีพื้นที่ว่าง
        while (count == MAX_SIZE) {
            pthread_cond_wait(&notFull, &mutex);
        }

        // เพิ่ม item ลงใน buffer
        buffer[count++] = i;
        printf("Produced: %d\n", i);

        // แจ้ง consumer ว่ามี item ใน buffer
        pthread_cond_signal(&notEmpty);
        pthread_mutex_unlock(&mutex); // ออกจาก critical section

        sleep(1); // จำลองการทำงาน
    }
    return NULL;
}

// ฟังก์ชันสำหรับ Consumer
void* consumer(void* arg) {
    for (int i = 1; i <= 10; i++) {
        pthread_mutex_lock(&mutex); // เข้าสู่ critical section

        // รอจน buffer มีข้อมูล
        while (count == 0) {
            pthread_cond_wait(&notEmpty, &mutex);
        }

        // ดึง item ออกจาก buffer
        int item = buffer[--count];
        printf("Consumed: %d\n", item);

        // แจ้ง producer ว่ามีพื้นที่ว่างใน buffer
        pthread_cond_signal(&notFull);
        pthread_mutex_unlock(&mutex); // ออกจาก critical section

        sleep(1); // จำลองการทำงาน
    }
    return NULL;
}

int main() {
    pthread_t producerThread, consumerThread;

    // สร้าง mutex และ condition variables
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&notFull, NULL);
    pthread_cond_init(&notEmpty, NULL);

    // สร้าง threads
    pthread_create(&producerThread, NULL, producer, NULL);
    pthread_create(&consumerThread, NULL, consumer, NULL);

    // รอให้ threads ทำงานเสร็จ
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    // ทำลาย mutex และ condition variables
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&notFull);
    pthread_cond_destroy(&notEmpty);

    return 0;
}
