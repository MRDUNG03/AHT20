#include <Arduino.h>
#include <AHT20.h>
AHT20 aht20;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  aht20.begin();

}
void SENSOR(){
    if (aht20.available() == true)
  {
    //Get the new temperature and humidity value
    float temperature = aht20.GetTEMP();
    float humidity = aht20.GetHUMI();

    //Print the results
    Serial.print("Temperature: ");
    Serial.print(temperature, 2);
    Serial.print(" C\t");
    Serial.print("Humidity: ");
    Serial.print(humidity, 2);
    Serial.print("% RH");

    Serial.println();
  }

  delay(1000);
}

void loop()
{
  SENSOR();

}
