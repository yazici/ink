/*
 * Copyright 2018 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef INK_ENGINE_INPUT_SINPUT_TEST_HELPERS_H_
#define INK_ENGINE_INPUT_SINPUT_TEST_HELPERS_H_

#include <vector>

#include "third_party/glm/glm/glm.hpp"
#include "ink/engine/camera/camera.h"
#include "ink/engine/input/input_data.h"
#include "ink/engine/input/sinput.h"
#include "ink/engine/input/sinput_helpers.h"
#include "ink/engine/public/types/input.h"
#include "ink/engine/util/funcs/step_utils.h"
#include "ink/engine/util/time/time_types.h"
#include "ink/proto/sengine_portable_proto.pb.h"

namespace ink {
namespace sinput_test_helpers {

/////////////////////// Touch Point Generation ////////////////////////
//
// For all the methods in this section, returns:
//   * A sorted vector of touch points (see SInput::LessThan)
//   * The first is a down, the last is an up.
//   * Positions are Lerped between "from_screen_pos" and "to_screen_pos"
//   * Time is Lerped from "start_time" to "end_time" (=start_time+duration)
//   * For single-touch: The default is id = 1, and the primary flag is set
//   * For multi-touch: The default is for the first id = 1, second id = 2,
//     and the primary flag is set on the first line
//   * InputType is Touch
//
// The methods differ in how the number of touch points returned is
// specified.

// Creates a number of input points >= 2 that would be similar to
// the amount of input generated by a real device oafter "duration_seconds"
// of touch.
//
// Note: although we render at 60 FPS, devices often get input faster than
// 60 hz, and your code should not break under these conditions.
//
// If you are writing a SEngine Integration test, you should probably be
// using this API.
std::vector<SInput> CreateSampledLine(glm::vec2 from_screen_pos,
                                      glm::vec2 to_screen_pos,
                                      InputTimeS start_time_seconds,
                                      DurationS duration_seconds);

// Creates N >= 4 touch points, simulating two fingers moving indepentently.
// The first finger moves in an interpolated line from first_start_pos to
// first_end_pos -- the second finger behaves likewise w.r.t. the second_*
// positions.
// The second finger will start and end 0.01 seconds after the first finger.
std::vector<SInput> CreateSampledMultiTouchLines(glm::vec2 first_start_pos,
                                                 glm::vec2 first_end_pos,
                                                 glm::vec2 second_start_pos,
                                                 glm::vec2 second_end_pos,
                                                 InputTimeS start_time,
                                                 DurationS duration);

// Creates num_interp_points+2 touch points.
// Requires: num_interp_points >= 0
//
// Intended use: You are writing a unit test for part of the input
// stack, you want to see that it responds exactly as you expect
// to special input.
std::vector<SInput> CreateExactLine(glm::vec2 from_screen_pos,
                                    glm::vec2 to_screen_pos,
                                    int num_interp_points,
                                    InputTimeS start_time, InputTimeS end_time);
std::vector<SInput> CreateExactLine(glm::vec2 from_screen_pos,
                                    glm::vec2 to_screen_pos,
                                    int num_interp_points,
                                    InputTimeS start_time, InputTimeS end_time,
                                    uint32_t id, bool set_primary_flag);

// Creates first_num_points + second_num_points + 4 touch points
std::vector<SInput> CreateExactMultiTouchLines(
    glm::vec2 first_start_pos, glm::vec2 first_end_pos,
    glm::vec2 second_start_pos, glm::vec2 second_end_pos,
    InputTimeS first_start_time, InputTimeS first_end_time,
    InputTimeS second_start_time, InputTimeS second_end_time,
    int first_num_points, int second_num_points);

// Creates a .1 second tap (2 touch points) at the given time and position
std::vector<SInput> CreateTap(glm::vec2 screen_pos, InputTimeS start_time);

// Creates num_interp_points+2 touch points.
std::vector<SInput> CreateArc(const glm::vec2& center, float radius,
                              int num_interp_points, float start_radians,
                              float end_radians, InputTimeS start_time,
                              InputTimeS end_time);

// Calculates an appropriate number of sample interpolation points for the
// given duration.
int GetNumberOfInterpolationPoints(DurationS duration_seconds);

// Creates a single scroll wheel input.
std::vector<SInput> CreateScrollWheelEvent(glm::vec2 screen_pos, float delta_x,
                                           float delta_y, InputTimeS time,
                                           uint32_t modifiers = 0);

//////////////////// Protocol buffer message generation ////////////////////////
//
// If you can, use go/proto-fixtures. If you can't, these methods might help.

// Constant screen attributes used by InputStream and MakePlaybackStream.
constexpr int kScreenWidth = 1024;
constexpr int kScreenHeight = 1024;
constexpr int kScreenPpi = 900;

// Flags for constructing SInputs.
constexpr uint32_t kFlagsBeginStroke =
    FlagBitfield(input::Flag::InContact, input::Flag::Primary,
                 input::Flag::Left, input::Flag::TDown);
constexpr uint32_t kFlagsEndStroke =
    FlagBitfield(input::Flag::Primary, input::Flag::Left, input::Flag::TUp);
constexpr uint32_t kFlagsMoveWhileDown = FlagBitfield(
    input::Flag::InContact, input::Flag::Primary, input::Flag::Left);
constexpr uint32_t kFlagsMoveWhileUp =
    FlagBitfield(input::Flag::Primary, input::Flag::Left);

// Returns an SInput with its fields set to the values given.
proto::SInput Input(const proto::SInput::InputType type, const uint32_t id,
                    const uint32_t flags, const uint32_t time_s,
                    const double screen_pos_x, const double screen_pos_y,
                    const double pressure, const double wheel_delta_x,
                    const double wheel_delta_y, const double tilt,
                    const double orientation);

// Returns an SInputStream consisting of the given SInputs.
proto::SInputStream InputStream(const std::vector<proto::SInput>& sinputs);

// Returns a PlaybackStream consisting of the given SInputs. Initial
// camera settings are taken from constants in this header.
proto::PlaybackStream MakePlaybackStream(
    const std::vector<proto::SInput>& sinputs);
}  // namespace sinput_test_helpers
}  // namespace ink

#endif  // INK_ENGINE_INPUT_SINPUT_TEST_HELPERS_H_