#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/widgets/canvas/Circle.hpp>
#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <touchgfx/hal/HAL.hpp>

// --- Các hằng số cấu hình ---
#define MAX_BALL_OFFSET_X           200   // Giới hạn độ lệch tối đa theo pixel
#define MAX_BALL_OFFSET_Y           100
#define ANGLE_TO_PIXEL_SENSITIVITY  7.0f  // Độ nhạy chung, số pixel cho mỗi độ nghiêng
#define MAX_INTEGRATED_ANGLE        70.0f // Giới hạn góc tích lũy để tránh giá trị quá lớn
#define GYRO_UPDATE_INTERVAL_MS     20    // Nên đồng bộ với tần suất gửi dữ liệu của task cảm biến
#define DAMPING_FACTOR              1.0f // Hệ số giảm chấn, càng gần 1 càng trôi chậm về 0

extern "C" {
    #include "cmsis_os.h"
    extern osMessageQueueId_t Queue1Handle;
}

typedef struct {
    float gx; // Dữ liệu đã được hiệu chỉnh (trừ offset)
    float gy;
    float gz;
} GyroData_t;

Screen1View::Screen1View():
    currentAngleX(0.0f),
    currentAngleY(0.0f),
    initialBallX(0),
    initialBallY(0)
{
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
    // Lưu vị trí ban đầu của quả bóng
    initialBallX = circle1.getX();
    initialBallY = circle1.getY();

    // Lý tưởng nhất là khi khởi động, bạn nên chạy một quy trình hiệu chỉnh gyro
    // (đọc N mẫu, tính trung bình offset) trong task cảm biến.
    // Ở đây, chúng ta giả định dữ liệu trong queue đã được hiệu chỉnh.
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

// Đổi tên hàm thành handleTickEvent để nhất quán với TouchGFX
void Screen1View::tickEvent()
{
    GyroData_t gyroData;
    bool newDataReceived = false;

    // Chỉ nên xử lý một message mỗi tick để tránh giao diện bị "giật" nếu queue đầy
    if (Queue1Handle != NULL && osMessageQueueGet(Queue1Handle, &gyroData, NULL, 0) == osOK)
    {
        newDataReceived = true;
    }

    if (newDataReceived)
    {
        // Thời gian delta, tính bằng giây
        float deltaTimeSeconds = (float)GYRO_UPDATE_INTERVAL_MS / 1000.0f;

        // Tích phân vận tốc góc để cập nhật góc nghiêng
        // Chú ý: Kiểm tra lại gx/gy và dấu +/- cho đúng với hướng nghiêng thực tế
        currentAngleX += gyroData.gy * deltaTimeSeconds;
        currentAngleY += gyroData.gx * deltaTimeSeconds; // Thường thì trục Y màn hình ngược với trục X của gyro

        // Giới hạn góc tích lũy để chống trôi vô hạn
        if (currentAngleX > MAX_INTEGRATED_ANGLE) currentAngleX = MAX_INTEGRATED_ANGLE;
        if (currentAngleX < -MAX_INTEGRATED_ANGLE) currentAngleX = -MAX_INTEGRATED_ANGLE;
        if (currentAngleY > MAX_INTEGRATED_ANGLE) currentAngleY = MAX_INTEGRATED_ANGLE;
        if (currentAngleY < -MAX_INTEGRATED_ANGLE) currentAngleY = -MAX_INTEGRATED_ANGLE;
    }
    else
    {
        // Nếu không có dữ liệu mới, áp dụng hiệu ứng giảm chấn (damping)
        // Điều này giúp quả bóng từ từ quay về trung tâm và chống lại hiện tượng drift
        currentAngleX *= DAMPING_FACTOR;
        currentAngleY *= DAMPING_FACTOR;
    }

    // Ánh xạ góc sang độ lệch pixel
    int16_t offsetX = (int16_t)(currentAngleX * ANGLE_TO_PIXEL_SENSITIVITY);
    int16_t offsetY = (int16_t)(currentAngleY * ANGLE_TO_PIXEL_SENSITIVITY);

    // Giới hạn độ lệch pixel
    if (offsetX > MAX_BALL_OFFSET_X) offsetX = MAX_BALL_OFFSET_X;
    if (offsetX < -MAX_BALL_OFFSET_X) offsetX = -MAX_BALL_OFFSET_X;
    if (offsetY > MAX_BALL_OFFSET_Y) offsetY = MAX_BALL_OFFSET_Y;
    if (offsetY < -MAX_BALL_OFFSET_Y) offsetY = -MAX_BALL_OFFSET_Y;

    // Di chuyển quả bóng đến vị trí mới và yêu cầu vẽ lại
    circle1.moveTo(initialBallX + offsetX, initialBallY + offsetY);
    circle1.invalidate();
}
//#include <gui/screen1_screen/Screen1View.hpp>
//#include <touchgfx/widgets/canvas/Circle.hpp>
//#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
//#include <touchgfx/hal/HAL.hpp>
//
//#define MAX_BALL_OFFSET_X 500 // Độ lệch tối đa của quả bóng theo trục X (tính từ tâm)
//#define MAX_BALL_OFFSET_Y 500  // Độ lệch tối đa của quả bóng theo trục Y
//#define ANGLE_TO_PIXEL_SENSITIVITY_X 2.0f // Số pixel lệch cho mỗi độ nghiêng X
//#define ANGLE_TO_PIXEL_SENSITIVITY_Y 2.0f // Số pixel lệch cho mỗi độ nghiêng Y
//#define MAX_ANGLE 180.0f // Giới hạn góc nghiêng tối đa mà chúng ta quan tâm (ví dụ: 45 độ)
//#define GYRO_UPDATE_INTERVAL_MS 100 // Khoảng thời gian giữa các lần gửi dữ liệu gyro (ms)
//
//extern uint32_t tickCount;
//extern "C" {
//    #include "cmsis_os.h"         // Để có osMessageQueueId_t
//    extern osMessageQueueId_t Queue1Handle; // Khai báo ngoài
//}
//typedef struct {
//    float gx;
//    float gy;
//    float gz;
//} GyroData_t;
//
//Screen1View::Screen1View():
//		currentAngleX(0.0f),
//		currentAngleY(0.0f)
//{
//
//}
//
//void Screen1View::setupScreen()
//{
//    Screen1ViewBase::setupScreen();
//    initialBallX = circle1.getX();
//    initialBallY = circle1.getY();
//}
//
//void Screen1View::tearDownScreen()
//{
//    Screen1ViewBase::tearDownScreen();
//}
//void Screen1View::tickEvent(){
////	GyroData_t currentGyroData;
////	    uint32_t count = osMessageQueueGetCount(Queue1Handle);
////	    if (count > 0)
////	    {
////	        osMessageQueueGet(Queue1Handle, &currentGyroData, NULL, osWaitForever);
////	        circle1.moveTo(initialBallX+currentGyroData.gy,initialBallY+currentGyroData.gx);
////	        circle1.invalidate();
////	    }
////	circle1.moveTo(initialBallX, initialBallY);
////	circle1.invalidate();
//	GyroData_t gyroData;
//	    // Fixed deltaTime dựa trên tần suất đọc/gửi của task GyroReadTask
//	    float deltaTimeSeconds = (float)GYRO_UPDATE_INTERVAL_MS / 1000.0f;
//
//	    if (Queue1Handle != NULL)
//	    {
//	        // Lấy dữ liệu từ queue (không block)
//	        if (osMessageQueueGet(Queue1Handle, &gyroData, NULL, 0) == osOK)
//	        {
//	            // Tích phân (ví dụ hướng trục, điều chỉnh dấu nếu cần)
//	            currentAngleY += gyroData.gx * deltaTimeSeconds;
//	            currentAngleX += gyroData.gy * deltaTimeSeconds;
//
//	            // Giới hạn góc
//	            if (currentAngleX > MAX_ANGLE) currentAngleX = MAX_ANGLE;
//	            if (currentAngleX < -MAX_ANGLE) currentAngleX = -MAX_ANGLE;
//	            if (currentAngleY > MAX_ANGLE) currentAngleY = MAX_ANGLE;
//	            if (currentAngleY < -MAX_ANGLE) currentAngleY = -MAX_ANGLE;
//
//	            // Ánh xạ và giới hạn độ lệch pixel
//	            float offsetX = currentAngleX * ANGLE_TO_PIXEL_SENSITIVITY_X;
//	            float offsetY = currentAngleY * ANGLE_TO_PIXEL_SENSITIVITY_Y;
//
//	            if (offsetX > MAX_BALL_OFFSET_X) offsetX = MAX_BALL_OFFSET_X;
//	            if (offsetX < -MAX_BALL_OFFSET_X) offsetX = -MAX_BALL_OFFSET_X;
//	            if (offsetY > MAX_BALL_OFFSET_Y) offsetY = MAX_BALL_OFFSET_Y;
//	            if (offsetY < -MAX_BALL_OFFSET_Y) offsetY = -MAX_BALL_OFFSET_Y;
//
//	            // Di chuyển quả bóng
//	            circle1.moveTo(initialBallX + (int16_t)offsetX, initialBallY + (int16_t)offsetY);
//	            circle1.invalidate();
//	        }
//	        // Nếu queue trống, currentAngleX/Y sẽ giữ nguyên.
//	        // Bạn có thể thêm code để giảm dần currentAngleX/Y về 0 ở đây nếu muốn hiệu ứng "quay trở lại"
//	        // khi không có dữ liệu mới.
//	        // Ví dụ:
//	        // else { // Nếu osMessageQueueGet không thành công (queue rỗng)
//	        //     currentAngleX *= 0.98f; // Giảm nhẹ mỗi tick
//	        //     currentAngleY *= 0.98f;
//	        // }
//	    }
//}
