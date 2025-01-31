// CPC ID: tc0aXw5la06j1g5yClUv-24
// Parent ID: tc0aXw5la06j1g5yClUv-37
// Name: Temperature Actuator Component
// Description: Involucra un módulo que utiliza un actuador de calefactor para el  invernadero(accionado a través de una señal digital) y cuya lógica de  control depende del valor de temperatura del invernadero monitoreado por otro nodo de la red.

#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <FreeRTOS_SAMD21.h>
#include <task.h>
#include "secrets.h"
#include "comm_utils.h"
#include <Arduino_JSON.h>
#include <ArduinoJson.h>

// Global variables for WiFi and MQTT connectivity
const char* temperatureData_ListenerThreadClientId = "temperatureActuatorComponentClient_tc0axw5la06j1g5ycluv_24";
WiFiClient temperatureData_ListenerThreadClient;
PubSubClient temperatureData_ListenerThreadMqttClient(temperatureData_ListenerThreadClient);

bool debug = true;

// MQTT topics for this CPC ({CPS_id}/{CPC_id}/{comm_thread_id})

// Listener Topics(Receiver)
const char* temperatureData_ListenerThread_topic = "ehetnx6obnygbtiqlszm_greenhouseMonitoringSystem/cvocu7cyytcjj8tokeac_5/tc0axw5la06j1g5ycluv_51_comm_thread/dependum";

// Thread Status variables
bool maintainGreenhouseTemperature_GoalAchieved = false; // Global variable for thread maintainGreenhouseTemperature(ID: tc0aXw5la06j1g5yClUv-25)
TaskHandle_t TaskmaintainGreenhouseTemperature;
bool optimizeResources_GoalAchieved = false; // Global variable for thread optimizeResources(ID: tc0aXw5la06j1g5yClUv-39)
TaskHandle_t TaskoptimizeResources;

// Function output variables
double checkGreenhouseTemperature_greenhouseTemp = 0.0; // Temperatura ambiental del invernadero.
double analyzeBatteryUsage_energyLevel = 0.0; // Nivel de energía de la batería del módulo.

// Global Operation Mode Variables
int maintainGreenhouseTemperature_operation_mode = 0; // Initial operation mode: Bajo consumo

// Global Data Structures (software resources and/or any dependum)
struct temperatureData_ListenerThread_data_structure {
    double greenhouseTemp; // Temperatura ambiental del invernadero.
} temperatureData_ListenerThread_data_structure;


// Listener Thread Handles
TaskHandle_t TaskreceiveDependum_temperatureData_ListenerThread;

void setup() {
    if (debug) {
        Serial.begin(9600);
        while (!Serial);
    }
    connectToWiFi();

    // Listener Topics(Receiver)
    mqttSetup(temperatureData_ListenerThreadMqttClient, callback_temperatureData_ListenerThread);
    connectToMQTT(temperatureData_ListenerThreadMqttClient, temperatureData_ListenerThreadClientId, temperatureData_ListenerThread_topic);
    
    // Create tasks for the operational goals
    xTaskCreate(
        maintainGreenhouseTemperatureTask,        // Function to implement the task
        "maintainGreenhouseTemperatureTask",      // Name of the task
        512,                      // Stack size (in words, not bytes)
        NULL,                     // Task input parameter
        1,                        // Priority of the task
        &TaskmaintainGreenhouseTemperature        // Task handle
    );
    xTaskCreate(
        optimizeResourcesTask,        // Function to implement the task
        "optimizeResourcesTask",      // Name of the task
        512,                      // Stack size (in words, not bytes)
        NULL,                     // Task input parameter
        1,                        // Priority of the task
        &TaskoptimizeResources        // Task handle
    );
    xTaskCreate(
        receiveDependum_temperatureData_ListenerThreadTask,        // Function to implement the task
        "receiveDependum_temperatureData_ListenerThreadTask",      // Name of the task
        512,                      // Stack size (in words, not bytes)
        NULL,                     // Task input parameter
        1,                        // Priority of the task
        &TaskreceiveDependum_temperatureData_ListenerThread        // Task handle
    );

    // Start the threads
    vTaskStartScheduler();
}


void callback_temperatureData_ListenerThread(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message on Topic 1: ");
    Serial.print(topic);
    Serial.println(" - ");
    // Parse the incoming JSON message
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (!error) {
        temperatureData_ListenerThread_data_structure.greenhouseTemp = doc["greenhouseTemp"];

        // Debug output to Serial
        Serial.println("Dependum data received:");
        if (debug) {
                    Serial.print("greenhouseTemp: ");
                    Serial.println(temperatureData_ListenerThread_data_structure.greenhouseTemp);
        }
    } else {
        Serial.println("Error parsing JSON message");
    }
    // Your custom code to process the dependum can go here



}




void receiveDependum_temperatureData_ListenerThreadTask(void *pvParameters) {

        //
        // --- Listener Thread Information ---
        // Name: Temperature Data - Listener Thread
        // ID: tc0aXw5la06j1g5yClUv-51-listener_thread
        // Description: This Listener Thread is responsible for receiving the dependum: temperatureData_ListenerThread_data_structure
        //
        // Original Element in PIM: Temperature Data - Listener Thread
        // Transformed To: Function `temperatureData_ListenerThread()`
        //
        // Note for Developers:
        // The `dependum` data should be stored on and then retrieved from the function checkGreenhouseTemperature().
        // Ensure the function completes its operation and receives the required data 
        // in the appropriate format for reception.
        // 
        // Qualification Array:
        //  * None specified.
        // 
        // Contribution Array:
        //  * None specified.
        // 
        //
    // This variable handles the period in milliseconds for thread execution
    const TickType_t xDelay = pdMS_TO_TICKS(10000);
    

    for (;;)
    {

        // Check MQTT connection status
        if (!temperatureData_ListenerThreadMqttClient.connected())
        {
          connectToMQTT(temperatureData_ListenerThreadMqttClient, temperatureData_ListenerThreadClientId, temperatureData_ListenerThread_topic);
        }

        // Always poll MQTT for new messages
        temperatureData_ListenerThreadMqttClient.loop();

        // Keep delay to avoid overloading loop but keep polling
        vTaskDelay(xDelay);
    }
}

void loop() {

    // Let FreeRTOS manage tasks, nothing to do here
    delay(100);
}

bool changeGreenhouseTemperature(double temperatureReferenceValue, int changeGreenhouseTemperature_operation_mode = 0) {
    // Function ID: tc0aXw5la06j1g5yClUv-26
    // Parent ID: tc0aXw5la06j1g5yClUv-26
    // Input Parameters:
        // Temperature Reference Value(double) - Temperatura a la cual se debería llegar.
    // Output Parameters:
    // Qualification Array:
    //  * None specified.
    // Contribution Array:
    //  * - "[{ "softgoal_id": "tc0aXw5la06j1g5yClUv-27", "name": "Resource Efficiency", "contribution": "hurt"}]"
    // Hardware Resource Assigned:
        // Greenhouse Heater:
        //     ID: tc0aXw5la06j1g5yClUv-42
        //     Parent ID: tc0aXw5la06j1g5yClUv-42
        //     Description: Este dispositivo corresponde a un calefactor que es controlado mediante un  relé via señal digital desde el CPC. Se debe implementar el conexionado  mediante protoboard.
    // Set output parameters
 

    // --- Your code goes here ---
    
    
    switch (changeGreenhouseTemperature_operation_mode) {
        case 0: // Bajo consumo - Se va accionar el calefactor cuando la temperatura del  invernadero disminuya un 10% del promedio de temperatura ideal, de  manera que se produzcan menos accionamientos en un periodo dado.
            // Your logic for Bajo consumo goes here
            break;
        case 1: // Alto Consumo - Se va accionar el calefactor cuando la temperatura del  invernadero disminuya un 1% del promedio de temperatura ideal, de manera  que se produzcan más accionamientos en un periodo dado para una  regulación de temperatura más precisa y estable.
            // Your logic for Alto Consumo goes here
            break;
        default:
            // Handle undefined operation modes
            break;
    }

    
    return true;

    // --- Your code goes here ---
}

bool changeEnergyMode(double energyLevel) {
    // Function ID: tc0aXw5la06j1g5yClUv-28
    // Parent ID: tc0aXw5la06j1g5yClUv-28
    // Input Parameters:
        // Energy Level(double) - Nivel de energía de la batería del módulo.
    // Output Parameters:
    // Qualification Array:
    //  * None specified.
    // Contribution Array:
    //  * None specified.
    // Hardware Resource Assigned:
    // Set output parameters
 

    // --- Your code goes here ---
    
    
    
    return true;

    // --- Your code goes here ---
}

bool checkGreenhouseTemperature() {
    // Function ID: tc0aXw5la06j1g5yClUv-31
    // Parent ID: tc0aXw5la06j1g5yClUv-31
    // Input Parameters:
    // Output Parameters:
        // Greenhouse Temp(double) - Temperatura ambiental del invernadero.
    // Qualification Array:
    //  * None specified.
    // Contribution Array:
    //  * None specified.
    // Hardware Resource Assigned:
    // Set output parameters
    checkGreenhouseTemperature_greenhouseTemp = 0.0; // Temperatura ambiental del invernadero.


    // --- Your code goes here ---
    
    
    
    return true;

    // --- Your code goes here ---
}

bool analyzeBatteryUsage() {
    // Function ID: tc0aXw5la06j1g5yClUv-29
    // Parent ID: tc0aXw5la06j1g5yClUv-29
    // Input Parameters:
    // Output Parameters:
        // Energy Level(double) - Nivel de energía de la batería del módulo.
    // Qualification Array:
    //  * None specified.
    // Contribution Array:
    //  * None specified.
    // Hardware Resource Assigned:
    // Set output parameters
    analyzeBatteryUsage_energyLevel = 0.0; // Nivel de energía de la batería del módulo.


    // --- Your code goes here ---
    
    
    
    return true;

    // --- Your code goes here ---
}

// Task for maintainGreenhouseTemperature
void maintainGreenhouseTemperatureTask(void *pvParameters) {
    // This variable handles the period in milliseconds for thread execution
    const TickType_t xDelay = pdMS_TO_TICKS(10000); // Example interval for maintainGreenhouseTemperature

    // --- maintainGreenhouseTemperature Context Information ---
    // ID: tc0aXw5la06j1g5yClUv-25
    // ID CIM Parent: tc0aXw5la06j1g5yClUv-25
    // qualification_array: 
    //  * None specified.
    // contribution_array: 
    //  * None specified.
    // ----------------------------------------------------------

    for (;;) {
        // --- Your code goes here ---
        // Evaluate the state of maintainGreenhouseTemperature

        double temperatureReferenceValue = 0.0;
        maintainGreenhouseTemperature_GoalAchieved = (checkGreenhouseTemperature() && changeGreenhouseTemperature(temperatureReferenceValue));
        
        switch (maintainGreenhouseTemperature_operation_mode) {
            case 0: // Bajo consumo - Se va accionar el calefactor cuando la temperatura del  invernadero disminuya un 10% del promedio de temperatura ideal, de  manera que se produzcan menos accionamientos en un periodo dado.
                // Your logic for Bajo consumo goes here
                break;
            case 1: // Alto Consumo - Se va accionar el calefactor cuando la temperatura del  invernadero disminuya un 1% del promedio de temperatura ideal, de manera  que se produzcan más accionamientos en un periodo dado para una  regulación de temperatura más precisa y estable.
                // Your logic for Alto Consumo goes here
                break;
            default:
                // Handle undefined operation modes
                break;
        }

        // --- Your code ends here ---

        vTaskDelay(xDelay);
    }
}

// Task for optimizeResources
void optimizeResourcesTask(void *pvParameters) {
    // This variable handles the period in milliseconds for thread execution
    const TickType_t xDelay = pdMS_TO_TICKS(10000); // Example interval for optimizeResources

    // --- optimizeResources Context Information ---
    // ID: tc0aXw5la06j1g5yClUv-39
    // ID CIM Parent: tc0aXw5la06j1g5yClUv-39
    // qualification_array: 
    //  * None specified.
    // contribution_array: 
    //  * - "[{ "softgoal_id": "tc0aXw5la06j1g5yClUv-27", "name": "Resource Efficiency", "contribution": "help"}]"
    // ----------------------------------------------------------

    for (;;) {
        // --- Your code goes here ---
        // Evaluate the state of optimizeResources

        double energyLevel = 0.0;
        optimizeResources_GoalAchieved = (changeEnergyMode(energyLevel) && analyzeBatteryUsage());
        
        // --- Your code ends here ---

        vTaskDelay(xDelay);
    }
}
