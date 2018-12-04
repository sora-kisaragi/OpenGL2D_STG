/**
* @file TweenAnimation.cpp
*/
#include "TweenAnimation.h"
#include "Node.h"
#include <algorithm>
#include <iostream>

namespace TweenAnimation {

/**
* コンストラクタ.
*
* @param d  動作時間.
* @param e  補間方法.
* @param t  ループ回数.
*/
Tween::Tween(glm::f32 d, EasingType e, glm::u32 t) :
  duration(d),
  reciprocalDuration(1.0f / d),
  times(static_cast<float>(t)),
  easing(e)
{
}

/**
* 状態を更新する.
*
* @param node    更新対象のノード.
* @param elapsed 経過時間.
*/
void Tween::UpdateWithEasing(Node& node, glm::f32 elapsed)
{
  const glm::u32 current = static_cast<glm::u32>(elapsed * ReciprocalUnitDuration());
  for (glm::u32 i = total; i < current; ++i) {
    Update(node, UnitDuration());
    Initialize(node);
  }
  total = current;
  glm::f32 ratio = std::fmod(elapsed, UnitDuration()) * ReciprocalUnitDuration();
  switch (easing) {
  default:
  case EasingType::Linear:
    /* 何もしない */
    break;
  case EasingType::EaseIn:
    ratio *= ratio;
    break;
  case EasingType::EaseOut:
    ratio = 2.0f * ratio - ratio * ratio;
    break;
  case EasingType::EaseInOut:
    ratio *= 2.0f;
    if (ratio < 1.0f) {
      ratio *= ratio;
    } else {
      ratio -= 1.0f;
      ratio = 2.0f * ratio - ratio * ratio;
      ratio += 1.0f;
    }
    ratio *= 0.5f;
    break;
  }
  Update(node, UnitDuration() * ratio);
}

/**
* 制御するトウィーニングオブジェクトを設定する.
*
* @param p トウィーニングオブジェクトへのポインタ.
*/
void Animate::Tween(const TweenPtr& p)
{
  tween = p;
  elapsed = 0.0f;
  isInitialized = false;
}

/**
* 状態を更新する.
*
* @param node  更新対象のノード.
* @param dt    前回の更新からの経過時間.
*/
void Animate::Update(Node& node, glm::f32 dt)
{
  if (!tween) {
    return;
  }
  if (!isInitialized) {
    isInitialized = true;
    tween->Initialize(node);
  }
  elapsed += dt;
  if (elapsed >= tween->TotalDuration() && isLoop) {
    tween->UpdateWithEasing(node, tween->TotalDuration());
    tween->Initialize(node);
    elapsed -= tween->TotalDuration();
  }
  tween->UpdateWithEasing(node, elapsed);
}

/**
* コンストラクタ.
*
* @param d  動作時間.
* @param v  移動する距離.
*/
MoveBy::MoveBy(glm::f32 d, const glm::vec3& v, EasingType e, Target t)
  : Tween(d, e)
  , vector(v)
  , target(t)
{
}

/**
* 移動状態を初期化する.
*
* @param node 制御対象のノード.
*/
void MoveBy::Initialize(Node& node)
{
  Tween::Initialize(node);
  start = node.Position();
}

/**
* 移動状態を更新する.
*
* @param node    更新対象のノード.
* @param elapsed 経過時間.
*/
void MoveBy::Update(Node& node, glm::f32 elapsed)
{
  const glm::f32 ratio = elapsed * ReciprocalUnitDuration();
  const glm::vec3 cur = start + vector * ratio;
  glm::vec3 pos = node.Position();
  if (static_cast<int>(target) & static_cast<int>(Target::X)) {
    pos.x = cur.x;
  }
  if (static_cast<int>(target) & static_cast<int>(Target::Y)) {
    pos.y = cur.y;
  }
  if (static_cast<int>(target) & static_cast<int>(Target::Z)) {
    pos.z = cur.z;
  }
  node.Position(pos);
}

/**
* トウィーニングオブジェクトを追加する.
*
* @param p 追加するトウィーニングオブジェクト.
*/
void Sequence::Add(const TweenPtr& p)
{
  seq.push_back(p);
  UnitDuration(UnitDuration() + p->TotalDuration());
}

/**
* 次のトウィーニングオブジェクトを設定する.
*
* @param node 制御対象のノード.
*/
bool Sequence::NextTween(Node& node)
{
  if (index >= static_cast<int>(seq.size()) - 1) {
    return false;
  }
  ++index;
  currentDurationBegin = currentDurationEnd;
  currentDurationEnd += seq[index]->TotalDuration();
  seq[index]->Initialize(node);
  return true;
}

/**
* トウィーニング列を初期化する.
*
* @param node 制御対象のノード.
*/
void Sequence::Initialize(Node& node)
{
  Tween::Initialize(node);
  index = -1;
  currentDurationEnd = 0.0f;
  NextTween(node);
}

/**
* トウィーニング列を更新する.
*
* @param node    更新対象のノード.
* @param elapsed 経過時間.
*/
void Sequence::Update(Node& node, glm::f32 elapsed)
{
  if (seq.empty()) {
    return;
  }
  while (elapsed >= currentDurationEnd) {
    seq[index]->UpdateWithEasing(node, seq[index]->TotalDuration());
    if (!NextTween(node)) {
      return;
    }
  }
  seq[index]->UpdateWithEasing(node, elapsed - currentDurationBegin);
}

/**
* 移動状態を初期化する.
*
* @param node 制御対象のノード.
*/
void Parallelize::Initialize(Node& node)
{
  Tween::Initialize(node);
  for (auto& e : tweens) {
    e->Initialize(node);
  }
}

/**
* 移動状態を更新する.
*
* @param node    更新対象のノード.
* @param elapsed 経過時間.
*/
void Parallelize::Update(Node& node, glm::f32 elapsed)
{
  if (tweens.empty()) {
    return;
  }
  for (auto& e : tweens) {
    e->UpdateWithEasing(node, std::min(e->TotalDuration(), elapsed));
  }
}

/**
* 並列に実行するトウィーニングオブジェクトを追加する.
*
* @param p 追加するトウィーニングオブジェクト.
*/
void Parallelize::Add(const TweenPtr& p)
{
  tweens.push_back(p);
  const glm::f32 d = p->TotalDuration();
  if (d > UnitDuration()) {
    UnitDuration(d);
  }
}

void RemoveFromParent::Update(Node& node, glm::f32 elapsed)
{
  if (node.Parent()) {
    node.Parent()->RemoveChild(&node);
  }
}

void Rotation::Initialize(Node& node)
{
  start = node.Rotation();
}

void Rotation::Update(Node& node, glm::f32 dt)
{
  node.Rotation(start + rotation * dt);
}

/**
* コンストラクタ.
*/
RemoveIfOutOfArea::RemoveIfOutOfArea(const glm::vec2& origin, const glm::vec2& size) :
  Tween(100, TweenAnimation::EasingType::Linear), origin(origin), size(size)
{
}

/**
* 更新.
*
* @param node    更新対象のノード.
* @param elapsed 経過時間.
*/
void RemoveIfOutOfArea::Update(Node& node, glm::f32 dt)
{
  const glm::vec3 pos = node.Position();
  if (pos.x < origin.x || pos.x > origin.x + size.x ||
    pos.y < origin.y || pos.y > origin.y + size.y) {
    if (node.Parent()) {
      node.Parent()->RemoveChild(&node);
    }
  }
}

} // namespace TweenAnimation