#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>

#if SIMULATED_DATA

void initSensor()
{
    // use SIMULATED_DATA, no sensor need to be inited
}

float readTemperature()
{
    return random(20, 30);
}

float readHumidity()
{
    return random(30, 40);
}

#else

int soilPin = A0;
int soilPower = 4;
int val = 0; 
int minSoil = 0;
int maxSoil = 900;

static DHT dht(DHT_PIN, DHT_TYPE);
void initSensor()
{
    dht.begin();
    Serial.println("before soil moisture");
    pinMode(soilPower, OUTPUT);//Set D7 as an OUTPUT
    digitalWrite(soilPower, LOW);//Set to LOW so no power is flowing through the sensor
    Serial.println("after soil moisture");
}

float readTemperature()
{
    return dht.readTemperature();
}

float readHumidity()
{
    return dht.readHumidity();
}

int readSoilMoisture()
{
    Serial.println("before soil moisture read");
    digitalWrite(soilPower, HIGH);//turn D7 "On"
    delay(10);//wait 10 milliseconds 
    //val = map(analogRead(soilPin), minSoil, maxSoil, 0, 100);//Read the SIG value form sensor 
    val = analogRead(soilPin);
    digitalWrite(soilPower, LOW);//turn D7 "Off"
    Serial.print("soil moisture:");
    Serial.println(val);
    return val;//send current moisture value
}

#endif

bool readMessage(int messageId, char *payload)
{
    float temperature = readTemperature();
    float humidity = readHumidity();
    int soilMoisture = readSoilMoisture();
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;
    root["soilMoisture"] = soilMoisture;
    bool temperatureAlert = false;

    // NAN is not the valid json, change it to NULL
    if (std::isnan(temperature))
    {
        root["temperature"] = NULL;
    }
    else
    {
        root["temperature"] = temperature;
        if (temperature > TEMPERATURE_ALERT)
        {
            temperatureAlert = true;
        }
    }

    if (std::isnan(humidity))
    {
        root["humidity"] = NULL;
    }
    else
    {
        root["humidity"] = humidity;
    }
    root.printTo(payload, MESSAGE_MAX_LEN);
    return temperatureAlert;
}

void parseTwinMessage(char *message)
{
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(message);
    if (!root.success())
    {
        Serial.printf("Parse %s failed.\r\n", message);
        return;
    }

    if (root["desired"]["interval"].success())
    {
        interval = root["desired"]["interval"];
    }
    else if (root.containsKey("interval"))
    {
        interval = root["interval"];
    }
}
