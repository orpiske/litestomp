/**
 *
 * Copyright 2005 LogicBlaze Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <stdlib.h>
#include "stomp_engine.h"

int die(int exitCode, const char *message, apr_status_t reason)
{
    char msgbuf[80];
    apr_strerror(reason, msgbuf, sizeof (msgbuf));
    fprintf(stderr, "%s: %s (%d)\n", message, msgbuf, reason);
    exit(exitCode);
    return reason;
}

static void terminate()
{
    apr_terminate();
}

static void check_status(apr_status_t rc, const char *message) {
    if (rc != APR_SUCCESS) {
        die(-2, message, rc);
    }
}

int main(int argc, char *argv[])
{
    apr_status_t rc;
    apr_pool_t *pool;
    stomp_connection *connection;

    setbuf(stdout, NULL);

    rc = apr_initialize();
    check_status(rc, "Could not initialize");
    atexit(terminate);

    rc = apr_pool_create(&pool, NULL);
    check_status(rc, "Could not allocate pool");
    

    fprintf(stdout, "Connecting......");
    rc = stomp_engine_connect(&connection, "localhost", 61613, pool);
    check_status(rc, "Could not connect");
    fprintf(stdout, "OK\n");

    fprintf(stdout, "Sending connect message.");
    {
        stomp_frame frame;
        frame.command = "CONNECT";
        frame.headers = apr_hash_make(pool);
        // apr_hash_set(frame.headers, "login", APR_HASH_KEY_STRING, "<username>");
        // apr_hash_set(frame.headers, "passcode", APR_HASH_KEY_STRING, "<password>");
        frame.body = NULL;
        frame.body_length = -1;
        rc = stomp_write(connection, &frame, pool);
        check_status(rc, "Could not send frame");
    }
    fprintf(stdout, "OK\n");
    fprintf(stdout, "Reading Response.");
    {
        stomp_frame *frame;
        rc = stomp_read(connection, &frame, pool);
        check_status(rc, "Could not read frame");
        fprintf(stdout, "Response: %s, %s\n", frame->command, frame->body);
    }
    fprintf(stdout, "OK\n");
    

    fprintf(stdout, "Sending Subscribe.");
    {
        stomp_frame frame;
        frame.command = "SUB";
        frame.headers = apr_hash_make(pool);
        apr_hash_set(frame.headers, "destination", APR_HASH_KEY_STRING, "/queue/FOO.BAR");
        frame.body_length = -1;
        frame.body = NULL;
        rc = stomp_write(connection, &frame, pool);
        check_status(rc, "Could not send frame");
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "Reading Response.");
    {
        stomp_frame *frame;
        rc = stomp_read(connection, &frame, pool);
        check_status(rc, "Could not read frame");
        fprintf(stdout, "Response: %s, %s\n", frame->command, frame->body);
        fprintf(stdout, "Body: %s", frame->body);
    }
    fprintf(stdout, "OK\n");


    fprintf(stdout, "Sending Disconnect.");
    {
        stomp_frame frame;
        frame.command = "DISCONNECT";
        frame.headers = NULL;
        frame.body_length = -1;
        frame.body = NULL;
        rc = stomp_write(connection, &frame, pool);
        check_status(rc, "Could not send frame");
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "Disconnecting...");
    rc = stomp_engine_disconnect(&connection);
    check_status(rc, "Could not disconnect");
    fprintf(stdout, "OK\n");

    apr_pool_destroy(pool);
    return 0;
}
