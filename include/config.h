#pragma once

#include "tgaimage.h"

// color (attention, BGRA order)
// clang-format off
constexpr TGAColor BLACK    = {   0,   0,   0, 255 };
constexpr TGAColor GRAY     = { 127, 127, 127, 255 };
constexpr TGAColor RED      = {   0,   0, 255, 255 };
constexpr TGAColor GREEN    = {   0, 255,   0, 255 };
constexpr TGAColor BLUE     = { 255,   0,   0, 255 };
constexpr TGAColor CYAN     = { 255, 255,   0, 255 };
constexpr TGAColor MAGENTA  = { 255,   0, 255, 255 };
constexpr TGAColor YELLOW   = {   0, 255, 255, 255 };
constexpr TGAColor PINK     = { 127, 127, 255, 255 };
constexpr TGAColor MINT     = { 127, 255, 127, 255 };
constexpr TGAColor SKY_BLUE = { 255, 127, 127, 255 };
constexpr TGAColor WHITE    = { 255, 255, 255, 255 };
// clang-format on

// model path
const std::vector<std::string> DIABLO_PATH = {
    "../assets/diablo3_pose.obj",
};
const std::vector<std::string> BOGGIE_PATHS = {
    "../assets/boggie/body.obj", "../assets/boggie/eyes.obj", "../assets/boggie/head.obj"};
const std::vector<std::string> AFRICAN_HEAD_PATHS = {
    "../assets/african_head/african_head.obj",
    "../assets/african_head/african_head_eye_inner.obj",
    "../assets/african_head/african_head_eye_outer.obj"};

constexpr int WIDTH  = 1000;
constexpr int HEIGHT = 1000;

constexpr int NUM_FRAMES = 96;
