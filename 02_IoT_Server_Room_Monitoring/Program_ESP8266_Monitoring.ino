
/*
 * ==============================================================
 * Project    : IoT Server Room Monitoring
 * Hardware   : ESP8266 NodeMCU, DHT22
 * Framework  : Arduino
 * Protocol   : MQTT
 * Author     : Aulia Rahma Pramudita
 * ==============================================================
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// ======================================================
// WiFi Configuration
// ======================================================

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ======================================================
// MQTT Broker Configuration
// ======================================================

const char* mqtt_server = "BROKER_IP_ADDRESS";
const int mqtt_port = 1883;

const char* mqtt_user = "MQTT_USERNAME";
const char* mqtt_pass = "MQTT_PASSWORD";

// ======================================================
// Device Configuration
// ======================================================

const char* DEVICE_ID = "ESP8266_SERVER_ROOM";

// ======================================================
// MQTT Topics
// ======================================================

String root_topic = "server_room/" + String(DEVICE_ID);

String topic_temperature = root_topic + "/temperature";
String topic_humidity    = root_topic + "/humidity";
String topic_ipaddress   = root_topic + "/ipaddress";

// ======================================================
// DHT22 Configuration
// ======================================================

#define DHTPIN 4
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// ======================================================
// Object Initialization
// ======================================================

WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);

// ======================================================
// Publish Interval
// ======================================================

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 15000;   // 15 seconds
// ======================================================
// Publish Sensor Data to MQTT Broker
// ======================================================

void publishSensorData()
{
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (!isnan(temperature) && !isnan(humidity))
    {
        client.publish(topic_temperature.c_str(), String(temperature).c_str(), true);
        client.publish(topic_humidity.c_str(), String(humidity).c_str(), true);

        Serial.printf(
            "Temperature : %.2f °C | Humidity : %.2f %%\n",
            temperature,
            humidity
        );
    }
    else
    {
        Serial.println("Failed to read data from DHT22 sensor.");
    }

    String ipAddress = WiFi.localIP().toString();

    client.publish(
        topic_ipaddress.c_str(),
        ipAddress.c_str(),
        true
    );

    Serial.print("IP Address : ");
    Serial.println(ipAddress);
}
// ======================================================
// MQTT Callback Function
// ======================================================

void callback(char* topic, byte* payload, unsigned int length)
{
    // This project only publishes sensor data.
    // No MQTT subscription is implemented.
}

// ======================================================
// Reconnect to MQTT Broker
// ======================================================

void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Connecting to MQTT Broker... ");

        if (client.connect(DEVICE_ID, mqtt_user, mqtt_pass))
        {
            Serial.println("Connected.");
        }
        else
        {
            Serial.print("Failed (Error Code: ");
            Serial.print(client.state());
            Serial.println("). Retrying in 5 seconds...");

            delay(5000);
        }
    }
}

// ======================================================
// Initialize WiFi Connection
// ======================================================

void setupWiFi()
{
    delay(10);

    Serial.println();
    Serial.println("========================================");
    Serial.println("Connecting to WiFi...");
    Serial.println("========================================");

    Serial.print("SSID : ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("----------------------------------------");
    Serial.println("WiFi Connected Successfully");
    Serial.print("IP Address : ");
    Serial.println(WiFi.localIP());
    Serial.println("----------------------------------------");
}
// ======================================================
// Web Dashboard
// ======================================================

void handleRoot()
{
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <title>IoT Server Room Monitoring</title>

    <style>
        body{
            font-family:Arial,Helvetica,sans-serif;
            background:#f4f6f9;
            text-align:center;
            margin-top:40px;
        }

        .card{
            background:white;
            width:350px;
            margin:auto;
            padding:20px;
            border-radius:12px;
            box-shadow:0 3px 10px rgba(0,0,0,0.15);
        }

        h2{
            color:#1f2937;
        }

        p{
            font-size:18px;
        }

        strong{
            color:#2563eb;
        }
    </style>

    <meta http-equiv="refresh" content="5">

</head>

<body>

<div class="card">

<h2>IoT Server Room Monitoring</h2>

)rawliteral";

    html += "<p><strong>Device ID :</strong> " + String(DEVICE_ID) + "</p>";
    html += "<p><strong>IP Address :</strong> " + WiFi.localIP().toString() + "</p>";

    if (!isnan(temperature) && !isnan(humidity))
    {
        html += "<p><strong>Temperature :</strong> " + String(temperature,1) + " &deg;C</p>";
        html += "<p><strong>Humidity :</strong> " + String(humidity,1) + " %</p>";
    }
    else
    {
        html += "<p><strong>Sensor Status :</strong> Failed to read DHT22</p>";
    }

    html += R"rawliteral(

</div>

</body>
</html>

)rawliteral";

    server.send(200, "text/html", html);
}
// ======================================================
// System Initialization
// ======================================================

void setup()
{
    Serial.begin(115200);

    dht.begin();

    setupWiFi();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    server.on("/", handleRoot);
    server.begin();

    Serial.println();
    Serial.println("========================================");
    Serial.println("IoT Server Room Monitoring Started");
    Serial.println("========================================");
}

// ======================================================
// Main Program Loop
// ======================================================

void loop()
{
    // Maintain MQTT Connection
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();

    // Handle HTTP Client Request
    server.handleClient();

    // Publish Sensor Data Periodically
    if (millis() - lastPublishTime >= publishInterval)
    {
        lastPublishTime = millis();

        publishSensorData();
    }
}
