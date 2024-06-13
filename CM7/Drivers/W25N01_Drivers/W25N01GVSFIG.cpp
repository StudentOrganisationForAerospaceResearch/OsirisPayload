/**
 * @file    W25N01GVSFIG.cpp
 * @brief   Contains code for the W25N01 Flash chip on the SAC 2024 payload
 * @author  Kail Olson
 * @date    May 18th, 2024
*/

#include "W25N01GVSFIG.hpp"
#include "SystemDefines.hpp"
#include <cstring>

// Constructor
W25N01GVSFIG::W25N01GVSFIG(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, QSPI_HandleTypeDef* HSPIPtr, uint16_t pageAddress, uint16_t columnAddress)
    : m_GPIOx { GPIOx },
      m_GPIO_Pin { GPIO_Pin },
      m_HSPIPtr { HSPIPtr },
      m_pageAddress { pageAddress },
      m_columnAddress { columnAddress }
    { /*No other function*/ }


    // Destructor
    W25N01GVSFIG::~W25N01GVSFIG()

	  { /*No other function*/

	  }


// Private member functions
inline void W25N01GVSFIG::m_updatePageAndColumnAddress(uint32_t address) {
    m_pageAddress = uint16_t (address / INT_W25N01_COLS_PER_PAGE);
    m_columnAddress = uint16_t (address % INT_W25N01_COLS_PER_PAGE);
}

void W25N01GVSFIG::m_pageDataRead() {
    // Create message
    uint8_t messagePtr[4];
    messagePtr[0] = OP_W25N01_PG_DATA_READ;
    messagePtr[1] = 0x00U; // Garbage
    messagePtr[2] = (uint8_t) ((m_pageAddress & 0xFF00) >> 8U);
    messagePtr[3] = (uint8_t) (m_pageAddress & 0x00FF);

    static QSPI_CommandTypeDef cmd;
    cmd.Address = 0x00;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = 0;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DummyCycles = 0;
    cmd.Instruction = OP_W25N01_PG_DATA_READ;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.NbData=3;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    // Transmit "Page Data Read" instruction
    if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    }
    if(HAL_QSPI_Transmit(m_HSPIPtr, &messagePtr[1], SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not transmit data\n");
    }


    // Wait 1ms (minimum 5us) required before page is fully read
    HAL_Delay(1);

    return;
}

e_flash_status W25N01GVSFIG::m_writeEnable() {
	e_flash_status flashStatus = FLASH_OK;

	static QSPI_CommandTypeDef cmd;
    cmd.Address = 0x00;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_NONE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DummyCycles = 0;
    cmd.Instruction = OP_W25N01_WRITE_EN;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.NbData=0;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    	return FLASH_NOT_OK;
    }

    // Return a NOT_OK enum if the write-enable latch was not set
    if (!(this->m_checkSR(3) & MASK_W25N01_SR3_WEL)) flashStatus = FLASH_NOT_OK;

    return flashStatus;
}

e_flash_status W25N01GVSFIG::m_writeStatReg(uint8_t SRNumber, uint8_t val) {
	e_flash_status flashStatus = FLASH_OK;

	static QSPI_CommandTypeDef cmd;

	if(SRNumber == 1) {
		cmd.Address = ADDR_W25N01_SR1;
	} else if (SRNumber == 2) {
		cmd.Address = ADDR_W25N01_SR2;
	} else {
		cmd.Address = ADDR_W25N01_SR3;
	}

    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_8_BITS;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DummyCycles = 0;
    cmd.Instruction = OP_W25N01_WRITE_SR;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.NbData=1;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    	return FLASH_NOT_OK;
    }
    if(HAL_QSPI_Transmit(m_HSPIPtr, &val, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not transmit data\n");
    	return FLASH_NOT_OK;
    }

    // Return a NOT_OK enum if the write-enable latch was not set
    if (!(this->m_checkSR(3) & MASK_W25N01_SR3_WEL)) flashStatus = FLASH_NOT_OK;

    return flashStatus;
}

e_flash_status W25N01GVSFIG::m_executeProgram() {
    e_flash_status flashStatus = FLASH_OK;

    // Create message
    uint8_t messagePtr[4];
    messagePtr[0] = OP_W25N01_PGM_EXECUTE;
    messagePtr[1] = 0x00U; // Garbage
    messagePtr[2] = (uint8_t) ((m_pageAddress & 0xFF00) >> 8U);
    messagePtr[3] = (uint8_t) (m_pageAddress & 0x00FF);

    // Transmit "Program Execute" instruction
    //HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 4, SPI_W25N01_TIMEOUT);

    static QSPI_CommandTypeDef cmd;
    cmd.Address = 0x00;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DummyCycles = 0;
    cmd.Instruction = OP_W25N01_PGM_EXECUTE;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.NbData=3;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    	return FLASH_NOT_OK;
    }
    if(HAL_QSPI_Transmit(m_HSPIPtr, messagePtr+1, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    	return FLASH_NOT_OK;
    }

    // Wait 1ms (minimum 10us as per datasheet)
    HAL_Delay(1);

    // Return a NOT_OK enum if the program-fail bit was set
    if (this->m_checkSR(3) & MASK_W25N01_SR3_P_FAIL) flashStatus = FLASH_NOT_OK;

    return flashStatus;
}

uint8_t W25N01GVSFIG::m_checkSR(uint8_t SRNumber) {
	uint8_t SRValue = 0xaa;

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

    static QSPI_CommandTypeDef cmd;
    cmd.Address = messagePtr[1];
    cmd.AddressMode = QSPI_ADDRESS_1_LINE;
    cmd.AddressSize = QSPI_ADDRESS_8_BITS;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = 0;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DummyCycles = 0;
    cmd.Instruction = OP_W25N01_READ_SR;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.NbData=1;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    	return FLASH_NOT_OK;
    }
//    if(HAL_QSPI_Transmit(m_HSPIPtr, &messagePtr[1],SPI_W25N01_TIMEOUT) != HAL_OK) {
//    	SOAR_PRINT("Could not set qspi transmit\n");
//    	return FLASH_NOT_OK;
//    }



    // Transmit "Read SR" instruction
    //HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 2, SPI_W25N01_TIMEOUT);

    // Receive SR value
    //HAL_SPI_Receive(m_HSPIPtr, (uint8_t*) &SRValue, 1, SPI_W25N01_TIMEOUT);

    HAL_QSPI_Receive(m_HSPIPtr, &SRValue, SPI_W25N01_TIMEOUT);

    return SRValue;
}


// Public member functions
e_flash_status W25N01GVSFIG::init() {
    e_flash_status flashStatus = FLASH_OK;

    if(m_HSPIPtr ==nullptr) {
    	m_HSPIPtr = new QSPI_HandleTypeDef;

    	m_HSPIPtr->Init.ClockPrescaler = 255;
    	m_HSPIPtr->Init.FifoThreshold = 4;
    	m_HSPIPtr->Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
    	m_HSPIPtr->Init.FlashSize = 26;
    	m_HSPIPtr->Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_8_CYCLE;
    	m_HSPIPtr->Init.ClockMode = QSPI_CLOCK_MODE_0;
    	m_HSPIPtr->Init.FlashID = QSPI_FLASH_ID_1;
    	m_HSPIPtr->Init.DualFlash = QSPI_DUALFLASH_DISABLE;

    	m_HSPIPtr->Instance = QUADSPI;

    	if(HAL_QSPI_Init(m_HSPIPtr) != HAL_OK) {
    		SOAR_PRINT("Could not init QSPI\n");
    		return FLASH_NOT_OK;
    	}
    }

    static QSPI_CommandTypeDef cmd;
    cmd.Address = 0x00;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DummyCycles = 8;
    cmd.Instruction = OP_W25N01_JEDEC_ID;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.NbData=3; // JEDEC ID plus two device IDs
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    	return FLASH_NOT_OK;
    }

    // Create receive buffer
    uint8_t receivedMessagePtr[3];
    memset(receivedMessagePtr,0x00,3);

    //Receive IDs
    HAL_StatusTypeDef result = HAL_QSPI_Receive(m_HSPIPtr, receivedMessagePtr, SPI_W25N01_TIMEOUT);

    if(result != HAL_OK) {
    	SOAR_PRINT("Could not receive JEDEC bytes\n");
    	return FLASH_NOT_OK;
    } else {
    	SOAR_PRINT("Received: %x %x %x\n",receivedMessagePtr[0],receivedMessagePtr[1],receivedMessagePtr[2]);
    }

    // Check for correct received message
    if (receivedMessagePtr[0] != KEY_W25N01_MFG_ID)         flashStatus = FLASH_NOT_OK;
    if (receivedMessagePtr[1] != KEY_W25N01_DEVICE_ID_U)    flashStatus = FLASH_NOT_OK;
    if (receivedMessagePtr[2] != KEY_W25N01_DEVICE_ID_L)    flashStatus = FLASH_NOT_OK;

    // Enable writing
    this->m_writeEnable();

    this->m_writeStatReg(1, 0x00); // SR-1 settings for Osiris application


    // Enable writing
    this->m_writeEnable();

    this->m_writeStatReg(2, 0x08);// SR-2 settings for Osiris application (BUF=1)


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


        static QSPI_CommandTypeDef cmd;
        cmd.Address = m_columnAddress;
        cmd.AddressMode = QSPI_ADDRESS_1_LINE;
        cmd.AddressSize = QSPI_ADDRESS_16_BITS;
        cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        cmd.AlternateBytes = 0;
        cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
        cmd.DataMode = QSPI_DATA_1_LINE;
        cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
        cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
        cmd.DummyCycles = 0;
        cmd.Instruction = OP_W25N01_RAND_LOAD_PGM_DATA;
        cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
        cmd.NbData=sizeToWrite;
        cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

        // Transmit "Random Load Program Data" instruction
        //HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 3, SPI_W25N01_TIMEOUT);

        if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
        	SOAR_PRINT("Could not set qspi command\n");
        	return FLASH_NOT_OK;
        }

        if(HAL_QSPI_Transmit(m_HSPIPtr, &data[dataOffset], SPI_W25N01_TIMEOUT) != HAL_OK) {
        	SOAR_PRINT("Could not set qspi command\n");
        	return FLASH_NOT_OK;
        }

        // Enable writing
        this->m_writeEnable();

        // Execute program
        if (this->m_executeProgram() != FLASH_OK) return FLASH_NOT_OK; // Evil return but who cares

        // Subtract written data from size
        remainingData -= sizeToWrite;

        // Set offset
        dataOffset += sizeToWrite;

        // Determine next "sizeToWrite"
        if (remainingData > INT_W25N01_COLS_PER_PAGE)   sizeToWrite = INT_W25N01_COLS_PER_PAGE;
        else                                            sizeToWrite = remainingData;


        // Increment page address and set column address to 0
        m_pageAddress++;
        m_columnAddress = 0;
    }

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
    uint8_t messagePtr[4];
    messagePtr[0] = OP_W25N01_BLOCK_ERASE;
    messagePtr[1] = 0x00U; // Garbage
    messagePtr[2] = (uint8_t) ((m_pageAddress & 0xFF00U) >> 8U);
    messagePtr[3] = (uint8_t) (m_pageAddress & 0x00FFU);

    // Enable writing
    this->m_writeEnable();

    static QSPI_CommandTypeDef cmd;
    cmd.Address = 0x00;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AddressSize = QSPI_ADDRESS_32_BITS;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.AlternateBytes = 0;
    cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DummyCycles = 0;
    cmd.Instruction = OP_W25N01_BLOCK_ERASE;
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.NbData=3;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    // Transmit "Erase Block" instruction
    if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    	return FLASH_NOT_OK;
    }
    if(HAL_QSPI_Transmit(m_HSPIPtr, messagePtr+1, SPI_W25N01_TIMEOUT) != HAL_OK) {
    	SOAR_PRINT("Could not set qspi command\n");
    	return FLASH_NOT_OK;
    }

    // Wait for 1ms (minimum 500us as per datasheet)
    HAL_Delay(1);

    // Check E-FAIL bit in SR3
    if (this->m_checkSR(3) & MASK_W25N01_SR3_E_FAIL) {
    	flashStatus = FLASH_NOT_OK;
    	SOAR_PRINT("failed erase\n");
    }

    return flashStatus;
}

uint32_t W25N01GVSFIG::getSmallestErasableSectionSize() {
    return INT_W25N01_COLS_PER_PAGE * INT_W25N01_PAGES_PER_BLOCK;
}

uint8_t W25N01GVSFIG::isBusy() {
    // Return value of busy bit in SR3
    return (this->m_checkSR(3) & MASK_W25N01_SR3_BUSY);
}

e_flash_status W25N01GVSFIG::read(uint8_t *data, uint32_t address,
		uint32_t size) {
    e_flash_status flashStatus = FLASH_OK;

    // Convert given address to page and column
    this->m_updatePageAndColumnAddress(address);

    // Reading data
    uint32_t remainingData = size;
    uint32_t sizeToRead = 0;
    uint32_t dataOffset = 0;

    if (remainingData > (INT_W25N01_COLS_PER_PAGE - m_columnAddress))   sizeToRead = INT_W25N01_COLS_PER_PAGE - m_columnAddress;
    else                                                                sizeToRead = remainingData;

    while (remainingData > 0) {
        // Read page data into buffer
        this->m_pageDataRead();

        static QSPI_CommandTypeDef cmd;
        cmd.Address = m_columnAddress;
        cmd.AddressMode = QSPI_ADDRESS_1_LINE;
        cmd.AddressSize = QSPI_ADDRESS_16_BITS;
        cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
        cmd.AlternateBytes = 0;
        cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_NONE;
        cmd.DataMode = QSPI_DATA_1_LINE;
        cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
        cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
        cmd.DummyCycles = 8;
        cmd.Instruction = OP_W25N01_READ;
        cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
        cmd.NbData=sizeToRead;
        cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

        // Transmit "Random Load Program Data" instruction
        //HAL_SPI_Transmit(m_HSPIPtr, messagePtr, 3, SPI_W25N01_TIMEOUT);

        if(HAL_QSPI_Command(m_HSPIPtr, &cmd, SPI_W25N01_TIMEOUT) != HAL_OK) {
        	SOAR_PRINT("Could not set qspi command\n");
        	return FLASH_NOT_OK;
        }


        if(HAL_QSPI_Receive(m_HSPIPtr, data+dataOffset, SPI_W25N01_TIMEOUT) != HAL_OK) {
        	SOAR_PRINT("Could not set qspi command\n");
        	return FLASH_NOT_OK;
        }

        // Subtract written data from size
        remainingData -= sizeToRead;


        // Set offset
        dataOffset += sizeToRead;

        // Determine next "sizeToWrite"
        if (remainingData > INT_W25N01_COLS_PER_PAGE)   sizeToRead = INT_W25N01_COLS_PER_PAGE;
        else                                            sizeToRead = remainingData;

        // Increment page address and set column address to 0
        m_pageAddress++;
        m_columnAddress = 0;
    }

    return flashStatus;

}
