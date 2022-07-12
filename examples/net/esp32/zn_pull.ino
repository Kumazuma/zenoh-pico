/*
 * Copyright (c) 2017, 2021 ADLINK Technology Inc.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Apache License, Version 2.0
 * which is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0
 *
 * Contributors:
 *   ADLINK zenoh team, <zenoh@adlink-labs.tech>
 */

#include <Arduino.h>
#include <WiFi.h>

extern "C"
{
#include "zenoh-pico.h"
}

#define SSID "SSID"
#define PASS "PASSWORD"

// Zenoh-specific parameters
#define MODE "client"
#define PEER "tcp/10.0.0.1:7447"
#define URI "/demo/example/**"

_z_subscriber_t *sub = NULL;

void data_handler(const _z_sample_t *sample, const void *arg)
{
    (void)(arg); // Unused argument
}

void setup()
{
    // Set WiFi in STA mode and trigger attachment
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASS);

    // Keep trying until connected
    while (WiFi.status() != WL_CONNECTED)
    {
    }
    delay(1000);

    _z_config_t *config = _z_config_default();
    _zp_config_insert(config, Z_CONFIG_MODE_KEY, z_string_make(MODE));
    _zp_config_insert(config, Z_CONFIG_PEER_KEY, z_string_make(PEER));

    _z_session_t *s = _z_open(config);
    if (s == NULL)
    {
        return;
    }

    _zp_start_read_task(s);
    _zp_start_lease_task(s);

    _z_subinfo_t subinfo;
    subinfo.reliability = Z_RELIABILITY_RELIABLE;
    subinfo.mode = Z_SUBMODE_PULL;
    subinfo.period = NULL;
    sub = _z_declare_subscriber(s, _z_rname(URI), subinfo, data_handler, NULL);
    if (sub == NULL)
    {
        return;
    }
}

void loop()
{
    delay(5000);
    if (sub == NULL)
        return;

    _z_pull(sub);
}
