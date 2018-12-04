/**
* @file FrameAnimation.h
*/
#ifndef FRAMEANIMATION_H_INCLUDED
#define FRAMEANIMATION_H_INCLUDED
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Sprite;

namespace FrameAnimation {

// 先行宣言.
struct Timeline;
class Animate;
using TimelinePtr = std::shared_ptr<Timeline>;
using AnimatePtr = std::shared_ptr<Animate>;

/**
* ある瞬間にスプライトとして表示するテクスチャ内の範囲.
*/
struct KeyFrame
{
  glm::f32 time; ///< 適用開始時間.
  glm::vec2 origin; ///< 原点.
  glm::vec2 size; ///< 大きさ.
};

/**
* キーフレームを時間に沿って並べたデータ型.
*/
struct Timeline
{
  static TimelinePtr Create(const KeyFrame* first, const KeyFrame* last);
  template<size_t N> static TimelinePtr Create(const KeyFrame(&array)[N]);

  std::vector<KeyFrame> data; ///< 時刻順に整列されているキーフレームの配列.
};


/**
* タイムラインを作成する.
*
* @param array タイムラインを構成するキーフレーム配列.
*
* @return タイムラインオブジェクト.
*
* キーフレーム配列からタイムラインを作成する.
* タイムライン中のキーフレームの順序は配列と同じになる.
*/
template<size_t N>
inline TimelinePtr Timeline::Create(const KeyFrame (&array)[N]) { return Create(array, array + N); }

/**
* スプライトのアニメーションを制御するクラス.
*/
class Animate
{
public:
  static AnimatePtr Create(const TimelinePtr&);

  Animate() = default;
  explicit Animate(const TimelinePtr&);
  ~Animate() = default;
  Animate(const Animate&) = default;
  Animate& operator=(const Animate&) = default;

  void Timeline(const TimelinePtr& list) { timeline = list; }
  const TimelinePtr& Timeline() const { return timeline; }
  void Speed(glm::f32 s) { speed = s; }
  glm::f32 Speed() const { return speed; }
  glm::f32 ElapsedTime() const { return elapsedTime; }
  glm::u32 KeyFrameIndex() const { return keyFrameIndex; }
  void Pause() { isPause = true; }
  void Resume() { isPause = false; }
  void Loop(bool f) { isLoop = f; }
  bool IsLoop() const { return isLoop; }
  bool IsFinished() const;

  void Update(Sprite&, glm::f32);

private:
  TimelinePtr timeline; ///< 時刻順に整列されているキーフレームの配列.
  glm::f32 speed = 1.0f; ///< タイムラインの進行速度.
  glm::f32 elapsedTime = 0.0f; ///< 経過時間.
  glm::u32 keyFrameIndex = 0; ///< 現在適用対象となっているタイムライン内のキーフレームを指すインデックス.
  bool isPause = false; ///< 時間経過を一時停止するかどうか.
  bool isLoop = true; ///< ループ再生を行うかどうか.
};

} // namespace FrameAnimation

#endif // FRAMEANIMATION_H_INCLUDED