#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

class Monitor {
private:
    std::queue<int> buffer;        // ใช้ queue เป็น buffer
    const unsigned int maxSize;   // ขนาดสูงสุดของ buffer
    std::mutex mtx;               // สำหรับ mutual exclusion
    std::condition_variable notFull;  // condition เมื่อ buffer ไม่เต็ม
    std::condition_variable notEmpty; // condition เมื่อ buffer ไม่ว่าง

public:
    Monitor(unsigned int size) : maxSize(size) {}

    // ฟังก์ชันสำหรับ Producer
    void produce(int item) {
        std::unique_lock<std::mutex> lock(mtx);  // เข้าสู่ critical section
        notFull.wait(lock, [this]() { return buffer.size() < maxSize; }); // รอจน buffer มีที่ว่าง

        buffer.push(item);  // เพิ่ม item ลงใน buffer
        std::cout << "Produced: " << item << std::endl;

        notEmpty.notify_one(); // แจ้ง consumer ว่ามี item ให้ใช้งาน
    }

    // ฟังก์ชันสำหรับ Consumer
    int consume() {
        std::unique_lock<std::mutex> lock(mtx);  // เข้าสู่ critical section
        notEmpty.wait(lock, [this]() { return !buffer.empty(); }); // รอจน buffer มี item

        int item = buffer.front();  // ดึง item จาก buffer
        buffer.pop();
        std::cout << "Consumed: " << item << std::endl;

        notFull.notify_one(); // แจ้ง producer ว่ามีพื้นที่ว่าง
        return item;
    }
};

// ฟังก์ชันของ Producer
void producer(Monitor& monitor) {
    for (int i = 1; i <= 10; ++i) {
        monitor.produce(i);  // ส่งข้อมูลไปยัง monitor
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // จำลองการทำงาน
    }
}

// ฟังก์ชันของ Consumer
void consumer(Monitor& monitor) {
    for (int i = 1; i <= 10; ++i) {
        monitor.consume();  // ดึงข้อมูลจาก monitor
        std::this_thread::sleep_for(std::chrono::milliseconds(150)); // จำลองการทำงาน
    }
}

int main() {
    Monitor monitor(5); // สร้าง Monitor โดย buffer มีขนาดสูงสุด 5

    // สร้าง threads สำหรับ Producer และ Consumer
    std::thread producerThread(producer, std::ref(monitor));
    std::thread consumerThread(consumer, std::ref(monitor));

    // รอให้ threads ทำงานเสร็จ
    producerThread.join();
    consumerThread.join();

    return 0;
}
