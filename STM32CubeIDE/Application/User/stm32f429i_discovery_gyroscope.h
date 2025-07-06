#ifndef __STM32F429I_DISCOVERY_GYROSCOPE_H
#define __STM32F429I_DISCOVERY_GYROSCOPE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h> // Cho int32_t
#include <math.h>   // Cho float_t (nếu dùng)

/* Exported constants --------------------------------------------------------*/
#define BSP_GYRO_OK      (0)
#define BSP_GYRO_ERROR   (-1)

/* Exported functions ------------------------------------------------------- */
int32_t BSP_GYRO_Init(void);
// Hàm này sẽ trả về giá trị đã được chuyển đổi sang dps
int32_t BSP_GYRO_GetXYZ(float pDataXYZ[3]);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F429I_DISCOVERY_GYROSCOPE_H */
