/**
* @file Audio.h
*/
#ifndef AUDIO_H_INCLUDED
#define AUDIO_H_INCLUDED
#include <memory>

namespace Audio {

enum State {
	State_Create = 0x01,
	State_Preparing = 0x02,
	State_Prepared = 0x04,
	State_Playing = 0x08,
	State_Stopped = 0x10,
	State_Pausing = 0x20,
	State_Failed = 0x40,
};

enum Flag
{
	Flag_None = 0,
	Flag_Loop = 0x01,
};

class Sound
{
public:
	virtual ~Sound() = default;
	virtual bool Play(int flags = 0) = 0;
	virtual bool Pause() = 0;
	virtual bool Seek() = 0;
	virtual bool Stop() = 0;
	virtual float SetVolume(float) = 0;
	virtual float SetPitch(float) = 0;
	virtual int GetState() const = 0;
};
typedef std::shared_ptr<Sound> SoundPtr;

class Engine
{
public:
	static Engine& Get();
	
	Engine() = default;

	virtual ~Engine() = default;
	virtual bool Initialize() = 0;
	virtual void Destroy() = 0;
	virtual bool Update() = 0;
	virtual SoundPtr Prepare(const wchar_t*) = 0;
	virtual SoundPtr PrepareStream(const wchar_t*) = 0;
    virtual SoundPtr PrepareMFStream(const wchar_t*) = 0;
    virtual void SetMasterVolume(float) = 0;

private:
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;
};

} // namespace Audio

#endif // AUDIO_H_INCLUDED