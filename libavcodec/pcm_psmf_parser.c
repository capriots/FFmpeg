/*
 * PCM PSMF packetizer
 * Copyright (c) 2026 Simon Capriotti
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "codec_id.h"
#include "parser.h"
#include "parser_internal.h"

static int parse(AVCodecParserContext *s, AVCodecContext *avctx,
                 const uint8_t **poutbuf, int *poutbuf_size,
                 const uint8_t *buf, int buf_size)
{
    ParseContext *const pc = s->priv_data;

    *poutbuf      = NULL;
    *poutbuf_size = 0;

    if (!avctx->block_align) {
        av_log(avctx, AV_LOG_FATAL, "The container must set AVCodecContext.block_align!\n");
        return buf_size;
    }

    const int remaining_size = avctx->block_align - pc->index;

    const int next = remaining_size <= buf_size ? remaining_size : END_NOT_FOUND;

    if (ff_combine_frame(pc, next, &buf, &buf_size) < 0)
        return buf_size;

    *poutbuf      = buf;
    *poutbuf_size = buf_size;
    return next;
}

const FFCodecParser ff_pcm_psmf_parser = {
    .codec_ids      = { AV_CODEC_ID_PCM_S16LE, AV_CODEC_ID_PCM_PAMF },
    .priv_data_size = sizeof(ParseContext),
    .parse          = parse,
    .close          = ff_parse_close,
};
