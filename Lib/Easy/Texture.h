/**
* @file Texture.h
*/
#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED
#include <GL/glew.h>
#include <memory>
#include <string>

class Texture;
typedef std::shared_ptr<Texture> TexturePtr; ///< テクスチャポインタ.

/**
* テクスチャクラス.
*/
class Texture
{
public:
  static bool Initialize();
  static void Finalize();
  static void Cache(const TexturePtr&);
  static bool IsCached(const char*);
  static TexturePtr LoadAndCache(const char*);
  static void RemoveOrphan();

  static TexturePtr Create(int width, int height, GLenum iformat, GLenum format, GLenum type, const void* data);
  static TexturePtr LoadFromFile(const char*);

  void Name(const char* str) { name = str; }
  const std::string& Name() const { return name; }
  GLuint Id() const { return texId; }
  GLsizei Width() const { return width; }
  GLsizei Height() const { return height; }

private:
  Texture() = default;
  ~Texture();
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  std::string name;
  GLuint texId = 0;
  int width = 0;
  int height = 0;
};

#endif // TEXTURE_H_INCLUDED