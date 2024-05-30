#ifndef AHT20_H
#define AHT20_H
#include "Arduino.h"
#include "Wire.h"

#define BME280_ADDRESS (0x77);
#define BME280_ADDRESS_ADD (0x76);
// DIA CHI CUA AHT20
#define AHT20_ADDRESS 0x38

enum RESSTER
{ /// DOC DIA CHI OW THANH GHI

    sfe_aht20_reg_reset = 0xBA,
    sfe_aht20_reg_initialize = 0xBE,
    sfe_aht20_reg_measure = 0xAC,
};
class AHT20
{
private:
    uint8_t _deviceADDRESS;
    bool StartMeasur = false;
    struct
    {
        uint32_t TEMP;
        uint32_t HUMI;

    } SENSORDATA;
    struct
    {
        uint8_t TEMP : 1;
        uint8_t HUMI : 1;
    } SENSORQUER;

public:
    explicit AHT20(const u_int8_t deviceADD = AHT20_ADDRESS);
    bool begin();
    bool isConnected(); // kiem tra xem da ket noi voi aht20 chua
    bool available();   // neu dung thi tra ve data moi
    //// HAM DO LUONG
    uint8_t GetStatus();
    bool isCalibrated();       // Trả về true nếu bit cal được đặt, nếu không thì trả về false
    bool isBusy();             // Returns true if the busy bit is set, false otherwise
    bool initialize();         // Initialize for taking measurement
    bool triggerMeasurement(); // Trigger the AHT20 to take a measurement
    void readData();           // Read and parse the 6 bytes of data into raw humidity and temp
    bool softReset();          // Restart the sensor system without turning power off and on
    // doc dem am nhiet do
    float GetTEMP();
    float GetHUMI();
};

#endif
