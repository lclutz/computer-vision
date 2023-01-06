#pragma once

#include <SDL.h>

#include <opencv2/opencv.hpp>

SDL_Texture *bgr_mat_to_sdl_texture(const cv::Mat &image_bgr, SDL_Renderer *renderer, SDL_Texture *texture);

void paint_sdL_texture_to_renderer(SDL_Renderer *renderer, SDL_Window *window, SDL_Texture *texture);
