/*
 * PSMF/PAMF demuxer
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

#ifndef AVFORMAT_PSMF_H
#define AVFORMAT_PSMF_H

#include "libavcodec/avcodec.h"
#include "libavutil/avassert.h"

#define PSMF_MAGIC "PSMF"
#define PSMF_VERSION_12 "0012"
#define PSMF_VERSION_13 "0013"
#define PSMF_VERSION_14 "0014"
#define PSMF_VERSION_15 "0015"
#define PAMF_MAGIC "PAMF"
#define PAMF_VERSION_40 "0040"
#define PAMF_VERSION_41 "0041"
#define PSMF_PCM_FRAME_SIZE 80

typedef struct PsmfContext {
    MpegDemuxContext mpeg;
    int64_t prev_position;
} PsmfContext;

static int psmf_probe(const AVProbeData *p)
{
    if (memcmp(p->buf, PSMF_MAGIC, strlen(PSMF_MAGIC)) == 0 &&
        (memcmp(p->buf + 4, PSMF_VERSION_12, strlen(PSMF_VERSION_12)) == 0 ||
        memcmp(p->buf + 4, PSMF_VERSION_13, strlen(PSMF_VERSION_13)) == 0 ||
        memcmp(p->buf + 4, PSMF_VERSION_14, strlen(PSMF_VERSION_14)) == 0 ||
        memcmp(p->buf + 4, PSMF_VERSION_15, strlen(PSMF_VERSION_15)) == 0))
        return AVPROBE_SCORE_MAX;
    if (memcmp(p->buf, PAMF_MAGIC, strlen(PAMF_MAGIC)) == 0 &&
        (memcmp(p->buf + 4, PAMF_VERSION_40, strlen(PAMF_VERSION_40)) == 0 ||
        memcmp(p->buf + 4, PAMF_VERSION_41, strlen(PAMF_VERSION_41)) == 0))
        return AVPROBE_SCORE_MAX;
    return 0;
}

static int psmf_set_stream_aspect_ratio(uint8_t aspect_ratio_idc,
                                        uint16_t sar_width,
                                        uint16_t sar_height, AVStream *s)
{
    switch (aspect_ratio_idc) {
    case 1:
        s->codecpar->sample_aspect_ratio.num = 1;
        s->codecpar->sample_aspect_ratio.den = 1;
        s->sample_aspect_ratio.num = 1;
        s->sample_aspect_ratio.den = 1;
        return 0;
    case 2:
        s->codecpar->sample_aspect_ratio.num = 12;
        s->codecpar->sample_aspect_ratio.den = 11;
        s->sample_aspect_ratio.num = 12;
        s->sample_aspect_ratio.den = 11;
        return 0;
    case 3:
        s->codecpar->sample_aspect_ratio.num = 10;
        s->codecpar->sample_aspect_ratio.den = 11;
        s->sample_aspect_ratio.num = 10;
        s->sample_aspect_ratio.den = 11;
        return 0;
    case 4:
        s->codecpar->sample_aspect_ratio.num = 16;
        s->codecpar->sample_aspect_ratio.den = 11;
        s->sample_aspect_ratio.num = 16;
        s->sample_aspect_ratio.den = 11;
        return 0;
    case 5:
        s->codecpar->sample_aspect_ratio.num = 40;
        s->codecpar->sample_aspect_ratio.den = 33;
        s->sample_aspect_ratio.num = 40;
        s->sample_aspect_ratio.den = 33;
        return 0;
    case 14:
        s->codecpar->sample_aspect_ratio.num = 4;
        s->codecpar->sample_aspect_ratio.den = 3;
        s->sample_aspect_ratio.num = 4;
        s->sample_aspect_ratio.den = 3;
        return 0;
    case 0xff:
        s->codecpar->sample_aspect_ratio.num = sar_width;
        s->codecpar->sample_aspect_ratio.den = sar_height;
        s->sample_aspect_ratio.num = sar_width;
        s->sample_aspect_ratio.den = sar_height;
        return 0;
    default:
        av_log(s, AV_LOG_FATAL, "Invalid aspect ratio indicator (%d)\n",
               aspect_ratio_idc);
        return AVERROR_INVALIDDATA;
    }
}

static int psmf_set_stream_frame_rate(uint8_t frame_rate_info, AVStream *s)
{
    switch (frame_rate_info) {
    case 0:
        s->codecpar->framerate.num = 24000;
        s->codecpar->framerate.den = 1001;
        s->avg_frame_rate.num = 24000;
        s->avg_frame_rate.den = 1001;
        s->r_frame_rate.num = 24000;
        s->r_frame_rate.den = 1001;
        return 0;
    case 1:
        s->codecpar->framerate.num = 24;
        s->codecpar->framerate.den = 1;
        s->avg_frame_rate.num = 24;
        s->avg_frame_rate.den = 1;
        s->r_frame_rate.num = 24;
        s->r_frame_rate.den = 1;
        return 0;
    case 2:
        s->codecpar->framerate.num = 25;
        s->codecpar->framerate.den = 1;
        s->avg_frame_rate.num = 25;
        s->avg_frame_rate.den = 1;
        s->r_frame_rate.num = 25;
        s->r_frame_rate.den = 1;
        return 0;
    case 3:
        s->codecpar->framerate.num = 30000;
        s->codecpar->framerate.den = 1001;
        s->avg_frame_rate.num = 30000;
        s->avg_frame_rate.den = 1001;
        s->r_frame_rate.num = 30000;
        s->r_frame_rate.den = 1001;
        return 0;
    case 4:
        s->codecpar->framerate.num = 30;
        s->codecpar->framerate.den = 1;
        s->avg_frame_rate.num = 30;
        s->avg_frame_rate.den = 1;
        s->r_frame_rate.num = 30;
        s->r_frame_rate.den = 1;
        return 0;
    case 5:
        s->codecpar->framerate.num = 50;
        s->codecpar->framerate.den = 1;
        s->avg_frame_rate.num = 50;
        s->avg_frame_rate.den = 1;
        s->r_frame_rate.num = 50;
        s->r_frame_rate.den = 1;
        return 0;
    case 6:
        s->codecpar->framerate.num = 60000;
        s->codecpar->framerate.den = 1001;
        s->avg_frame_rate.num = 60000;
        s->avg_frame_rate.den = 1001;
        s->r_frame_rate.num = 60000;
        s->r_frame_rate.den = 1001;
        return 0;
    default:
        av_log(s, AV_LOG_FATAL, "Invalid frame rate indicator (%d)\n",
               frame_rate_info);
        return AVERROR_INVALIDDATA;
    }
}

static int pamf_parse_avc_info(AVStream *s, AVIOContext *pb)
{
    const uint8_t profile_idc = avio_r8(pb);
    const uint8_t level_idc = avio_r8(pb);
    const uint8_t flags_1 = avio_r8(pb);
    const uint8_t frame_mbs_only_flag = flags_1 >> 7;
    const uint8_t video_signal_info_flag = flags_1 >> 6 & 1;
    const uint8_t frame_rate_info = (flags_1 & 0xf) - 1;
    const uint8_t aspect_ratio_idc = avio_r8(pb);
    const uint16_t sar_width = avio_rb16(pb);
    const uint16_t sar_height = avio_rb16(pb);
    const uint16_t horizontal_size = (avio_r8(pb), avio_r8(pb) * 0x10);
    const uint16_t vertical_size = (avio_r8(pb), avio_r8(pb) * 0x10);
    av_unused const uint16_t frame_crop_left_offset = avio_rb16(pb);
    av_unused const uint16_t frame_crop_right_offset = avio_rb16(pb);
    av_unused const uint16_t frame_crop_top_offset = avio_rb16(pb);
    av_unused const uint16_t frame_crop_bottom_offset = avio_rb16(pb);
    const uint8_t flags_2 = avio_r8(pb);
    av_unused const uint8_t video_format = flags_2 >> 5;
    const uint8_t video_full_range_flag = flags_2 >> 4 & 1;
    const uint8_t color_primaries = avio_r8(pb);
    const uint8_t transfer_characteristics = avio_r8(pb);
    const uint8_t matrix_coefficients = avio_r8(pb);
    const uint8_t flags_3 = avio_r8(pb);
    av_unused const  uint8_t entropy_coding_mode_flag = flags_3 >> 7;
    av_unused const uint8_t deblocking_filter_flag = flags_3 >> 6 & 1;
    av_unused const uint8_t min_num_slice_per_picture_idx = flags_3 >> 4 & 3;
    av_unused const uint8_t nfw_idc = flags_3 & 3;
    av_unused const uint8_t max_mean_bitrate = avio_r8(pb);

    avio_skip(pb, 6); // Unused

    s->codecpar->profile = profile_idc;
    s->codecpar->level = level_idc;

    if (frame_mbs_only_flag)
        s->codecpar->field_order = AV_FIELD_PROGRESSIVE;

    if (video_signal_info_flag) {
        s->codecpar->color_range = video_full_range_flag + 1;
        s->codecpar->color_primaries = color_primaries;
        s->codecpar->color_trc = transfer_characteristics;
        s->codecpar->color_space = matrix_coefficients;
    }

    s->codecpar->format = AV_PIX_FMT_YUV420P;

    int ret = psmf_set_stream_aspect_ratio(aspect_ratio_idc, sar_width, sar_height, s);
    if (ret != 0)
        return ret;

    s->codecpar->width = horizontal_size;
    s->codecpar->height = vertical_size;

    ret = psmf_set_stream_frame_rate(frame_rate_info, s);
    if (ret != 0)
        return ret;

    return 0;
}

static int pamf_parse_m2v_info(AVStream *s, AVIOContext *pb)
{
    const uint8_t profile_and_level_idc = avio_r8(pb);
    const uint8_t profile_idc = profile_and_level_idc >> 4 & 3;
    const uint8_t level_idc = profile_and_level_idc & 7;
    avio_skip(pb, 1); // Unused
    const uint8_t flags_1 = avio_r8(pb);
    const uint8_t progressive_sequence = flags_1 >> 7;
    const uint8_t video_signal_info_flag = flags_1 >> 6 & 1;
    const uint8_t frame_rate_info = flags_1 & 0xf;
    const uint8_t aspect_ratio_idc = avio_r8(pb);
    const uint16_t sar_width = avio_rb16(pb);
    const uint16_t sar_height = avio_rb16(pb);
    avio_skip(pb, 1); // Unused
    av_unused const uint16_t horizontal_size = avio_r8(pb) * 0x10;
    avio_skip(pb, 1); // Unused
    av_unused const uint16_t vertical_size = avio_r8(pb) * 0x10;
    const uint16_t horizontal_size_value = avio_rb16(pb);
    const uint16_t vertical_size_value = avio_rb16(pb);
    avio_skip(pb, 4); // Unused
    const uint8_t flags_2 = avio_r8(pb);
    av_unused const uint8_t video_format = flags_2 >> 5;
    const uint8_t video_full_range_flag = flags_2 >> 4 & 1;
    const uint8_t color_primaries = avio_r8(pb);
    const uint8_t transfer_characteristics = avio_r8(pb);
    const uint8_t matrix_coefficients = avio_r8(pb);

    if (profile_and_level_idc != 0xff) {
        if (profile_idc != 0) {
            av_log(s, AV_LOG_FATAL, "Invalid profile indicator (%d)\n",
                   profile_idc);
            return AVERROR_INVALIDDATA;
        }

        s->codecpar->profile = AV_PROFILE_MPEG2_MAIN;
        s->codecpar->level = level_idc;
    }

    if (progressive_sequence)
        s->codecpar->field_order = AV_FIELD_PROGRESSIVE;

    if (video_signal_info_flag) {
        s->codecpar->color_range = video_full_range_flag + 1;
        s->codecpar->color_primaries = color_primaries;
        s->codecpar->color_trc = transfer_characteristics;
        s->codecpar->color_space = matrix_coefficients;
    }

    s->codecpar->format = AV_PIX_FMT_YUV420P;

    int ret = psmf_set_stream_aspect_ratio(aspect_ratio_idc, sar_width, sar_height, s);
    if (ret != 0)
        return ret;

    s->codecpar->width = horizontal_size_value;
    s->codecpar->height = vertical_size_value;

    ret = psmf_set_stream_frame_rate(frame_rate_info - 1, s);
    if (ret != 0)
        return ret;

    avio_skip(pb, 8); // Unused

    return 0;
}

static int psmf_parse_audio_info(AVStream *s, AVIOContext *pb)
{
    avio_skip(pb, 2); // Unused
    const uint8_t nb_channels = avio_r8(pb);
    const uint8_t sample_rate_idx = avio_r8(pb) & 0xf;

    int ch_layout_mask;
    switch (nb_channels) {
    case 1:
        ch_layout_mask = AV_CH_LAYOUT_MONO;
        break;
    case 2:
        ch_layout_mask = AV_CH_LAYOUT_STEREO;
        break;
    default:
        av_log(s, AV_LOG_FATAL, "Invalid number of channels (%d)\n", nb_channels);
        return AVERROR_INVALIDDATA;
    }

    av_channel_layout_from_mask(&s->codecpar->ch_layout, ch_layout_mask);

    if (sample_rate_idx != 2) {
        av_log(s, AV_LOG_FATAL, "Invalid sample rate indicator (%d)\n",
               sample_rate_idx);
        return AVERROR_INVALIDDATA;
    }

    s->codecpar->sample_rate = 44100;

    if (s->codecpar->codec_id == AV_CODEC_ID_PCM_S16LE)
        s->codecpar->block_align = PSMF_PCM_FRAME_SIZE * nb_channels * sizeof(int16_t);

    return 0;
}

static int pamf_parse_audio_info(AVStream *s, AVIOContext *pb)
{
    avio_skip(pb, 2); // Unused
    const uint8_t nb_channels = avio_r8(pb) & 0xf;
    const uint8_t sample_rate_idx = avio_r8(pb) & 0xf;
    const uint8_t bits_per_sample_idx = avio_r8(pb) >> 6;
    avio_skip(pb, 27); // Unused

    int ch_layout_mask;
    switch (nb_channels) {
    case 1:
        ch_layout_mask = AV_CH_LAYOUT_MONO;
        break;
    case 2:
        ch_layout_mask = AV_CH_LAYOUT_STEREO;
        break;
    case 6:
        ch_layout_mask = AV_CH_LAYOUT_5POINT1;
        break;
    case 8:
        ch_layout_mask = AV_CH_LAYOUT_7POINT1;
        break;
    default:
        av_log(s, AV_LOG_FATAL, "Invalid number of channels (%d)\n", nb_channels);
        return AVERROR_INVALIDDATA;
    }

    av_channel_layout_from_mask(&s->codecpar->ch_layout, ch_layout_mask);

    if (sample_rate_idx != 1) {
        av_log(s, AV_LOG_FATAL, "Invalid sample rate indicator (%d)\n",
               sample_rate_idx);
        return AVERROR_INVALIDDATA;
    }

    s->codecpar->sample_rate = 48000;

    if (s->codecpar->codec_id == AV_CODEC_ID_PCM_PAMF) {
        switch (bits_per_sample_idx) {
        case 1:
            s->codecpar->bits_per_coded_sample =
            s->codecpar->bits_per_raw_sample = 16;
            s->codecpar->format = AV_SAMPLE_FMT_S16;
            break;
        case 3:
            s->codecpar->bits_per_coded_sample =
            s->codecpar->bits_per_raw_sample = 24;
            s->codecpar->format = AV_SAMPLE_FMT_S32;
            break;
        default:
            av_log(s, AV_LOG_FATAL, "Invalid bits per sample indicator (%d)\n",
                   bits_per_sample_idx);
            return AVERROR_INVALIDDATA;
        }

        s->codecpar->bit_rate = FFALIGN(s->codecpar->ch_layout.nb_channels, 2) *
                                s->codecpar->sample_rate * s->codecpar->bits_per_coded_sample;
        s->codecpar->frame_size = s->codecpar->sample_rate / 200;
        s->codecpar->block_align = s->codecpar->bit_rate / CHAR_BIT / 200;
    } else {
        s->codecpar->format = AV_SAMPLE_FMT_FLT;
    }

    return 0;
}

static int psmf_parse_entry_point_table(AVFormatContext *s, AVStream *st,
                                        uint32_t entry_point_table_offset,
                                        uint32_t nb_entry_points,
                                        uint64_t header_size,
                                        uint64_t stream_size,
                                        unsigned int *ep_tables_start_pos,
                                        unsigned int *ep_tables_end_pos, int is_pamf)
{
    av_assert0(ep_tables_start_pos && ep_tables_end_pos);

    const uint32_t ep_size = is_pamf ? 12 : 10;

    if (nb_entry_points == 0 && entry_point_table_offset != 0 ||
        nb_entry_points != 0 && entry_point_table_offset == 0 ||
        entry_point_table_offset +
        nb_entry_points * ep_size > header_size) {
        av_log(s, AV_LOG_FATAL, "Invalid entry point table offset (%X)"
               " or number of entry points (%d)\n",
               entry_point_table_offset, nb_entry_points);
        return AVERROR_INVALIDDATA;
    }

    if (entry_point_table_offset == 0)
        return 0;

    if (*ep_tables_start_pos == 0) {
        *ep_tables_end_pos = *ep_tables_start_pos = entry_point_table_offset;
    } else if (*ep_tables_end_pos != entry_point_table_offset) {
        av_log(s, AV_LOG_FATAL, "Invalid entry point table offset (%X)\n",
               entry_point_table_offset);
        return AVERROR_INVALIDDATA;
    }

    *ep_tables_end_pos += nb_entry_points * ep_size;

    const int64_t pos = avio_tell(s->pb);
    avio_seek(s->pb, entry_point_table_offset, SEEK_SET);

    uint64_t prev_offset = 0;
    for (unsigned int i = 0; i < nb_entry_points; i++) {
        int64_t pts;
        if (is_pamf) {
            avio_skip(s->pb, 2); // 2 bits: indexN, 1 bit: unused, 13 bits: nThRefPictureOffset
            pts = (int64_t)avio_rb16(s->pb) << 32 | avio_rb32(s->pb);
        } else {
            // 2 bits: indexN, 2 bit: unused, 11 bits: nThRefPictureOffset
            pts = ((int64_t)avio_rb16(s->pb) & 1) << 32 | avio_rb32(s->pb);
        }

        if (pts > UINT32_MAX) {
            av_log(s, AV_LOG_FATAL, "Invalid entry point pts (%lld)\n", pts);
            return AVERROR_INVALIDDATA;
        }

        const uint64_t offset = (uint64_t)avio_rb32(s->pb) * PSMF_PACK_SIZE_ALIGN; // From the start of the MPEG-PS stream
        if (offset > stream_size || offset < prev_offset) {
            av_log(s, AV_LOG_FATAL, "Invalid entry point offset (%lld)\n", offset);
            return AVERROR_INVALIDDATA;
        }

        ff_reduce_index(s, st->index);
        av_add_index_entry(st, offset + header_size, pts, 0, offset - prev_offset, AVINDEX_KEYFRAME);
        prev_offset = offset;
    }

    avio_seek(s->pb, pos, SEEK_SET);
    return 0;
}

static int psmf_parse_stream_info(AVFormatContext *s, uint64_t header_size,
                                  uint64_t stream_size,
                                  unsigned int *ep_tables_start_pos,
                                  unsigned int *ep_tables_end_pos)
{
    MpegDemuxContext *m = s->priv_data;

    const uint8_t stream_id = avio_r8(s->pb);
    const uint8_t private_stream_id = avio_r8(s->pb);

    uint8_t type;
    if ((stream_id & 0xf0) == VIDEO_ID) {
        if (m->psm_es_type[stream_id] != 0) {
            av_log(s, AV_LOG_FATAL, "Stream id %X already in use\n", stream_id);
            return AVERROR_INVALIDDATA;
        }

        m->psm_es_type[stream_id] =
        type = STREAM_TYPE_VIDEO_H264;
    } else if (stream_id == (PRIVATE_STREAM_1 & 0xff)) {
        if (m->psm_es_type[private_stream_id] != 0) {
            av_log(s, AV_LOG_FATAL, "Private stream id %X already in use\n",
                   private_stream_id);
            return AVERROR_INVALIDDATA;
        }

        switch (private_stream_id & 0xf0) {
        case 0x00:
            m->psm_es_type[private_stream_id] =
            type = STREAM_TYPE_PSMF_AUDIO_ATRAC3P;
            break;
        case 0x10:
            m->psm_es_type[private_stream_id] =
            type = STREAM_TYPE_PSMF_AUDIO_PCM;
            break;
        case 0x20:
            m->psm_es_type[private_stream_id] =
            type = STREAM_TYPE_PSMF_USER_DATA;
            break;
        default:
            av_log(s, AV_LOG_FATAL, "Invalid private stream id (%X)\n", stream_id);
            return AVERROR_INVALIDDATA;
        }
    } else {
        av_log(s, AV_LOG_FATAL, "Invalid stream id (%X)\n", stream_id);
        return AVERROR_INVALIDDATA;
    }

    AVStream *const st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->time_base = (AVRational){ 1, 90000 };
    avpriv_stream_set_need_parsing(st, AVSTREAM_PARSE_FULL);

    avio_skip(s->pb, 2); // 2 bits: unused, 1 bit: P_STD_buffer_scale, 13 bits: P_STD_buffer_size

    const uint32_t entry_point_table_offset = avio_rb32(s->pb);
    const uint32_t nb_entry_points = avio_rb32(s->pb);
    int ret = psmf_parse_entry_point_table(s, st, entry_point_table_offset,
                                           nb_entry_points, header_size,
                                           stream_size, ep_tables_start_pos,
                                           ep_tables_end_pos, 0);
    if (ret < 0)
        return ret;

    ret = 0;
    switch (type) {
    case STREAM_TYPE_VIDEO_H264:
        st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        st->codecpar->codec_id = AV_CODEC_ID_H264;
        st->id = 1 << 8 | stream_id;

        st->codecpar->profile = AV_PROFILE_H264_MAIN;
        st->codecpar->level = 21;
        st->codecpar->field_order = AV_FIELD_PROGRESSIVE;
        st->codecpar->format = AV_PIX_FMT_YUV420P;
        st->codecpar->sample_aspect_ratio.num = 1;
        st->codecpar->sample_aspect_ratio.den = 1;
        st->sample_aspect_ratio.num = 1;
        st->sample_aspect_ratio.den = 1;
        st->codecpar->framerate.num = 30000;
        st->codecpar->framerate.den = 1001;
        st->avg_frame_rate.num = 30000;
        st->avg_frame_rate.den = 1001;
        st->r_frame_rate.num = 30000;
        st->r_frame_rate.den = 1001;

        st->codecpar->width = avio_r8(s->pb) * 16;
        st->codecpar->height = avio_r8(s->pb) * 16;

        avio_skip(s->pb, 2); // Unused
        break;
    case STREAM_TYPE_PSMF_AUDIO_PCM:
        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codecpar->codec_id = AV_CODEC_ID_PCM_S16LE;
        st->id = private_stream_id;
        st->codecpar->format = AV_SAMPLE_FMT_S16;
        st->codecpar->frame_size = PSMF_PCM_FRAME_SIZE;
        ret = psmf_parse_audio_info(st, s->pb);
        break;
    case STREAM_TYPE_PSMF_AUDIO_ATRAC3P:
        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codecpar->codec_id = AV_CODEC_ID_ATRAC3P_ATS;
        st->id = private_stream_id;
        st->codecpar->format = AV_SAMPLE_FMT_FLT;
        ret = psmf_parse_audio_info(st, s->pb);
        break;
    case STREAM_TYPE_PSMF_USER_DATA:
        st->codecpar->codec_type = AVMEDIA_TYPE_DATA;
        st->codecpar->codec_id = AV_CODEC_ID_BIN_DATA;
        st->id = private_stream_id;
        avio_skip(s->pb, 4); // Unused
        break;
    default:
        av_unreachable("The stream type wasn't set properly");
    }

    return ret;
}

static int pamf_parse_stream_info(AVFormatContext *s, uint64_t header_size,
                                  uint64_t stream_size,
                                  unsigned int *ep_tables_start_pos,
                                  unsigned int *ep_tables_end_pos)
{
    MpegDemuxContext *m = s->priv_data;

    const uint8_t type = avio_r8(s->pb);
    avio_skip(s->pb, 3); // Unused
    const uint8_t stream_id = avio_r8(s->pb);
    const uint8_t private_stream_id = avio_r8(s->pb);

    switch (type) {
    case STREAM_TYPE_VIDEO_MPEG2:
    case STREAM_TYPE_VIDEO_H264:
        if ((stream_id & 0xf0) != VIDEO_ID) {
            av_log(s, AV_LOG_FATAL, "Invalid stream id (%X)\n", stream_id);
            return AVERROR_INVALIDDATA;
        }
        if (m->psm_es_type[stream_id] != 0) {
            av_log(s, AV_LOG_FATAL, "Stream id %X already in use\n", stream_id);
            return AVERROR_INVALIDDATA;
        }

        m->psm_es_type[stream_id] = type;
        break;

    case STREAM_TYPE_PSMF_AUDIO_PCM:
    case STREAM_TYPE_AUDIO_AC3:
    case STREAM_TYPE_PSMF_AUDIO_ATRAC3P:
    case STREAM_TYPE_PSMF_USER_DATA:
        if (stream_id != (PRIVATE_STREAM_1 & 0xff)) {
            av_log(s, AV_LOG_FATAL, "Invalid stream id (%X)\n", stream_id);
            return AVERROR_INVALIDDATA;
        }
        if (private_stream_id >> 4 == 1 ||
            private_stream_id >> 4 > 4) {
            av_log(s, AV_LOG_FATAL, "Invalid private stream id (%X)\n", stream_id);
            return AVERROR_INVALIDDATA;
        }
        if (m->psm_es_type[private_stream_id] != 0) {
            av_log(s, AV_LOG_FATAL, "Private stream id %X already in use\n",
                   private_stream_id);
            return AVERROR_INVALIDDATA;
        }

        m->psm_es_type[private_stream_id] = type;
        break;

    default:
        av_log(s, AV_LOG_FATAL, "Invalid stream type (%X)\n", type);
        return AVERROR_INVALIDDATA;
    }

    AVStream *const st = avformat_new_stream(s, NULL);
    if (!st)
        return AVERROR(ENOMEM);

    st->time_base = (AVRational){ 1, 90000 };
    avpriv_stream_set_need_parsing(st, AVSTREAM_PARSE_FULL);

    avio_skip(s->pb, 2); // 2 bits: unused, 1 bit: P_STD_buffer_scale, 13 bits: P_STD_buffer_size

    const uint32_t entry_point_table_offset = avio_rb32(s->pb);
    const uint32_t nb_entry_points = avio_rb32(s->pb);
    int ret = psmf_parse_entry_point_table(s, st, entry_point_table_offset,
                                           nb_entry_points, header_size,
                                           stream_size, ep_tables_start_pos,
                                           ep_tables_end_pos, 1);
    if (ret < 0)
        return ret;

    ret = 0;
    switch (type) {
    case STREAM_TYPE_VIDEO_MPEG2:
        st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        st->codecpar->codec_id = AV_CODEC_ID_MPEG2VIDEO;
        st->id = 1 << 8 | stream_id;
        ret = pamf_parse_m2v_info(st, s->pb);
        break;
    case STREAM_TYPE_VIDEO_H264:
        st->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
        st->codecpar->codec_id = AV_CODEC_ID_H264;
        st->id = 1 << 8 | stream_id;
        ret = pamf_parse_avc_info(st, s->pb);
        break;
    case STREAM_TYPE_PSMF_AUDIO_PCM:
        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codecpar->codec_id = AV_CODEC_ID_PCM_PAMF;
        st->id = private_stream_id;
        ret = pamf_parse_audio_info(st, s->pb);
        break;
    case STREAM_TYPE_AUDIO_AC3:
        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codecpar->codec_id = AV_CODEC_ID_AC3;
        st->id = private_stream_id;
        ret = pamf_parse_audio_info(st, s->pb);
        break;
    case STREAM_TYPE_PSMF_AUDIO_ATRAC3P:
        st->codecpar->codec_type = AVMEDIA_TYPE_AUDIO;
        st->codecpar->codec_id = AV_CODEC_ID_ATRAC3P_ATS;
        st->id = private_stream_id;
        ret = pamf_parse_audio_info(st, s->pb);
        break;
    case STREAM_TYPE_PSMF_USER_DATA:
        st->codecpar->codec_type = AVMEDIA_TYPE_DATA;
        st->codecpar->codec_id = AV_CODEC_ID_BIN_DATA;
        st->id = private_stream_id;
        break;
    default:
        av_unreachable("The stream type should have been checked above");
    }

    return ret;
}

static int psmf_read_header(AVFormatContext *const s)
{
    PsmfContext *const ctx = s->priv_data;
    int is_pamf = 0;

    ctx->mpeg.psmf = 1;
    s->packet_size = PSMF_PACK_SIZE_ALIGN;

    uint8_t magic[4] = { 0 };
    avio_read(s->pb, magic, sizeof(magic));

    if (memcmp(magic, PAMF_MAGIC, strlen(PAMF_MAGIC)) == 0) {
        is_pamf = 1;
    } else if (memcmp(magic, PSMF_MAGIC, strlen(PSMF_MAGIC)) != 0) {
        av_log(s, AV_LOG_FATAL, "Invalid magic string (%c%c%c%c)\n",
               magic[0], magic[1], magic[2], magic[3]);
        return AVERROR_INVALIDDATA;
    }

    uint8_t version[4] = { 0 };
    avio_read(s->pb, version, sizeof(version));

    if (is_pamf &&
        memcmp(version, PAMF_VERSION_40, strlen(PAMF_VERSION_40)) != 0 &&
        memcmp(version, PAMF_VERSION_41, strlen(PAMF_VERSION_41)) != 0 ||
        !is_pamf &&
        memcmp(version, PSMF_VERSION_12, strlen(PSMF_VERSION_12)) != 0 &&
        memcmp(version, PSMF_VERSION_13, strlen(PSMF_VERSION_13)) != 0 &&
        memcmp(version, PSMF_VERSION_14, strlen(PSMF_VERSION_14)) != 0 &&
        memcmp(version, PSMF_VERSION_15, strlen(PSMF_VERSION_15)) != 0) {
        av_log(s, AV_LOG_FATAL, "Invalid version (%c%c%c%c)\n",
               version[0], version[1], version[2], version[3]);
        return AVERROR_INVALIDDATA;

    }

    const uint64_t header_size = (uint64_t)avio_rb32(s->pb) *
                                 (is_pamf ? PSMF_PACK_SIZE_ALIGN : 1);
    const uint64_t stream_size = (uint64_t)avio_rb32(s->pb) *
                                 (is_pamf ? PSMF_PACK_SIZE_ALIGN : 1);

    if (header_size == 0 || stream_size == 0 ||
        header_size + stream_size > (uint64_t)avio_size(s->pb)) {
        av_log(s, AV_LOG_FATAL, "Invalid header size (%lld) or stream size (%lld)\n",
               header_size, stream_size);
        return AVERROR_INVALIDDATA;
    }

    const uint32_t psmf_marks_offset = avio_rb32(s->pb);
    const uint32_t psmf_marks_size = avio_rb32(s->pb);
    const uint32_t unk_offset = avio_rb32(s->pb);
    const uint32_t unk_size = avio_rb32(s->pb);

    if (psmf_marks_offset == 0 && psmf_marks_size != 0 ||
        psmf_marks_offset != 0 && psmf_marks_size == 0 ||
        psmf_marks_offset + psmf_marks_size > header_size) {
        av_log(s, AV_LOG_FATAL, "Invalid PSMF marks table offset (%X) or size (%d)\n",
               psmf_marks_offset, psmf_marks_size);
        return AVERROR_INVALIDDATA;
    }

    if (unk_offset != 0 || unk_size != 0)
        return AVERROR_INVALIDDATA;

    avio_skip(s->pb, 0x30); // Unused

    const uint64_t seq_info_offset = avio_tell(s->pb);

    const uint32_t seq_info_size = avio_rb32(s->pb);

    if (seq_info_offset + sizeof(uint32_t) + seq_info_size > header_size) {
        av_log(s, AV_LOG_FATAL, "Invalid sequence info size (%d)\n",
               seq_info_size);
        return AVERROR_INVALIDDATA;
    }

    if (is_pamf)
        avio_rb16(s->pb); // Unused

    const int64_t start_pts = (int64_t)avio_rb16(s->pb) << 32 | avio_rb32(s->pb);
    const int64_t end_pts = (int64_t)avio_rb16(s->pb) << 32 | avio_rb32(s->pb);

    if (end_pts >= UINT32_MAX || start_pts >= end_pts) {
        av_log(s, AV_LOG_FATAL, "Invalid start time stamp (%lld) or end time stamp (%lld)\n",
               start_pts, end_pts);
        return AVERROR_INVALIDDATA;
    }

    s->duration = (end_pts - start_pts) * AV_TIME_BASE / 90000;

    av_unused const uint32_t mux_rate_bound = avio_rb32(s->pb);
    av_unused const uint32_t std_delay = avio_rb32(s->pb);

    av_unused const uint32_t nb_streams = is_pamf ? avio_rb32(s->pb) : avio_r8(s->pb);

    if (is_pamf)
        avio_skip(s->pb, 1); // Unused

    const uint8_t nb_grp_periods = avio_r8(s->pb);

    if (nb_grp_periods != 1) {
        av_log(s, AV_LOG_FATAL, "Invalid number of grouping periods (%d)\n",
               nb_grp_periods);
        return AVERROR_INVALIDDATA;
    }

    const uint64_t grp_period_offset = avio_tell(s->pb);

    const uint32_t grp_period_size = avio_rb32(s->pb);

    if (grp_period_offset + sizeof(uint32_t) + grp_period_size > header_size) {
        av_log(s, AV_LOG_FATAL, "Invalid grouping period size (%d)\n",
               grp_period_size);
        return AVERROR_INVALIDDATA;
    }

    const int64_t grp_period_start_pts = (int64_t)avio_rb16(s->pb) << 32 | avio_rb32(s->pb);
    const int64_t grp_period_end_pts = (int64_t)avio_rb16(s->pb) << 32 | avio_rb32(s->pb);

    if (grp_period_end_pts >= UINT32_MAX ||
        grp_period_start_pts >= grp_period_end_pts ||
        grp_period_start_pts != start_pts) {
        av_log(s, AV_LOG_FATAL, "Invalid grouping period start time stamp (%lld)"
               " or end time stamp (%lld)\n",
               grp_period_start_pts, grp_period_end_pts);
        return AVERROR_INVALIDDATA;
    }

    avio_skip(s->pb, 1); // Unused

    const uint8_t nb_grps = avio_r8(s->pb);

    if (nb_grps != 1) {
        av_log(s, AV_LOG_FATAL, "Invalid number of groups (%d)\n", nb_grps);
        return AVERROR_INVALIDDATA;
    }

    const uint64_t grp_offset = avio_tell(s->pb);

    const uint32_t grp_size = avio_rb32(s->pb);

    if (grp_offset + sizeof(uint32_t) + grp_size > header_size) {
        av_log(s, AV_LOG_FATAL, "Invalid group size (%d)\n", grp_size);
        return AVERROR_INVALIDDATA;
    }

    avio_skip(s->pb, 1); // Unused

    const uint8_t grp_nb_streams = avio_r8(s->pb);

    if (grp_nb_streams == 0 || grp_nb_streams != nb_streams) {
        av_log(s, AV_LOG_FATAL, "Invalid number of streams in the group (%d)\n",
               grp_nb_streams);
        return AVERROR_INVALIDDATA;
    }

    unsigned int ep_tables_start_pos = 0;
    unsigned int ep_tables_end_pos = 0;
    for (int i = 0; i < grp_nb_streams; i++) {
        const int ret = is_pamf
            ? pamf_parse_stream_info(s, header_size, stream_size,
                                     &ep_tables_start_pos, &ep_tables_end_pos)
            : psmf_parse_stream_info(s, header_size, stream_size,
                                     &ep_tables_start_pos, &ep_tables_end_pos);
        if (ret < 0)
            return ret;

        AVStream *const st = s->streams[i];
        switch (st->codecpar->codec_type) {
        case AVMEDIA_TYPE_VIDEO:
            st->priv_data = av_mallocz(sizeof(PsmfVideoStreamContext));
            if (!st->priv_data)
                return AVERROR(ENOMEM);
            break;
        case AVMEDIA_TYPE_AUDIO:
            st->priv_data = av_mallocz(sizeof(PsmfAudioStreamContext));
            if (!st->priv_data)
                return AVERROR(ENOMEM);
        }
    }

    if (ep_tables_start_pos != 0) {
        if (avio_tell(s->pb) != ep_tables_start_pos) {
            av_log(s, AV_LOG_FATAL, "Entry point table not immediately after stream info\n");
            return AVERROR_INVALIDDATA;
        }

        avio_seek(s->pb, ep_tables_end_pos, SEEK_SET);
    }

    if (psmf_marks_offset) {
        if (psmf_marks_offset != avio_tell(s->pb)) {
            av_log(s, AV_LOG_FATAL, "Invalid PSMF marks offset (%X)\n",
                    psmf_marks_offset);
            return AVERROR_INVALIDDATA;
        }

        const uint32_t psmf_marks_size2 = avio_rb32(s->pb);
        if (psmf_marks_size2 + sizeof(uint32_t) != psmf_marks_size) {
            av_log(s, AV_LOG_FATAL, "Invalid PSMF marks size (%d)\n",
                   psmf_marks_size2);
            return AVERROR_INVALIDDATA;
        }

        const uint16_t nb_psmf_marks = avio_rb16(s->pb);
        if (nb_psmf_marks * 0x28 + sizeof(uint16_t) != psmf_marks_size2) {
            av_log(s, AV_LOG_FATAL, "Invalid number of PSMF marks (%d)\n",
                   nb_psmf_marks);
            return AVERROR_INVALIDDATA;
        }

        for (int i = 0; i < nb_psmf_marks; i++) {
            const AVRational time_base = { 1, 90000 };
            const uint8_t type = avio_r8(s->pb);
            const uint8_t name_length = avio_r8(s->pb);
            if (name_length > 24) {
                av_log(s, AV_LOG_FATAL, "Inavlid PSMF mark name length (%d)",
                       name_length);
                return AVERROR_INVALIDDATA;
            }

            const uint64_t pts = avio_rb64(s->pb);
            if (pts >= UINT32_MAX || pts < (uint64_t)start_pts ||
                pts > (uint64_t)end_pts) {
                av_log(s, AV_LOG_FATAL, "Invalid PSMF mark time stamp (%lld)", pts);
                return AVERROR_INVALIDDATA;
            }

            av_unused const uint8_t stream_id = avio_r8(s->pb);
            av_unused const uint8_t private_stream_id = avio_r8(s->pb);
            uint8_t mark_data[4];
            avio_read(s->pb, mark_data, 4);

            char name[25];
            avio_read(s->pb, name, 24);
            name[24] = '\0';

            if (type == 5)
                if (!avpriv_new_chapter(s, i, time_base, pts, AV_NOPTS_VALUE, name))
                    return AVERROR_INVALIDDATA;
        }
    }

    avio_seek(s->pb, header_size, SEEK_SET);
    ctx->prev_position = avio_tell(s->pb);

    return 0;
}

static int psmf_read_packet(AVFormatContext *s, AVPacket *pkt)
{
    PsmfContext *const ctx = s->priv_data;

    const int discontinuity = avio_tell(s->pb) != ctx->prev_position ||
                              s->io_repositioned;

    // Make sure to always start demuxing at the start of a pack since
    // startcodes can appear in the middle of PES packets
    if (discontinuity)
        avio_seek(s->pb, avio_tell(s->pb) & ~(PSMF_PACK_SIZE_ALIGN - 1), SEEK_SET);

    for (unsigned int i = 0; i < s->nb_streams; i++) {
        AVStream *const st = s->streams[i];
        AVCodecParserContext **const parser = &ffstream(st)->parser;

        if (st->discard >= AVDISCARD_ALL || discontinuity) {
            switch (st->codecpar->codec_type) {
            case AVMEDIA_TYPE_VIDEO:
                ((PsmfVideoStreamContext *)st->priv_data)->keyframe = 0;
                break;
            case AVMEDIA_TYPE_AUDIO:
                ((PsmfAudioStreamContext *)st->priv_data)->continuity = 0;
            }

            if (*parser) {
                av_parser_close(*parser);
                *parser = NULL;
            }
        }
    }

    const int ret = mpegps_read_packet(s, pkt);

    ctx->prev_position = avio_tell(s->pb);

    return ret;
}

const FFInputFormat ff_psmf_demuxer = {
    .p.name         = "psmf",
    .p.long_name    = NULL_IF_CONFIG_SMALL("PSP/PlayStation Advanced Movie Format"),
    .priv_data_size = sizeof(PsmfContext),
    .flags_internal = FF_INFMT_FLAG_INIT_CLEANUP,
    .read_probe     = psmf_probe,
    .read_header    = psmf_read_header,
    .read_packet    = psmf_read_packet,
    .read_timestamp = mpegps_read_dts
};

#endif /* AVFORMAT_PSMF_H */
