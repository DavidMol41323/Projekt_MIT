#include <stdbool.h>
#include <stm8s.h>
#include <stdio.h>
#include "main.h"
#include "milis.h"
//#include "delay.h"
#include "uart1.h"
#include "swi2c.h"
//#include <stdint.h>

//#include <stm/stm8s_i2c.h>

#define GREEN_LED_0_PORT GPIOD
#define GREEN_LED_0_PIN GPIO_PIN_0

#define RED_LED_1_PORT GPIOD
#define RED_LED_1_PIN GPIO_PIN_6
/* #define RED_LED_2_PORT GPIOD
#define RED_LED_2_PIN GPIO_PIN_2
#define RED_LED_3_PORT GPIOD
#define RED_LED_3_PIN GPIO_PIN_3
#define RED_LED_4_PORT GPIOD
#define RED_LED_4_PIN GPIO_PIN_4 */

#define BTN1_PORT GPIOD
#define BTN1_PIN GPIO_PIN_5






void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz

    
    
    
    // Inicializace LEDek
    GPIO_Init(RED_LED_1_PORT, RED_LED_1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    
    // Inicializace tlačítek
    GPIO_Init(BTN1_PORT, BTN1_PIN, GPIO_MODE_IN_PU_NO_IT);
    
    
    
    swi2c_init();
    init_milis();
    init_uart1();
}


int main(void)  {

    init();

    printf("\nScan I2C bus:\n \r");
    printf("Recover: 0x%02X\n \r", swi2c_recover());
    for (uint8_t addr = 0; addr < 128; addr++) {
        if (swi2c_test_slave(addr << 1) == 0) {
            printf("0x%02X \n \r", addr);
        }
    }
    printf("------------- scan end --------------------\n \r");

    // Přepínání ledky pomocí tlačítka
    while(1) {
    
        if (GPIO_ReadInputPin(BTN1_PORT, BTN1_PIN) == RESET) {
            GPIO_WriteHigh(RED_LED_1_PORT, RED_LED_1_PIN);
        }
            
        else {
            GPIO_WriteLow(RED_LED_1_PORT, RED_LED_1_PIN);
        }
    } 
}


/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
