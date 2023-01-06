#include "sdl_opencv_helper.h"
#include "logging.h"

SDL_Texture *
bgr_mat_to_sdl_texture(
    const cv::Mat &image_bgr,
    SDL_Renderer *renderer,
    SDL_Texture *texture)
{
    SDL_Texture *result;
    bool texture_size_fits = false;
    if (texture) {
        int texture_width, texture_height;
        SDL_QueryTexture(texture, 0, 0, &texture_width, &texture_height);
        texture_size_fits =
            (texture_width == image_bgr.cols) && (texture_height == image_bgr.rows);
    }

    if (texture && texture_size_fits) {
        result = texture;
    } else {
        if (texture) { SDL_DestroyTexture(texture); }
        result = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_BGR24,
            SDL_TEXTUREACCESS_STREAMING,
            image_bgr.cols, image_bgr.rows);
    }

    if (!result) {
        logw("Failed to create output texture: %s", SDL_GetError());
    }

    if (SDL_UpdateTexture(result, 0, image_bgr.data, 3*image_bgr.cols) != 0) {
        logw("Failed to update output texture: %s", SDL_GetError());
    }

    return result;
}

void
paint_sdL_texture_to_renderer(
    SDL_Renderer *renderer,
    SDL_Window *window,
    SDL_Texture *texture)
{
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    float window_aspect_ratio = static_cast<float>(window_width) / static_cast<float>(window_height);

    int texture_width, texture_height;
    SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);
    float texture_aspect_ratio = static_cast<float>(texture_width) / static_cast<float>(texture_height);

    SDL_Rect dstrect;

    if (window_aspect_ratio <= texture_aspect_ratio)
    {
        int height =
            static_cast<int>(
                roundf(
                    static_cast<float>(window_width) *
                    static_cast<float>(texture_height) /
                    static_cast<float>(texture_width)
                )
            );

        dstrect.x = 0;
        dstrect.y = (window_height - height) / 2;
        dstrect.w = window_width;
        dstrect.h = height;
    } else {
        int width =
            static_cast<int>(
                roundf(
                    static_cast<float>(window_height) *
                    static_cast<float>(texture_width) /
                    static_cast<float>(texture_height)
                )
            );

        dstrect.x = (window_width - width) / 2;
        dstrect.y = 0;
        dstrect.w = width;
        dstrect.h = window_height;
    }

    SDL_RenderCopyEx(
        renderer,
        texture,
        0,        // source SDL_Rect or NULL for thhe entire texture
        &dstrect, // destination SDL_Rect or NULL for the entire rendering target
        0.0,      // angle of clockwise rotation
        0,        // center of rotation or NULL for rotating around the center of dstrect
        SDL_FLIP_NONE
    );
}
