/*
 * ATRAC3plus ATS parser
 *
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

#ifndef AVCODEC_ATRAC3PLUS_ATS_PARSER_H
#define AVCODEC_ATRAC3PLUS_ATS_PARSER_H

#include <stdint.h>

/* TODO downmix levels from 0 to 7 are the following:
 -0dB, -1.5dB, -3dB, -4.5dB, -6dB, -7.5dB, -9dB, -infinity dB */
typedef struct Atrac3pAtsDownmixLevels {
    uint8_t front_downmix_level : 3;
    uint8_t center_downmix_level : 3;
    uint8_t back_downmix_level : 3;
    uint8_t lfe_downmix_level : 3;
    uint8_t side_downmix_level : 3;
} Atrac3pAtsDownmixLevels;

/**
 * Parses the ATS header and updates the codec context with the values from the
 * header.
 *
 * Optionally retrieves downmix levels from the header.
 *
 * @param buf            Input buffer containing the header
 * @param buf_size       Input buffer size
 * @param avctx          Codec context to update
 * @param downmix_levels Will contain the downmix levels after parsing.
 *                       Can be NULL
 * @retval 0                   Success
 * @retval AVERROR_INVALIDDATA The header contained an invalid value
 */
int ff_atrac3p_ats_parse_header(const uint8_t *buf, int buf_size, AVCodecContext *avctx,
                                Atrac3pAtsDownmixLevels *downmix_levels);

#endif /* AVCODEC_ATRAC3PLUS_ATS_PARSER_H */
