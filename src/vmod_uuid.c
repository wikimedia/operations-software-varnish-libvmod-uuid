/*
** Copyright 2013 Sharecare, Inc.
**
**   Licensed under the Apache License, Version 2.0 (the "License");
**   you may not use this file except in compliance with the License.
**   You may obtain a copy of the License at
**
**       http://www.apache.org/licenses/LICENSE-2.0
**
**   Unless required by applicable law or agreed to in writing, software
**   distributed under the License is distributed on an "AS IS" BASIS,
**   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**   See the License for the specific language governing permissions and
**   limitations under the License.
**
**
** vmod_uuid.c  Generate a UUID for use by varnish
** Date:        08/23/2013
** By:          Mitchell Broome <mbroome@sharecare.com>
** Version:     0.1
**
**
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uuid.h>

#include <stdarg.h>
#include <syslog.h>

#include "vrt.h"
#include "bin/varnishd/cache.h"

#include "vcc_if.h"

#define DEBUG 0

static void
debug(const char *fmt, ...){
   va_list ap;
   va_start(ap, fmt);
   vsyslog(LOG_DAEMON|LOG_INFO, fmt, ap);
   va_end(ap);
}

#define UUID_CALL(RC,SP,CALL,UUID,STR)                                  \
   do {                                                                 \
     if (((RC) = (CALL)) != UUID_RC_OK) {                               \
        WSP((SP), SLT_VCL_error, "vmod uuid error %d: %s", (RC),        \
            uuid_error(RC));                                            \
        if ((UUID) != NULL)                                             \
           uuid_destroy(UUID);                                          \
        if ((STR) != NULL)                                              \
           free(STR);                                                   \
        return(NULL);                                                   \
     }                                                                  \
   } while(0)
   
static char *
uuid(struct sess *sp, int utype, va_list ap) {
    uuid_t *uuid = NULL, *uuid_ns;
    uuid_rc_t rc;
    char *str = NULL, *ns, *name;

    if (utype == UUID_MAKE_V3 || utype == UUID_MAKE_V5) {
       UUID_CALL(rc, sp, uuid_create(&uuid_ns), uuid, str);
       ns = (char *) va_arg(ap, char *);
       AN(ns);
       name = (char *) va_arg(ap, char *);
       AN(name);
       if (uuid_load(uuid_ns, ns) != UUID_RC_OK
           && uuid_import(uuid_ns, UUID_FMT_STR, (const void *) ns, strlen(ns))
              != UUID_RC_OK) {
          UUID_CALL(rc, sp, uuid_destroy(uuid_ns), uuid, str);
          return(NULL);
       }
    }

    UUID_CALL(rc, sp, uuid_create(&uuid), uuid, str);
    UUID_CALL(rc, sp, uuid_make(uuid, utype, uuid_ns, name), uuid, str);
    str = NULL;
    UUID_CALL(rc, sp, uuid_export(uuid, UUID_FMT_STR, &str, NULL), uuid, str);
    UUID_CALL(rc, sp, uuid_destroy(uuid), uuid, str);
    if (DEBUG)
       debug("uuid: %s", str);
    return(str);
}

static const char *
_uuid(struct sess *sp, int utype, ...) {
   char *p;
   unsigned u, v;
   va_list ap;

   CHECK_OBJ_NOTNULL(sp, SESS_MAGIC);
   assert(utype == UUID_MAKE_V1 || utype == UUID_MAKE_V3
          || utype == UUID_MAKE_V4 || utype == UUID_MAKE_V5);

   va_start(ap, utype);
   char *uuid_str = uuid(sp, utype, ap);
   va_end(ap);
   if (uuid_str == NULL)
      return(NULL);

   u = WS_Reserve(sp->wrk->ws, 0);     // Reserve some work space 
   if (sizeof(uuid_str) > u) {
      // No space, reset and leave 
      WS_Release(sp->wrk->ws, 0);
      return(NULL);
   }

   p = sp->wrk->ws->f;                 // Front of workspace area 

   strcpy(p, uuid_str);
   // keep track of how much we actually used
   v += strlen(uuid_str);
   // free up the uuid string once it's copied in place
   if(uuid_str){
      free(uuid_str);
   }

   // Update work space with what we've used 
   WS_Release(sp->wrk->ws, v);
   if (DEBUG)
      debug("uuid: %s", p);
   return(p);
}

const char *
vmod_uuid_v1(struct sess *sp) {
   return _uuid(sp, UUID_MAKE_V1);
}

const char *
vmod_uuid_v3(struct sess *sp, const char *ns, const char *name) {
   return _uuid(sp, UUID_MAKE_V3, ns, name);
}

const char *
vmod_uuid_v4(struct sess *sp) {
   return _uuid(sp, UUID_MAKE_V4);
}

const char *
vmod_uuid_v5(struct sess *sp, const char *ns, const char *name) {
   return _uuid(sp, UUID_MAKE_V5, ns, name);
}

const char *
vmod_uuid(struct sess *sp) {
   return vmod_uuid_v1(sp);
}
