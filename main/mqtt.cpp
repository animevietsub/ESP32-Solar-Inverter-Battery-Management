#include <mqtt.h>
#include <logger.h>

static const char *TAG = "[MQTT]";
static esp_mqtt_client_handle_t client;
static uint8_t mqttStatus = MQTT_EVENT_DISCONNECTED;
information_logger_t *__informationLogger;

static void mqtt_EventHandler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        mqttStatus = MQTT_EVENT_CONNECTED;
        esp_mqtt_client_subscribe(client, "downlink/utc/all/json", 1);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        mqttStatus = MQTT_EVENT_DISCONNECTED;
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        if (strstr(event->topic, "downlink/utc/all/json") != NULL)
        {
            char *start_point = (char *)(strstr(event->data, "{\"time\":") + strlen("{\"time\":"));
            char *end_point = strstr(start_point, ",");
            *(end_point) = '\0';
            // ESP_LOGI(TAG, "%s", start_point);
            __informationLogger->globalRealTime = (atoll(start_point) / 1000) + 7 * 60 * 60;
            // ESP_LOGI(TAG, "%lld", __informationLogger->globalRealTime);
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_Init()
{
    logger_GetInformation(&__informationLogger);
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = BROKER_URI,
        .port = 1883,
        .username = "device",
        .password = AUTH_TOKEN,
        .keepalive = 45,
    };
    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, MQTT_EVENT_ANY, mqtt_EventHandler, NULL);
}

void mqtt_Start(void *pvParameter)
{
    esp_mqtt_client_stop(client);
    esp_mqtt_client_start(client);
}

void mqtt_Publish(const char *topic, const char *data)
{
    if (mqttStatus == MQTT_EVENT_CONNECTED)
            esp_mqtt_client_publish(client, topic, data, 0, 1, 0);
}