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
** Original Varnish 3 version:
** vmod_uuid.c  Generate a UUID for use by varnish
** Date:        08/23/2013
** By:          Mitchell Broome <mbroome@sharecare.com>
** Version:     0.1
**
**
** Varnish 4 version, supporting all UUID variants:
** Geoffrey Simmons <geoff@uplex.de>, UPLEX Nils Goroll Systemoptimierung
** for Otto GmbH & KG
** Version 1.0
*/

#include <string.h>
#include <uuid.h>
#include <stdarg.h>

#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"

#define UUID_CALL(RC,CTX,CALL,UUID,UUIDNS)			\
	do {							\
		if (((RC) = (CALL)) != UUID_RC_OK) {		\
			VSLb((CTX)->vsl, SLT_VCL_Error,		\
			    "vmod uuid error %d: %s", (RC),	\
			    uuid_error(RC));			\
			if ((UUID) != NULL)			\
				uuid_destroy(UUID);		\
			if ((UUIDNS) != NULL)			\
				uuid_destroy(UUIDNS);		\
			return(-1);				\
		}						\
	} while(0)
   
static inline int
mkuuid(const struct vrt_ctx *ctx, int utype, const char *str, va_list ap)
{
	uuid_t *uuid = NULL, *uuid_ns = NULL;
	uuid_rc_t rc;
	char *ns, *name;
	size_t len = UUID_LEN_STR + 1;

	if (utype == UUID_MAKE_V3 || utype == UUID_MAKE_V5) {
		UUID_CALL(rc, ctx, uuid_create(&uuid_ns), uuid, uuid_ns);
		ns = (char *) va_arg(ap, char *);
		AN(ns);
		name = (char *) va_arg(ap, char *);
		AN(name);
		if (uuid_load(uuid_ns, ns) != UUID_RC_OK
		    && uuid_import(uuid_ns, UUID_FMT_STR, (const void *) ns,
                                   strlen(ns)) != UUID_RC_OK) {
			UUID_CALL(rc, ctx, uuid_destroy(uuid_ns), uuid,
                                  uuid_ns);
			return(-1);
		}
		AN(uuid_ns);
	}

	UUID_CALL(rc, ctx, uuid_create(&uuid), uuid, uuid_ns);
	UUID_CALL(rc, ctx, uuid_make(uuid, utype, uuid_ns, name), uuid,
                  uuid_ns);
	UUID_CALL(rc, ctx, uuid_export(uuid, UUID_FMT_STR, &str, &len), uuid,
	          uuid_ns);
	assert(len == UUID_LEN_STR + 1);
	UUID_CALL(rc, ctx, uuid_destroy(uuid), uuid, uuid_ns);
	if (uuid_ns != NULL)
		uuid_destroy(uuid_ns);
	return(0);
}

static VCL_STRING
_uuid(const struct vrt_ctx *ctx, int utype, ...)
{
	char *p, uuid_str[UUID_LEN_STR + 1];
	va_list ap;
	int ret;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	assert(utype == UUID_MAKE_V1 || utype == UUID_MAKE_V3
	    || utype == UUID_MAKE_V4 || utype == UUID_MAKE_V5);

	va_start(ap, utype);
	ret = mkuuid(ctx, utype, uuid_str, ap);
	va_end(ap);
	if (ret != 0)
		return(NULL);

	assert(strlen(uuid_str) == UUID_LEN_STR);
	p = WS_Copy(ctx->ws, (const void *) uuid_str, UUID_LEN_STR + 1);
	if (p == NULL) {
		VSLb(ctx->vsl, SLT_VCL_Error,
                    "vmod uuid: insufficient workspace");
		return(NULL);
	}
	return(p);
}

VCL_STRING
vmod_uuid_v1(const struct vrt_ctx *ctx)
{
	return _uuid(ctx, UUID_MAKE_V1);
}

VCL_STRING
vmod_uuid_v3(const struct vrt_ctx *ctx, VCL_STRING ns, VCL_STRING name)
{
	return _uuid(ctx, UUID_MAKE_V3, ns, name);
}

VCL_STRING
vmod_uuid_v4(const struct vrt_ctx *ctx)
{
	return _uuid(ctx, UUID_MAKE_V4);
}

VCL_STRING
vmod_uuid_v5(const struct vrt_ctx *ctx, VCL_STRING ns, VCL_STRING name)
{
	return _uuid(ctx, UUID_MAKE_V5, ns, name);
}

VCL_STRING
vmod_uuid(const struct vrt_ctx *ctx)
{
	return vmod_uuid_v1(ctx);
}
