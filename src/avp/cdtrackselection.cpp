#include "3dc.h"
#include "ourasert.h"
#include "psndplat.h"
#include "dxlog.h"
#include "cd_player.h"
#include "avp_menus.h"
#include "gamedef.h"

#include "avp_envinfo.h"

#include "list_tem.hpp"

#include <vector>

//lists of tracks for each level
std::vector<int> LevelCDTracks[AVP_ENVIRONMENT_END_OF_LIST];

//lists of tracks for each species in multiplayer games
std::vector<int> MultiplayerCDTracks[3];

static int LastTrackChosen = -1;

void EmptyCDTrackList()
{
    for (int i = 0; i < AVP_ENVIRONMENT_END_OF_LIST; i++)
        LevelCDTracks[i].clear();

    for (int i = 0; i < 3; i++)
        MultiplayerCDTracks[i].clear();
}

#define CDTrackFileName "cd tracks.txt"

static void ExtractTracksForLevel(char *&buffer, std::vector<int> &track_list)
{
    //search for a line starting with a #
    while (*buffer) {
        if (*buffer == '#')
            break;
        //search for next line
        while (*buffer) {
            if (*buffer == '\n') {
                buffer++;
                if (*buffer == '\r')
                    buffer++;
                break;
            }
            buffer++;
        }
    }

    while (*buffer) {
        //search for a track number or comment
        if (*buffer == ';') {
            //comment , so no further info on this line
            break;
        } else if (*buffer == '\n' || *buffer == '\r') {
            //reached end of line
            break;
        } else if (*buffer >= '0' && *buffer <= '9') {
            int track = -1;
            //find a number , add it to the list
            sscanf(buffer, "%d", &track);

            if (track >= 0) {
                track_list.push_back(track);
            }

            //skip to the next non numerical character
            while (*buffer >= '0' && *buffer <= '9')
                buffer++;
        } else {
            buffer++;
        }
    }

    //go to the next line
    while (*buffer) {
        if (*buffer == '\n') {
            buffer++;
            if (*buffer == '\r')
                buffer++;
            break;
        }
        buffer++;
    }
}

void LoadCDTrackList()
{
    //clear out the old list first
    EmptyCDTrackList();

    FILE *file = OpenGameFile(CDTrackFileName, FILEMODE::READONLY, FILETYPE::OPTIONAL);

    if (file == NULL) {
        LOGDXFMT(("Failed to open %s", CDTrackFileName));
        return;
    }

    char *buffer;
    int file_size;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    //copy the file contents into a buffer
    buffer = new char[file_size + 1];
    fread(buffer, 1, file_size, file);
    fclose(file);

    char *bufferptr = buffer;

    //first extract the multiplayer tracks
    for (int i = 0; i < 3; i++) {
        ExtractTracksForLevel(bufferptr, MultiplayerCDTracks[i]);
    }

    //now the level tracks
    for (int i = 0; i < AVP_ENVIRONMENT_END_OF_LIST; i++) {
        ExtractTracksForLevel(bufferptr, LevelCDTracks[i]);
    }

    delete[] buffer;
}

static unsigned int TrackSelectCounter = 0;

static bool PickCDTrack(std::vector<int> &track_list)
{
    //make sure we have some tracks in the list
    if (track_list.empty())
        return FALSE;

    //pick the next track in the list
    unsigned int index = TrackSelectCounter % track_list.size();

    TrackSelectCounter++;

    //play it
    CDDA_Stop();
    CDDA_Play(track_list[index]);

    LastTrackChosen = track_list[index];
    return TRUE;
}

void CheckCDAndChooseTrackIfNeeded()
{
    static enum playertypes lastPlayerType;

    //are we bothering with cd tracks
    if (!CDDA_IsOn())
        return;
    //is our current track still playing
    if (CDDA_IsPlaying()) {
        //if in a multiplayer game see if we have changed character type
        if (AvP.Network == I_No_Network || AvP.PlayerType == lastPlayerType)
            return;

        //have changed character type , is the current track in the list for this character type
        if (std::find(MultiplayerCDTracks[AvP.PlayerType].begin(),
                      MultiplayerCDTracks[AvP.PlayerType].end(),
                      LastTrackChosen) != MultiplayerCDTracks[AvP.PlayerType].end())
            return;

        //Lets choose a new track then
    }

    if (AvP.Network == I_No_Network) {
        int level = NumberForCurrentLevel();
        if (level >= 0 && level < AVP_ENVIRONMENT_END_OF_LIST) {
            //pick track based on level
            if (PickCDTrack(LevelCDTracks[level])) {
                return;
            }
        }
    }

    //multiplayer (or their weren't ant level specific tracks)
    lastPlayerType = AvP.PlayerType;
    PickCDTrack(MultiplayerCDTracks[AvP.PlayerType]);
}

void ResetCDPlayForLevel()
{
    //check the number of tracks available while we're at it
    CDDA_CheckNumberOfTracks();

    TrackSelectCounter = 0;
    CDDA_Stop();
}
