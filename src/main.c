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

// Definice struktury SegmentPin, která obsahuje ukazatele na port a pin
typedef struct {
    GPIO_TypeDef* port;  // Ukazatel na GPIO port 
    GPIO_Pin_TypeDef pin; // Typ pinu GPIO 
} SegmentPin;

// Inicializace pole segment1, které obsahuje 7 SegmentPin struktur pro každý segment 7-segmentového displeje
SegmentPin segment1[7] = {
    {SEGMENT1_PORT_1, SEGMENT1_PIN_1},  // Segment A
    {SEGMENT1_PORT_2, SEGMENT1_PIN_2},  // Segment B
    {SEGMENT1_PORT_3, SEGMENT1_PIN_3},  // Segment C
    {SEGMENT1_PORT_4, SEGMENT1_PIN_4},  // Segment D
    {SEGMENT1_PORT_5, SEGMENT1_PIN_5},  // Segment E
    {SEGMENT1_PORT_6, SEGMENT1_PIN_6},  // Segment F
    {SEGMENT1_PORT_7, SEGMENT1_PIN_7}   // Segment G
};

// Inicializace pole segment2, které obsahuje 7 SegmentPin struktur pro druhý 7-segmentový displej
SegmentPin segment2[7] = {
    {SEGMENT2_PORT_1, SEGMENT2_PIN_1},  // Segment A
    {SEGMENT2_PORT_2, SEGMENT2_PIN_2},  // Segment B
    {SEGMENT2_PORT_3, SEGMENT2_PIN_3},  // Segment C
    {SEGMENT2_PORT_4, SEGMENT2_PIN_4},  // Segment D
    {SEGMENT2_PORT_5, SEGMENT2_PIN_5},  // Segment E
    {SEGMENT2_PORT_6, SEGMENT2_PIN_6},  // Segment F
    {SEGMENT2_PORT_7, SEGMENT2_PIN_7}   // Segment G
};

// Funkce set_segment pro nastavení segmentů pro zobrazení čísla
void set_segment(SegmentPin* segment, int number) {
    // Statické pole segment_map, které definuje, které segmenty jsou zapnuté (0) nebo vypnuté (1) pro čísla 0-9
    static const uint8_t segment_map[10][7] = {
        {0, 0, 0, 0, 0, 1, 0}, // 0: Zapne A, B, C, D, E, F, a vypne G
        {1, 0, 0, 1, 1, 1, 1}, // 1: Zapne B, C a vypne ostatní segmenty
        {0, 0, 1, 0, 0, 0, 1}, // 2: Zapne A, B, D, E, G a vypne C, F
        {0, 0, 0, 0, 1, 0, 1}, // 3: Zapne A, B, C, D, G a vypne E, F
        {1, 0, 0, 1, 1, 0, 0}, // 4: Zapne B, C, F, G a vypne A, D, E
        {0, 1, 0, 0, 1, 0, 0}, // 5: Zapne A, C, D, F, G a vypne B, E
        {0, 1, 0, 0, 0, 0, 0}, // 6: Zapne A, C, D, E, F, G a vypne B
        {0, 0, 0, 1, 1, 1, 1}, // 7: Zapne A, B, C a vypne D, E, F, G
        {0, 0, 0, 0, 0, 0, 0}, // 8: Zapne všechny segmenty
        {0, 0, 0, 0, 1, 0, 0}  // 9: Zapne A, B, C, D, F, G a vypne E
    };

    // Pro každý ze 7 segmentů
    for (int i = 0; i < 7; i++) {
        // Pokud je hodnota v segment_map pro dané číslo 0, zapne segment
        if (segment_map[number][i]) {
            GPIO_WriteHigh(segment[i].port, segment[i].pin);  // Zapnutí segmentu (výstup na vysoké úrovni)
        } else {
            GPIO_WriteLow(segment[i].port, segment[i].pin);   // Vypnutí segmentu (výstup na nízké úrovni)
        }
    }
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
    
    for (int i = 0; i < 10; i++) {
        set_segment(segment1, i);
        delay_ms(500);
    }

    // Přepínání ledky pomocí tlačítka
    while(1) {
        
        

/*         if (GPIO_ReadInputPin(BTN1_PORT, BTN1_PIN) == RESET) {
            GPIO_WriteHigh(ON_OFF_PORT, ON_OFF_PIN);
        }
            
        else {
            GPIO_WriteLow(ON_OFF_PORT, ON_OFF_PIN);
        } */

        uint16_t raw_temp = read_temp_LM75A(); // Čtení teploty
        raw_temp = raw_temp>>5; // Posun o 5 bit
        uint16_t temp = (10*raw_temp+4)/8; // Vychází jako 10-ti násobek skutečné teploty
        uint16_t real_temp = temp / 10;

        // Zjištění 1. a 2. cifry pro 1. a 2. segment
        uint16_t temp_1 = real_temp / 10; // Celé dělení 10
        uint16_t temp_2 = real_temp % 10; // Zbytek po dělení 10
        
        set_segment(segment1, temp_1);
        set_segment(segment2, temp_2);

        //delay_ms(2000); // Pro zobrazení adresy
        delay_ms(500);
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
