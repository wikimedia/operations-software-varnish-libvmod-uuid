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
#include <stdlib.h>

#include "vrt.h"
#include "cache/cache.h"

#include "vcc_if.h"

#define UUID_CALL(RC,CTX,CALL,RET)				\
	do {							\
		if (((RC) = (CALL)) != UUID_RC_OK) {		\
			VSLb((CTX)->vsl, SLT_VCL_Error,		\
			     "vmod uuid error %d: %s", (RC),	\
			     uuid_error(RC));			\
			return(RET);				\
		}						\
	} while(0)

static inline int
mkuuid(VRT_CTX, int utype, uuid_t *uuid, const char *str, va_list ap)
{
	uuid_t *uuid_ns = NULL;
	uuid_rc_t rc;
	char *ns, *name;
	size_t len = UUID_LEN_STR + 1;

	if (utype == UUID_MAKE_V3 || utype == UUID_MAKE_V5) {
		uuid_ns = (uuid_t *) va_arg(ap, uuid_t *);
		AN(uuid_ns);
		ns = (char *) va_arg(ap, char *);
		AN(ns);
		name = (char *) va_arg(ap, char *);
		AN(name);
		if (uuid_load(uuid_ns, ns) != UUID_RC_OK
		    && uuid_import(uuid_ns, UUID_FMT_STR, (const void *) ns,
                                   strlen(ns)) != UUID_RC_OK)
			return(-1);
		AN(uuid_ns);
	}

	UUID_CALL(rc, ctx, uuid_make(uuid, utype, uuid_ns, name), -1);
	UUID_CALL(rc, ctx, uuid_export(uuid, UUID_FMT_STR, &str, &len), -1);
	assert(len == UUID_LEN_STR + 1);
	return(0);
}

static VCL_STRING
_uuid(VRT_CTX, uuid_t *uuid, int utype, ...)
{
	char *p, uuid_str[UUID_LEN_STR + 1];
	va_list ap;
	int ret;

	CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
	AN(uuid);
	assert(utype == UUID_MAKE_V1 || utype == UUID_MAKE_V3
	    || utype == UUID_MAKE_V4 || utype == UUID_MAKE_V5);

	va_start(ap, utype);
	ret = mkuuid(ctx, utype, uuid, uuid_str, ap);
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

static void
free1uuid(void *priv)
{
	AN(priv);
	uuid_destroy((uuid_t *) priv);
}

static inline uuid_t *
get1uuid(VRT_CTX, struct vmod_priv *priv)
{
	AN(priv);
	if (priv->priv == NULL) {
		uuid_rc_t rc;
		uuid_t *uuid = NULL;

		CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
		UUID_CALL(rc, ctx, uuid_create(&uuid), NULL);
		priv->priv = uuid;
		priv->free = free1uuid;
	}
	return (uuid_t *) priv->priv;
}

static void
free2uuids(void *priv)
{
	uuid_t **p;

	AN(priv);
	p = priv;
	uuid_destroy(p[0]);
	uuid_destroy(p[1]);
}

static inline uuid_t *
get2uuids(VRT_CTX, struct vmod_priv *priv, uuid_t **uuid_ns)
{
	uuid_t **uuids;

	AN(priv);
	AN(uuid_ns);
	if (priv->priv == NULL) {
		uuid_rc_t rc;
		uuid_t *uuid = NULL;

		CHECK_OBJ_NOTNULL(ctx, VRT_CTX_MAGIC);
		*uuid_ns = NULL;
		UUID_CALL(rc, ctx, uuid_create(&uuid), NULL);
		UUID_CALL(rc, ctx, uuid_create(uuid_ns), NULL);
		priv->priv = malloc(2 * sizeof(uuid_t *));
		AN(priv->priv);
		uuids = (uuid_t **) priv->priv;
		uuids[0] = uuid;
		uuids[1] = *uuid_ns;
		priv->free = free2uuids;
		return uuid;
	}
	uuids = (uuid_t **) priv->priv;
	*uuid_ns = uuids[1];
	return uuids[0];
}

VCL_STRING
vmod_uuid_v1(VRT_CTX, struct vmod_priv *priv)
{
	uuid_t *uuid = get1uuid(ctx, priv);
	return _uuid(ctx, uuid, UUID_MAKE_V1);
}

VCL_STRING
vmod_uuid_v3(VRT_CTX, struct vmod_priv *priv, VCL_STRING ns, VCL_STRING name)
{
	uuid_t *uuid_ns;
	uuid_t *uuid = get2uuids(ctx, priv, &uuid_ns);
	return _uuid(ctx, uuid, UUID_MAKE_V3, uuid_ns, ns, name);
}

VCL_STRING
vmod_uuid_v4(VRT_CTX, struct vmod_priv *priv)
{
	uuid_t *uuid = get1uuid(ctx, priv);
	return _uuid(ctx, uuid, UUID_MAKE_V4);
}

VCL_STRING
vmod_uuid_v5(VRT_CTX, struct vmod_priv *priv, VCL_STRING ns, VCL_STRING name)
{
	uuid_t *uuid_ns;
	uuid_t *uuid = get2uuids(ctx, priv, &uuid_ns);
	return _uuid(ctx, uuid, UUID_MAKE_V5, uuid_ns, ns, name);
}

VCL_STRING
vmod_uuid(VRT_CTX, struct vmod_priv *priv)
{
	return vmod_uuid_v1(ctx, priv);
}
