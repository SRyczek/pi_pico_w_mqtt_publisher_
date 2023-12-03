
/*
 * MIT License
 * 
 * Copyright (c) 2023 Sebastian Ry
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/* 
 * This code shows an example use and implementation of the mqtt publisher on the 
 * Raspberry Pi Pico W using LWIP, implemented in the Pico SDK. The code sends a 
 * sample message to the specified broker address. 
 * I tested it using mosquitto for ubuntu 20.04. 
 */


#include <string.h>
#include <stdio.h>

#include <string.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/apps/mqtt.h"

#include "mqtt_fun.h"

MQTT_CLIENT_T *mqtt;

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

    /* pass your ssid and password */
    if (cyw43_arch_wifi_connect_timeout_ms("SSID", "PASS", CYW43_AUTH_WPA2_AES_PSK, 30000) != 0) {
        printf("failed to connect\n");
        return 1;
    }

    mqtt = (MQTT_CLIENT_T*)calloc(1, sizeof(MQTT_CLIENT_T));
    mqtt->mqtt_client = mqtt_client_new();  

    /* broker ip */
    ip_addr_t addr;
    int ret = ip4addr_aton("192.168.1.64", &addr);
    if (ret) {
        printf("ip error\n");
        return ret;
    }

    err_t err = mqtt_client_connect(mqtt->mqtt_client,
                                    &addr, MQTT_PORT,
                                    mqtt_connection_cb, mqtt, 
                                    &mqtt_client_info);

    if (err != ERR_OK) {
        printf("Connect error\n");
        return err;
    }

    mqtt_set_inpub_callback(mqtt->mqtt_client,
                            mqtt_incoming_publish_cb,
                            mqtt_incoming_data_cb,
                            0);

    /* config */
  u8_t qos = 2; /* 0 1 or 2 */
  u8_t retain = 0;
  char buffer[128];
  char topic[20];
  /* message to broker 
    (can be changed according to requirements, e.g. for reading sensor values) */
  sprintf(buffer, "hello from pico\n"); 
  sprintf(topic, "test");

    while (1) {
      cyw43_arch_poll();

      cyw43_arch_lwip_begin();
      mqtt_publish(mqtt->mqtt_client, 
                   topic, buffer, 
                   strlen(buffer), 
                   qos, retain,  
                   mqtt_sub_request_cb, 
                   mqtt);

      cyw43_arch_lwip_end();
      sleep_ms(3000);
    }
}