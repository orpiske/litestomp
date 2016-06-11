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
 * 
 * ********************************************************************
 * The contents of this file are part of the original stomp.c and stomp.h file 
 * from which this code is derived
 */

#include "stomp_io.h"

/********************************************************************************
 * 
 * Wrappers around the apr_socket_send and apr_socket_recv calls so that they 
 * read/write their buffers fully.
 *
 ********************************************************************************/
APR_DECLARE(apr_status_t) stomp_write_buffer(stomp_connection *connection, const char *data, apr_size_t size)
{
    apr_size_t remaining = size;
    size = 0;
    while (remaining > 0) {
        apr_size_t length = remaining;
        apr_status_t rc = apr_socket_send(connection->socket, data, &length);
        data += length;
        remaining -= length;
        //      size += length;
        if (rc != APR_SUCCESS) {
            return rc;
        }
    }
    return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) stomp_read_line(stomp_connection *connection, char **data, int* length, apr_pool_t *pool)
{
    apr_pool_t *tpool;
    apr_status_t rc;
    data_block_list *head, *tail;
    apr_size_t i = 0;
    apr_size_t bytesRead = 0;
    char *p;

    rc = apr_pool_create(&tpool, pool);
    if (rc != APR_SUCCESS) {
        return rc;
    }

    head = tail = apr_pcalloc(tpool, sizeof (data_block_list));
    if (head == NULL)
        return APR_ENOMEM;

#define CHECK_SUCCESS if( rc!=APR_SUCCESS ) { apr_pool_destroy(tpool); return rc; }

    while (1) {

        apr_size_t length = 1;
        apr_status_t rc = apr_socket_recv(connection->socket, tail->data + i, &length);
        CHECK_SUCCESS;

        if (length == 1) {
            i++;
            bytesRead++;

            // Keep reading bytes till end of line
            if (tail->data[i - 1] == '\n') {
                // Null terminate the string instead of having the newline
                tail->data[i - 1] = 0;
                break;
            } else if (tail->data[i - 1] == 0) {
                // Encountered 0 before end of line
                apr_pool_destroy(tpool);
                return APR_EGENERAL;
            }

            // Do we need to allocate a new block?
            if (i >= sizeof ( tail->data)) {
                tail->next = apr_pcalloc(tpool, sizeof (data_block_list));
                if (tail->next == NULL) {
                    apr_pool_destroy(tpool);
                    return APR_ENOMEM;
                }
                tail = tail->next;
                i = 0;
            }
        }
    }

#undef CHECK_SUCCESS
    // Now we have the whole frame and know how big it is.  Allocate it's buffer
    *data = apr_pcalloc(pool, bytesRead);
    p = *data;
    if (p == NULL) {
        apr_pool_destroy(tpool);
        return APR_ENOMEM;
    }

    // Copy the frame over to the new buffer.
    *length = bytesRead - 1;
    for (; head != NULL; head = head->next) {
        int len = bytesRead > sizeof (head->data) ? sizeof (head->data) : bytesRead;
        memcpy(p, head->data, len);
        p += len;
        bytesRead -= len;
    }

    apr_pool_destroy(tpool);
    return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) stomp_read_buffer(stomp_connection *connection, char **data, apr_pool_t *pool)
{
    apr_pool_t *tpool;
    apr_status_t rc;
    data_block_list *head, *tail;
    apr_size_t i = 0;
    apr_size_t bytesRead = 0;
    char *p;

    rc = apr_pool_create(&tpool, pool);
    if (rc != APR_SUCCESS) {
        return rc;
    }

    head = tail = apr_pcalloc(tpool, sizeof (data_block_list));
    if (head == NULL)
        return APR_ENOMEM;

#define CHECK_SUCCESS if( rc!=APR_SUCCESS ) { apr_pool_destroy(tpool); return rc; }

    // Keep reading bytes till end of frame is encountered.
    while (1) {

        apr_size_t length = 1;
        apr_status_t rc = apr_socket_recv(connection->socket, tail->data + i, &length);
        CHECK_SUCCESS;

        if (length == 1) {
            i++;
            bytesRead++;

            // Keep reading bytes till end of frame
            if (tail->data[i - 1] == 0) {
                char endline[1];
                // We expect a newline after the null.
                apr_socket_recv(connection->socket, endline, &length);
                CHECK_SUCCESS;
                if (endline[0] != '\n') {
                    return APR_EGENERAL;
                }
                break;
            }

            // Do we need to allocate a new block?
            if (i >= sizeof ( tail->data)) {
                tail->next = apr_pcalloc(tpool, sizeof (data_block_list));
                if (tail->next == NULL) {
                    apr_pool_destroy(tpool);
                    return APR_ENOMEM;
                }
                tail = tail->next;
                i = 0;
            }
        }
    }
#undef CHECK_SUCCESS

    // Now we have the whole frame and know how big it is.  Allocate it's buffer
    *data = apr_pcalloc(pool, bytesRead);
    p = *data;
    if (p == NULL) {
        apr_pool_destroy(tpool);
        return APR_ENOMEM;
    }

    // Copy the frame over to the new buffer.
    for (; head != NULL; head = head->next) {
        int len = bytesRead > sizeof (head->data) ? sizeof (head->data) : bytesRead;
        memcpy(p, head->data, len);
        p += len;
        bytesRead -= len;
    }

    apr_pool_destroy(tpool);
    return APR_SUCCESS;
}

/********************************************************************************
 * 
 * Handles reading and writing stomp_frames to and from the connection
 *
 ********************************************************************************/

APR_DECLARE(apr_status_t) stomp_write(stomp_connection *connection, stomp_frame *frame, apr_pool_t* pool)
{
    apr_status_t rc;

#define CHECK_SUCCESS if( rc!=APR_SUCCESS ) { return rc; }
    // Write the command.
    rc = stomp_write_buffer(connection, frame->command, strlen(frame->command));
    CHECK_SUCCESS;
    rc = stomp_write_buffer(connection, "\n", 1);
    CHECK_SUCCESS;

    // Write the headers
    if (frame->headers != NULL) {

        apr_hash_index_t *i;
        const void *key;
        void *value;
        for (i = apr_hash_first(NULL, frame->headers); i; i = apr_hash_next(i)) {
            apr_hash_this(i, &key, NULL, &value);

            rc = stomp_write_buffer(connection, key, strlen(key));
            CHECK_SUCCESS;
            rc = stomp_write_buffer(connection, ":", 1);
            CHECK_SUCCESS;
            rc = stomp_write_buffer(connection, value, strlen(value));
            CHECK_SUCCESS;
            rc = stomp_write_buffer(connection, "\n", 1);
            CHECK_SUCCESS;
        }

        if (frame->body_length >= 0) {
            apr_pool_t *length_pool;
            char *length_string;

            apr_pool_create(&length_pool, pool);
            rc = stomp_write_buffer(connection, "content-length:", 15);
            CHECK_SUCCESS;

            int body_length = frame->body_length;
            if (body_length <= 0 && frame->body != NULL)
                body_length = strlen(frame->body) + 1;
            length_string = apr_itoa(length_pool, body_length);
            rc = stomp_write_buffer(connection, length_string, strlen(length_string));
            CHECK_SUCCESS;
            rc = stomp_write_buffer(connection, "\n", 1);
            CHECK_SUCCESS;

            apr_pool_destroy(length_pool);
        }
    }
    rc = stomp_write_buffer(connection, "\n", 1);
    CHECK_SUCCESS;

    // Write the body.
    if (frame->body != NULL) {
        int body_length = frame->body_length;
        if (body_length <= 0)
            body_length = strlen(frame->body) + 1;
        rc = stomp_write_buffer(connection, frame->body, body_length);
        CHECK_SUCCESS;
    }
    rc = stomp_write_buffer(connection, "\0\n", 2);
    CHECK_SUCCESS;

#undef CHECK_SUCCESS

    return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) stomp_read(stomp_connection *connection, stomp_frame **frame, apr_pool_t *pool)
{

    apr_status_t rc;
    stomp_frame *f;

    f = apr_pcalloc(pool, sizeof (stomp_frame));
    if (f == NULL)
        return APR_ENOMEM;

    f->headers = apr_hash_make(pool);
    if (f->headers == NULL)
        return APR_ENOMEM;

#define CHECK_SUCCESS if( rc!=APR_SUCCESS ) { return rc; }

    // Parse the frame out.
    {
        char *p;
        int length;

        // Parse the command.
        rc = stomp_read_line(connection, &p, &length, pool);
        CHECK_SUCCESS;

        f->command = p;

        // Start parsing the headers.
        while (1) {
            rc = stomp_read_line(connection, &p, &length, pool);
            CHECK_SUCCESS;

            // Done with headers
            if (length == 0)
                break;

            {
                // Parse the header line.
                char *p2;
                void *key;
                void *value;

                p2 = strstr(p, ":");
                if (p2 == NULL) {
                    // Expected at 1 : to delimit the key from the value.
                    return APR_EGENERAL;
                }

                // Null terminate the key
                *p2 = 0;
                key = p;

                // The rest if the value.
                value = p2 + 1;

                // Insert key/value into hash table.
                apr_hash_set(f->headers, key, APR_HASH_KEY_STRING, value);
            }
        }

        // Check for content length
        {
            char* content_length = apr_hash_get(f->headers, "content-length", APR_HASH_KEY_STRING);
            if (content_length) {
                char endbuffer[2];
                apr_size_t length = 2;

                f->body_length = apr_atoi64(content_length);
                f->body = apr_pcalloc(pool, f->body_length);
                rc = apr_socket_recv(connection->socket, f->body, &f->body_length);
                CHECK_SUCCESS;

                // Expect a \n after the end
                rc = apr_socket_recv(connection->socket, endbuffer, &length);
                CHECK_SUCCESS;
                if (length != 2 || endbuffer[0] != '\0' || endbuffer[1] != '\n')
                    return APR_EGENERAL;
            } else {
                // The remainder of the buffer (including the \n at the end) is the body)
                rc = stomp_read_buffer(connection, &f->body, pool);
                CHECK_SUCCESS;
            }
        }
    }

#undef CHECK_SUCCESS
    *frame = f;
    return APR_SUCCESS;
}
