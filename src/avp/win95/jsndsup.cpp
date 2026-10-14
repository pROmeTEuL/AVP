#include "3dc.h"
#include "inline.h"
#include "module.h"

#define UseLocalAssert true
#include "ourasert.h"
#include "psndplat.h"

#include "jsndsup.h"

#include <algorithm>
#include <vector>

extern int SoundSwitchedOn;
// Pat sets this up

std::vector<LOADED_SOUND *> loaded_sounds;

void LoseSound(LOADED_SOUND const *ls)
{
    LOADED_SOUND **ucls_p = (LOADED_SOUND **) &ls;
    LOADED_SOUND *ucls = *ucls_p;

    auto sound = std::find(loaded_sounds.begin(), loaded_sounds.end(), ucls);
    if (sound != loaded_sounds.end()) {
        LOADED_SOUND *ls_in_list = *sound;
        ls_in_list->num_attached--;

        if (ls_in_list->permanent == 1) {
            if (ls_in_list->num_attached <= 0) {
                loaded_sounds.erase(sound);
                DeallocateMem(ls_in_list->wavname);
                DeallocateMem(ls_in_list);
            }

            return;
        }

        if (ls_in_list->num_attached <= 0) {
            PlatEndGameSound((SOUNDINDEX) ls_in_list->sound_num);
            loaded_sounds.erase(sound);
            DeallocateMem(ls_in_list->wavname);
            DeallocateMem(ls_in_list);
        }
    } else {
        GLOBALASSERT(0);
    }
}

void LoseAllNonCommonSounds()
{
    for (LOADED_SOUND *ls : loaded_sounds) {

        if (!ls->permanent) {
            PlatEndGameSound((SOUNDINDEX) ls->sound_num);
        }

        DeallocateMem(ls->wavname);
        DeallocateMem(ls);
    }
    loaded_sounds.clear();
}

static int find_empty_game_sound()
{
    if (!SoundSwitchedOn)
        return (-1);

    for (int i = SID_STARTOF_LOADSLOTS; i <= SID_ENDOF_LOADSLOTS; i++) {
        if (GameSounds[i].loaded == 0) {
            return (i);
        }
    }
    return (-1);
}

static int find_permanent_game_sound(const char *wavname)
{
    if (!SoundSwitchedOn)
        return (-1);

    for (int i = 0; i < SID_MAXIMUM; i++) {
        if (GameSounds[i].wavName) {
            if (!(_stricmp(GameSounds[i].wavName, wavname))) {
                return (i);
            }
        }
    }
    return (-1);
}

LOADED_SOUND const *GetSound(char const *fname)
{
    if (!SoundSwitchedOn)
        return (0);

    const char *wavname = strrchr(fname, '\\');

    if (wavname) {
        wavname++;
    } else {
        wavname = fname;
    }

    // check if wavname already loaded

    for (LOADED_SOUND *loaded_sound : loaded_sounds) {
        if (!_stricmp(loaded_sound->wavname, wavname)) {
            loaded_sound->num_attached++;
            return loaded_sound;
        }
    }

    LOADED_SOUND *ls = 0;

    int perm_snum = find_permanent_game_sound(wavname);

    if (perm_snum != -1) {
        ls = (LOADED_SOUND *) AllocateMem(sizeof(LOADED_SOUND));

        ls->sound_num = perm_snum;
        ls->wavname = (char *) AllocateMem(sizeof(char) * (strlen(wavname) + 1));
        strcpy(ls->wavname, wavname);
        ls->num_attached = 1;
        ls->permanent = 1;
        loaded_sounds.push_back(ls);
        return (ls);
    }

    // not loaded, so try and load it

    int soundNum = find_empty_game_sound();
    if (soundNum == -1) {
        GLOBALASSERT(0 == "Run out of sound slots");
        return (0);
    }

    int ok = FindAndLoadWavFile(soundNum, (char *) fname);

    if (ok) {
        GameSounds[soundNum].loaded = 1;
        GameSounds[soundNum].activeInstances = 0;
        ;
        GameSounds[soundNum].volume = VOLUME_DEFAULT;
        GameSounds[soundNum].pitch = 0;
        InitialiseBaseFrequency((SOUNDINDEX) soundNum);

        ls = (LOADED_SOUND *) AllocateMem(sizeof(LOADED_SOUND));

        ls->sound_num = soundNum;
        ls->wavname = (char *) AllocateMem(sizeof(char) * (strlen(wavname) + 1));
        strcpy(ls->wavname, wavname);
        ls->num_attached = 1;
        ls->permanent = 0;
        loaded_sounds.push_back(ls);
    }

    return (ls);
}
