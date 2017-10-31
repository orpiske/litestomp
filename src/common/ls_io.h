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
#ifndef LITESTOMP_LS_IO_H
#define LITESTOMP_LS_IO_H

#include <common/gru_status.h>
#include <log/gru_logger.h>

#include "stomp_status.h"
#include "ls_connection.h"
#include "ls_frame.h"

#ifdef __cplusplus
extern "C" {
#endif

stomp_status_code_t ls_io_read_frame(ls_connection_t *connection, ls_frame_t *frame, gru_status_t *status);
stomp_status_code_t ls_io_write_frame(ls_connection_t *connection, ls_frame_t *frame, gru_status_t *status);

#ifdef __cplusplus
}
#endif

#endif // LITESTOMP_LS_IO_H