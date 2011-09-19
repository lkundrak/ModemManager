/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2010 Red Hat, Inc.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <string.h>

#include "test-wmc-utils.h"
#include "utils.h"

static const char decap_inbuf[] = {
    0x40, 0x03, 0x00, 0x01, 0x00, 0x19, 0xf0, 0x00, 0x16, 0x00, 0x21, 0x00,
    0x1c, 0x00, 0xd8, 0x00, 0x3f, 0x00, 0x56, 0x01, 0x3f, 0x00, 0x15, 0x00,
    0x1a, 0x00, 0x11, 0x01, 0x3f, 0x00, 0x92, 0x01, 0x3f, 0x00, 0x39, 0x00,
    0x3f, 0x00, 0x95, 0x01, 0x3f, 0x00, 0x12, 0x00, 0x3f, 0x00, 0x23, 0x01,
    0x3f, 0x00, 0x66, 0x00, 0x3f, 0x00, 0x0b, 0x01, 0x3f, 0x00, 0xae, 0x00,
    0x3f, 0x00, 0x02, 0x01, 0x3f, 0x00, 0xa8, 0x00, 0x3f, 0x00, 0x50, 0x01,
    0x3f, 0x00, 0xf8, 0x01, 0x3f, 0x00, 0x57, 0x00, 0x3f, 0x00, 0x7d, 0x5e,
    0x00, 0x3f, 0x00, 0x93, 0x00, 0x3f, 0x00, 0xbd, 0x00, 0x3f, 0x00, 0x77,
    0x01, 0x3f, 0x00, 0xb7, 0x00, 0x3f, 0x00, 0xab, 0x00, 0x3f, 0x00, 0x33,
    0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xad, 0xde, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13,
    0x13, 0x50, 0x1f, 0x00, 0x00, 0xff, 0xff, 0x00, 0xaa, 0x19, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb1, 0xc4, 0x7d, 0x5e,
    0x7d, 0x5e, 0x7d, 0x5d, 0x5d, 0x04, 0x58, 0x1b, 0x5b, 0x1b, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x65, 0x69, 0x7e
};

void
test_utils_decapsulate_basic_buffer (void *f, void *data)
{
    gboolean success;
    char outbuf[512];
    gsize decap_len = 0;
    gsize used = 0;
    gboolean more = FALSE;

    success = hdlc_decapsulate_buffer (decap_inbuf, sizeof (decap_inbuf),
                                       FALSE, 0, outbuf, sizeof (outbuf),
                                       &decap_len, &used, &more);
    g_assert (success);
    g_assert (decap_len == 214);
    g_assert (used == 221);
    g_assert (more == FALSE);
}


static const char encap_outbuf[] = {
    0x4b, 0x05, 0x08, 0x00, 0x01, 0xdd, 0x7e
};

void
test_utils_encapsulate_basic_buffer (void *f, void *data)
{
    char cmdbuf[10];
    char outbuf[512];
    gsize encap_len = 0;

    cmdbuf[0] = 0x4B;   /* DIAG_CMD_SUBSYS */
    cmdbuf[1] = 0x05;   /* DIAG_SUBSYS_HDR */
    cmdbuf[2] = 0x08;   /* first byte of DIAG_SUBSYS_HDR_STATE_INFO in LE */
    cmdbuf[3] = 0x00;   /* second byte of DIAG_SUBSYS_HDR_STATE_INFO in LE */

    encap_len = hdlc_encapsulate_buffer (cmdbuf, 4, sizeof (cmdbuf),
                                         0, TRUE, FALSE,
                                         &outbuf[0], sizeof (outbuf));
    g_assert (encap_len == sizeof (encap_outbuf));
    g_assert (memcmp (outbuf, encap_outbuf, encap_len) == 0);
}

static const char cns_inbuf[] = {
    0x00, 0x0a, 0x6b, 0x74, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x7e
};

void
test_utils_decapsulate_sierra_cns (void *f, void *data)
{
    gboolean success;
    char outbuf[512];
    gsize decap_len = 0;
    gsize used = 0;
    gboolean more = FALSE;

    success = hdlc_decapsulate_buffer (cns_inbuf, sizeof (cns_inbuf),
                                       FALSE, 0, outbuf, sizeof (outbuf),
                                       &decap_len, &used, &more);
    g_assert (success == FALSE);
}


static const char uml290_encap_src[] = {
    0xc8, 0x0d, 0xda, 0x07, 0x0c, 0x00, 0x14, 0x00, 0x10, 0x00, 0x19, 0x00,
    0x04, 0x00, 0x01, 0x00, 0x07, 0x88
};

static const char uml290_encap_outbuf[] = {
    0x41, 0x54, 0x2a, 0x57, 0x4d, 0x43, 0x3d, 0xc8, 0x7d, 0x2d, 0xda, 0x7d,
    0x27, 0x7d, 0x2c, 0x7d, 0x20, 0x7d, 0x34, 0x7d, 0x20, 0x7d, 0x30, 0x7d,
    0x20, 0x7d, 0x39, 0x7d, 0x20, 0x7d, 0x24, 0x7d, 0x20, 0x7d, 0x21, 0x7d,
    0x20, 0x7d, 0x27, 0x88, 0x0d
};

void
test_utils_encapsulate_uml290_wmc1 (void *f, void *data)
{
    char inbuf[512];
    char outbuf[512];
    gsize encap_len = 0;

    memcpy (inbuf, uml290_encap_src, sizeof (uml290_encap_src));
    encap_len = uml290_wmc_encapsulate (inbuf, sizeof (uml290_encap_src),
                                         sizeof (inbuf), outbuf, sizeof (outbuf));
    g_assert (encap_len == sizeof (encap_outbuf));
    g_assert (memcmp (outbuf, encap_outbuf, encap_len) == 0);
}

static const char uml290_src[] = {
    0xc8, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0xda, 0x07, 0x0c, 0x00,
    0x14, 0x00, 0x12, 0x00, 0x19, 0x00, 0x06, 0x00, 0xc2, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x7d, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x7d, 0x5d, 0x00, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x01, 0x56, 0x65, 0x72, 0x69, 0x7a, 0x6f, 0x6e, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39,
    0x00, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00, 0x30, 0x30, 0x7e
};

static const char uml290_expected[] = {
    0xc8, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0xda, 0x07, 0x0c, 0x00,
    0x14, 0x00, 0x12, 0x00, 0x19, 0x00, 0x06, 0x00, 0xc2, 0x02, 0x00, 0x00,
    0x00, 0x00, 0x7d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x56, 0x65, 0x72, 0x69, 0x7a, 0x6f, 0x6e, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00,
    0x00, 0x40, 0x06, 0x00, 0x00
};

void
test_utils_decapsulate_uml290_wmc1 (void *f, void *data)
{
    gboolean success;
    char outbuf[512];
    gsize decap_len = 0;
    gsize used = 0;
    gboolean more = FALSE;

    success = hdlc_decapsulate_buffer (uml290_src, sizeof (uml290_src),
                                       TRUE, 0x3030, outbuf, sizeof (outbuf),
                                       &decap_len, &used, &more);
    g_assert (success == TRUE);
    g_assert (more == 0);
    g_assert_cmpint (used, ==, sizeof (uml290_src));
    g_assert_cmpint (decap_len, ==, sizeof (uml290_expected));
    g_assert_cmpint (memcmp (outbuf, uml290_expected, decap_len), ==, 0);
}


static const char pc5740_src[] = {
    0xc8, 0x0b, 0x17, 0x00, 0x00, 0x00, 0x06, 0x00, 0xdb, 0x07, 0x06, 0x00,
    0x11, 0x00, 0x0d, 0x00, 0x2d, 0x00, 0x10, 0x00, 0xe4, 0x03, 0xd4, 0xfe,
    0xff, 0xff, 0x4e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0e, 0x92, 0x7e
};

static const char pc5740_expected[] = {
    0xc8, 0x0b, 0x17, 0x00, 0x00, 0x00, 0x06, 0x00, 0xdb, 0x07, 0x06, 0x00,
    0x11, 0x00, 0x0d, 0x00, 0x2d, 0x00, 0x10, 0x00, 0xe4, 0x03, 0xd4, 0xfe,
    0xff, 0xff, 0x4e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

void
test_utils_decapsulate_pc5740_wmc1 (void *f, void *data)
{
    gboolean success;
    char outbuf[512];
    gsize decap_len = 0;
    gsize used = 0;
    gboolean more = FALSE;

    success = hdlc_decapsulate_buffer (pc5740_src, sizeof (pc5740_src),
                                       FALSE, 0, outbuf, sizeof (outbuf),
                                       &decap_len, &used, &more);
    g_assert (success == TRUE);
    g_assert (more == 0);
    g_assert_cmpint (used, ==, sizeof (pc5740_src));
    g_assert_cmpint (decap_len, ==, sizeof (pc5740_expected));
    g_assert_cmpint (memcmp (outbuf, pc5740_expected, decap_len), ==, 0);
}

