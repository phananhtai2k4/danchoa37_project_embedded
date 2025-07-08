# CHUỘT MÁY TÍNH CÂN BẰNG

- Thiết kế một hệ thống tương tác trực quan sử dụng giao diện TouchGFX.

- Tận dụng cảm biến gia tốc và gyro tích hợp trên STM32F429I-DISC1 để phát hiện góc nghiêng thiết bị.

- Quả bóng trên màn hình di chuyển tương ứng với hướng nghiêng.

- Tích hợp HID mouse để điều khiển con trỏ chuột theo chuyển động nghiêng của thiết bị.

- Tăng cường khả năng trực quan hóa và trình diễn tương tác thực tế giữa cảm biến và giao diện.

## GIỚI THIỆU

**Đề bài**: Thiết kế hệ thống trên STM32F429I-DISC1 để điều khiển quả bóng và chuột HID theo góc nghiêng bằng cảm biến gyro, sử dụng TouchGFX để hiển thị giao diện.

**Sản phẩm:**

1. Điều khiển chuyển động quả bóng theo cảm biến gyro
2. Mô phỏng chuột HID theo góc nghiêng thiết bị
3. Đồng bộ thời gian thực giữa giao diện và chức năng điều khiển chuột

<img src="https://github.com/phananhtai2k4/danchoa37_project_embedded/blob/main/img/logo-soict.png" alt="Logo Khoa KHKTMT" height="400">

## TÁC GIẢ

- Tên nhóm: Dân choa 37

- Thành viên trong nhóm
  |STT|Họ tên|MSSV|Công việc|
  |--:|--|--|--|
  |1|Phan Anh Tài|20225391|UI & TouchGFX|
  |2|Nguyễn Văn Thái|20225394|Cảm biến & xử lý dữ liệu|
  |3|Ngô Đức Đại|20225272|Tích hợp HID Mouse & kết nối hệ thống|

## MÔI TRƯỜNG HOẠT ĐỘNG

- Module CPU/dev kit: STM32F429I-DISC1
- Các kit, module được sử dụng: Module Cảm biến Chuyển động (Gyroscope I3G4250D), Module Màn hình LCD-TFT, Module USB OTG

## SƠ ĐỒ SCHEMATIC

<img src="https://github.com/phananhtai2k4/danchoa37_project_embedded/blob/main/img/gyro_img.jpeg" alt="Logo Khoa KHKTMT" height="400">

_cách nối dây, kết nối giữa các linh kiện_

| STM32F429 | Module ngoại vi |
| --------- | --------------- |
| PF7       | SPI5_SCK        |
| PF8       | SPI5_MISO       |
| PF9       | SPI5_MOSI       |
| PC1       | GYRO_CS         |

hoặc bất cứ dạng nào thể hiện được cách đấu nối giữa các linh kiện

### TÍCH HỢP HỆ THỐNG

**Các thành phần phần cứng và vai trò của chúng:**

- Thiết bị điều khiển trung tâm (Central Control Device):
  Thành phần: Toàn bộ kit STM32F429I-DISC1.
  Vai trò: Đây là "bộ não" và cũng là "cơ thể" của toàn bộ hệ thống. Nó đóng vai trò của cả "Thiết bị IoT" (thu thập dữ liệu từ cảm biến) và "Bộ xử lý" (xử lý và hiển thị ngay tại chỗ). Nó không gửi dữ liệu đi đâu xa mà tự xử lý và tương tác trực tiếp.
- Module Vi xử lý (Processor Module):
  Thành phần: Chip STM32F429ZIT6 trên kit.
  Vai trò: Là đơn vị xử lý trung tâm (CPU). Nó thực thi toàn bộ mã nguồn chương trình, đọc dữ liệu từ cảm biến, tính toán tọa độ mới cho quả bóng và con trỏ chuột, điều khiển màn hình LCD và giao tiếp qua cổng USB.
- Module Hiển thị (Display Module):
  Thành phần: Màn hình LCD-TFT 2.4 inch tích hợp trên kit.
  Vai trò: Là thiết bị đầu ra chính của hệ thống. Nó trực quan hóa kết quả xử lý bằng cách hiển thị giao diện đồ họa (quả bóng) và cập nhật vị trí của nó theo thời gian thực.
- Thiết bị chủ (Host Device):
  Thành phần: Máy tính (PC/Laptop).
  Vai trò: Đóng vai trò là "máy trạm" nhận tín hiệu điều khiển. Nó nhận dữ liệu vị trí chuột từ kit STM32 qua cổng USB và di chuyển con trỏ trên màn hình của nó.
  **Các thành phần phần mềm và vai trò của chúng, vị trí nằm trên phần cứng nào:**
- Giao diện đồ họa (GUI - Có thể xem như là "Front-end" của thiết bị)
  Thành phần: Thư viện TouchGFX.
  Vai trò: Chịu trách nhiệm cho tất cả những gì người dùng nhìn thấy trên màn hình LCD. Nó vẽ nền, vẽ quả bóng và cung cấp các hàm để di chuyển quả bóng.
  Vị trí: Chạy trên STM32F429ZIT6, sử dụng các tài nguyên phần cứng là bộ điều khiển LTDC và bộ tăng tốc đồ họa DMA2D.
- Lõi ứng dụng (Application Logic - Có thể xem như là "Back-end" của thiết bị)
  Thành phần: Mã nguồn trong main.c và các file logic do bạn viết.
  Vai trò: Đây là phần xử lý chính. Nó thực hiện vòng lặp vô tận: đọc dữ liệu thô từ cảm biến, tính toán để chuyển đổi thành tọa độ (cho quả bóng) và độ dịch chuyển (cho chuột), sau đó ra lệnh cho TouchGFX cập nhật màn hình và gửi dữ liệu qua USB.
  Vị trí: Chạy trên STM32F429ZIT6.
- Các lớp Middleware và Trình điều khiển (Drivers & Middleware)
  Thành phần:
  HAL (Hardware Abstraction Layer): Thư viện của ST cung cấp các hàm cấp cao để làm việc với phần cứng (SPI, GPIO, USB...).
  USB Device Stack (Lớp HID): Thư viện xử lý giao thức USB, định dạng dữ liệu theo chuẩn của một con chuột.
  BSP (Board Support Package): Thư viện cung cấp các hàm đã được viết sẵn để khởi tạo và giao tiếp với cảm biến Gyro (I3G4250D).
  Vai trò: Là các lớp phần mềm trung gian. Chúng che giấu sự phức tạp của việc tương tác trực tiếp với thanh ghi phần cứng, giúp cho Lõi ứng dụng có thể gọi các hàm đơn giản như HAL_SPI_Transmit() hay USBD_CUSTOM_HID_SendReport() một cách dễ dàng.
  Vị trí: Chạy trên STM32F429ZIT6.
- Phần mềm trên máy chủ (Host Software)
  Thành phần: Trình điều khiển chuột (Mouse Driver) của hệ điều hành (Windows, macOS, Linux).
  Vai trò: Đây là phần mềm có sẵn trên máy tính. Nó tự động nhận diện thiết bị của bạn khi cắm vào, nhận các "báo cáo HID" mà kit gửi qua và dịch chúng thành hành động di chuyển con trỏ trên màn hình máy tính.
  Vị trí: Chạy trên CPU của máy tính.

### ĐẶC TẢ HÀM

#### **Module Đọc và Phân Phối Dữ Liệu Cảm Biến (`StartDefaultTask`)**

Đây là tác vụ (task) trung tâm trong hệ điều hành thời gian thực (RTOS). Vai trò của nó là "nhà sản xuất" (producer): liên tục đọc dữ liệu từ cảm biến Gyroscope, thực hiện hiệu chỉnh ban đầu, và sau đó phân phối dữ liệu đã xử lý đến các tác vụ "người tiêu dùng" (consumer) khác thông qua hàng đợi thông điệp (message queue).

##### **Mã nguồn**

```C
  void StartDefaultTask(void *argument)
  {
    MX_USB_DEVICE_Init();

    GyroData_t currentGyroData;
    float bsp_gyro_data[3];
    float gyro_offset[3] = {0.0f, 0.0f, 0.0f};

    const int CALIBRATION_SAMPLES = 200;

    osDelay(500);
    if (BSP_GYRO_Init() != BSP_GYRO_OK)
    {
      Error_Handler();
    }

    for (int i = 0; i < CALIBRATION_SAMPLES; i++)
    {
      if (BSP_GYRO_GetXYZ(bsp_gyro_data) == BSP_GYRO_OK)
      {
        gyro_offset[0] += bsp_gyro_data[0];
        gyro_offset[1] += bsp_gyro_data[1];
        gyro_offset[2] += bsp_gyro_data[2];
      }
      osDelay(5);
    }
    gyro_offset[0] /= CALIBRATION_SAMPLES;
    gyro_offset[1] /= CALIBRATION_SAMPLES;
    gyro_offset[2] /= CALIBRATION_SAMPLES;

    for(;;)
    {
      if (BSP_GYRO_GetXYZ(bsp_gyro_data) == BSP_GYRO_OK)
      {
          currentGyroData.gx = bsp_gyro_data[0] - gyro_offset[0];
          currentGyroData.gy = bsp_gyro_data[1] - gyro_offset[1];
          currentGyroData.gz = bsp_gyro_data[2] - gyro_offset[2];

          if (Queue1Handle != NULL) {
              osMessageQueuePut(Queue1Handle, &currentGyroData, 0U, 0);
          }

          if (Queue2Handle != NULL) {
              osMessageQueuePut(Queue2Handle, &currentGyroData, 0U, 0);
          }
      }
      osDelay(20);
    }
  }
```

##### 🛠️ Giải thích Luồng Hoạt Động

Luồng hoạt động của tác vụ này được chia thành 3 giai đoạn chính:

---

###### 1. Khởi tạo (Initialization)

- `MX_USB_DEVICE_Init()` : Khởi tạo ngoại vi USB, chuẩn bị để vi điều khiển có thể giao tiếp với máy tính như một thiết bị HID.

- `BSP_GYRO_Init()` : Khởi tạo cảm biến Gyroscope bằng cách sử dụng thư viện BSP (Board Support Package), thiết lập các thanh ghi cần thiết để cảm biến bắt đầu hoạt động.

---

###### 2. Hiệu chỉnh cảm biến (Sensor Calibration)

- Cảm biến Gyroscope thường có một sai số cố hữu gọi là **"bias" hoặc "zero-rate offset"**, nghĩa là nó vẫn cho ra một giá trị khác không ngay cả khi đang đứng yên.

- Để khắc phục, chương trình thực hiện một vòng lặp đọc dữ liệu khi thiết bị đang ở trạng thái nghỉ.

- Sau đó, nó tính toán giá trị trung bình của sai số này (`gyro_offset`) cho cả 3 trục. Giá trị offset này sẽ được sử dụng để hiệu chỉnh các lần đọc sau này.

- **Lưu ý**: Quá trình này đòi hỏi người dùng phải giữ yên thiết bị trong vài giây đầu tiên sau khi cấp nguồn để có kết quả hiệu chỉnh chính xác.

---

###### 3. Vòng lặp chính – Đọc, Xử lý và Phân phối

- Vòng lặp này chạy liên tục để thực hiện các công việc cốt lõi:

###### ▸ Đọc và Hiệu chỉnh dữ liệu:

- `BSP_GYRO_GetXYZ()` : Đọc dữ liệu vận tốc góc thô từ cảm biến.

- Dữ liệu thô sau đó được trừ đi giá trị `gyro_offset` đã tính ở bước 2 để có được giá trị vận tốc góc chính xác hơn.

###### ▸ Phân phối dữ liệu:

- Dữ liệu đã được hiệu chỉnh (`currentGyroData`) được gửi vào hàng đợi khác nhau:

  1. `osMessageQueuePut(QueueHandle, ...)` : Gửi một bản sao dữ liệu vào hàng đợi cho tác vụ xử lý **giao diện quả bóng**.

  2. `osMessageQueuePut(Queue2Handle, ...)` : Gửi một bản sao dữ liệu vào hàng đợi cho tác vụ xử lý việc gửi tín hiệu **chuột HID**.

- Kiến trúc này giúp tách biệt các logic, làm cho hệ thống linh hoạt và dễ quản lý.

###### ▸ Tần số cập nhật:

- `osDelay(20)` : Tạm dừng tác vụ trong **20ms**. Điều này tạo ra tần số cập nhật khoảng **50Hz** (1000ms / 20ms), đủ nhanh để tạo ra chuyển động mượt mà cho cả quả bóng và con trỏ chuột mà không chiếm dụng quá nhiều tài nguyên CPU.

#### **Module Xử Lý Giao Diện (`Screen1View::tickEvent`)**

Đây là hàm được gọi lặp đi lặp lại bởi bộ khung (framework) của TouchGFX, tương ứng với mỗi "tick" của giao diện. Nhiệm vụ của nó là nhận dữ liệu vận tốc góc từ hàng đợi, tính toán vị trí mới cho quả bóng, và yêu cầu hệ thống vẽ lại giao diện. Đây là tác vụ "người tiêu dùng" (consumer) cho giao diện quả bóng.

##### **Mã nguồn**

```C
  void Screen1View::tickEvent()
  {
    GyroData_t gyroData;
    bool newDataReceived = false;

    if (Queue1Handle != NULL && osMessageQueueGet(Queue1Handle, &gyroData, NULL, 0) == osOK)
    {
        newDataReceived = true;
    }

    if (newDataReceived)
    {
        float deltaTimeSeconds = (float)GYRO_UPDATE_INTERVAL_MS / 1000.0f;

        currentAngleX += gyroData.gy * deltaTimeSeconds;
        currentAngleY += gyroData.gx * deltaTimeSeconds;

        if (currentAngleX > MAX_INTEGRATED_ANGLE) currentAngleX = MAX_INTEGRATED_ANGLE;
        if (currentAngleX < -MAX_INTEGRATED_ANGLE) currentAngleX = -MAX_INTEGRATED_ANGLE;
        if (currentAngleY > MAX_INTEGRATED_ANGLE) currentAngleY = MAX_INTEGRATED_ANGLE;
        if (currentAngleY < -MAX_INTEGRATED_ANGLE) currentAngleY = -MAX_INTEGRATED_ANGLE;
    }
    else
    {
        currentAngleX *= DAMPING_FACTOR;
        currentAngleY *= DAMPING_FACTOR;
    }

    int16_t offsetX = (int16_t)(currentAngleX * ANGLE_TO_PIXEL_SENSITIVITY);
    int16_t offsetY = (int16_t)(currentAngleY * ANGLE_TO_PIXEL_SENSITIVITY);

    if (offsetX > MAX_BALL_OFFSET_X) offsetX = MAX_BALL_OFFSET_X;
    if (offsetX < -MAX_BALL_OFFSET_X) offsetX = -MAX_BALL_OFFSET_X;
    if (offsetY > MAX_BALL_OFFSET_Y) offsetY = MAX_BALL_OFFSET_Y;
    if (offsetY < -MAX_BALL_OFFSET_Y) offsetY = -MAX_BALL_OFFSET_Y;

    circle1.moveTo(initialBallX + offsetX, initialBallY + offsetY);
    circle1.invalidate();
  }
```

##### 🛠️ Giải thích Luồng Hoạt Động

Hàm này mô phỏng một hệ thống vật lý để tạo ra chuyển động mượt mà cho quả bóng.

---

###### 1. Nhận Dữ Liệu từ Hàng đợi

- `MX_USB_DEVICE_Init()` : Khởi tạo ngoại vi USB, chuẩn bị để vi điều khiển có thể giao tiếp với máy tính như một thiết bị HID.

- `BSP_GYRO_Init()` : Khởi tạo cảm biến Gyroscope bằng cách sử dụng thư viện BSP (Board Support Package), thiết lập các thanh ghi cần thiết để cảm biến bắt đầu hoạt động.

---

###### 2. Xử lý khi có Dữ liệu mới (Khi người dùng nghiêng thiết bị)

- Tích phân Vận tốc góc: Đây là bước quan trọng nhất. Cảm biến Gyro cung cấp vận tốc góc (nhanh hay chậm). Để có được góc nghiêng (vị trí), chúng ta phải thực hiện phép tích phân theo thời gian: Góc mới = Góc cũ + (Vận tốc _ Thời gian): currentAngleX += gyroData.gy _ deltaTimeSeconds;

- Giới hạn Góc: Để chống lại hiện tượng trôi (drift) khiến góc tích lũy tăng vô hạn, giá trị currentAngleX và currentAngleY được giới hạn trong một khoảng (-MAX_INTEGRATED_ANGLE đến +MAX_INTEGRATED_ANGLE).

---

###### 3. Xử lý khi không có Dữ liệu mới (Khi người dùng giữ yên thiết bị)

- Hiệu ứng Giảm chấn (Damping): Khi không có dữ liệu mới (hoặc dữ liệu gần bằng 0), góc nghiêng hiện tại sẽ được nhân với một hệ số nhỏ hơn 1 (DAMPING_FACTOR).
  : currentAngleX \*= DAMPING_FACTOR;
- Điều này tạo ra hiệu ứng quả bóng từ từ lăn về vị trí trung tâm một cách tự nhiên, thay vì dừng đột ngột. Nó cũng giúp chống lại sự trôi của cảm biến một cách hiệu quả.

###### 4. Ánh xạ Góc sang Tọa độ Màn hình

- Góc nghiêng đã tính toán được chuyển đổi thành độ lệch pixel trên màn hình bằng cách nhân với một hệ số độ nhạy (ANGLE_TO_PIXEL_SENSITIVITY).

- Độ lệch này sau đó cũng được giới hạn để đảm bảo quả bóng không bao giờ di chuyển ra khỏi một vùng định trước trên màn hình.

###### 5. Cập nhật Giao diện

- circle1.moveTo(...): Di chuyển đối tượng "quả bóng" đến vị trí mới (vị trí ban đầu + độ lệch vừa tính).

- circle1.invalidate(): Báo cho TouchGFX rằng khu vực chứa quả bóng đã thay đổi và cần được vẽ lại trên màn hình trong khung hình (frame) tiếp theo.

#### **Module Điều Khiển Chuột HID (`MouseControlTask`)**

Đây là hàm được gọi lặp đi lặp lại bởi bộ khung (framework) của TouchGFX, tương ứng với mỗi "tick" của giao diện. Nhiệm vụ của nó là nhận dữ liệu vận tốc góc từ hàng đợi, tính toán vị trí mới cho quả bóng, và yêu cầu hệ thống vẽ lại giao diện. Đây là tác vụ "người tiêu dùng" (consumer) cho giao diện quả bóng.

##### **Mã nguồn**

```C
  void MouseControlTask(void *argument)
  {
  GyroData_t gyroData;
  mouseHID mousehid_report;

  for(;;)
  {
      if (osMessageQueueGet(Queue2Handle, &gyroData, NULL, osWaitForever) == osOK)
      {
          mousehid_report.button = 0;
          mousehid_report.mouse_x = 0;
          mousehid_report.mouse_y = 0;
          mousehid_report.wheel = 0;

          if (fabsf(gyroData.gx) > GYRO_DEADZONE) {
              mousehid_report.mouse_y = (int8_t)(-gyroData.gx * GYRO_TO_MOUSE_SENSITIVITY);
          }
          if (fabsf(gyroData.gy) > GYRO_DEADZONE) {
              mousehid_report.mouse_x = (int8_t)(gyroData.gy * GYRO_TO_MOUSE_SENSITIVITY);
          }

          USBD_HID_SendReport(&hUsbDeviceHS, (uint8_t*)&mousehid_report, sizeof(mousehid_report));
      }
  }
}
```

##### 🛠️ Giải thích Luồng Hoạt Động

---

###### 1. Chờ đợi Dữ liệu một cách Hiệu quả

- osMessageQueueGet(Queue2Handle, ..., osWaitForever): Đây là cơ chế cốt lõi của tác vụ. Thay vì kiểm tra liên tục, hàm này sẽ khiến tác vụ rơi vào trạng thái "ngủ" (Blocked state), không tiêu tốn CPU. Tác vụ sẽ chỉ được hệ điều hành "đánh thức" khi có một gói dữ liệu mới được đẩy vào Queue2Handle (từ StartDefaultTask).

---

###### 2. Chuyển đổi Dữ liệu Gyro sang Chuyển động Chuột

- Vùng không nhạy (Deadzone): if (fabsf(gyroData.gx) > GYRO_DEADZONE): Lệnh này kiểm tra xem giá trị vận tốc góc có lớn hơn một ngưỡng nhỏ (GYRO_DEADZONE) hay không. Điều này giúp loại bỏ các chuyển động nhỏ, run tay hoặc nhiễu của cảm biến, tránh cho con trỏ chuột bị "trôi" khi thiết bị gần như đứng yên.
- Ánh xạ Trục:
  +Vận tốc góc quanh trục X của Gyro (gx, nghiêng lên/xuống) được ánh xạ vào chuyển động theo trục Y của chuột (mouse_y).
  +Vận tốc góc quanh trục Y của Gyro (gy, nghiêng trái/phải) được ánh xạ vào chuyển động theo trục X của chuột (mouse_x).
- Điều chỉnh Độ nhạy: Giá trị vận tốc góc được nhân với một hệ số GYRO_TO_MOUSE_SENSITIVITY để điều chỉnh tốc độ di chuyển của chuột cho phù hợp.

---

###### 3. Gửi Báo cáo HID (HID Report)

- USBD_HID_SendReport(...): Sau khi đã chuẩn bị xong gói dữ liệu mousehid_report (chứa thông tin về độ dịch chuyển delta X, delta Y), hàm này sẽ gửi nó qua cổng USB đến máy tính. Hệ điều hành của máy tính sẽ nhận báo cáo này và tự động di chuyển con trỏ chuột tương ứng.

###### 4. Tối ưu hóa CPU

- Góc nghiêng đã tính toán được chuyển đổi thành độ lệch pixel trên màn hình bằng cách nhân với một hệ số độ nhạy (ANGLE_TO_PIXEL_SENSITIVITY).

- Độ lệch này sau đó cũng được giới hạn để đảm bảo quả bóng không bao giờ di chuyển ra khỏi một vùng định trước trên màn hình.

###### 5. Cập nhật Giao diện

- Tác vụ này không cần osDelay() vì nó hoạt động theo cơ chế sự kiện (event-driven). Nó chỉ chạy khi có việc cần làm (có dữ liệu mới), thời gian còn lại nó sẽ nhường CPU cho các tác vụ khác.

### KẾT QUẢ

- sẽ trình bày trong buổi báo cáo
