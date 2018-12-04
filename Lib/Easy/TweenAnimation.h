/**
* @file TweenAnimation.h
*/
#ifndef TWEENANIMATION_H_INCLUDED
#define TWEENANIMATION_H_INCLUDED
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class Node;

namespace TweenAnimation {

// ��s�錾.
class Animate;
class Tween;
class MoveBy;
class Sequence;
class Parallelize;
class Wait;
class Rotation;
class RemoveFromParent;
class RemoveIfOutOfArea;
using AnimatePtr = std::shared_ptr<Animate>;
using TweenPtr = std::shared_ptr<Tween>;
using MoveByPtr = std::shared_ptr<MoveBy>;
using SequencePtr = std::shared_ptr<Sequence>;
using ParallelizePtr = std::shared_ptr<Parallelize>;
using WaitPtr = std::shared_ptr<Wait>;
using RotationPtr = std::shared_ptr<Rotation>;
using RemoveFromParentPtr = std::shared_ptr<RemoveFromParent>;
using RemoveIfOutOfAreaPtr = std::shared_ptr<RemoveIfOutOfArea>;

/**
* �C�[�W���O�̎��.
*/
enum class EasingType {
  Linear, ///< ����.
  EaseIn, ///< ����.
  EaseOut, ///< ����.
  EaseInOut, ///< ����+����.
};

/**
* �A�j���[�V������K�p����v�f.
*/
enum class Target {
  X = 1, ///< X�v�f�̂�.
  Y = 2, ///< Y�v�f�̂�.
  XY = 3, ///< X�v�f��Y�v�f.
  Z = 4, ///< Z�v�f�̂�.
  XZ = 5, ///< X�v�f��Z�v�f.
  YZ = 6, ///< Y�v�f��Z�v�f.
  XYZ = 7 ///< �S�Ă̗v�f.
};

/**
* �g�E�B�[�j���O��{�N���X.
*/
class Tween
{
public:
  Tween() = default;
  explicit Tween(glm::f32 d, EasingType e = EasingType::Linear, glm::u32 t = 1);
  Tween(const Tween&) = delete;
  Tween& operator=(const Tween&) = delete;
  virtual ~Tween() = default;

  glm::f32 TotalDuration() const { return duration * times; }
  glm::f32 UnitDuration() const { return duration; }
  glm::f32 ReciprocalUnitDuration() const { return reciprocalDuration; }
  void UnitDuration(glm::f32 d) {
    duration = d;
    reciprocalDuration = 1.0f / d;
  }
  EasingType Easing() const { return easing; }
  void Easing(EasingType type) { easing = type; }

  virtual void Initialize(Node&) { total = 0; }
  void UpdateWithEasing(Node& node, glm::f32 ratio);
  virtual void Update(Node&, glm::f32 ratio) = 0;

private:
  glm::f32 duration = 1.0f; ///< ���쎞��.
  glm::f32 reciprocalDuration = 1.0f; ///< ���쎞�Ԃ̋t��.
  glm::f32 times = 1.0f;
  EasingType easing = EasingType::Linear;
  glm::u32 total = 0;
};

/**
* �g�E�B�[�j���O�𐧌䂷��N���X.
*/
class Animate
{
public:
  static AnimatePtr Create(const TweenPtr& p) { return std::make_shared<Animate>(p); }

  Animate() = default;
  explicit Animate(const TweenPtr& p) { Tween(p); }
  Animate(const Animate&) = delete;
  Animate& operator=(const Animate&) = delete;
  ~Animate() = default;

  void Tween(const TweenPtr& p);
  const TweenPtr& Tween() const { return tween; }

  void Pause() { isPause = true; }
  void Resume() { isPause = false; }
  void Loop(bool f) { isLoop = f; }
  bool IsLoop() const { return isLoop; }
  bool IsFinished() const { return !tween || (!isLoop && elapsed >= tween->TotalDuration()); }

  void Update(Node&, glm::f32);

private:
  glm::f32 elapsed = 0.0f; ///< �o�ߎ���.
  bool isInitialized = false;
  bool isPause = false; ///< ���Ԍo�߂��ꎞ��~���邩�ǂ���.
  bool isLoop = false; ///< ���[�v�Đ����s�����ǂ���.

  TweenPtr tween;
};

/**
* �ړ��A�j���[�V����.
*/
class MoveBy : public Tween
{
public:
  static MoveByPtr Create(glm::f32 d, const glm::vec3& v, EasingType e = EasingType::Linear, Target t = Target::XYZ)
  {
    return std::make_shared<MoveBy>(d, v, e, t);
  }

  MoveBy() = default;
  MoveBy(glm::f32 d, const glm::vec3& v, EasingType e = EasingType::Linear, Target t = Target::XYZ);
  MoveBy(const MoveBy&) = delete;
  MoveBy& operator=(const MoveBy&) = delete;
  virtual ~MoveBy() = default;

  virtual void Initialize(Node&) override;
  virtual void Update(Node&, glm::f32) override;

private:
  glm::vec3 start; ///< �ړ��J�n���W.
  glm::vec3 vector; ///< �ړ����鋗��.
  Target target = Target::XYZ; ///< ����Ώ�.
};

/**
* �g�E�B�[�j���O�̗�.
*/
class Sequence : public Tween
{
public:
  static SequencePtr Create(glm::u32 times) { return std::make_shared<Sequence>(times); }

  explicit Sequence(glm::u32 t = 1) : Tween(0.0f, EasingType::Linear, t) {}
  Sequence(const Sequence&) = delete;
  Sequence& operator=(const Sequence&) = delete;
  virtual ~Sequence() = default;

  virtual void Initialize(Node&) override;
  virtual void Update(Node&, glm::f32) override;
  void Add(const TweenPtr&);

private:
  bool NextTween(Node&);

  std::vector<TweenPtr> seq; ///< �g�E�B�[�j���O���X�g.
  int index = -1;///< ���s���̃g�E�B�[�j���O�I�u�W�F�N�g�̃C���f�b�N�X.
  glm::f32 currentDurationBegin;///< ���s���̃g�E�B�[�j���O�I�u�W�F�N�g�̊J�n����.
  glm::f32 currentDurationEnd;///< ���s���̃g�E�B�[�j���O�I�u�W�F�N�g�̏I������.
};

/**
* �m�[�h�̈ړ��A�j���[�V����.
*/
class Parallelize : public Tween
{
public:
  static ParallelizePtr Create(glm::u32 times) { return std::make_shared<Parallelize>(times); }

  explicit Parallelize(glm::u32 t = 1) : Tween(0.0f, EasingType::Linear, t) {}
  Parallelize(const Parallelize&) = delete;
  Parallelize& operator=(const Parallelize&) = delete;
  virtual ~Parallelize() = default;

  virtual void Initialize(Node&) override;
  virtual void Update(Node&, glm::f32) override;
  void Add(const TweenPtr& p);

private:
  std::vector<TweenPtr> tweens;
};

/**
* �w�肳�ꂽ���ԉ������Ȃ�.
*/
class Wait : public TweenAnimation::Tween
{
public:
  static WaitPtr Create(glm::f32 duration) { return std::make_shared<Wait>(duration); }

  Wait(glm::f32 d) : Tween(d, TweenAnimation::EasingType::Linear) {}
  virtual void Update(Node&, glm::f32) override {}
};

/**
* �e�m�[�h����폜.
*/
class RemoveFromParent : public TweenAnimation::Tween
{
public:
  static RemoveFromParentPtr Create() { return std::make_shared<RemoveFromParent>(); }

  virtual void Update(Node& node, glm::f32 elapsed) override;
};

/**
* ��]�A�j���[�V����.
*/
class Rotation : public Tween
{
public:
  static RotationPtr Create(glm::f32 duration, glm::f32 rot, EasingType e = EasingType::Linear) { return std::make_shared<Rotation>(duration, rot, e); }

  Rotation(glm::f32 d, glm::f32 rot, EasingType e = EasingType::Linear) :
    Tween(d, e),
    rotation(rot)
  {}
  virtual void Initialize(Node& node) override;
  virtual void Update(Node& node, glm::f32 dt) override;

private:
  glm::f32 start;
  glm::f32 rotation;
};

/**
* �͈͊O�ɏo����e�m�[�h����폜����.
*/
class RemoveIfOutOfArea : public TweenAnimation::Tween {
public:
  static RemoveIfOutOfAreaPtr Create(const glm::vec2& origin, const glm::vec2& size) { return std::make_shared<RemoveIfOutOfArea>(origin, size); }

  RemoveIfOutOfArea(const glm::vec2& origin, const glm::vec2& size);
  virtual void Update(Node& node, glm::f32 dt) override;

private:
  glm::vec2 origin;
  glm::vec2 size;
};

} // namespace TweenAnimation

#endif // TWEENANIMATION_H_INCLUDED