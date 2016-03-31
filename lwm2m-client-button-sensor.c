/**
 * @file
 * LightWeightM2M LWM2M client button application.
 *
 * @author Imagination Technologies
 *
 * @copyright Copyright (c) 2016, Imagination Technologies Limited and/or its affiliated group
 * companies and/or licensors.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to
 *    endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/***************************************************************************************************
 * Includes
 **************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

#include "lib/sensors.h"
#include "button-sensor.h"

#include <awa/static.h>

#include "coap_abstraction.h"

#include "lwm2m-client-flow-object.h"
#include "lwm2m-client-flow-access-object.h"
//#include "lwm2m-client-ipso-digital-input.h"


/***************************************************************************************************
 * Definitions
 **************************************************************************************************/

//! \{
#define COAP_PORT			(6000)
#define IPC_PORT			(12345)
#define BOOTSTRAP_PORT		"15683"
#define END_POINT_NAME		"ButtonDevice"
//! \}

/***************************************************************************************************
 * Implementation
 **************************************************************************************************/

PROCESS(lwm2m_client, "LwM2M Client");
AUTOSTART_PROCESSES(&lwm2m_client);

PROCESS_THREAD(lwm2m_client, ev, data)
{
	PROCESS_BEGIN();

	PROCESS_PAUSE();

	printf("\n\nStarting LWM2M Client for lwm2m-client-button-sensor\n");

#ifdef RF_CHANNEL
	printf("RF channel: %u\n", RF_CHANNEL);
#endif
#ifdef IEEE802154_PANID
	printf("PAN ID: 0x%04X\n", IEEE802154_PANID);
#endif

	printf("uIP buffer: %u\n", UIP_BUFSIZE);
	printf("LL header: %u\n", UIP_LLH_LEN);
	printf("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
#ifdef REST_MAX_CHUNK_SIZE
	printf("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);
#endif

	uip_ipaddr_t ipaddr;
	uip_ip6addr(&ipaddr, BOOTSTRAP_IPv6_ADDR1, BOOTSTRAP_IPv6_ADDR2, BOOTSTRAP_IPv6_ADDR3,
		BOOTSTRAP_IPv6_ADDR4, BOOTSTRAP_IPv6_ADDR5, BOOTSTRAP_IPv6_ADDR6, BOOTSTRAP_IPv6_ADDR7,
		BOOTSTRAP_IPv6_ADDR8);
	uip_ds6_defrt_add(&ipaddr, 0);

	static AwaStaticClient *awaClient;
	awaClient = AwaStaticClient_New();
	AwaStaticClient_SetLogLevel(AwaLogLevel_Error);
	AwaStaticClient_SetEndPointName(awaClient, "ButtonDevice");
	AwaStaticClient_SetCoAPListenAddressPort(awaClient, "0.0.0.0", COAP_PORT);
	AwaStaticClient_SetBootstrapServerURI(awaClient, "coap://["BOOTSTRAP_IPv6_ADDR"]:"BOOTSTRAP_PORT"/");

	AwaError error;
	error = AwaStaticClient_Init(awaClient);
	if (error != AwaError_Success)
	{
		printf("failed to init\n");
	}
	if (!DefineFlowObject(awaClient))
		printf("----> defined flow object\n");
	if (!CreateFlowObject(awaClient))
		printf("----> created flow object\n");
	if (!DefineFlowAccessObject(awaClient))
		printf("----> defined flow access object\n");
	if (!CreateFlowAccessObject(awaClient))
		printf("----> created flow access object\n");

	static struct etimer et;
	static int WaitTime;

	/* Define application-specific events here. */
	while(1)
	{
		WaitTime = AwaStaticClient_Process(awaClient);
		etimer_set(&et, (WaitTime * CLOCK_SECOND) / 1000);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	}

	AwaStaticClient_Free(&awaClient);
	PROCESS_END();
}
//! \}
