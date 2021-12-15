#pragma once
#include <string>
#include <tuple>

struct BrainAliveInputParams {
  Core::String channel_1;
  Core::String channel_2;
  Core::String channel_3;
  Core::String channel_4;
  Core::String channel_5;
  Core::String channel_6;
  Core::String channel_7;
  Core::String channel_8;

  BrainAliveInputParams() {
    channel_1 = "F7";
    channel_2 = "FT7";
    channel_3 = "T7";
    channel_4 = "TP7";
    channel_5 = "Cz";
    channel_6 = "C4";
    channel_7 = "FC4";
    channel_8 = "F4";
  }
};