/**
 Copyright 2016 Otavio Rodolfo Piske
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
#ifndef STOMP_H_
#define STOMP_H_

#include "stomp_status.h"
#include "stomp_connection.h"
#include "stomp_frame.h"
#include "stomp_io.h"


typedef struct stomp_messenger_t_ {
    stomp_connection *connection;
    stomp_status_t status;
    apr_pool_t *pool;
} stomp_messenger_t;

stomp_messenger_t *stomp_messenger_init();
void stomp_messenger_destroy(stomp_messenger_t **messenger);

stomp_status_code_t stomp_connect(stomp_messenger_t *messenger, 
                                  stomp_connection_header_t *header);

// stomp_status_t stomp_connect();

#endif /* STOMP_H_ */

