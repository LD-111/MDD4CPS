// CPC ID: tc0aXw5la06j1g5yClUv-2
// Parent ID: tc0aXw5la06j1g5yClUv-15
// Name: Soil Moisture Actuator Component
// Description: Involucra un módulo que utiliza un actuador de válvula controlada por un servo motor y cuya lógica de control depende del valor de humedad monitoreado  por otro nodo de la red y de la especie configurada para la planta.

#include <WiFiNINA.h>
#include <ArduinoMqttClient.h>
#include <FreeRTOS_SAMD21.h>
#include <task.h>
#include "secrets.h"
#include "comm_utils.h"
#include <Arduino_JSON.h>
#include <ArduinoJson.h>

// Global variables for WiFi and MQTT connectivity
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
bool debug = true;

// MQTT topics for this CPC ({CPS_id}/{CPC_id}/{comm_thread_id})

// Listener Topics(Receiver)
const char* soilMoistureData_ListenerThread_topic = "ehetnx6obnygbtiqlszm_greenhouseMonitoringSystem/cvocu7cyytcjj8tokeac_32/tc0axw5la06j1g5ycluv_50_comm_thread/dependum";

// Thread Status variables
bool maintainSoilMoistureLevels_GoalAchieved = false; // Global variable for thread maintainSoilMoistureLevels(ID: tc0aXw5la06j1g5yClUv-3)
TaskHandle_t TaskmaintainSoilMoistureLevels;
bool optimizeResources_GoalAchieved = false; // Global variable for thread optimizeResources(ID: tc0aXw5la06j1g5yClUv-17)
TaskHandle_t TaskoptimizeResources;

// Function output variables
double analyzeBatteryUsage_energyLevel = 0.0; // Nivel de energía de la batería del módulo.
double checkSoilMoistureLevels_soilMoisture = 0.0; // Nivel de humedad de la planta correspondiente.

// Global Operation Mode Variables
int maintainSoilMoistureLevels_operation_mode = 0; // Initial operation mode: Bajo consumo

// Global Data Structures (software resources and/or any dependum)
struct soilMoistureData_ListenerThread_data_structure {
    double soilMoisture; // Nivel de humedad de la planta correspondiente.
} soilMoistureData_ListenerThread_data_structure;


// Listener Thread Handles
TaskHandle_t TaskreceiveDependum_soilMoistureData_ListenerThread;

void setup() {
    if (debug) {
        Serial.begin(9600);
        while (!Serial);
    }
    connectToWiFi(debug);
    mqttClient.setId("CPC_Client");

    // Listener Topics(Receiver)
    connectToMQTT(soilMoistureData_ListenerThread_topic, debug);
    
    // Create tasks for the operational goals
    xTaskCreate(
        maintainSoilMoistureLevelsTask,        // Function to implement the task
        "maintainSoilMoistureLevelsTask",      // Name of the task
        512,                      // Stack size (in words, not bytes)
        NULL,                     // Task input parameter
        1,                        // Priority of the task
        &TaskmaintainSoilMoistureLevels        // Task handle
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
        receiveDependum_soilMoistureData_ListenerThreadTask,        // Function to implement the task
        "receiveDependum_soilMoistureData_ListenerThreadTask",      // Name of the task
        512,                      // Stack size (in words, not bytes)
        NULL,                     // Task input parameter
        1,                        // Priority of the task
        &TaskreceiveDependum_soilMoistureData_ListenerThread        // Task handle
    );

    // Start the threads
    vTaskStartScheduler();
}


void receiveDependum_soilMoistureData_ListenerThreadTask(void *pvParameters) {

        //
        // --- Listener Thread Information ---
        // Name: Soil Moisture Data - Listener Thread
        // ID: tc0aXw5la06j1g5yClUv-50-listener_thread
        // Description: This Listener Thread is responsible for receiving the dependum: soilMoistureData_ListenerThread_data_structure
        //
        // Original Element in PIM: Soil Moisture Data - Listener Thread
        // Transformed To: Function `soilMoistureData_ListenerThread()`
        //
        // Note for Developers:
        // The `dependum` data should be stored on and then retrieved from the function checkSoilMoistureLevels().
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
    for (;;) {
        // Maintain the MQTT connection
        mqttClient.poll();

        // Check if there is a new message on the subscribed topic
        if (mqttClient.parseMessage()) {
            // Ensure the message is on the correct topic
            if (mqttClient.messageTopic() == soilMoistureData_ListenerThread_topic) {
                // Parse the incoming JSON message
                StaticJsonDocument<200> doc;
                DeserializationError error = deserializeJson(doc, mqttClient);

                if (!error) {
                    soilMoistureData_ListenerThread_data_structure.soilMoisture = doc["soilMoisture"];

                    // Debug output to Serial
                    Serial.println("Dependum data received:");
                    if (debug) {
                    Serial.print("soilMoisture: ");
                    Serial.println(soilMoistureData_ListenerThread_data_structure.soilMoisture);
                    }
                } else {
                    Serial.println("Error parsing JSON message");
                }
            }
        }


        // Your custom code to process the dependum can go here

        vTaskDelay(xDelay); // Reduce polling frequency to avoid overloading the loop
    }
}

void loop() {

    // Let FreeRTOS manage tasks, nothing to do here
    vTaskDelay(100);
}

bool changeSoilMoisture(double soilMoistureReferenceValue, int changeSoilMoisture_operation_mode = 0) {
    // Function ID: tc0aXw5la06j1g5yClUv-4
    // Parent ID: tc0aXw5la06j1g5yClUv-4
    // Input Parameters:
        // Soil Moisture Reference Value(double) - Nivel de humedad al cual se debería llegar.
    // Output Parameters:
    // Qualification Array:
    //  * None specified.
    // Contribution Array:
    //  * - "[{ "softgoal_id": "tc0aXw5la06j1g5yClUv-5", "name": "Resource Efficiency", "contribution": "hurt"}]"
    // Hardware Resource Assigned:
        // Plant Waterer:
        //     ID: tc0aXw5la06j1g5yClUv-20
        //     Parent ID: tc0aXw5la06j1g5yClUv-20
        //     Description: Este dispositivo corresponde a una válvula de agua controlada por un servo  motor e 180 grados, para lo cual se debe incluir la librería correspondiente(servo.h).
        // Plant Configuration Pin:
        //     ID: U5gbfCX47OS8Pz70eV5W-3
        //     Parent ID: U5gbfCX47OS8Pz70eV5W-3
        //     Description: Se utilizará un pin digital para seleccionar la especie en cultivo. 0 es  para tomate, y 1 para lechuga. Se debe implementar el conexionado  mediante dip switch para la selección.
    // Set output parameters
 

    // --- Your code goes here ---
    
    
    switch (changeSoilMoisture_operation_mode) {
        case 0: // Bajo consumo - Dependiendo de la planta configurada, se va accionar el  riego cuando el nivel de humedad se encuentre a un 20% inferior al nivel  promedio establecido para la especie, llegando al nivel promedio de  humedad.
            // Your logic for Bajo consumo goes here
            break;
        case 1: // Alto Consumo - Dependiendo de la planta configurada, se va accionar el  riego cuando el nivel de humedad se encuentre a un 10% inferior al nivel  promdio establecido para la especie, de manera que se llegue al  promedio del rango óptimo de humedad.
            // Your logic for Alto Consumo goes here
            break;
        default:
            // Handle undefined operation modes
            break;
    }

    
    return true;

    // --- Your code goes here ---
}

bool analyzeBatteryUsage() {
    // Function ID: tc0aXw5la06j1g5yClUv-7
    // Parent ID: tc0aXw5la06j1g5yClUv-7
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

bool checkSoilMoistureLevels() {
    // Function ID: tc0aXw5la06j1g5yClUv-9
    // Parent ID: tc0aXw5la06j1g5yClUv-9
    // Input Parameters:
    // Output Parameters:
        // Soil Moisture(double) - Nivel de humedad de la planta correspondiente.
    // Qualification Array:
    //  * None specified.
    // Contribution Array:
    //  * None specified.
    // Hardware Resource Assigned:
    // Set output parameters
    checkSoilMoistureLevels_soilMoisture = 0.0; // Nivel de humedad de la planta correspondiente.


    // --- Your code goes here ---
    
    
    
    return true;

    // --- Your code goes here ---
}

bool changeEnergyMode(double energyLevel) {
    // Function ID: tc0aXw5la06j1g5yClUv-6
    // Parent ID: tc0aXw5la06j1g5yClUv-6
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

// Task for maintainSoilMoistureLevels
void maintainSoilMoistureLevelsTask(void *pvParameters) {
    // This variable handles the period in milliseconds for thread execution
    const TickType_t xDelay = pdMS_TO_TICKS(10000); // Example interval for maintainSoilMoistureLevels

    // --- maintainSoilMoistureLevels Context Information ---
    // ID: tc0aXw5la06j1g5yClUv-3
    // ID CIM Parent: tc0aXw5la06j1g5yClUv-3
    // qualification_array: 
    //  * None specified.
    // contribution_array: 
    //  * None specified.
    // ----------------------------------------------------------

    for (;;) {
        // --- Your code goes here ---
        // Evaluate the state of maintainSoilMoistureLevels

        double soilMoistureReferenceValue = 0.0;
        maintainSoilMoistureLevels_GoalAchieved = (checkSoilMoistureLevels() && changeSoilMoisture(soilMoistureReferenceValue));
        
        switch (maintainSoilMoistureLevels_operation_mode) {
            case 0: // Bajo consumo - Dependiendo de la planta configurada, se va accionar el  riego cuando el nivel de humedad se encuentre a un 20% inferior al nivel  promedio establecido para la especie, llegando al nivel promedio de  humedad.
                // Your logic for Bajo consumo goes here
                break;
            case 1: // Alto Consumo - Dependiendo de la planta configurada, se va accionar el  riego cuando el nivel de humedad se encuentre a un 10% inferior al nivel  promdio establecido para la especie, de manera que se llegue al  promedio del rango óptimo de humedad.
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
    // ID: tc0aXw5la06j1g5yClUv-17
    // ID CIM Parent: tc0aXw5la06j1g5yClUv-17
    // qualification_array: 
    //  * None specified.
    // contribution_array: 
    //  * - "[{ "softgoal_id": "tc0aXw5la06j1g5yClUv-5", "name": "Resource Efficiency", "contribution": "help"}]"
    // ----------------------------------------------------------

    for (;;) {
        // --- Your code goes here ---
        // Evaluate the state of optimizeResources

        double energyLevel = 0.0;
        optimizeResources_GoalAchieved = (analyzeBatteryUsage() && changeEnergyMode(energyLevel));
        
        // --- Your code ends here ---

        vTaskDelay(xDelay);
    }
}
