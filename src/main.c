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



// Definice LED
#define GREEN_LED_0_PORT GPIOD
#define GREEN_LED_0_PIN GPIO_PIN_0

#define RED_LED_1_PORT GPIOD
#define RED_LED_1_PIN GPIO_PIN_6


// Definice tlačítek
/* #define BTN1_PORT GPIOE
#define BTN1_PIN GPIO_PIN_1 */

#define LM75A_ADDR (0x49 << 1) // Shift left to account for R/W bit
#define TEMP_REG 0x00

// Definice 7 segmentu 1
#define SEGMENT1_PORT_1 GPIOG 
#define SEGMENT1_PIN_1 GPIO_PIN_0
#define SEGMENT1_PORT_2 GPIOC 
#define SEGMENT1_PIN_2 GPIO_PIN_2
#define SEGMENT1_PORT_3 GPIOC 
#define SEGMENT1_PIN_3 GPIO_PIN_3
#define SEGMENT1_PORT_4 GPIOC 
#define SEGMENT1_PIN_4 GPIO_PIN_1
#define SEGMENT1_PORT_5 GPIOE 
#define SEGMENT1_PIN_5 GPIO_PIN_0
#define SEGMENT1_PORT_6 GPIOD 
#define SEGMENT1_PIN_6 GPIO_PIN_5
#define SEGMENT1_PORT_7 GPIOD 
#define SEGMENT1_PIN_7 GPIO_PIN_6

// Definicie 7 segmentu 2
#define SEGMENT2_PORT_1 GPIOC 
#define SEGMENT2_PIN_1 GPIO_PIN_5
#define SEGMENT2_PORT_2 GPIOC 
#define SEGMENT2_PIN_2 GPIO_PIN_7
#define SEGMENT2_PORT_3 GPIOC 
#define SEGMENT2_PIN_3 GPIO_PIN_6
#define SEGMENT2_PORT_4 GPIOE 
#define SEGMENT2_PIN_4 GPIO_PIN_5
#define SEGMENT2_PORT_5 GPIOC 
#define SEGMENT2_PIN_5 GPIO_PIN_4
#define SEGMENT2_PORT_6 GPIOD 
#define SEGMENT2_PIN_6 GPIO_PIN_3
#define SEGMENT2_PORT_7 GPIOB
#define SEGMENT2_PIN_7 GPIO_PIN_0



void init(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    

    // Inicializace LEDek
    GPIO_Init(RED_LED_1_PORT, RED_LED_1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    
    // Inicializace tlačítek
    //GPIO_Init(BTN1_PORT, BTN1_PIN, GPIO_MODE_IN_PU_NO_IT);
    
    // Inicializace 7 Segmentu 1
    GPIO_Init(SEGMENT1_PORT_1, SEGMENT1_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT1_PORT_2, SEGMENT1_PIN_2, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT1_PORT_3, SEGMENT1_PIN_3, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT1_PORT_4, SEGMENT1_PIN_4, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT1_PORT_5, SEGMENT1_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT1_PORT_6, SEGMENT1_PIN_6, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT1_PORT_7, SEGMENT1_PIN_7, GPIO_MODE_OUT_PP_HIGH_SLOW);
    
    // Inicializace 7 Segmentu 2
    GPIO_Init(SEGMENT2_PORT_1, SEGMENT2_PIN_1, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT2_PORT_2, SEGMENT2_PIN_2, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT2_PORT_3, SEGMENT2_PIN_3, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT2_PORT_4, SEGMENT2_PIN_4, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT2_PORT_5, SEGMENT2_PIN_5, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT2_PORT_6, SEGMENT2_PIN_6, GPIO_MODE_OUT_PP_HIGH_SLOW);
    GPIO_Init(SEGMENT2_PORT_7, SEGMENT2_PIN_7, GPIO_MODE_OUT_PP_HIGH_SLOW);

    // Inicializace funkcí ze source
    swi2c_init();
    init_milis();
    init_uart1();
}

uint16_t read_temp_LM75A(void) {
    uint8_t temp_reg = TEMP_REG;
    uint8_t temp_data[2] = {0, 0};
    
    // Write to pointer register to set it to temperature register
    swi2c_write_buf(LM75A_ADDR, 0x00, &temp_reg, 1);

    // Read temperature data
    swi2c_read_buf(LM75A_ADDR, 0x00, temp_data, 2);

    // Combine MSB and LSB to get full temperature data
    uint16_t temp = (temp_data[0] << 8) | temp_data[1];

    return temp;
}


int main(void)  {

    init();

    // Pomocí UART zobrazuje adresu scl
    printf("\nScan I2C bus:\n \r");
    printf("Recover: 0x%02X\n \r", swi2c_recover());
    for (uint8_t addr = 0; addr < 128; addr++) {
        if (swi2c_test_slave(addr << 1) == 0) {
            printf("0x%02X \n \r", addr);
        }
    }
    printf("------------- scan end --------------------\n \r");

    bool led_on = false;
    const int upper_threshold = 260; // Horní práh pro zapnutí LED
    const int lower_threshold = 255; // Dolní práh pro vypnutí LED
    
    GPIO_WriteLow(SEGMENT1_PORT_1, SEGMENT1_PIN_1);
    GPIO_WriteLow(SEGMENT1_PORT_2, SEGMENT1_PIN_2);
    GPIO_WriteLow(SEGMENT2_PORT_3, SEGMENT2_PIN_3);
    GPIO_WriteLow(SEGMENT1_PORT_4, SEGMENT1_PIN_4);
    GPIO_WriteLow(SEGMENT1_PORT_5, SEGMENT1_PIN_5);
    GPIO_WriteLow(SEGMENT1_PORT_6, SEGMENT1_PIN_6);
    GPIO_WriteLow(SEGMENT2_PORT_7, SEGMENT2_PIN_7); 

    // Přepínání ledky pomocí tlačítka
    while(1) {
        
        

/*         if (GPIO_ReadInputPin(BTN1_PORT, BTN1_PIN) == RESET) {
            GPIO_WriteHigh(ON_OFF_PORT, ON_OFF_PIN);
        }
            
        else {
            GPIO_WriteLow(ON_OFF_PORT, ON_OFF_PIN);
        } */

        uint16_t raw_temp = read_temp_LM75A();
        raw_temp = raw_temp>>5;
        uint16_t temp = (10*raw_temp+4)/8; // Vychází jako 10-ti násobek skutečné teploty

        //delay_ms(2000); // Pro zobrazení adresy
        delay_ms(25);
        printf("%d\n\r", temp);

/*         if (temp > upper_threshold && !led_on){
            GPIO_WriteHigh(RED_LED_1_PORT, RED_LED_1_PIN);
            led_on = true;
        }
        else if (temp < lower_threshold && led_on) {
            GPIO_WriteLow(RED_LED_1_PORT, RED_LED_1_PIN);
            led_on = false;
        }*/

    }  

}


/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
