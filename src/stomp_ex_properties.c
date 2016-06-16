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
#include "stomp_ex_properties.h"

void stomp_exchange_add(stomp_exchange_properties_t *properties, 
                              const char *name, const char *value)
{
  apr_hash_set(properties, name, APR_HASH_KEY_STRING, value);
}

const char *stomp_exchange_get(stomp_exchange_properties_t *properties, 
                              const char *name)
{
  return apr_hash_get(properties, name, APR_HASH_KEY_STRING);
}


void stomp_exchange_clear(stomp_exchange_properties_t *properties) {
  apr_hash_clear (properties);
}