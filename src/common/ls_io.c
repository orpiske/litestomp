/**
 *    Copyright 2017 Otavio Rodolfo Piske
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include "ls_io.h"
#include "ls_frame.h"

stomp_status_code_t ls_io_read_frame(ls_connection_t *connection, ls_frame_t *frame, gru_status_t *status) {
	gru_net_socket_t socket = ls_connection_get_socket(connection);

	char buff[1024] = {0};

	int ret = gru_net_recv(&socket, &buff, sizeof(buff));
	fprintf(stderr, "Received: %s\n", buff);

	return STOMP_FAILURE;
}