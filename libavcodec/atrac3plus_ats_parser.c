/*
 * ATRAC3+ ATS parser
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

#include "atrac3plus.h"
#include "atrac3plus_ats_parser.h"
#include "codec_id.h"
#include "get_bits.h"
#include "parser.h"
#include "parser_internal.h"

int ff_atrac3p_ats_parse_header(const uint8_t *buf, int buf_size, AVCodecContext *avctx,
                                Atrac3pAtsDownmixLevels *downmix_levels)
{
    static const uint16_t ch_layouts_tab[7] = {
        AV_CH_LAYOUT_MONO,
        AV_CH_LAYOUT_STEREO,
        AV_CH_LAYOUT_SURROUND,
        AV_CH_LAYOUT_4POINT0,
        AV_CH_LAYOUT_5POINT1_BACK,
        AV_CH_LAYOUT_6POINT1_BACK,
        AV_CH_LAYOUT_7POINT1
    };
    static const uint16_t sample_rate_tab[3] = { 32000, 44100, 48000 };

    GetBitContext gb;

    if (buf_size < ATRAC3P_ATS_HEADER_SIZE) {
        av_log(avctx, AV_LOG_ERROR, "buf_size is too small\n");
        return AVERROR_INVALIDDATA;
    }

    const int ret = init_get_bits8(&gb, buf, buf_size);
    av_assert0(ret == 0);

    const uint16_t sync_word = get_bits(&gb, 16);
    if (sync_word != ATRAC3P_ATS_SYNC_WORD) {
        av_log(avctx, AV_LOG_ERROR, "Invalid sync word in the ATS header (%04X)\n",
               sync_word);
        return AVERROR_INVALIDDATA;
    }

    const uint8_t sample_rate_idc = get_bits(&gb, 3);
    if (sample_rate_idc >= FF_ARRAY_ELEMS(sample_rate_tab)) {
        av_log(avctx, AV_LOG_ERROR, "Invalid sample rate indicator in the ATS header (%d)\n",
               sample_rate_idc);
        return AVERROR_INVALIDDATA;
    }

    const uint16_t sample_rate = sample_rate_tab[sample_rate_idc];

    const uint8_t channel_config_idc = get_bits(&gb, 3);
    if (channel_config_idc - 1 >= FF_ARRAY_ELEMS(ch_layouts_tab)) {
        av_log(avctx, AV_LOG_ERROR, "Invalid channel configuration indicator in the ATS header (%d)\n",
               channel_config_idc);
        return AVERROR_INVALIDDATA;
    }

    const uint16_t ch_layout_mask = ch_layouts_tab[channel_config_idc - 1];

    const uint16_t block_align = (get_bits(&gb, 10) + 1) * 8 + ATRAC3P_ATS_HEADER_SIZE;

    if (downmix_levels) {
        downmix_levels->front_downmix_level = get_bits(&gb, 3);
        downmix_levels->center_downmix_level = get_bits(&gb, 3);
        downmix_levels->back_downmix_level = get_bits(&gb, 3);
        downmix_levels->lfe_downmix_level = get_bits(&gb, 3);
        downmix_levels->side_downmix_level = get_bits(&gb, 3);
    }

    avctx->sample_rate = sample_rate;
    av_channel_layout_uninit(&avctx->ch_layout);
    av_channel_layout_from_mask(&avctx->ch_layout, ch_layout_mask);
    avctx->block_align = block_align;
    avctx->frame_size = ATRAC3P_FRAME_SAMPLES;
    avctx->bit_rate = block_align * CHAR_BIT * sample_rate / ATRAC3P_FRAME_SAMPLES;
    return 0;
}

typedef struct Atrac3pAtsParseContext {
    ParseContext pc;
    int remaining_size;
} Atrac3pAtsParseContext;

static int parse(AVCodecParserContext *s1, AVCodecContext *avctx,
                 const uint8_t **poutbuf, int *poutbuf_size,
                 const uint8_t *buf, int buf_size)
{
    Atrac3pAtsParseContext *const s = s1->priv_data;
    int next = END_NOT_FOUND;

    s1->duration  = ATRAC3P_FRAME_SAMPLES;

    if (s1->flags & PARSER_FLAG_COMPLETE_FRAMES) {
        (void)ff_atrac3p_ats_parse_header(buf, buf_size, avctx, NULL);
        next = buf_size;
    } else {
        if (!s->pc.frame_start_found)
            for (int i = 0; i < buf_size; i++) {
                s->pc.state64 = (s->pc.state64 << 8) | buf[i];
                if (s->pc.state64 >> 48 == ATRAC3P_ATS_SYNC_WORD) {
                    uint8_t tmp_buf[ATRAC3P_ATS_HEADER_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
                    AV_WB64(tmp_buf, s->pc.state64);
                    if (ff_atrac3p_ats_parse_header(tmp_buf, ATRAC3P_ATS_HEADER_SIZE, avctx, NULL) < 0)
                        continue;

                    s->pc.frame_start_found = 1;
                    s->remaining_size = avctx->block_align + i - 7;
                    break;
                }
            }

        if (s->pc.frame_start_found)
            if (s->remaining_size <= buf_size) {
                next = s->remaining_size;
                s->remaining_size = 0;
                s->pc.frame_start_found = 0;
                s->pc.state64 = 0;
            } else {
                s->remaining_size -= buf_size;
            }

        if (ff_combine_frame(&s->pc, next, &buf, &buf_size) < 0) {
            *poutbuf      = NULL;
            *poutbuf_size = 0;
            return buf_size;
        }
    }

    *poutbuf      = buf;
    *poutbuf_size = buf_size;
    return next;
}

const FFCodecParser ff_atrac3p_ats_parser = {
    .codec_ids      = { AV_CODEC_ID_ATRAC3P_ATS },
    .priv_data_size = sizeof(Atrac3pAtsParseContext),
    .parse          = parse,
    .close          = ff_parse_close,
};
