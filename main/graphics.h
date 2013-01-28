#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "RagDrawer.h"
#include "RagAnim.h"

#define WIN1_HEIGHT 600
#define WIN1_WIDTH 640
#define CWIN_HEIGHT 100
#define CWIN_WIDTH 640
#define SWIN_WIDTH 260
#define SWIN_HEIGHT 700

#define TILE_SIZE 32

enum peType{P_NONE, P_DARKDUST, P_ARROW};
enum align{LEFT, CENTER, RIGHT};

//it may be worth noting that all these colors are fully transparent
const color red        = {255,   0,   0, 0};  const color scarlet    = {255,  63,   0, 255};  const color orange     = {255, 127,   0, 0};
const color amber      = {255, 191,   0, 0};  const color yellow     = {255, 255,   0, 0};  const color lime       = {191, 255,   0, 0};
const color chartreuse = {127, 255,   0, 0};  const color harlequin  = { 63, 255,   0, 0};  const color green      = {  0, 255,   0, 0};
const color jade       = {  0, 255,  63, 0};  const color spring     = {  0, 255, 127, 0};  const color turquoise  = {  0, 255, 191, 0};
const color cyan       = {  0, 255, 255, 0};  const color aqua       = {  0, 191, 255, 0};  const color azure      = {  0, 127, 255, 0};
const color cobalt     = {  0,  63, 255, 0};  const color blue       = {  0,   0, 255, 0};  const color iris       = { 63,   0, 255, 0};
const color violet     = {127,   0, 255, 0};  const color amethyst   = {191,   0, 255, 0};  const color magenta    = {255,   0, 255, 0};
const color fuchsia    = {255,   0, 191, 0};  const color rose       = {255,   0, 127, 0};  const color crimson    = {255,   0,  63, 0};

const color maroon     = {127,   0,   0, 0};  const color brick      = {191,   0,   0, 0};  const color pink       = {255, 127, 127, 0};  const color salmon     = {191,  63,  63, 0};
const color olive      = {127, 127,   0, 0};  const color cream      = {255, 255, 127, 0};  const color olivine    = {191, 191,  63, 0};
const color forest     = {  0, 127,   0, 0};  const color grass      = {127, 255, 127, 0};  const color fern       = { 63, 191,  63, 0};
const color teal       = {  0, 127, 127, 0};  const color celeste    = {127, 255, 255, 0};  const color aquamarine = { 63, 191, 191, 0};
const color navy       = {  0,   0, 127, 0};  const color sky        = {127, 127, 255, 0};  const color glaucous   = { 63,  63, 191, 0};
const color purple     = {127,   0, 127, 0};  const color thistle    = {255, 127, 255, 0};  const color lavender   = {191,  63, 191, 0};

const color brown      = {127,  63,   0, 0};  const color swamp      = {  0, 127,  63, 0};  const color indigo     = { 63,   0, 127, 0};
const color coral      = {255, 127,  63, 0};  const color blellow    = { 63, 255, 127, 0};  const color slue       = {127,  63, 255, 0};
const color tann       = {223, 191, 127, 0};  const color orchid     = {218, 127, 218, 0};
const color white      = {255, 255, 255, 0};  const color shine      = {223, 223, 223, 0};  const color silver     = {191, 191, 191, 0};  const color gray       = {127, 127, 127, 0};
const color grey       = { 63,  63,  63, 0};  const color charcoal   = { 31,  31,  31, 0};  const color tar        = { 15,  15,  15, 255};  const color black      = {  0,   0,   0, 255};

#endif // GRAPHICS_H_INCLUDED
