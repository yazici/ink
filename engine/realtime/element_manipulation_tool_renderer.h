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

#ifndef INK_ENGINE_REALTIME_ELEMENT_MANIPULATION_TOOL_RENDERER_H_
#define INK_ENGINE_REALTIME_ELEMENT_MANIPULATION_TOOL_RENDERER_H_

#include <memory>
#include <vector>

#include "third_party/absl/container/flat_hash_map.h"
#include "third_party/absl/types/span.h"
#include "ink/engine/camera/camera.h"
#include "ink/engine/geometry/primitives/rect.h"
#include "ink/engine/geometry/primitives/rot_rect.h"
#include "ink/engine/geometry/shape/shape.h"
#include "ink/engine/rendering/compositing/live_renderer.h"
#include "ink/engine/rendering/compositing/single_partition_renderer.h"
#include "ink/engine/rendering/gl_managers/gl_resource_manager.h"
#include "ink/engine/rendering/renderers/element_renderer.h"
#include "ink/engine/rendering/renderers/mesh_renderer.h"
#include "ink/engine/rendering/renderers/shape_renderer.h"
#include "ink/engine/scene/frame_state/frame_state.h"
#include "ink/engine/scene/graph/scene_graph.h"
#include "ink/engine/service/unchecked_registry.h"
#include "ink/engine/util/time/time_types.h"
#include "ink/engine/util/time/timer.h"
#include "ink/engine/util/time/wall_clock.h"

namespace ink {
namespace tools {

enum class ElementManipulationToolHandle {
  NONE,
  RIGHT,
  TOP,
  LEFT,
  BOTTOM,
  RIGHTTOP,
  LEFTTOP,
  LEFTBOTTOM,
  RIGHTBOTTOM,
  ROTATION,
};

// A list of all values in the ElementManipulationToolHandle enum, suitable for
// iterating over.
extern const absl::Span<const ElementManipulationToolHandle>
    kAllElementManipulationToolHandles;

// Returns the world position of the given selection box handle.  For NONE, this
// returns the center of the rect.
glm::vec2 ElementManipulationToolHandlePosition(
    ElementManipulationToolHandle handle, const Camera& camera,
    RotRect world_rect);

// Returns the world position of the anchor point for the given selection box
// handle, i.e. the position of the handle on the opposite corner/side.  For
// NONE and ROTATION, this returns the center of the rect.
glm::vec2 ElementManipulationToolHandleAnchor(
    ElementManipulationToolHandle handle, RotRect world_rect);

class ElementManipulationToolRendererInterface {
 public:
  virtual ~ElementManipulationToolRendererInterface(void) {}
  virtual void Draw(const Camera& cam, FrameTimeS draw_time,
                    glm::mat4 transform) const = 0;
  virtual void Update(const Camera& cam, FrameTimeS draw_time, Rect element_mbr,
                      RotRect region, glm::mat4 transform) = 0;
  virtual void Enable(bool enabled) = 0;
  virtual void Synchronize(void) = 0;
  virtual void SetElements(const Camera& cam,
                           const std::vector<ElementId>& elements,
                           Rect element_mbr, RotRect region) = 0;
};

class ElementManipulationToolRenderer
    : public ElementManipulationToolRendererInterface {
 public:
  explicit ElementManipulationToolRenderer(
      const service::UncheckedRegistry& registry);
  void Draw(const Camera& cam, FrameTimeS draw_time,
            glm::mat4 transform) const override;
  void Update(const Camera& cam, FrameTimeS draw_time, Rect element_mbr,
              RotRect region, glm::mat4 transform) override;
  void Enable(bool enabled) override;
  void Synchronize() override;
  void SetElements(const Camera& cam, const std::vector<ElementId>& elements,
                   Rect element_mbr, RotRect region) override;

 private:
  void SetOutlinePosition(const Camera& cam, RotRect region);
  void SetOutlineVisible(bool visible);
  void UpdateWithTimer(const Camera& cam, FrameTimeS draw_time,
                       Rect element_mbr, RotRect region, glm::mat4 transform,
                       Timer timer);

  std::shared_ptr<SceneGraph> scene_graph_;
  std::shared_ptr<FrameState> frame_state_;
  std::shared_ptr<LiveRenderer> renderer_;
  std::shared_ptr<WallClockInterface> wall_clock_;
  std::shared_ptr<GLResourceManager> gl_resources_;
  Shape outline_;
  Shape outline_glow_;
  Shape rotation_bar_;
  absl::flat_hash_map<ElementManipulationToolHandle, std::unique_ptr<Shape>>
      handle_shapes_;
  ShapeRenderer shape_renderer_;
  SinglePartitionRenderer partition_renderer_;
  MeshRenderer mesh_renderer_;
  Mesh bg_overlay_;
  std::shared_ptr<settings::Flags> flags_;
};

class SingleElementManipulationToolRenderer
    : public ElementManipulationToolRendererInterface {
 public:
  explicit SingleElementManipulationToolRenderer(
      const service::UncheckedRegistry& registry)
      : has_id_(false),
        scene_graph_(registry.GetShared<SceneGraph>()),
        renderer_(registry) {}
  void Draw(const Camera& cam, FrameTimeS draw_time,
            glm::mat4 transform) const override {
    if (has_id_) renderer_.Draw(id_, *scene_graph_, cam, draw_time, transform);
  }
  void Update(const Camera& cam, FrameTimeS draw_time, Rect element_mbr,
              RotRect region, glm::mat4 transform) override {}
  void Enable(bool enabled) override {}
  void Synchronize(void) override {}
  void SetElements(const Camera& cam, const std::vector<ElementId>& elements,
                   Rect element_mbr, RotRect region) override {
    has_id_ = !elements.empty();
    if (has_id_) {
      ASSERT(elements.size() == 1);
      id_ = elements.front();
    }
  }

 private:
  bool has_id_;
  ElementId id_;
  std::shared_ptr<SceneGraph> scene_graph_;
  ElementRenderer renderer_;
};

}  // namespace tools
}  // namespace ink

#endif  // INK_ENGINE_REALTIME_ELEMENT_MANIPULATION_TOOL_RENDERER_H_
