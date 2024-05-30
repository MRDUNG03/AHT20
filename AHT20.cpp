#include <AHT20.h>

AHT20 ::AHT20(const uint8_t deviceADDRESS)
{
    _deviceADDRESS = deviceADDRESS;
}
bool AHT20 ::begin()
{
    if (isConnected() == false)
    
        return false;
        delay(50);

        if (isCalibrated() == false)
        {
            // send bit 0XBE000
            triggerMeasurement();
            delay(80);
            uint8_t counter = 0;
            while (isBusy())
            {
                delay(1);
                if (counter++ > 100)
                    return (false);
            }
            if (isCalibrated() == false)
            {
                return (false);
            }
        }
        /// check bit has been set
        if (isCalibrated() == false)
            return false;

        // Mark all datums as fresh (not read before)
        SENSORQUER.TEMP = true;
        SENSORQUER.HUMI = true;

        return true;
    }

/// I2C ADDRESS
bool AHT20 ::isConnected()
{
    Wire.beginTransmission(_deviceADDRESS);
    if (Wire.endTransmission() == 0)
    {
        return true;
    }

    // If IC failed to respond, give it 20ms more for Power On Startup
    // Datasheet pg 7
    delay(20);

    Wire.beginTransmission(_deviceADDRESS);
    if (Wire.endTransmission() == 0)
    {
        return true;
    }
    return false;
}
uint8_t AHT20::GetStatus()
{
    Wire.requestFrom(_deviceADDRESS, (uint8_t)1);
    if (Wire.available())
        return (Wire.read());
    return (0);
}

//Returns the state of the cal bit in the status byte
bool AHT20::isCalibrated()
{
    return (GetStatus() & (1 << 3));
}

//Returns the state of the busy bit in the status byte
bool AHT20::isBusy()
{
    return (GetStatus() & (1 << 7));
}

bool AHT20::initialize()
{
    Wire.beginTransmission(_deviceADDRESS);
    Wire.write(sfe_aht20_reg_initialize);
    Wire.write((uint8_t)0x08);
    Wire.write((uint8_t)0x00);
    if (Wire.endTransmission() == 0)
        return true;
    return false;
}

bool AHT20 :: triggerMeasurement(){
   Wire.beginTransmission(_deviceADDRESS);
    Wire.write(sfe_aht20_reg_measure);
    Wire.write((uint8_t)0x33);
    Wire.write((uint8_t)0x00);
    if (Wire.endTransmission() == 0)
        return true;
    return false;
}
void AHT20::readData()
{
    //Clear previous data
    SENSORDATA.TEMP = 0;
    SENSORDATA.HUMI = 0;

    if (Wire.requestFrom( _deviceADDRESS, (uint8_t)6) > 0)
    {
        Wire.read(); // Read and discard state

        uint32_t incoming = 0;
        incoming |= (uint32_t)Wire.read() << (8 * 2);
        incoming |= (uint32_t)Wire.read() << (8 * 1);
        uint8_t midByte = Wire.read();

        incoming |= midByte;
        SENSORDATA.HUMI = incoming >> 4;

        SENSORDATA.TEMP = (uint32_t)midByte << (8 * 2);
        SENSORDATA.TEMP |= (uint32_t)Wire.read() << (8 * 1);
        SENSORDATA.TEMP |= (uint32_t)Wire.read() << (8 * 0);

        //Need to get rid of data in bits > 20
        SENSORDATA.TEMP = SENSORDATA.TEMP & ~(0xFFF00000);

        //Mark data as fresh
        SENSORQUER.TEMP = false;
        SENSORQUER.HUMI = false;
    }
}
bool AHT20::available()
{
    if (StartMeasur == false)
    {
        triggerMeasurement();
        StartMeasur = true;
        return (false);
    }

    if (isBusy() == true)
    {
        return (false);
    }

    readData();
    StartMeasur= false;
    return (true);
}

bool AHT20::softReset()
{
    Wire.beginTransmission( _deviceADDRESS);
    Wire.write(sfe_aht20_reg_reset);
    if (Wire.endTransmission() == 0)
        return true;
    return false;
}

/*------------------------- Make Measurements ----------------------------*/

float AHT20::GetTEMP()
{
    if (SENSORQUER.TEMP == true)
    {
        //We've got old data so trigger new measurement
        triggerMeasurement();

        delay(75); //Wait for measurement to complete

        uint8_t counter = 0;
        while (isBusy())
        {
            delay(1);
            if (counter++ > 100)
                return (false); //Give up after 100ms
        }

        readData();
    }

    //From datasheet pg 8
    float tempCelsius = ((float)SENSORDATA.TEMP / 1048576) * 200 - 50;

    //Mark data as old
 SENSORQUER.TEMP = true;

    return tempCelsius;
}

float AHT20::GetHUMI()
{
    if (SENSORQUER.HUMI == true)
    {
        //We've got old data so trigger new measurement
        triggerMeasurement();

        delay(75); //Wait for measurement to complete

        uint8_t counter = 0;
        while (isBusy())
        {
            delay(1);
            if (counter++ > 100)
                return (false); //Give up after 100ms
        }

        readData();
    }

    //From datasheet pg 8
    float relHumidity = ((float)SENSORDATA.HUMI / 1048576) * 100;

    //Mark data as old
    SENSORQUER.HUMI = true;

    return relHumidity;
}
