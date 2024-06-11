#include <Sensors/Inc/MS561101BA03_50.hpp>


MS5611::MS5611(I2C_HandleTypeDef port)
{
this->port = port;
}


uint8_t MS5611::init()
{
	bool retCheck =HAL_I2C_Master_Transmit(&port, 0b11101110, &reset_data, 1, HAL_MAX_DELAY);
	if(retCheck == HAL_OK)//reset sequence - needed after every power-on
	{
		osDelay(10); //wait 10 ms for reset
		return true;
	} else {
		osDelay(10);
		return false;
	}
}
void MS5611::read()
{
	convertP();
	readADCP();
	convertT();
	readADCT();
}

uint8_t MS5611::readADCP()
{
    osDelay(30);
//	uint8_t REG_TEMP=0x00;
//	for (int i =0;i<3;i++){
//	bufT[i]=REG_TEMP;
//}

	// Transmit ADC Read command after conversion
	HAL_I2C_Master_Transmit(&port, 0b11101111, &cmdReadADC, 1, HAL_MAX_DELAY);

	HAL_I2C_Master_Receive(&port, 0b11101111, bufP, 3, HAL_MAX_DELAY);

//	dpv = (uint32_t)(bufP[0]<<16)|(bufP[1]<<8)|bufP[2];
	dpv = 0x00000000;
	dpv = dpv | (bufP[0] << 16);
	dpv = dpv | (bufP[1] << 8);
	dpv = dpv | (bufP[2] << 0);
}

uint8_t MS5611::readADCT()
{
    osDelay(30);
//	uint8_t REG_TEMP=0x00;
//	for (int i =0;i<3;i++){
//	bufT[i]=REG_TEMP;
//}

	// Transmit ADC Read command after conversion
 	HAL_I2C_Master_Transmit(&port, 0b11101111, &cmdReadADC, 1, HAL_MAX_DELAY);

	HAL_I2C_Master_Receive(&port, 0b11101111, bufT, 3, HAL_MAX_DELAY);
	//dtv = ((uint32_t)bufT[0]<<16)|(bufT[1]<<8)|bufT[2];
	dtv = 0x00000000;
	dtv = dtv | (bufT[0] << 16);
	dtv = dtv | (bufT[1] << 8);
	dtv = dtv | (bufT[2] << 0);
}


void MS5611::readPROM()
{
    uint8_t cmd = 0xA2;// first variable cmd to read PROM data for C1
    uint8_t data[2];

    for (int i = 0; i < 6; i++)
    {
        HAL_I2C_Master_Transmit(&port, 0b11101111, &cmd, 1, HAL_MAX_DELAY);
        HAL_I2C_Master_Receive(&port, 0b11101111, data, 2, HAL_MAX_DELAY);
        prom[i] = (data[0] << 8) | data[1];
        cmd += 2;
    }
}

void MS5611::calculate()
{
    int64_t dT = dtv - ((uint32_t)prom[4] << 8);//dT = D2 - C5 * 2^8
    int64_t TEMP = 2000 + (((dtv - ((uint32_t)prom[4] << 8)) * prom[5]) >> 23);//TEMP = 20°C + dT* TEMPSENS= 2000 + dT * C6 / 2^23

    int64_t OFF = ((uint64_t)prom[1] << 17) + ((prom[3] * dT) >> 7);
    int64_t SENS = ((uint64_t)prom[0] << 16) + ((prom[2] * dT) >> 8);

    //Second order temperature compensation
    int64_t OFF2 = 0;
    int64_t SENS2 = 0;

    if (TEMP < 2000) {
        OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 2;
        SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 4;
        if (TEMP < -1500) {
            OFF2 = OFF2 + 7 * ((TEMP + 1500) * (TEMP + 1500));
            SENS2 = SENS2 + 11 * ((TEMP + 1500) * (TEMP + 1500)) / 2;
        }
    }

    //Offset and sensitivity compensation
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;

    int32_t P = (((dpv * SENS) >> 21) - OFF) >> 15;

    temperature = TEMP;
    pressure = P;
}

void MS5611::convertP()
{
	osrP = 0x48;//convert pressure command code, OSR - 512 (datasheet page 10)

	//set delay between transmit and receive based on osr for pressure
	if(osrP==0x40)
	{
		delayP = 1;
	}
	else if(osrP==0x42)
	{
		delayP = 2;
	}
	else if(osrP==0x44)
	{
		delayP = 3;
	}
	else if(osrP==0x46)
	{
		delayP = 5;
	}
	else if(osrP==0x48)
	{
		delayP = 10;
	}

	HAL_I2C_Master_Transmit(&port, 0b11101110, &osrP, 1, HAL_MAX_DELAY);
	osDelay(delayP);//from page 3 of datasheet based on OSR
}

void MS5611::convertT()
{
	osrT = 0x58;//convert temperature command, OSR - 512 (datasheet page 10)

	//set delay between transmit and receive based on osr for temperature
	if(osrT==0x50)
	{
		delayT = 1;
	}
	else if(osrT==0x52)
	{
		delayT = 2;
	}
	else if(osrP==0x54)
	{
		delayT = 3;
	}
	else if(osrP==0x56)
	{
		delayT = 5;
	}
	else if(osrP==0x58)
	{
		delayT = 10;
	}

	HAL_I2C_Master_Transmit(&port, 0b11101110, &osrT, 1, HAL_MAX_DELAY);
	osDelay(delayT);//from page 3 of datasheet based on OSR
	}
//11101111 00111111 00001111 10010011 00010111 01100010 10010000
/*int main() {
    UART_HandleTypeDef huart3;
    MS5611 ms5611;
    char msg[50];

    // Initialize the MS5611 sensor
    ms5611.init();

    // Read the PROM data
    ms5611.readPROM();

    // Continuously read and calculate temperature and pressure
    while (1) {
        ms5611.read();
        ms5611.calculate();

        // Format the temperature and pressure values as a string
        sprintf(msg, "Temperature: %.2f C, Pressure: %.2f hPa\r\n", ms5611.temperature, ms5611.pressure);

        // Transmit the string over UART3
        HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);


        // Delay for a second
        osDelay(1000);
    }

    return 0;
}
*/
