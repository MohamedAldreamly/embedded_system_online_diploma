/**
 ******************************************************************************
 * @file           : main.c
 * @project        : Smart Car Parking System
 * @version        : 1.0
 * @author         : Mohamed Aldreamly
 * @date           : 29 September 2026
 * @ECU            : Central ECU
 * @MCU            : STM32F103C6
 * @layer          : Application
 * @module         : Main
 * @path           : STM32_Central/App/main.c
 * @brief          : Central ECU system initialization and main scheduler.
 *
 * @responsibility :
 *                   - Initialize GPIO required by Central ECU.
 *                   - Initialize SPI1 for external EEPROM.
 *                   - Initialize 25LC256 EEPROM.
 *                   - Initialize Entry/Exit communication channels.
 *                   - Run the Central ECU cooperative scheduler.
 ******************************************************************************
 */


/* ========================================================================== */
/*                                  Includes                                  */
/* ========================================================================== */

#include "stm32f103x6.h"
#include "Stm32_F103C6_gpio_driver.h"
#include "Stm32_F103C6_SPI_driver.h"

#include "hal_eeprom.h"
#include "communication.h"
#include "parking_data.h"


/* ========================================================================== */
/*                         Private Function Prototypes                         */
/* ========================================================================== */

static void Central_SPI1_Init(void);
static void Central_EEPROM_CS_Init(void);


/* ========================================================================== */
/*                                    Main                                    */
/* ========================================================================== */

int main(void)
{
    /*
     * Enable GPIOA clock.
     *
     * GPIOA is used by:
     * PA4 -> EEPROM CS
     * PA5 -> SPI1 SCK
     * PA6 -> SPI1 MISO
     * PA7 -> SPI1 MOSI
     */
    RCC_GPIOA_CLK_EN();


    /*
     * Initialize EEPROM Chip Select before SPI/EEPROM access.
     */
    Central_EEPROM_CS_Init();


    /*
     * Initialize SPI1 peripheral and SPI pins.
     */
    Central_SPI1_Init();


    /*
     * Initialize external 25LC256 EEPROM.
     */
    if (HAL_EEPROM_Init() == true)
    {
        /*
         * Prototype database seed:
         * ID 1111 -> RFID ASCII "1111"
         * ID 2222 -> RFID ASCII "2222"
         * ID 3333 -> RFID ASCII "3333"
         *
         * Users are added only when EEPROM database is empty.
         */
        (void)ParkingData_SeedDefaultUsers();
    }


    /*
     * Initialize communication with:
     *
     * USART1 -> Entry ECU
     * USART2 -> Exit ECU
     */
    Communication_Init();


    /*
     * Main cooperative scheduler.
     */
    while (1)
    {
        Communication_Update();
    }


    return 0;
}


/* ========================================================================== */
/*                         Private Function Definitions                       */
/* ========================================================================== */


/**
 * @brief Initialize EEPROM Chip Select pin.
 *
 * PA4 is used as a normal GPIO output because EEPROM
 * chip selection is controlled manually by the HAL driver.
 */
static void Central_EEPROM_CS_Init(void)
{
    GPIO_PinConfig_t Local_PinConfig;


    Local_PinConfig.GPIO_PinNumber =
        GPIO_PIN_4;

    Local_PinConfig.GPIO_MODE =
        GPIO_MODE_OUTPUT_PP;

    Local_PinConfig.GPIO_Output_Speed =
        GPIO_SPEED_10M;


    MCAL_GPIO_Init(GPIOA,
                   &Local_PinConfig);


    /*
     * EEPROM is inactive when CS is HIGH.
     */
    MCAL_GPIO_WritePin(GPIOA,
                       GPIO_PIN_4,
                       GPIO_PIN_SET);
}


/**
 * @brief Initialize SPI1 for 25LC256 EEPROM communication.
 *
 * SPI configuration:
 * Master
 * Full duplex
 * 8-bit
 * MSB first
 * CPOL = 0
 * CPHA = 0
 * Software NSS
 * Polling mode
 */
static void Central_SPI1_Init(void)
{
    SPI_Config_t Local_SPIConfig;


    Local_SPIConfig.Device_Mode =
        SPI_Device_Mode_MASTER;

    Local_SPIConfig.Communication_Mode =
        SPI_DIRECTIONAL_2LINES;

    Local_SPIConfig.Frame_Format =
        SPI_Frame_Format_MSB_transmited_fisrt;

    Local_SPIConfig.DataSize =
        SPI_DataSize_8BIT;

    Local_SPIConfig.CLKPolarity =
        SPI_CLKPolarity_LOW_when_idle;

    Local_SPIConfig.CLKPhase =
        SPI_CLKPhase_1EDGE_frist_capture_egde;

    Local_SPIConfig.NSS =
        SPI_NSS_Soft_NSSInternalSoft_Set;

    Local_SPIConfig.SPI_BAUDRATEPRESCALER =
        SPI_BAUDRATEPRESCALER_8;

    Local_SPIConfig.IRQ_Enable =
        SPI_IRQ_Enable_NONE;

    Local_SPIConfig.P_IRQ_CallBack =
        NULL;


    MCAL_SPI_Init(SPI1,
                  &Local_SPIConfig);


    /*
     * Configure:
     *
     * PA5 -> SCK
     * PA6 -> MISO
     * PA7 -> MOSI
     *
     * PA4 is intentionally NOT controlled by SPI hardware.
     */
    MCAL_SPI_Set_Pin(SPI1);
}