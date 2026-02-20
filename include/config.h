#pragma once

#include "tgaimage.h"

// attention, BGRA order
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

constexpr int WIDTH  = 1000;
constexpr int HEIGHT = 1000;

constexpr int NUM_FRAMES = 96;
