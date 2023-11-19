
#include <string.h>
#include <stdio.h>

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/apps/mqtt.h"


/** Define this to a compile-time IP address initialization
 * to connect anything else than IPv4 loopback
 */
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

typedef struct MQTT_CLIENT_T_ {
    ip_addr_t remote_addr;
    mqtt_client_t *mqtt_client;
    u32_t received;
    u32_t counter;
    u32_t reconnect;
} MQTT_CLIENT_T;

MQTT_CLIENT_T *mqtt;

static void mqtt_incoming_data_cb(void *arg, const u8_t *data, u16_t len, u8_t flags)
{
  const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
  LWIP_UNUSED_ARG(data);

  LWIP_PLATFORM_DIAG(("MQTT client \"%s\" data cb: len %d, flags %d\n", client_info->client_id,
          (int)len, (int)flags));
}

static void mqtt_incoming_publish_cb(void *arg, const char *topic, u32_t tot_len)
{
  const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

  LWIP_PLATFORM_DIAG(("MQTT client \"%s\" publish cb: topic %s, len %d\n", client_info->client_id,
          topic, (int)tot_len));
}

static void mqtt_request_cb(void *arg, err_t err)
{
  const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;

  LWIP_PLATFORM_DIAG(("MQTT client \"%s\" request cb: err %d\n", client_info->client_id, (int)err));
}

static void mqtt_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status)
{
  const struct mqtt_connect_client_info_t* client_info = (const struct mqtt_connect_client_info_t*)arg;
  LWIP_UNUSED_ARG(client);

  LWIP_PLATFORM_DIAG(("MQTT client \"%s\" connection cb: status %d\n", client_info->client_id, (int)status));
  printf("mqtt con cb works\n");
  if (status == MQTT_CONNECT_ACCEPTED) {
    mqtt_sub_unsub(client,
            "topic_qos1", 1,
            mqtt_request_cb, LWIP_CONST_CAST(void*, client_info),
            1);
    mqtt_sub_unsub(client,
            "topic_qos0", 0,
            mqtt_request_cb, LWIP_CONST_CAST(void*, client_info),
            1);
  }
}

int main(void)
{

    stdio_init_all();

    for (int i = 0; i < 20; i++) {
        printf("Sleep: %d s\n", i);
        sleep_ms(1000);
    }
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_timeout_ms("multimedia_maj12", "opjkl1327", CYW43_AUTH_WPA2_AES_PSK, 30000) != 0) {
        printf("failed to connect\n");
        return 1;
    }


    mqtt = (MQTT_CLIENT_T*)calloc(1, sizeof(MQTT_CLIENT_T));
  


    ip_addr_t addr;
    if (!ip4addr_aton("192.168.0.20", &addr)) {
        printf("ip error\n");
        return 0;
      }
    mqtt->mqtt_client = mqtt_client_new();

    err_t err = mqtt_client_connect(mqtt->mqtt_client,
                &addr, MQTT_PORT,
                mqtt_connection_cb, mqtt, &mqtt_client_info);
    if(err != ERR_OK) {
        printf("Connect error\n");
        return err;
    }

    mqtt_set_inpub_callback(mqtt->mqtt_client,
            mqtt_incoming_publish_cb,
            mqtt_incoming_data_cb,
            mqtt);

  u8_t qos = 0; /* 0 1 or 2, see MQTT specification.  AWS IoT does not support QoS 2 */
  u8_t retain = 0;
  char buffer[128];
  sprintf(buffer, "hello from pico\n");


    while (1) {
        // cyw43_arch_lwip_begin();
        // mqtt_publish(mqtt->mqtt_client_inst, "pico_w/test", buffer, strlen(buffer), qos, retain, NULL, NULL);
        // cyw43_arch_lwip_end();
        sleep_ms(1000);
    }
}