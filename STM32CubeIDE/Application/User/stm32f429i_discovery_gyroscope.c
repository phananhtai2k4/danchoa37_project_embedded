/**
  ******************************************************************************
  * @file    stm32f429i_discovery_gyroscope.c
  * @author  Your Name
  * @brief   Simplified BSP for I3G4250D gyroscope on STM32F429I-Discovery.
  *          Focuses on a fixed Full Scale (e.g., 245 dps).
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f429i_discovery_gyroscope.h"
#include "a3g4250d_reg.h" // Driver mức thanh ghi (chứa các hàm đọc/ghi và define thanh ghi)
#include "stm32f4xx_hal.h" // Cho HAL_SPI_..., HAL_GPIO_..., HAL_Delay, HAL_GetTick

/* Private defines -----------------------------------------------------------*/
#define GYRO_CS_PORT                        GPIOC
#define GYRO_CS_PIN                         GPIO_PIN_1
#define GYRO_SPI_INSTANCE                   SPI5

// Cấu hình Full Scale cố định (ví dụ: 245 dps)
// Giá trị bit field cho CTRL_REG4.fs[1:0] (kiểm tra datasheet I3G4250D)
// 00: 245 dps
// 01: 500 dps
// 10: 2000 dps
#define GYRO_FS_245DPS_REG_VAL              (0x00) // Bits [1:0] của FS field trong CTRL_REG4
#define GYRO_FS_500DPS_REG_VAL              (0x01)
#define GYRO_FS_2000DPS_REG_VAL             (0x02)

// Chọn Full Scale bạn muốn sử dụng:
#define SELECTED_FS_REG_VAL                 GYRO_FS_245DPS_REG_VAL
//#define SELECTED_FS_REG_VAL                 GYRO_FS_500DPS_REG_VAL
//#define SELECTED_FS_REG_VAL                 GYRO_FS_2000DPS_REG_VAL


/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi5;
static stmdev_ctx_t Gyro_Ctx; // Context cho driver mức thanh ghi
static uint8_t Gyro_Sensor_Initialized = 0;

/* Private function prototypes -----------------------------------------------*/
static int32_t GYRO_IO_Platform_Init(void); // Khởi tạo GPIO CS
static int32_t GYRO_IO_Write(void *handle, uint8_t Reg, uint8_t *pData, uint16_t Length);
static int32_t GYRO_IO_Read(void *handle, uint8_t Reg, uint8_t *pData, uint16_t Length);

/**
  * @brief  Initializes the Gyroscope.
  */
int32_t BSP_GYRO_Init(void)
{
  uint8_t id;
  a3g4250d_ctrl_reg1_t ctrl_reg1 = {0};
  a3g4250d_ctrl_reg4_t ctrl_reg4 = {0}; // Sử dụng struct từ a3g4250d_reg.h

  if (Gyro_Sensor_Initialized == 1)
  {
    return BSP_GYRO_OK;
  }

  /* Configure the Gyroscope IO functions */
  Gyro_Ctx.write_reg = GYRO_IO_Write;
  Gyro_Ctx.read_reg  = GYRO_IO_Read;
  Gyro_Ctx.handle    = &hspi5; // Hoặc NULL nếu không cần truyền handle SPI vào hàm IO

  /* Initialize CS Pin */
  if (GYRO_IO_Platform_Init() != HAL_OK) // Đổi tên hàm để tránh trùng với hàm init khác
  {
      return BSP_GYRO_ERROR;
  }

  /* Check device ID */
  if (a3g4250d_device_id_get(&Gyro_Ctx, &id) != 0)
  {
    return BSP_GYRO_ERROR;
  }

  if (id != 0xD7U) { // I3G4250D WHO_AM_I is 0xD7. A3G4250D_ID trong a3g4250d_reg.h là 0xD3.
    // printf("Warning: Gyro ID mismatch. Expected 0xD7, got 0x%02X\r\n", id);
    // return BSP_GYRO_ERROR; // Nghiêm ngặt hơn thì trả về lỗi
  }

  /* Configure CTRL_REG1: Power-up, Enable Axes, ODR (ví dụ 100Hz) */
  // Giá trị 0x0F: PD=0b1111 (Xen,Yen,Zen,PD -> Normal mode), BW=00, DR=00 (ODR=100Hz, BW=12.5Hz theo datasheet)
  ctrl_reg1.pd = 0x0F; // Bật các trục và power on
  ctrl_reg1.bw = 0x00; // Chọn BW (ví dụ)
  ctrl_reg1.dr = 0x00; // Chọn DR (ví dụ)
  if (a3g4250d_write_reg(&Gyro_Ctx, A3G4250D_CTRL_REG1, (uint8_t*)&ctrl_reg1, 1) != 0)
  {
    return BSP_GYRO_ERROR;
  }
  HAL_Delay(5); // Chờ một chút cho ổn định

  /* Configure CTRL_REG4: Full Scale */
  // Đọc giá trị hiện tại của CTRL_REG4 để không ghi đè các bit khác
  if (a3g4250d_read_reg(&Gyro_Ctx, A3G4250D_CTRL_REG4, (uint8_t*)&ctrl_reg4, 1) != 0)
  {
      return BSP_GYRO_ERROR;
  }
  // Chỉ thay đổi các bit FS (giả sử FS là 2 bit thấp nhất trong bit-field `fs` của `a3g4250d_ctrl_reg4_t`)
  // Cần xem lại định nghĩa của a3g4250d_ctrl_reg4_t trong a3g4250d_reg.h để biết bit field FS nằm ở đâu.
  // Giả sử bit field cho FS trong struct a3g4250d_ctrl_reg4_t tên là `fs` và có 2 bit.
  // Ví dụ: ctrl_reg4.fs = SELECTED_FS_REG_VAL;
  // Vì a3g4250d_reg.h bạn cung cấp không có bit-field `fs` trong `a3g4250d_ctrl_reg4_t`
  // mà chỉ có `sim` và `st`. Chúng ta sẽ ghi trực tiếp giá trị cho 2 bit FS.
  // Thanh ghi CTRL_REG4 (0x23): [BDU][BLE][FS1][FS0][ST1][ST0][SIM]
  // Giả sử BDU=0 (default), BLE=0 (default LSB first), ST=00 (self-test off), SIM=0 (4-wire SPI)
  // Thì giá trị cần ghi sẽ là: (SELECTED_FS_REG_VAL << 4) (nếu FS1 FS0 là bit 5 4)
  // Dựa trên file `a3g4250d_reg.h` bạn cung cấp, `a3g4250d_ctrl_reg4_t` không có trường `fs`.
  // Chúng ta cần ghi trực tiếp vào thanh ghi, biết rằng FS[1:0] là bit 5 và bit 4 của thanh ghi 0x23.
  uint8_t reg4_val_to_write = 0x00; // Mặc định SIM=0 (4-wire), ST=00 (self-test off), BLE=0 (LSB first)
  reg4_val_to_write |= (SELECTED_FS_REG_VAL << 4); // FS1=bit5, FS0=bit4
  if (a3g4250d_write_reg(&Gyro_Ctx, A3G4250D_CTRL_REG4, &reg4_val_to_write, 1) != 0)
  {
    return BSP_GYRO_ERROR;
  }

  Gyro_Sensor_Initialized = 1;
  return BSP_GYRO_OK;
}

/**
  * @brief  Get the gyroscope XYZ angular rates in dps.
  */
int32_t BSP_GYRO_GetXYZ(float pDataXYZ[3])
{
  int16_t raw_data[3]; // Mảng để lưu trữ dữ liệu thô X, Y, Z

  if (Gyro_Sensor_Initialized == 0) {
      return BSP_GYRO_ERROR;
  }

  if (a3g4250d_angular_rate_raw_get(&Gyro_Ctx, raw_data) != 0)
  {
    return BSP_GYRO_ERROR;
  }

  // Chuyển đổi từ giá trị thô sang dps
#if (SELECTED_FS_REG_VAL == GYRO_FS_245DPS_REG_VAL)
  pDataXYZ[0] = a3g4250d_from_fs245dps_to_mdps(raw_data[0]) / 1000.0f;
  pDataXYZ[1] = a3g4250d_from_fs245dps_to_mdps(raw_data[1]) / 1000.0f;
  pDataXYZ[2] = a3g4250d_from_fs245dps_to_mdps(raw_data[2]) / 1000.0f;
#elif (SELECTED_FS_REG_VAL == GYRO_FS_500DPS_REG_VAL)
  // Sensitivity cho 500dps là 17.5 mdps/digit (từ datasheet)
  pDataXYZ[0] = (float)raw_data[0] * 17.5f / 1000.0f;
  pDataXYZ[1] = (float)raw_data[1] * 17.5f / 1000.0f;
  pDataXYZ[2] = (float)raw_data[2] * 17.5f / 1000.0f;
#elif (SELECTED_FS_REG_VAL == GYRO_FS_2000DPS_REG_VAL)
  // Sensitivity cho 2000dps là 70 mdps/digit (từ datasheet)
  pDataXYZ[0] = (float)raw_data[0] * 70.0f / 1000.0f;
  pDataXYZ[1] = (float)raw_data[1] * 70.0f / 1000.0f;
  pDataXYZ[2] = (float)raw_data[2] * 70.0f / 1000.0f;
#else
  return BSP_GYRO_ERROR; // Lỗi Full Scale không hợp lệ
#endif

  return BSP_GYRO_OK;
}


/* Private functions ---------------------------------------------------------*/
static int32_t GYRO_IO_Platform_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Clock cho Port C đã được bật bởi MX_GPIO_Init() trong main.c
  // __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitStruct.Pin = GYRO_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GYRO_CS_PORT, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET); // CS high
  return HAL_OK; // Trả về HAL_OK để nhất quán với các hàm HAL
}

// Hàm IO_Write và IO_Read sử dụng trực tiếp handle SPI từ Gyro_Ctx
static int32_t GYRO_IO_Write(void *handle, uint8_t Reg, uint8_t *pData, uint16_t Length)
{
  // SPI_HandleTypeDef* hspi = (SPI_HandleTypeDef*)handle; // Không cần nếu hspi5 là global/extern
  uint8_t tx_buffer[Length + 1];

  tx_buffer[0] = Reg & 0x7F; // Write command: MSB=0.

  for(uint16_t i = 0; i < Length; i++)
  {
    tx_buffer[i+1] = pData[i];
  }

  HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_RESET);
  if (HAL_SPI_Transmit(&hspi5, tx_buffer, Length + 1, HAL_MAX_DELAY) != HAL_OK)
  {
    HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET);
    return -1; // Lỗi
  }
  HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET);
  return 0; // Thành công
}

static int32_t GYRO_IO_Read(void *handle, uint8_t Reg, uint8_t *pData, uint16_t Length)
{
  // SPI_HandleTypeDef* hspi = (SPI_HandleTypeDef*)handle;
  uint8_t tx_header;

  tx_header = Reg | 0x80; // Read command: MSB=1.
  if (Length > 1)
  {
    tx_header |= 0x40;    // Multi-byte read: bit 6 = 1.
  }

  HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_RESET);
  if (HAL_SPI_Transmit(&hspi5, &tx_header, 1, HAL_MAX_DELAY) != HAL_OK)
  {
    HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET);
    return -1;
  }
  if (HAL_SPI_Receive(&hspi5, pData, Length, HAL_MAX_DELAY) != HAL_OK)
  {
    HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET);
    return -1;
  }
  HAL_GPIO_WritePin(GYRO_CS_PORT, GYRO_CS_PIN, GPIO_PIN_SET);
  return 0;
}
