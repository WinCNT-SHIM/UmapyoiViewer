#include "CSoundManager.h"
#include "CommonDefine.h"
#include <windows.h>
//#include "../Engine/CTime.h"

/// <summary>
/// Create a System objectand initialize
/// </summary>
void CSoundManager::Initialize()
{
	//Common_Init(&extradriverdata);
	HRESULT _result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	result = FMOD::System_Create(&system);
	ERRCHECK(result);

	result = system->init(32, FMOD_INIT_NORMAL, extradriverdata);
	ERRCHECK(result);

	//AudioClip* _atk1 = ResourceManager::Instance()->GetAudioClip("Atk1");
	//result = system->createSound(_atk1->m_MetaFilePath.c_str(), FMOD_DEFAULT, 0, &sound_atk1);
	//ERRCHECK(result);

	m_ElapsedTime = 0;
}

bool CSoundManager::LoadSoundFile(std::string name, std::string filePath, unsigned int mode = 0)
{
	// �̹� ������ �ε� �� ��
	if (m_SoundList.find(name) != m_SoundList.end())
		return true;

	//FMOD_DEFAULT 0
	//FMOD_LOOP_OFF 1
	FMOD::Sound* _newSound = nullptr;
	result = system->createSound(filePath.c_str(), mode, 0, &_newSound);
	ERRCHECK(result);

	if (result == FMOD_ERR_FILE_NOTFOUND)
	{
		return false;
	}

	m_SoundList.insert({ name, _newSound });
	return true;
}

void CSoundManager::Update(float deltaTime)
{
	// ����ý��� ������Ʈ�� Ư�� �ð����� �Ͼ����
	if (m_ElapsedTime < 3.0f)
	{
		m_ElapsedTime += deltaTime;
	}
	else
	{
		result = system->update();
		ERRCHECK(result);

		m_ElapsedTime = 0;
	}

	{
		unsigned int ms = 0;
		unsigned int lenms = 0;
		bool         playing = 0;
		bool         paused = 0;
		int          channelsplaying = 0;

		if (bgmChannel)
		{
			FMOD::Sound* currentsound = 0;

			result = bgmChannel->isPlaying(&playing);
			if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
			{
				ERRCHECK(result);
			}

			result = bgmChannel->getPaused(&paused);
			if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
			{
				ERRCHECK(result);
			}

			result = bgmChannel->getPosition(&ms, FMOD_TIMEUNIT_MS);
			if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
			{
				ERRCHECK(result);
			}

			bgmChannel->getCurrentSound(&currentsound);

			if (currentsound)
			{
				result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
				{
					ERRCHECK(result);
				}
			}
		}

		if (effectChannel)
		{
			FMOD::Sound* currentsound = 0;

			for (int i = 0; i < 6; i++)
			{

				result = effectChannel[i]->isPlaying(&playing);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
				{
					ERRCHECK(result);
				}

				result = effectChannel[i]->getPaused(&paused);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
				{
					ERRCHECK(result);
				}

				result = effectChannel[i]->getPosition(&ms, FMOD_TIMEUNIT_MS);
				if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
				{
					ERRCHECK(result);
				}

				effectChannel[i]->getCurrentSound(&currentsound);

				if (currentsound)
				{
					result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
					if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
					{
						ERRCHECK(result);
					}
				}
			}

		}

		system->getChannelsPlaying(&channelsplaying, NULL);

		/*
		int test = channelsplaying;
		Common_Draw("==================================================");
		Common_Draw("Play Sound Example.");
		Common_Draw("Copyright (c) Firelight Technologies 2004-2021.");
		Common_Draw("==================================================");
		Common_Draw("");
		Common_Draw("Press %s to play a mono sound (drumloop)", Common_BtnStr(BTN_ACTION1));
		Common_Draw("Press %s to play a mono sound (jaguar)", Common_BtnStr(BTN_ACTION2));
		Common_Draw("Press %s to play a stereo sound (swish)", Common_BtnStr(BTN_ACTION3));
		Common_Draw("Press %s to quit", Common_BtnStr(BTN_QUIT));
		Common_Draw("");
		Common_Draw("Time %02d:%02d:%02d/%02d:%02d:%02d : %s", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped");
		Common_Draw("Channels Playing %d", channelsplaying);
		*/

		// ���� ������ �ִ� �ڵ��ε� ���� ����..
		//Sleep(10);
	}
}

void CSoundManager::Play(std::string name, IPlayMode mode)
{
	switch (mode)
	{
	case IPlayMode::BGM:
		PlayBGM(name);
		break;
	case IPlayMode::Effect:
		PlayEffect(name);
		break;
	}
}

void CSoundManager::Delay(float delayTime, IPlayMode mode)
{
	// ã�ƺ��� ��
	//bgmChannel->setDelay()
	switch (mode)
	{
	case IPlayMode::BGM:
		bgmChannel->setDelay(1, 2);
		break;
	case IPlayMode::Effect:
		for (int i = 0; i < 6; i++)
		{
			effectChannel[i]->setDelay(1, (unsigned long)delayTime);
		}
		break;
	default:
		break;
	}
}

void CSoundManager::Pause(IPlayMode mode, bool isPause)
{
	switch (mode)
	{
	case IPlayMode::BGM:
		bgmChannel->setPaused(isPause);
		break;
	case IPlayMode::Effect:
		for (int i = 0; i < 6; i++)
		{
			effectChannel[i]->setPaused(isPause);
		}
		break;
	default:
		break;
	}
}

void CSoundManager::Stop(std::string name, IPlayMode mode)
{
	switch (mode)
	{
	case IPlayMode::BGM:
		StopBGM(name);
		break;
	case IPlayMode::Effect:
		StopEffect(name);
		break;
	}
}

void CSoundManager::PlayBGM(std::string name)
{
	auto kvp = m_SoundList.find(name);
	if (kvp != m_SoundList.end())
	{
		bool isPlay = false;
		bgmChannel->isPlaying(&isPlay);
		if (!isPlay)
		{
			bgmChannel->stop();
		}
		result = system->playSound(kvp->second, 0, false, &bgmChannel);
		ERRCHECK(result);
	}
	else
	{
		::MessageBoxA(nullptr, "Sound NotFound", nullptr, MB_OK);
	}
}

void CSoundManager::PlayEffect(std::string name)
{
	auto kvp = m_SoundList.find(name);
	if (kvp != m_SoundList.end())
	{
		for (int i = 0; i < 6; i++)
		{
			bool isPlay;
			effectChannel[i]->isPlaying(&isPlay);

			if (!isPlay)
			{
				result = system->playSound(kvp->second, 0, false, &effectChannel[i]);
				effectChannel[i]->setVolume(m_EffectSoundVolume);
				ERRCHECK(result);
				break;
			}
		}
	}
	else
	{
		::MessageBoxA(nullptr, " Sound NotFound", nullptr, MB_OK);
	}
}

void CSoundManager::StopBGM(std::string name)
{
	auto kvp = m_SoundList.find(name);
	if (kvp != m_SoundList.end())
	{
		bool isPlay = false;
		bgmChannel->isPlaying(&isPlay);
		if (isPlay)
		{
			bgmChannel->stop();
		}
	}
	else
	{
		::MessageBoxA(nullptr, "Sound NotFound", nullptr, MB_OK);
	}
}

void CSoundManager::StopEffect(std::string name)
{
	auto kvp = m_SoundList.find(name);
	if (kvp != m_SoundList.end())
	{
		for (int i = 0; i < 6; i++)
		{
			bool isPlay;
			effectChannel[i]->isPlaying(&isPlay);

			if (isPlay)
			{
				effectChannel[i]->stop();
				break;
			}
		}
	}
	else
	{
		::MessageBoxA(nullptr, " Sound NotFound", nullptr, MB_OK);
	}
}

void CSoundManager::SetSoundSpeed(float speed, IPlayMode mode)
{
	switch (mode)
	{
	case IPlayMode::BGM:
		bgmChannel->setPitch(speed);
		break;
	case IPlayMode::Effect:
		for (int i = 0; i < 6; i++)
		{
			effectChannel[i]->setPitch(speed);
		}
		break;
	default:
		break;
	}
}

void CSoundManager::SetSoundVoulume(float volume, IPlayMode mode)
{
	switch (mode)
	{
	case IPlayMode::BGM:
		bgmChannel->setVolume(volume);
		break;
	case IPlayMode::Effect:
		m_EffectSoundVolume = volume;
		break;
	default:
		break;
	}
}

void CSoundManager::SetEffectSoundVoulume(float volume)
{
	for (int i = 0; i < 6; i++)
	{
		effectChannel[i]->setVolume(m_ElapsedTime * volume);
	}
}

/// <summary>
/// Shut down Sounds
/// </summary>
void CSoundManager::Finalize()
{
	result = system->update();
	ERRCHECK(result);

	for (auto _fmodSound : m_SoundList)
	{
		result = _fmodSound.second->release();
		ERRCHECK(result);
	}
	m_SoundList.clear();

	result = system->release();
	ERRCHECK(result);

	result = system->close();
	ERRCHECK(result);


	//Common_Close();
	CoUninitialize();
}
