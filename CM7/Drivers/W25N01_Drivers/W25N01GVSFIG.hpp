/**
 * @file    W25N01GVSFIG.hpp
 * @brief   Header file for the W25N01 Flash chip on the SAC 2024 payload
 * @author  Kail Olson
 * @date    May 18th, 2024
*/

// Opcodes
#define     OP_W25N01_DEVICE_RESET            0xFFU
#define     OP_W25N01_JEDEC_ID                0x9FU
#define     OP_W25N01_READ_SR                 0x0FU
#define     OP_W25N01_WRITE_SR                0x1FU
#define     OP_W25N01_WRITE_EN                0x06U
#define     OP_W25N01_WRITE_DISABLE           0x04U
#define     OP_W25N01_BBM_SWAP_BLOCKS         0xA1U
#define     OP_W25N01_READ_BBM_LUT            0xA5U
#define     OP_W25N01_ECC_FAIL_PG_ADDR        0xA9U
#define     OP_W25N01_BLOCK_ERASE             0xD8U
#define     OP_W25N01_LOAD_PGM_DATA           0x02U
#define     OP_W25N01_RAND_LOAD_PGM_DATA      0x84U
#define     OP_W25N01_Q_LOAD_PGM_DATA         0x32U
#define     OP_W25N01_Q_RAND_LOAD_PGM_DATA    0x34U
#define     OP_W25N01_PGM_EXECUTE             0x10U
#define     OP_W25N01_PG_DATA_READ            0x13U
#define     OP_W25N01_READ                    0x03U
#define     OP_W25N01_FAST_READ               0x0BU
#define     OP_W25N01_FAST_READ_4B_ADDR       0x0CU
#define     OP_W25N01_DO_FAST_READ            0x3BU
#define     OP_W25N01_DO_FAST_READ_4B_ADDR    0x3CU
#define     OP_W25N01_QO_FAST_READ            0x6BU
#define     OP_W25N01_QO_FAST_READ_4B_ADDR    0x6CU
#define     OP_W25N01_DIO_FAST_READ           0xBBU
#define     OP_W25N01_DIO_FAST_READ_4B_ADDR   0xBCU
#define     OP_W25N01_QIO_FAST_READ           0xEBU
#define     OP_W25N01_QIO_FAST_READ_4B_ADDR   0xECU

// Special addresses
#define     ADDR_W25N01_SR1         0xA0U
#define     ADDR_W25N01_SR2         0xB0U
#define     ADDR_W25N01_SR3         0xC0U

// Special keys
#define     KEY_W25N01_MFG_ID       0xEFU
#define     KEY_W25N01_DEVICE_ID_U  0xAAU
#define     KEY_W25N01_DEVICE_ID_L  0x21U

// Bitmasks
#define     MASK_W25N01_SR2_RESERVED_BITS   0x07U
#define     MASK_W25N01_SR3_P_FAIL          0x08U
#define     MASK_W25N01_SR3_E_FAIL          0x04U
#define     MASK_W25N01_SR3_WEL             0x02U
#define     MASK_W25N01_SR3_BUSY            0x01U

// Special numbers
#define     INT_W25N01_COLS_PER_PAGE    2112U
#define     INT_W25N01_PAGES_PER_BLOCK  64U
#define     INT_W25N01_NUM_BLOCKS       1024U

// For SPI driver
#define     SPI_W25N01_TIMEOUT  1000U

#include <stdint.h>
#include <stdlib.h>
#include "main.h"

// Enum definition
enum e_flash_status {
    FLASH_OK,
    FLASH_NOT_OK,
};
typedef enum e_flash_status e_flash_status;


class W25N01GVSFIG {
    private:
        // Member variables
        GPIO_TypeDef*       m_GPIOx;
        uint16_t            m_GPIO_Pin;
        SPI_HandleTypeDef*  m_HSPIPtr;
        uint16_t            m_pageAddress;
        uint16_t            m_columnAddress;

        // Private member functions
        inline void     m_updatePageAndColumnAddress(uint32_t address);
        void            m_pageDataRead();
        e_flash_status  m_writeEnable();
        e_flash_status  m_executeProgram();
        uint8_t         m_checkSR(uint8_t SRNumber);

    public:
        // Constructor and destructor
        W25N01GVSFIG(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, SPI_HandleTypeDef* HSPIPtr, uint16_t pageAddress, uint16_t columnAddress);
        ~W25N01GVSFIG();

        // Public member functions
        e_flash_status  init();
        e_flash_status  write(uint8_t* data, uint32_t address, uint32_t size);
        e_flash_status  read(uint8_t* data, uint32_t address, uint32_t size);
        e_flash_status  eraseChip();
        e_flash_status  eraseSmallestSection(uint32_t address);
        uint32_t        getSmallestErasableSectionSize();
        uint8_t         isBusy();
};
