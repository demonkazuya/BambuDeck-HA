#pragma once

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <lvgl.h>
#include <rom/miniz.h>
#include "filesystem.h"
#include "types.h"
#include "debug.h"
#include "../ui/ui.h"
#include "ha_api.h"

// Decoded once at 120x120 (RGB565, alpha pre-blended onto the card colour to save RAM).
// Shown natively on the idle card, and zoomed to 80x80 on the printing card.
#define BAMBUDECK_THUMB_W 120
#define BAMBUDECK_THUMB_H 120
#define BAMBUDECK_THUMB_PX_BYTES 2
#define BAMBUDECK_THUMB_PRINTING_ZOOM 171 // 80px / 120px * 256

static uint8_t bambudeck_thumb_pixels[BAMBUDECK_THUMB_W * BAMBUDECK_THUMB_H * BAMBUDECK_THUMB_PX_BYTES];
static lv_img_dsc_t bambudeck_thumb_dsc;
static volatile bool bambudeck_has_thumb = false;
static char bambudeck_last_thumb_key[128] = "";
static int bambudeck_thumb_retry_count = 0;

struct BambuStreamingPngCtx
{
    tinfl_decompressor inflator;
    uint8_t dict[TINFL_LZ_DICT_SIZE];
    size_t dict_ofs;

    uint32_t width;
    uint32_t height;
    uint8_t bit_depth;
    uint8_t color_type;
    uint8_t interlace;
    uint8_t bpp;
    uint32_t row_bytes;

    uint8_t prev_row[2052];
    uint8_t curr_row[2052];
    uint32_t row_pos;
    uint32_t cur_y;

    uint8_t palette[256 * 3];
    uint8_t trns[256];
    uint16_t palette_len;
    uint16_t trns_len;

    bool has_ihdr;
    bool done;
    bool error;
};

static inline uint8_t bambudeck_paeth(uint8_t a, uint8_t b, uint8_t c)
{
    int p = (int)a + (int)b - (int)c;
    int pa = abs(p - (int)a);
    int pb = abs(p - (int)b);
    int pc = abs(p - (int)c);
    if (pa <= pb && pa <= pc)
        return a;
    if (pb <= pc)
        return b;
    return c;
}

static void bambudeck_png_render_scanline(BambuStreamingPngCtx *ctx)
{
    if (ctx->cur_y >= ctx->height || ctx->width == 0 || ctx->height == 0)
        return;

    uint8_t filter = ctx->curr_row[0];
    uint8_t *raw = &ctx->curr_row[1];
    uint8_t *prev = &ctx->prev_row[1];
    uint32_t rb = ctx->row_bytes;
    uint8_t bpp = ctx->bpp;

    for (uint32_t i = 0; i < rb; i++)
    {
        uint8_t a = (i >= bpp) ? raw[i - bpp] : 0;
        uint8_t b = prev[i];
        uint8_t c = (i >= bpp) ? prev[i - bpp] : 0;
        switch (filter)
        {
        case 1: // Sub
            raw[i] = (uint8_t)(raw[i] + a);
            break;
        case 2: // Up
            raw[i] = (uint8_t)(raw[i] + b);
            break;
        case 3: // Average
            raw[i] = (uint8_t)(raw[i] + (((int)a + (int)b) >> 1));
            break;
        case 4: // Paeth
            raw[i] = (uint8_t)(raw[i] + bambudeck_paeth(a, b, c));
            break;
        default: // None (0)
            break;
        }
    }

    // Downscale rows onto BAMBUDECK_THUMB_H target rows
    uint32_t bg = BAMBU_BG_CARD_HEX();
    uint8_t bgR = (bg >> 16) & 0xFF, bgG = (bg >> 8) & 0xFF, bgB = bg & 0xFF;
    uint32_t dy_start = (ctx->cur_y * BAMBUDECK_THUMB_H) / ctx->height;
    uint32_t dy_end = ((ctx->cur_y + 1) * BAMBUDECK_THUMB_H) / ctx->height;
    if (dy_end <= dy_start && ((ctx->cur_y * BAMBUDECK_THUMB_H) % ctx->height) == 0)
    {
        dy_end = dy_start + 1;
    }

    for (uint32_t dy = dy_start; dy < dy_end && dy < BAMBUDECK_THUMB_H; dy++)
    {
        for (uint32_t dx = 0; dx < BAMBUDECK_THUMB_W; dx++)
        {
            uint32_t sx = (dx * ctx->width) / BAMBUDECK_THUMB_W;
            if (sx >= ctx->width)
                sx = ctx->width - 1;

            uint8_t r = 0, g = 0, b = 0, a = 255;
            if (ctx->color_type == 6) // RGBA8
            {
                uint32_t idx = sx * 4;
                r = raw[idx];
                g = raw[idx + 1];
                b = raw[idx + 2];
                a = raw[idx + 3];
            }
            else if (ctx->color_type == 2) // RGB8
            {
                uint32_t idx = sx * 3;
                r = raw[idx];
                g = raw[idx + 1];
                b = raw[idx + 2];
            }
            else if (ctx->color_type == 3) // Indexed Palette
            {
                uint8_t pidx = raw[sx];
                if (pidx < ctx->palette_len)
                {
                    r = ctx->palette[pidx * 3];
                    g = ctx->palette[pidx * 3 + 1];
                    b = ctx->palette[pidx * 3 + 2];
                }
                if (pidx < ctx->trns_len)
                {
                    a = ctx->trns[pidx];
                }
            }
            else if (ctx->color_type == 4) // Grayscale + Alpha
            {
                uint32_t idx = sx * 2;
                r = g = b = raw[idx];
                a = raw[idx + 1];
            }
            else if (ctx->color_type == 0) // Grayscale
            {
                r = g = b = raw[sx];
            }

            if (a < 255)
            {
                r = (uint8_t)((r * a + bgR * (255 - a)) / 255);
                g = (uint8_t)((g * a + bgG * (255 - a)) / 255);
                b = (uint8_t)((b * a + bgB * (255 - a)) / 255);
            }
            lv_color_t c = lv_color_make(r, g, b);
            uint32_t dst_idx = (dy * BAMBUDECK_THUMB_W + dx) * BAMBUDECK_THUMB_PX_BYTES;
            bambudeck_thumb_pixels[dst_idx] = (uint8_t)(c.full & 0xFF);
            bambudeck_thumb_pixels[dst_idx + 1] = (uint8_t)((c.full >> 8) & 0xFF);
        }
    }

    memcpy(ctx->prev_row, ctx->curr_row, rb + 1);
    ctx->cur_y++;
    if (ctx->cur_y >= ctx->height)
    {
        ctx->done = true;
    }
}

static void bambudeck_png_feed_idat(BambuStreamingPngCtx *ctx, const uint8_t *data, size_t len)
{
    if (!ctx->has_ihdr || ctx->error || ctx->done)
        return;

    size_t in_pos = 0;
    while (in_pos < len && !ctx->error && !ctx->done)
    {
        size_t in_bytes = len - in_pos;
        size_t out_bytes = TINFL_LZ_DICT_SIZE - ctx->dict_ofs;

        tinfl_status status = tinfl_decompress(
            &ctx->inflator,
            data + in_pos,
            &in_bytes,
            ctx->dict,
            ctx->dict + ctx->dict_ofs,
            &out_bytes,
            TINFL_FLAG_PARSE_ZLIB_HEADER | TINFL_FLAG_HAS_MORE_INPUT);

        in_pos += in_bytes;

        if (out_bytes > 0)
        {
            const uint8_t *decomp = ctx->dict + ctx->dict_ofs;
            uint32_t target_len = ctx->row_bytes + 1;
            for (size_t i = 0; i < out_bytes; i++)
            {
                ctx->curr_row[ctx->row_pos++] = decomp[i];
                if (ctx->row_pos == target_len)
                {
                    bambudeck_png_render_scanline(ctx);
                    ctx->row_pos = 0;
                    if (ctx->done)
                        break;
                }
            }
            ctx->dict_ofs = (ctx->dict_ofs + out_bytes) & (TINFL_LZ_DICT_SIZE - 1);
        }

        if (status < 0)
        {
            ctx->error = true;
            break;
        }
        if (status == TINFL_STATUS_DONE)
        {
            ctx->done = true;
            break;
        }
    }
}

static inline uint32_t bambudeck_read_be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

static bool bambudeck_stream_read_exact(WiFiClient *stream, uint8_t *buf, size_t len, uint32_t timeoutMs = 4000)
{
    size_t got = 0;
    unsigned long start = millis();
    while (got < len)
    {
        int avail = stream->available();
        if (avail > 0)
        {
            int r = stream->read(buf + got, len - got);
            if (r > 0)
            {
                got += (size_t)r;
                start = millis();
                continue;
            }
        }
        if (!stream->connected() && stream->available() <= 0)
            return false;
        if (millis() - start > timeoutMs)
            return false;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    return true;
}

static bool bambudeck_stream_skip_exact(WiFiClient *stream, size_t len, uint32_t timeoutMs = 4000)
{
    uint8_t discard[128];
    size_t left = len;
    while (left > 0)
    {
        size_t chunk = (left > sizeof(discard)) ? sizeof(discard) : left;
        if (!bambudeck_stream_read_exact(stream, discard, chunk, timeoutMs))
            return false;
        left -= chunk;
    }
    return true;
}

inline void bambudeck_thumbnail_init()
{
    if (LittleFS.exists("/bambudeck/cover.jpg"))
        LittleFS.remove("/bambudeck/cover.jpg");
    if (LittleFS.exists("/bambudeck/cover.png"))
        LittleFS.remove("/bambudeck/cover.png");

    memset(&bambudeck_thumb_dsc, 0, sizeof(bambudeck_thumb_dsc));
    bambudeck_thumb_dsc.header.always_zero = 0;
    bambudeck_thumb_dsc.header.w = BAMBUDECK_THUMB_W;
    bambudeck_thumb_dsc.header.h = BAMBUDECK_THUMB_H;
    bambudeck_thumb_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
    bambudeck_thumb_dsc.data_size = sizeof(bambudeck_thumb_pixels);
    bambudeck_thumb_dsc.data = bambudeck_thumb_pixels;
}

inline void bambudeck_thumbnail_clear()
{
    bambudeck_has_thumb = false;
    bambudeck_last_thumb_key[0] = '\0';
    bambudeck_thumb_retry_count = 0;
}

// Called when the theme changes so the thumbnail is re-decoded against the new card colour
extern "C" void bambudeck_thumbnail_invalidate(void)
{
    bambudeck_thumbnail_clear();
    bambudeck_ha_poll_now = true;
}

extern "C" void bambudeck_thumbnail_update_ui()
{
    if (bambuDeckConfig.currentScreenIndex != 0)
        return;

    bambudeck_update_ams_ui_colors();

    if (ui_homeComponent != NULL)
    {
        lv_obj_t *nozzleIcon = ui_comp_get_child(ui_homeComponent, UI_COMP_HOMECOMPONENT_MAINSCREENLEFT_MAINSCREENPLAYER_MAINSCREENCONTROLLER_MAINSCREENNOZZLEICON);
        if (nozzleIcon != NULL)
        {
            if (bambudeck_has_thumb)
            {
                lv_img_set_src(nozzleIcon, &bambudeck_thumb_dsc);
                lv_img_set_pivot(nozzleIcon, 0, 0);
                lv_img_set_zoom(nozzleIcon, BAMBUDECK_THUMB_PRINTING_ZOOM);
                lv_obj_invalidate(nozzleIcon);
            }
            else
            {
                lv_img_set_zoom(nozzleIcon, LV_IMG_ZOOM_NONE);
                lv_img_set_src(nozzleIcon, LV_SYMBOL_IMAGE);
            }
        }
    }

    if (ui_mainStatusComponent != NULL)
    {
        lv_obj_t *statusLogo = ui_comp_get_child(ui_mainStatusComponent, UI_COMP_MAINSCREENSTATUS_MAINSCREENSTATUSLOGO);
        lv_obj_t *statusThumb = ui_comp_get_child(ui_mainStatusComponent, UI_COMP_MAINSCREENSTATUS_MAINSCREENSTATUSTHUMBNAIL);
        if (statusLogo != NULL && statusThumb != NULL)
        {
            if (bambudeck_has_thumb)
            {
                lv_img_set_src(statusThumb, &bambudeck_thumb_dsc);
                lv_obj_clear_flag(statusThumb, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(statusLogo, LV_OBJ_FLAG_HIDDEN);
                lv_obj_invalidate(statusThumb);
            }
            else
            {
                lv_obj_add_flag(statusThumb, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(statusLogo, LV_OBJ_FLAG_HIDDEN);
            }
        }
    }
}

inline void bambudeck_thumbnail_check_download()
{
    if (!bambudeck_use_ha_direct() || WiFi.status() != WL_CONNECTED)
        return;

    if (strlen(bambudeck_ha_cover_state) == 0 || strlen(bambudeck_ha_cover_entity) == 0)
    {
        if (bambudeck_has_thumb)
        {
            bambudeck_has_thumb = false;
            bambudeck_last_thumb_key[0] = '\0';
            bambudeck_ha_ui_dirty = true;
        }
        return;
    }

    char currentKey[128];
    snprintf(currentKey, sizeof(currentKey), "%s|%s", bambudeck_ha_cover_entity, bambudeck_ha_cover_state);

    if (strcmp(currentKey, bambudeck_last_thumb_key) == 0)
    {
        if (bambudeck_has_thumb || bambudeck_thumb_retry_count >= 2)
            return;
    }
    else
    {
        strncpy(bambudeck_last_thumb_key, currentKey, sizeof(bambudeck_last_thumb_key) - 1);
        bambudeck_thumb_retry_count = 0;
    }

    bambudeck_thumb_retry_count++;

    if (ESP.getFreeHeap() < 55000)
    {
        ConsoleError.printf("[BambuDeck][Thumb] Skipping thumbnail decode, low heap (%u bytes)\n", ESP.getFreeHeap());
        return;
    }

    bambudeck_ha_init_mutex();
    if (xSemaphoreTake(bambudeck_ha_mutex, pdMS_TO_TICKS(5000)) != pdTRUE)
        return;

    BambuStreamingPngCtx *ctx = (BambuStreamingPngCtx *)calloc(1, sizeof(BambuStreamingPngCtx));
    if (ctx == NULL)
    {
        ConsoleError.println("[BambuDeck][Thumb] Failed to allocate streaming PNG context");
        xSemaphoreGive(bambudeck_ha_mutex);
        return;
    }

    tinfl_init(&ctx->inflator);
    memset(ctx->trns, 255, sizeof(ctx->trns));

    String path = String("/api/image_proxy/") + String(bambudeck_ha_cover_entity);
    String fullUrl = bambudeck_ha_base_url() + path;

    HTTPClient http;
    WiFiClient client;
    http.setTimeout(5000);

    bool ok = false;
    if (http.begin(client, fullUrl))
    {
        http.addHeader("Authorization", String("Bearer ") + String(bambuDeckConfig.haToken));
        int code = http.GET();
        if (code == 200)
        {
            WiFiClient *stream = http.getStreamPtr();
            uint8_t sig[8];
            const uint8_t png_sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
            if (bambudeck_stream_read_exact(stream, sig, 8) && memcmp(sig, png_sig, 8) == 0)
            {
                memset(bambudeck_thumb_pixels, 0, sizeof(bambudeck_thumb_pixels));

                while (!ctx->done && !ctx->error)
                {
                    uint8_t chdr[8];
                    if (!bambudeck_stream_read_exact(stream, chdr, 8))
                        break;

                    uint32_t clen = bambudeck_read_be32(chdr);
                    const uint8_t *ctype = &chdr[4];

                    if (memcmp(ctype, "IHDR", 4) == 0 && clen == 13)
                    {
                        uint8_t ihdr[13];
                        if (!bambudeck_stream_read_exact(stream, ihdr, 13))
                            break;
                        ctx->width = bambudeck_read_be32(&ihdr[0]);
                        ctx->height = bambudeck_read_be32(&ihdr[4]);
                        ctx->bit_depth = ihdr[8];
                        ctx->color_type = ihdr[9];
                        ctx->interlace = ihdr[12];

                        uint8_t channels = 0;
                        if (ctx->color_type == 6)
                            channels = 4;
                        else if (ctx->color_type == 2)
                            channels = 3;
                        else if (ctx->color_type == 4)
                            channels = 2;
                        else if (ctx->color_type == 0 || ctx->color_type == 3)
                            channels = 1;

                        ctx->bpp = channels;
                        ctx->row_bytes = ctx->width * channels;

                        if (ctx->bit_depth != 8 || ctx->interlace != 0 || channels == 0 || ctx->width == 0 || ctx->height == 0 || ctx->row_bytes > 2048)
                        {
                            ConsoleError.printf("[BambuDeck][Thumb] Unsupported PNG format %ux%u depth=%u type=%u\n",
                                                ctx->width, ctx->height, ctx->bit_depth, ctx->color_type);
                            ctx->error = true;
                            break;
                        }
                        ctx->has_ihdr = true;
                        bambudeck_stream_skip_exact(stream, 4); // CRC
                    }
                    else if (memcmp(ctype, "PLTE", 4) == 0 && clen <= 768)
                    {
                        if (!bambudeck_stream_read_exact(stream, ctx->palette, clen))
                            break;
                        ctx->palette_len = clen / 3;
                        bambudeck_stream_skip_exact(stream, 4); // CRC
                    }
                    else if (memcmp(ctype, "tRNS", 4) == 0 && clen <= 256)
                    {
                        if (!bambudeck_stream_read_exact(stream, ctx->trns, clen))
                            break;
                        ctx->trns_len = clen;
                        bambudeck_stream_skip_exact(stream, 4); // CRC
                    }
                    else if (memcmp(ctype, "IDAT", 4) == 0)
                    {
                        uint8_t buf[256];
                        uint32_t rem = clen;
                        while (rem > 0)
                        {
                            uint32_t toRead = (rem > sizeof(buf)) ? sizeof(buf) : rem;
                            if (!bambudeck_stream_read_exact(stream, buf, toRead))
                            {
                                ctx->error = true;
                                break;
                            }
                            bambudeck_png_feed_idat(ctx, buf, toRead);
                            rem -= toRead;
                        }
                        if (!bambudeck_stream_skip_exact(stream, 4)) // CRC
                            break;
                    }
                    else if (memcmp(ctype, "IEND", 4) == 0)
                    {
                        bambudeck_stream_skip_exact(stream, clen + 4);
                        break;
                    }
                    else
                    {
                        if (!bambudeck_stream_skip_exact(stream, clen + 4))
                            break;
                    }
                }

                if (ctx->has_ihdr && !ctx->error && ctx->cur_y > 0)
                {
                    ok = true;
                    ConsoleInfo.printf("[BambuDeck][Thumb] Decoded %ux%u STL cover image -> 120x120 (%u bytes free heap)\n",
                                       ctx->width, ctx->height, ESP.getFreeHeap());
                }
            }
        }
        else
        {
            ConsoleError.printf("[BambuDeck][Thumb] GET %s returned HTTP %d\n", path.c_str(), code);
        }
        http.end();
    }

    free(ctx);
    xSemaphoreGive(bambudeck_ha_mutex);

    if (ok)
    {
        bambudeck_has_thumb = true;
        bambudeck_ha_ui_dirty = true;
    }
}
