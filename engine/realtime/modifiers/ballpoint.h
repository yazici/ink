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

#ifndef INK_ENGINE_REALTIME_MODIFIERS_BALLPOINT_H_
#define INK_ENGINE_REALTIME_MODIFIERS_BALLPOINT_H_

#include "ink/engine/geometry/mesh/vertex.h"
#include "ink/engine/realtime/modifiers/line_modifier.h"
#include "ink/engine/util/funcs/piecewise_interpolator.h"
#include "ink/engine/util/time/time_types.h"

namespace ink {

// A ballpoint pen line modifier that fades out slightly when the pressure
// value gets low.
class BallpointModifier : public LineModifier {
 public:
  explicit BallpointModifier(const glm::vec4& rgba);

  void OnAddVert(Vertex* vert, glm::vec2 center_pt, float radius,
                 float pressure) override;

  ShaderType GetShaderType() override { return ShaderType::ColoredVertShader; }

 private:
  static LineModParams MakeLineModParams() {
    LineModParams l;
    return l;
  }

  PiecewiseInterpolator opacity_interpolator_;
};

}  // namespace ink

#endif  // INK_ENGINE_REALTIME_MODIFIERS_BALLPOINT_H_
