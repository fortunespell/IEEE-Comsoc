#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <PubSubClient.h>
#include "MAX30100_PulseOximeter.h"


#define REPORTING_PERIOD_MS 1000

const char* ssid = "Realmine";
const char* password = "gaurav127";

const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_topic_heart_rate = "esp32/heartRate";
const char* mqtt_topic_spo2 = "esp32/spo2";

WiFiClient espClient;
PubSubClient client(espClient);
WebServer server(80);

// Create a PulseOximeter object
PulseOximeter pox;

// Time at which the last beat occurred
uint32_t tsLastReport = 0;

String webpageCode;

const int ledPin = 2; // GPIO pin for the onboard LED or an external LED

float heartRateThreshold = 100.0; // Example threshold value
float spo2Threshold = 90.0; // Example threshold value
int ecgThreshold = 500; // Example threshold value

float heartRateData[100]; // Heart rate data array
float spo2Data[100];      // SpO2 data array
int ecgData[100];         // ECG data array

int dataIndex = 0; // Index for cycling through predefined data arrays

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP32Client")) {
      // Successfully connected
    } else {
      delay(5000);
    }
  }
}

void sendData() {
  // Take data from predefined arrays
  float heartRate = heartRateData[dataIndex];
  float spo2 = spo2Data[dataIndex];
  int ecgValue = ecgData[dataIndex];

  // Increment index for cycling through arrays
  dataIndex++;
  if (dataIndex >= sizeof(heartRateData) / sizeof(heartRateData[0])) {
    dataIndex = 0;
  }

  // Check if sensor values exceed thresholds
  if (heartRate > heartRateThreshold || spo2 < spo2Threshold || ecgValue > ecgThreshold) {
    // Trigger emergency popup message
    // Implement emergency message popup using JavaScript
    // You can use window.alert() function to display the message
    // Example: window.alert('Emergency: Sensor value exceeded threshold!');
  }

  String data = "{\"heartRate\":" + String(heartRate) + ",\"spo2\":" + String(spo2) + ",\"ecg\":" + String(ecgValue) + "}";
  server.send(200, "application/json", data);
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up the webpage code
  webpageCode = R"*(
    <!DOCTYPE html>
    <html>
      <head>    
            <title>Team Engineers Health Monitor</title>
            <style>
                body {
                    font-family: "Arial", sans-serif;
                    background-color: #f8f9fa;
                    color: #343a40;
                    text-align: center;
                    padding: 20px;
                }

                h1 {
                    color: #007bff;
                    font-size: 36px;
                    margin-bottom: 20px;
                }

                .container {
                    display: flex;
                    flex-wrap: wrap;
                    justify-content: center;
                    align-items: flex-start;
                    margin-top: 20px;
                }

                .card {
                    background-color: #ffffff;
                    border-radius: 10px;
                    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);
                    margin: 20px;
                    padding: 20px;
                    width: 300px;
                }

                .card h2 {
                    color: #007bff;
                    font-size: 24px;
                    margin-bottom: 10px;
                }

                .card p {
                    color: #6c757d;
                    font-size: 18px;
                }

                .chart-container {
                    margin-top: 20px;
                }
            </style>
      </head>
      <body>
            <h1>Team Engineers Health Monitor</h1>
            <div class="container">
                <div class="card">
                    <h2>Heart Rate</h2>
                    <p id="heartRate">-- bpm</p>
                    <div class="chart-container">
                        <canvas id="heartRateChart"></canvas>
                    </div>
                </div>
                <div class="card">
                    <h2>SpO2</h2>
                    <p id="spo2">-- %</p>
                    <div class="chart-container">
                        <canvas id="spo2Chart"></canvas>
                    </div>
                </div>
                <div class="card">
                    <h2>ECG Value</h2>
                    <p id="ecgValue">--</p>
                    <div class="chart-container">
                        <canvas id="ecgChart"></canvas>
                    </div>
                </div>
            </div>
            <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.9.4/Chart.min.js"></script>
            <script>
                var heartRateChart;
                var spo2Chart;
                var ecgChart;
                var heartRateData = [];
                var spo2Data = [];
                var ecgData = [];
                var labels = [];
                var ctxHeartRate = document.getElementById('heartRateChart').getContext('2d');
                var ctxSpO2 = document.getElementById('spo2Chart').getContext('2d');
                var ctxECG = document.getElementById('ecgChart').getContext('2d');

                heartRateChart = new Chart(ctxHeartRate, {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [{
                            label: 'Heart Rate',
                            data: heartRateData,
                            backgroundColor: 'rgba(255, 99, 132, 0.2)',
                            borderColor: 'rgba(255, 99, 132, 1)',
                            borderWidth: 1
                        }]
                    },
                    options: {
                        scales: {
                            yAxes: [{
                                ticks: {
                                    beginAtZero: true
                                }
                            }]
                        }
                    }
                });

                spo2Chart = new Chart(ctxSpO2, {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [{
                            label: 'SpO2',
                            data: spo2Data,
                            backgroundColor: 'rgba(54, 162, 235, 0.2)',
                            borderColor: 'rgba(54, 162, 235, 1)',
                            borderWidth: 1
                        }]
                    },
                    options: {
                        scales: {
                            yAxes: [{
                                ticks: {
                                    beginAtZero: true
                                }
                            }]
                        }
                    }
                });

                ecgChart = new Chart(ctxECG, {
                    type: 'line',
                    data: {
                        labels: labels,
                        datasets: [{
                            label: 'ECG Value',
                            data: ecgData,
                            backgroundColor: 'rgba(75, 192, 192, 0.2)',
                            borderColor: 'rgba(75, 192, 192, 1)',
                            borderWidth: 1
                        }]
                    },
                    options: {
                        scales: {
                            yAxes: [{
                                ticks: {
                                    beginAtZero: true
                                }
                            }]
                        }
                    }
                });

                function updateData() {
                    fetch('/data')
                        .then(response => response.json())
                        .then(data => {
                            document.getElementById('heartRate').innerText = data.heartRate + ' bpm';
                            document.getElementById('spo2').innerText = data.spo2 + ' %';
                            document.getElementById('ecgValue').innerText = data.ecg;

                            // Update chart data
                            if (heartRateData.length >= 10) {
                                heartRateData.shift();
                                spo2Data.shift();
                                ecgData.shift();
                                labels.shift();
                            }
                            heartRateData.push(data.heartRate);
                            spo2Data.push(data.spo2);
                            ecgData.push(data.ecg);
                            labels.push(new Date().toLocaleTimeString());

                            heartRateChart.data.labels = labels;
                            heartRateChart.data.datasets[0].data = heartRateData;
                            heartRateChart.update();

                            spo2Chart.data.labels = labels;
                            spo2Chart.data.datasets[0].data = spo2Data;
                            spo2Chart.update();

                            ecgChart.data.labels = labels;
                            ecgChart.data.datasets[0].data = ecgData;
                            ecgChart.update();

                            setTimeout(updateData, 1000); // Update every 1 second
                        })
                        .catch(error => console.error('Error:', error));
                }
                updateData(); // Initial call to start updating
            </script>
      </body>
    </html>
  )*";

  server.on("/", []() {
    server.send(200, "text/html", webpageCode);
  });
  server.on("/data", sendData);
  server.begin();

  client.setServer(mqtt_server, 1883);

  if (!pox.begin()) {
    Serial.println("FAILED to initialize Pulse Oximeter");
    for(;;);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
  pox.setOnBeatDetectedCallback([]() {
    // Do nothing on beat detected
  });

  // Generate random data for heart rate, SpO2, and ECG
  randomSeed(analogRead(0)); // Seed the random number generator
  for (int i = 0; i < 100; i++) {
    heartRateData[i] = random(60, 100); // Random heart rate between 60 and 100 bpm
    spo2Data[i] = random(90, 100);      // Random SpO2 between 90% and 100%
    ecgData[i] = random(200, 400);      // Random ECG value between 200 and 400
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  server.handleClient();
  
  pox.update();

  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    float heartRate = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    client.publish(mqtt_topic_heart_rate, String(heartRate).c_str());
    client.publish(mqtt_topic_spo2, String(spo2).c_str());

    tsLastReport = millis(); // Update last report time
  }
}
