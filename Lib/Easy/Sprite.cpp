/**
* @file Sprite.cpp
*/
#include "Sprite.h"
#include "Shader.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

/**
* 頂点データ型.
*/
struct Vertex
{
  glm::vec3 position; ///< 座標
  glm::vec4 color; ///< 色
  glm::vec2 texCoord; ///< テクスチャ座標
  glm::i32 blendMode; ///< 色合成モード.
};

namespace /* unnamed */ {

/**
* Vertex Buffer Objectを作成する.
*
* @param size 頂点データのサイズ.
* @param data 頂点データへのポインタ.
*
* @return 作成したVBOのID.
*/
GLuint CreateVBO(GLsizeiptr size, const GLvoid* data)
{
  GLuint vbo = 0;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  return vbo;
}

/**
* Index Buffer Objectを作成する.
*
* @param size インデックスデータのサイズ.
* @param data インデックスデータへのポインタ.
*
* @return 作成したIBOのID.
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data)
{
  GLuint ibo = 0;
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  return ibo;
}

/**
* 頂点アトリビュートを設定する.
*
* @param index 頂点アトリビュートのインデックス.
* @param cls   頂点データ型名.
* @param mbr   頂点アトリビュートに設定するclsのメンバ変数名.
*/
#define SetVertexAttribPointer(index, cls, mbr) \
  SetVertexAttribPointerI(index, sizeof(cls::mbr) / sizeof(float), sizeof(cls), reinterpret_cast<GLvoid*>(offsetof(cls, mbr)))
void SetVertexAttribPointerI(GLuint index, GLint size, GLsizei stride, const GLvoid* pointer)
{
  glEnableVertexAttribArray(index);
  glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}

/**
* 頂点アトリビュートを設定する(整数).
*
* @param index 頂点アトリビュートのインデックス.
* @param cls   頂点データ型名.
* @param mbr   頂点アトリビュートに設定するclsのメンバ変数名.
*/
#define SetVertexAttribIntPointer(index, cls, mbr) \
  SetVertexAttribIntPointerI(index, sizeof(cls::mbr) / sizeof(float), sizeof(cls), reinterpret_cast<GLvoid*>(offsetof(cls, mbr)))
void SetVertexAttribIntPointerI(GLuint index, GLint size, GLsizei stride, const GLvoid* pointer)
{
  glEnableVertexAttribArray(index);
  glVertexAttribIPointer(index, size, GL_INT, stride, pointer);
}

/**
* Vertex Array Objectを作成する.
*
* @param vbo VAOに関連付けられるVBOのID.
* @param ibo VAOに関連付けられるIBOのID.
*
* @return 作成したVAOのID.
*/
GLuint CreateVAO(GLuint vbo, GLuint ibo)
{
  GLuint vao = 0;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  SetVertexAttribPointer(0, Vertex, position);
  SetVertexAttribPointer(1, Vertex, color);
  SetVertexAttribPointer(2, Vertex, texCoord);
  SetVertexAttribIntPointer(3, Vertex, blendMode);
  glBindVertexArray(0);
  return vao;
}

} // unnamed namespace

/**
* Spriteを作成する.
*
* @param texname テクスチャファイルのパス.
*/
Sprite::Sprite(const char* texname) :
  texture(Texture::LoadAndCache(texname))
{
  if (texture) {
    Rectangle({ glm::vec2(), glm::vec2(texture->Width(), texture->Height()) });
  }
}

/**
* Spriteを作成する.
*
* @param texname テクスチャファイルのパス.
* @param pos     表示する座標.
*/
Sprite::Sprite(const char* texname, const glm::vec3& pos) :
  texture(Texture::LoadAndCache(texname))
{
  if (texture) {
    Rectangle({ glm::vec2(), glm::vec2(texture->Width(), texture->Height()) });
  }
  Position(pos);
}

/**
* Spriteを作成する.
*
* @param texname テクスチャファイルのパス.
* @param pos     表示する座標.
* @param r       テクスチャのどの部分を表示するか.
*/
Sprite::Sprite(const char* texname, const glm::vec3& pos, const Rect& r) :
  texture(Texture::LoadAndCache(texname)),
  rect(r)
{
  Position(pos);
}

/**
* Spriteコンストラクタ.
*/
Sprite::Sprite(const TexturePtr& tex) :
  texture(tex),
  rect({ glm::vec2(), glm::vec2(tex->Width(), tex->Height()) })
{
}

/**
* Spriteコンストラクタ.
*/
Sprite::Sprite(const TexturePtr& tex, const Rect& r) :
  texture(tex),
  rect(r)
{
}

/**
* 描画に使用するテクスチャを指定する.
*
* @param tex 描画に使用するテクスチャ.
*/
void Sprite::Texture(const TexturePtr& tex)
{
  texture = tex;
  if (tex) {
    Rectangle({ glm::vec2(), glm::vec2(tex->Width(), tex->Height()) });
  }
}

/**
* スプライトの状態を更新する.
*
* @param dt 前回の更新からの経過時間.
*/
void Sprite::Update(glm::f32 dt)
{
  Node::Update(dt);
  if (animator) {
    animator->Update(*this, dt);
  }
}

/**
* スプライトを描画する.
*
* @param 描画を行うレンダラー.
*/
void Sprite::Draw(SpriteRenderer& renderer) const
{
  renderer.AddVertices(*this);
}

/**
* SpriteRendererデストラクタ.
*/
SpriteRenderer::~SpriteRenderer()
{
  if (shaderProgram) {
    glDeleteProgram(shaderProgram);
  }
  if (vao) {
    glDeleteVertexArrays(1, &vao);
  }
  if (ibo) {
    glDeleteBuffers(1, &ibo);
  }
  if (vbo) {
    glDeleteBuffers(1, &vbo);
  }
}

/**
* スプライト描画クラスを初期化する.
*
* @param maxSpriteCount 描画可能な最大スプライト数.
*
* @retval true  初期化成功.
* @retval false 初期化失敗.
*/
bool SpriteRenderer::Initialize(size_t maxSpriteCount)
{
  vbo = CreateVBO(sizeof(Vertex) * maxSpriteCount * 4, nullptr);
  std::vector<GLushort> indices;
  indices.resize(maxSpriteCount * 6);
  for (GLushort i = 0; i < maxSpriteCount; ++i) {
    indices[i * 6 + 0] = (i * 4) + 0;
    indices[i * 6 + 1] = (i * 4) + 1;
    indices[i * 6 + 2] = (i * 4) + 2;
    indices[i * 6 + 3] = (i * 4) + 2;
    indices[i * 6 + 4] = (i * 4) + 3;
    indices[i * 6 + 5] = (i * 4) + 0;
  }
  ibo = CreateIBO(indices.size() * sizeof(GLushort), indices.data());
  vao = CreateVAO(vbo, ibo);
  shaderProgram = Shader::CreateProgramFromFile("Res/Shader/Sprite.vert", "Res/Shader/Sprite.frag");
  if (!vbo || !ibo || !vao || !shaderProgram) {
    return false;
  }

  vboCapacity = static_cast<GLsizei>(4 * maxSpriteCount);
  return true;
}

/**
* 頂点データの作成を開始する.
*/
void SpriteRenderer::BeginUpdate()
{
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  pVBO = static_cast<Vertex*>(glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * vboCapacity, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
  vboSize = 0;
  ClearDrawData();
}

/**
* 頂点データを追加する.
*
* @param sprite 頂点データの元になるスプライト.
*
* @retval true  追加成功.
* @retval false 頂点バッファが満杯で追加できない.
*/
bool SpriteRenderer::AddVertices(const Sprite& sprite)
{
  if (!pVBO || vboSize >= vboCapacity) {
    return false;
  }
  const TexturePtr& texture = sprite.Texture();
  if (!texture) {
    return true;
  }
  const glm::vec2 textureSize(texture->Width(), texture->Height());
  const glm::vec2 reciprocalSize(glm::vec2(1) / textureSize);

  Rect rect = sprite.Rectangle();
  rect.origin *= reciprocalSize;
  rect.size *= reciprocalSize;
  const glm::vec2 halfSize = sprite.Rectangle().size * 0.5f;
  const glm::mat4x4& transform = sprite.Transform();

  pVBO[0].position = transform * glm::vec4(-halfSize.x, -halfSize.y, 0, 1);
  pVBO[0].color = sprite.Color();
  pVBO[0].texCoord = rect.origin;
  pVBO[0].blendMode = sprite.ColorMode();

  pVBO[1].position = transform * glm::vec4(halfSize.x, -halfSize.y, 0, 1);
  pVBO[1].color = sprite.Color();
  pVBO[1].texCoord = glm::vec2(rect.origin.x + rect.size.x, rect.origin.y);
  pVBO[1].blendMode = sprite.ColorMode();

  pVBO[2].position = transform * glm::vec4(halfSize.x, halfSize.y, 0, 1);
  pVBO[2].color = sprite.Color();
  pVBO[2].texCoord = rect.origin + rect.size;
  pVBO[2].blendMode = sprite.ColorMode();

  pVBO[3].position = transform * glm::vec4(-halfSize.x, halfSize.y, 0, 1);
  pVBO[3].color = sprite.Color();
  pVBO[3].texCoord = glm::vec2(rect.origin.x, rect.origin.y + rect.size.y);
  pVBO[3].blendMode = sprite.ColorMode();

  pVBO += 4;
  vboSize += 4;

  if (drawDataList.empty()) {
    drawDataList.push_back({ 6, 0, sprite.Texture() });
  } else {
    auto& data = drawDataList.back();
    if (data.texture == sprite.Texture()) {
      data.count += 6;
    } else {
      drawDataList.push_back({ 6, data.offset + data.count * sizeof(GLushort), sprite.Texture() });
    }
  }
  return true;
}

/**
* 頂点データの作成を終了する.
*/
void SpriteRenderer::EndUpdate()
{
  glUnmapBuffer(GL_ARRAY_BUFFER);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  pVBO = nullptr;
}

/**
* シーングラフを描画する.
*
* @param node シーングラフのルートノード.
*/
void SpriteRenderer::Update(const Node& node)
{
  std::vector<const Node*> nodeList;
  nodeList.reserve(1024);

  MakeNodeList(node, nodeList);
  std::stable_sort(nodeList.begin(), nodeList.end(), [](const Node* lhs, const Node* rhs) {
    return lhs->Position().z < rhs->Position().z;
  });

  BeginUpdate();
  for (const auto e : nodeList) {
    e->Draw(*this);
  }
  EndUpdate();
}

/**
* ノードリストを更新する.
*
* @param node
* @param spriteList 描画するノードのリスト.
*/
void SpriteRenderer::MakeNodeList(const Node& node, std::vector<const Node*>& nodeList)
{
  nodeList.push_back(&node);
  for (auto e : node.Children()) {
    MakeNodeList(*e, nodeList);
  }
}

/**
* スプライトを描画する.
*
* @param texture    描画に使用するテクスチャ.
* @param screenSize 画面サイズ.
*/
void SpriteRenderer::Draw(const glm::vec2& screenSize) const
{
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUseProgram(shaderProgram);

  const GLint matMVPLoc = glGetUniformLocation(shaderProgram, "matMVP");
  if (matMVPLoc >= 0) {
    const glm::mat4x4 matProj = glm::ortho(-screenSize.x * 0.5f, screenSize.x * 0.5f, -screenSize.y * 0.5f, screenSize.y * 0.5f, 200.0f, 1200.0f);
    //const glm::mat4x4 matProj = glm::perspective(glm::radians(45.0f), screenSize.x / screenSize.y, 200.0f, 1200.0f);
    const glm::mat4x4 matView = glm::lookAt(glm::vec3(0, 0, glm::tan(glm::radians(90.0f - 22.5f)) * screenSize.y * 0.5f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    const glm::mat4x4 matMVP = matProj * matView;
    glUniformMatrix4fv(matMVPLoc, 1, GL_FALSE, &matMVP[0][0]);
  }

  const GLint colorSamplerLoc = glGetUniformLocation(shaderProgram, "colorSampler");
  if (colorSamplerLoc >= 0) {
    glUniform1i(colorSamplerLoc, 0);
    glActiveTexture(GL_TEXTURE0);
  }

  glBindVertexArray(vao);
  for (const auto& data : drawDataList) {
    glBindTexture(GL_TEXTURE_2D, data.texture->Id());
    glDrawElements(GL_TRIANGLES, data.count, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid*>(data.offset));
  }
  glBindVertexArray(0);
}

/**
* スプライト描画データを消去する.
*/
void SpriteRenderer::ClearDrawData()
{
  drawDataList.clear();
}
