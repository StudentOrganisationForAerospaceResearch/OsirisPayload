/**
 * @file    W25N01GVSFIG.cpp
 * @brief   Contains code for the W25N01 Flash chip on the SAC 2024 payload
 * @author  Kail Olson
 * @date    May 18th, 2024
*/

#include "W25N01GVSFIG.hpp"

// Constructor
W25N01GVSFIG::W25N01GVSFIG(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, SPI_HandleTypeDef* HSPIPtr, uint16_t pageAddress, uint16_t columnAddress)
    : m_GPIOx { GPIOx },
      m_GPIO_Pin { GPIO_Pin },
      m_HSPIPtr { HSPIPtr },
      m_pageAddress { pageAddress },
      m_columnAddress { columnAddress }
    { /*No other function*/ }


// Private member functions
inline void W25N01GVSFIG::m_updatePageAndColumnAddress(uint32_t address) {
    m_pageAddress = uint16_t (address / 2112);
    m_columnAddress = uint16_t (address % 2112);
}

void W25N01GVSFIG::m_pageDataRead() {
    // Create message
    uint8_t* messagePtr = new uint8_t[4];
    messagePtr[0] = OP_W25N01_PG_DATA_READ;
    messagePtr[1] = 0x00U; // Garbage
    messagePtr[2] = (uint8_t) ((m_pageAddress & 0xFF00) >> 8U);
    messagePtr[3] = (uint8_t) (m_pageAddress & 0x00FF);

    // Pull CS low
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

    // Transmit "Page Data Read" instruction
    HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 4, SPI_W25N01_TIMEOUT);

    // Pull CS high
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

    // Delete message
    delete[] messagePtr;

    // Wait 1ms (minimum 5us) required before page is fully read
    HAL_Delay(1);

    return;
}

e_flash_status W25N01GVSFIG::m_writeEnable() {
	e_flash_status flashStatus = FLASH_OK;

    // Create message
    uint8_t* messagePtr = new uint8_t;
    *messagePtr = OP_W25N01_WRITE_EN;

    // Pull CS low
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

    // Transmit "Write Enable" instruction
    HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 1, SPI_W25N01_TIMEOUT);

    // Pull CS high
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

    // Delete message
    delete messagePtr;

    // Return a NOT_OK enum if the write-enable latch was not set
    if (!(this->m_checkSR(3) & MASK_W25N01_SR3_WEL)) flashStatus = FLASH_NOT_OK;

    return flashStatus;
}

e_flash_status W25N01GVSFIG::m_executeProgram() {
    e_flash_status flashStatus = FLASH_OK;

    // Create message
    uint8_t* messagePtr = new uint8_t[4];
    messagePtr[0] = OP_W25N01_PGM_EXECUTE;
    messagePtr[1] = 0x00U; // Garbage
    messagePtr[2] = (uint8_t) ((m_pageAddress & 0xFF00) >> 8U);
    messagePtr[3] = (uint8_t) (m_pageAddress & 0x00FF);

    // Pull CS low
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

    // Transmit "Program Execute" instruction
    HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 4, SPI_W25N01_TIMEOUT);

    // Pull CS high
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

    // Delete message
    delete[] messagePtr;

    // Wait 1ms (minimum 10us as per datasheet)
    HAL_Delay(1);

    // Return a NOT_OK enum if the program-fail bit was set
    if (this->m_checkSR(3) & MASK_W25N01_SR3_P_FAIL) flashStatus = FLASH_NOT_OK;

    return flashStatus;
}

uint8_t W25N01GVSFIG::m_checkSR(uint8_t SRNumber) {
	uint8_t SRValue;

    // Prepare message
    uint8_t* messagePtr = new uint8_t[2];
    messagePtr[0] = OP_W25N01_READ_SR;
    if (SRNumber == 1) {
    	messagePtr[1] = ADDR_W25N01_SR1;
    } else if (SRNumber == 2) {
    	messagePtr[1] = ADDR_W25N01_SR2;
    } else {
    	messagePtr[1] = ADDR_W25N01_SR3;
    }

    // Pull CS low
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

    // Transmit "Read SR" instruction
    HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 2, SPI_W25N01_TIMEOUT);

    // Receive SR value
    HAL_SPI_Receive(m_HSPIPtr, (uint8_t*) &SRValue, 1, SPI_W25N01_TIMEOUT);

    // Pull CS high
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

    return SRValue;
}


// Public member functions
e_flash_status W25N01GVSFIG::init() {
    e_flash_status flashStatus = FLASH_OK;

    // Create message
    uint8_t* messagePtr = new uint8_t[2];
    messagePtr[0] = OP_W25N01_JEDEC_ID;
    messagePtr[1] = 0x00U; // Garbage

    // Create receive buffer
    uint8_t* receivedMessagePtr = new uint8_t[3];

    // Pull CS low
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

    // Transmit "JEDEC ID" instruction
    HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 2, SPI_W25N01_TIMEOUT);

    // Receive message back
    HAL_SPI_Receive(m_HSPIPtr, receivedMessagePtr, 3, SPI_W25N01_TIMEOUT);

    // Pull CS high
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

    // Check for correct received message
    if (receivedMessagePtr[0] != KEY_W25N01_MFG_ID)         flashStatus = FLASH_NOT_OK;
    if (receivedMessagePtr[1] != KEY_W25N01_DEVICE_ID_U)    flashStatus = FLASH_NOT_OK;
    if (receivedMessagePtr[2] != KEY_W25N01_DEVICE_ID_L)    flashStatus = FLASH_NOT_OK;

    // Delete sent and received messages
    delete[] messagePtr;
    delete[] receivedMessagePtr;

    // Create SR-1 message
    messagePtr = new uint8_t[3];
    messagePtr[0] = OP_W25N01_WRITE_SR;
    messagePtr[1] = ADDR_W25N01_SR1;
    messagePtr[2] = 0x00U; // SR-1 settings for Osiris application

    // Enable writing
    this->m_writeEnable();

    // Pull CS low
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

    // Transmit "Write Status Register" instruction
    HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 3, SPI_W25N01_TIMEOUT);

    // Pull CS high
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

    // Create SR-2 message
    // messagePtr[0] already has SR write instruction
    messagePtr[1] = ADDR_W25N01_SR2;
    messagePtr[2] = 0x08U; // SR-2 settings for Osiris application (BUF=1)

    // Enable writing
    this->m_writeEnable();

    // Pull CS low
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

    // Transmit "Write Status Register" instruction
    HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 3, SPI_W25N01_TIMEOUT);

    // Pull CS high
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

    // Delete message
    delete[] messagePtr;

    // Check that the SR registers were correctly configured
    if (this->m_checkSR(1) != 0x00U)                                    flashStatus = FLASH_NOT_OK;
    if ((this->m_checkSR(2) & ~MASK_W25N01_SR2_RESERVED_BITS) != 0x08U) flashStatus = FLASH_NOT_OK;

    // Return
    return flashStatus;
}

e_flash_status W25N01GVSFIG::write(uint8_t* data, uint32_t address, uint32_t size) {
    e_flash_status flashStatus = FLASH_OK;

    // Convert given address to page and column
    this->m_updatePageAndColumnAddress(address);

    // Make program loading message
    uint8_t* messagePtr = new uint8_t[3];
    messagePtr[0] = OP_W25N01_RAND_LOAD_PGM_DATA;
    messagePtr[1] = (uint8_t) ((m_columnAddress & 0xFF00U) >> 8U);
    messagePtr[2] = (uint8_t) (m_columnAddress & 0x00FFU);

    // Writing data
    uint32_t remainingData = size;
    uint32_t sizeToWrite = 0;
    uint32_t dataOffset = 0;
    
    if (remainingData > (INT_W25N01_COLS_PER_PAGE - m_columnAddress))   sizeToWrite = INT_W25N01_COLS_PER_PAGE - m_columnAddress;
    else                                                                sizeToWrite = remainingData;

    while (remainingData > 0) {
        // Read page data into buffer
        this->m_pageDataRead();

        // Enable writing
        this->m_writeEnable();

        // Pull CS low
        HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

        // Transmit "Random Load Program Data" instruction
        HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 3, SPI_W25N01_TIMEOUT);

        // CS still low

        // Writing data
        HAL_SPI_Transmit(m_HSPIPtr, &data[dataOffset-1], sizeToWrite, SPI_W25N01_TIMEOUT);

        // Pull CS high
        HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

        // Enable writing
        this->m_writeEnable();

        // Execute program
        if (this->m_executeProgram() != FLASH_OK) return FLASH_NOT_OK; // Evil return but who cares

        // Subtract written data from size
        remainingData -= sizeToWrite;

        // Determine next "sizeToWrite"
        if (remainingData > INT_W25N01_COLS_PER_PAGE)   sizeToWrite = INT_W25N01_COLS_PER_PAGE;
        else                                            sizeToWrite = remainingData;

        // Set offset
        dataOffset += sizeToWrite;

        // Increment page address and set column address to 0
        m_pageAddress++;
        m_columnAddress = 0;
    }

    // Delete message
    delete[] messagePtr;

    return flashStatus;
}

e_flash_status W25N01GVSFIG::eraseChip() {
    // Loop through all blocks and erase
    for (uint32_t i = 0; i < INT_W25N01_NUM_BLOCKS; i++) {
        if (this->eraseSmallestSection(i * INT_W25N01_PAGES_PER_BLOCK * INT_W25N01_COLS_PER_PAGE) != FLASH_OK) return FLASH_NOT_OK;
    }

    return FLASH_OK;
}

e_flash_status W25N01GVSFIG::eraseSmallestSection(uint32_t address) {
    e_flash_status flashStatus = FLASH_OK;

    // Update page and column address
    this->m_updatePageAndColumnAddress(address);

    // Prepare message
    uint8_t* messagePtr = new uint8_t[4];
    messagePtr[0] = OP_W25N01_BLOCK_ERASE;
    messagePtr[1] = 0x00U; // Garbage
    messagePtr[2] = (uint8_t) ((m_pageAddress & 0xFF00U) >> 8U);
    messagePtr[3] = (uint8_t) (m_pageAddress & 0x00FFU);

    // Enable writing
    this->m_writeEnable();

    // Pull CS low
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_RESET);

    // Transmit "Erase Block" instruction
    HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 4, SPI_W25N01_TIMEOUT);

    // Pull CS high
    HAL_GPIO_WritePin(m_GPIOx, m_GPIO_Pin, GPIO_PIN_SET);

    // Delete message
    delete[] messagePtr;

    // Wait for 1ms (minimum 500us as per datasheet)
    HAL_Delay(1);

    // Check E-FAIL bit in SR3
    if (this->m_checkSR(3) & MASK_W25N01_SR3_E_FAIL) flashStatus = FLASH_NOT_OK;

    return flashStatus;
}

uint32_t W25N01GVSFIG::getSmallestErasableSectionSize() {
    return INT_W25N01_COLS_PER_PAGE * INT_W25N01_PAGES_PER_BLOCK;
}

uint8_t W25N01GVSFIG::isBusy() {
    // Return value of busy bit in SR3
    return (this->m_checkSR(3) & MASK_W25N01_SR3_BUSY);
}


