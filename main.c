/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
 */
#include "project.h"

void out7seg(int16, uint8);

uint16 indyk[4] = {0x10FF, 0x20FF, 0x40FF, 0x80FF};
const uint8 KOD7[10] = {0x03, 0x9F, 0x25, 0x0D, 0x99,
                        0x49, 0x41, 0x1F, 0x01, 0x09};
const uint8 MINUS = 0xFD;

CY_ISR(isr_100Hz) {
  static uint8 N_indyk = 0u;

  SPIM_WriteTxData(indyk[N_indyk]);

  N_indyk++;
  N_indyk &= 3;
}

void out7seg(int16 data, uint8 point_location) {
  uint8 i;
  uint8 sign = data < 0;
  if (data < 0)
    data = -data;
  for (i = 0; i < 4 - sign; i++) {
    indyk[i] &= 0xFF00;
    indyk[i] |= (uint16)KOD7[data % 10];
    data /= 10;
    if (data == 0)
      break;
  }
  if (sign) {
    i++;
    indyk[i] &= 0xFF00;
    indyk[i] |= (uint16)MINUS;
  }

  for (i++; i < 4; i++) {
    indyk[i] |= 0x00FF;
  }

  if (point_location <= 4) {
    indyk[point_location] ^= 0x01;
  }
}


int main() {
  int OUTPUTS[8] = {0, 1, 0, 0, 0, 1, 1, 0};
  CyGlobalIntEnable;

  SPIM_Start();
  isr_100Hz_StartEx(isr_100Hz);

  for (;;) {
    for (uint8 a = 0; a <= 1; ++a) {
        for (uint8 b = 0; b <= 1; ++b) {
            for (uint8 c = 0; c <= 1; ++c) {
                Pin_A_Write(5 * a);
                Pin_B_Write(5 * b);
                Pin_C_Write(5 * c);
                out7seg(10000 + a*1000 + b*100 + c*10 + OUTPUTS[a*4 + b*2 + c], 1);
                CyDelay(2000);
            }
        }
    }
  }
}
