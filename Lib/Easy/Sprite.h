/**
* @file Sprite.h
*/
#ifndef SPRITE_H_INCLUDED
#define SPRITE_H_INCLUDED
#include "Node.h"
#include "Texture.h"
#include "FrameAnimation.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

// ��s�錾
class Sprite;
using SpritePtr = std::shared_ptr<Sprite>; ///< �X�v���C�g�|�C���^.

/**
* ��`�\����.
*/
struct Rect
{
  Rect() = default;
  Rect(const glm::vec2& xy, const glm::vec2& wh) : origin(xy), size(wh) {}
  Rect(float x, float y, float w, float h) : origin(x, y), size(w, h) {}

  glm::vec2 origin; ///< �������_.
  glm::vec2 size; ///< �c���̕�.
};

/// �F�������[�h.
enum BlendMode {
  BlendMode_Multiply,
  BlendMode_Add,
  BlendMode_Subtract,
};

/**
* �X�v���C�g�N���X.
*/
class Sprite : public Node
{
public:
  explicit Sprite(const char* texname);
  Sprite(const char* texname, const glm::vec3& pos);
  Sprite(const char* texname, const glm::vec3& pos, const Rect& r);

  Sprite() = default;
  explicit Sprite(const TexturePtr&);
  Sprite(const TexturePtr&, const Rect&);
  virtual ~Sprite() = default;
  Sprite(const Sprite&) = default;
  Sprite& operator=(const Sprite&) = default;

  const TexturePtr& Texture() const { return texture; }
  void Texture(const TexturePtr& tex);
  void Rectangle(const Rect& r) { rect = r; }
  const Rect& Rectangle() const { return rect; }
  void Color(const glm::vec4& c) { color = c; }
  const glm::vec4& Color() const { return color; }
  void ColorMode(BlendMode mode) { colorMode = mode; }
  BlendMode ColorMode() const { return colorMode; }

  void Animator(const FrameAnimation::AnimatePtr& anm) { animator = anm; }
  const FrameAnimation::AnimatePtr& Animator() const { return animator; }

  virtual void Update(glm::f32) override;

private:
  virtual void Draw(SpriteRenderer&) const override;

  TexturePtr texture;
  Rect rect = { glm::vec2(0, 0), glm::vec2(1, 1) };
  glm::vec4 color = glm::vec4(1);
  BlendMode colorMode = BlendMode_Multiply;

  FrameAnimation::AnimatePtr animator;
};

/**
* �X�v���C�g�`��N���X.
*/
class SpriteRenderer
{
public:
  SpriteRenderer() = default;
  ~SpriteRenderer();
  SpriteRenderer(const SpriteRenderer&) = delete;
  SpriteRenderer& operator=(const SpriteRenderer&) = delete;

  bool Initialize(size_t maxSpriteCount);
  void Update(const Node&);
  void Draw(const glm::vec2&) const;
  void ClearDrawData();

  void BeginUpdate();
  bool AddVertices(const Sprite&);
  void EndUpdate();

private:
  void MakeNodeList(const Node&, std::vector<const Node*>&);

  GLuint vbo = 0;
  GLuint ibo = 0;
  GLuint vao = 0;
  GLuint shaderProgram = 0;
  GLsizei vboCapacity = 0;        ///< VBO�Ɋi�[�\�ȍő咸�_��.
  GLsizei vboSize = 0;            ///< VBO�Ɋi�[����Ă��钸�_��.
  struct Vertex* pVBO = nullptr;  ///< VBO�ւ̃|�C���^.

  struct DrawData {
    size_t count;
    size_t offset;
    TexturePtr texture;
  };
  std::vector<DrawData> drawDataList;
};

#endif // SPRITE_H_INCLUDED