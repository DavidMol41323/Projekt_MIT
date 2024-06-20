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

#define RED_LED_PORT GPIOB
#define RED_LED_PIN GPIO_PIN_4
#define BLUE_LED_PORT GPIOB
#define BLUE_LED_PIN GPIO_PIN_5


// Definice tlačítek
#define BTN1_PORT GPIOB
#define BTN1_PIN GPIO_PIN_1
#define BTN2_PORT GPIOB
#define BTN2_PIN GPIO_PIN_2
#define BTN3_PORT GPIOB
#define BTN3_PIN GPIO_PIN_3

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
    GPIO_Init(RED_LED_PORT, RED_LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(BLUE_LED_PORT, BLUE_LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    
    // Inicializace tlačítek
    GPIO_Init(BTN1_PORT, BTN1_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(BTN2_PORT, BTN2_PIN, GPIO_MODE_IN_PU_NO_IT);
    GPIO_Init(BTN3_PORT, BTN3_PIN, GPIO_MODE_IN_PU_NO_IT);
    
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

// Funkce pro přepínání módu
int mod(void) {
    static int state = 0;  // Statická proměnná pro uchování stavu (0 nebo 1)
    static uint16_t last_button_state;
    uint16_t button_state = GPIO_ReadInputPin(BTN1_PORT, BTN1_PIN); // Čtení stavu tlačítka

    if (button_state == RESET && last_button_state == SET) { // Tlačítko stisknuto (LOW stav)
        state = 1 - state;  // Přepnutí stavu mezi 0 a 1
    }

    last_button_state = button_state;
    delay_ms(10);
    return state; // Vrácení aktuálního stavu (0 nebo 1)
}
   

int main(void) {
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
    
    // Načítání
    for (int i = 0; i < 10; i++) {
        set_segment(segment1, i);
        delay_ms(50);
        for (int i = 0; i < 9; i++) {
            set_segment(segment2, i);
            delay_ms(50);
        }
    }

    // Přepínání ledky pomocí tlačítka
    uint16_t needed_temp = 10;  // Inicializace potřebné teploty mimo cyklus
    uint16_t current_mod = 0;   // Inicializace módu

    // Proměnné pro udržení stavu tlačítek mezi cykly
    static uint16_t last_button_state = RESET;  
    static uint16_t last_button_plus_state = RESET;  
    static uint16_t last_button_minus_state = RESET;

    while (1) {
        uint16_t raw_temp = read_temp_LM75A(); // Čtení teploty
        raw_temp = raw_temp >> 5; // Posun o 5 bit
        uint16_t temp = (10 * raw_temp + 4) / 8; // Vychází jako 10-ti násobek skutečné teploty
        uint16_t real_temp = temp / 10;

        // Získání aktuálního módu
        current_mod = mod();
        if (current_mod == 0) {
            // Mód zobrazování teploty na segmentovém displeji
            uint16_t temp_1 = real_temp / 10; // První číslice teploty
            uint16_t temp_2 = real_temp % 10; // Druhá číslice teploty
            
            set_segment(segment1, temp_1);
            set_segment(segment2, temp_2);
        } else if (current_mod == 1) {
            // Mód nastavení požadované teploty
            int button_state;

            // Cyklus pro nastavení požadované teploty
            while (1) {
                button_state = GPIO_ReadInputPin(BTN1_PORT, BTN1_PIN); // Čtení stavu hlavního tlačítka

                // Detekce změny stavu hlavního tlačítka
                if (button_state == RESET && last_button_state == SET) {
                    break; // Opustí cyklus po změně stavu tlačítka z SET na RESET
                }

                int button_state_plus = GPIO_ReadInputPin(BTN2_PORT, BTN2_PIN); // Čtení stavu tlačítka plus
                int button_state_minus = GPIO_ReadInputPin(BTN3_PORT, BTN3_PIN); // Čtení stavu tlačítka minus

                // Aktualizace potřebné teploty podle stisknutých tlačítek plus a minus
                if (button_state_plus == RESET && last_button_plus_state == SET) {  // Přičíst k needed_temp 1
                    needed_temp += 1;
                    last_button_plus_state = button_state_plus;
                } else if (button_state_minus == RESET && last_button_minus_state == SET) { // Odečíst od needed_temp 1
                    needed_temp -= 1;
                    last_button_minus_state = button_state_minus;
                }

                // Ochrana proti nastavení potřebné teploty pod 0
                if (needed_temp < 0) {
                    needed_temp = 0;
                }

                // Rozdělení potřebné teploty na desítky a jednotky
                uint16_t needed_temp_1 = needed_temp / 10;
                uint16_t needed_temp_2 = needed_temp % 10;
                
                // Nastavení segmentů pro zobrazení potřebné teploty
                set_segment(segment1, needed_temp_1);
                set_segment(segment2, needed_temp_2);

                delay_ms(10); // Zpoždění pro stabilitu

                // Aktualizace stavů tlačítek plus a minus
                last_button_plus_state = button_state_plus;
                last_button_minus_state = button_state_minus;
                last_button_state = button_state;

                // Výpis potřebné teploty (pro kontrolu)
                printf("needed temp: %d\n\r", needed_temp);
            }
        }
        
        delay_ms(50); // Zpoždění mezi cykly
        
        // Výpis aktuální teploty (pro kontrolu)
        printf("real temp: %d\n\r", real_temp);
        
        
            // Aktualizace podmínek pro LED
        uint16_t up_needed_temp = needed_temp + 1;
        uint16_t down_needed_temp = needed_temp - 1;

        if (real_temp > up_needed_temp) {
            GPIO_WriteHigh(BLUE_LED_PORT, BLUE_LED_PIN);
            GPIO_WriteLow(RED_LED_PORT, RED_LED_PIN);
        } else if (real_temp <= up_needed_temp && real_temp >= down_needed_temp) {
            GPIO_WriteLow(BLUE_LED_PORT, BLUE_LED_PIN);
            GPIO_WriteLow(RED_LED_PORT, RED_LED_PIN);
        } else if (real_temp < down_needed_temp) {
            GPIO_WriteLow(BLUE_LED_PORT, BLUE_LED_PIN);
            GPIO_WriteHigh(RED_LED_PORT, RED_LED_PIN);
        }
    }
}
    

