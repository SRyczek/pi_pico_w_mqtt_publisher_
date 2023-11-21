
#include "lwip/apps/mqtt.h"

static const struct mqtt_connect_client_info_t mqtt_client_info =
{
    "test",
    NULL, /* user */
    NULL, /* pass */
    100,  /* keep alive */
    NULL, /* will_topic */
    NULL, /* will_msg */
    0,    /* will_qos */
    0     /* will_retain */
    #if LWIP_ALTCP && LWIP_ALTCP_TLS
    , NULL
    #endif
};

typedef struct MQTT_CLIENT_T {
    ip_addr_t remote_addr;
    mqtt_client_t *mqtt_client;
    u32_t received;
    u32_t counter;
} MQTT_CLIENT_T;

void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags);
void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len);
void mqtt_request_cb(void *arg, err_t err);
void mqtt_sub_request_cb(void *arg, err_t err);
void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status);