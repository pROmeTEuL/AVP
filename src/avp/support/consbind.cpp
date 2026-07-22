/*******************************************************************
 *
 *    DESCRIPTION: 	consbind.cpp
 *
 *		Ability to kind keystrokes to strings so they appear in the
 *	console when you hit the key.  Initial implementation went through
 * the WM_KEYDOWN hook so that we get debouncing and typematic action
 * for free; subsequently added an implementation based on the KeyboadInput[]
 * array.
 *
 *    AUTHOR: David Malcolm
 *
 *    HISTORY:  Created 6/4/98
 *
 *******************************************************************/

/* Includes ********************************************************/
#include <ctype.h>

#include "3dc.h"
#include "consbind.hpp"

#if KeyBindingUses_KEY_ID
#include "iofocus.h"
#include "scstring.hpp"
#include "strtab.hpp"
#endif

#define UseLocalAssert true
#include "ourasert.h"
#include "avp_menus.h"

/* Version settings ************************************************/

/* Constants *******************************************************/
#if KeyBindingUses_WM_KEYDOWN
#define MAX_VALUE_BINDABLE_KEY (VK_DIVIDE)
#endif

#if KeyBindingUses_KEY_ID
#define MAX_VALUE_BINDABLE_KEY (MAX_NUMBER_OF_INPUT_KEYS)
#endif
/* Macros **********************************************************/

/* Imported function prototypes ************************************/

/* Imported data ***************************************************/
extern unsigned char KeyboardInput[];
extern unsigned char DebouncedKeyboardInput[];

/* Exported globals ************************************************/

/* Internal type definitions ***************************************/
typedef enum TEXTSTRING_ID TextID;

/* Internal function prototypes ************************************/

/* Internal globals ************************************************/

/* Exported function definitions ***********************************/
// class KeyBinding
// public:

// static
void KeyBinding ::ParseBindCommand(ProjChar *pProjCh_ToParse)
{
    GLOBALASSERT(pProjCh_ToParse);

    BindableKey theKey;
    ProjChar *pProjCh_FollowingTheKey;

    if (KeyBinding ::ParseBindCommand(
            theKey,                   // BindableKey& theKey_Out,
            &pProjCh_FollowingTheKey, // ProjChar** ppProjCh_Out,
                                      // returns where in the input string to continue processing

            pProjCh_ToParse // ProjChar* pProjCh_In
            )) {
        SCString *pSCString_ToBind = new SCString(pProjCh_FollowingTheKey);

        // Create the KeyBinding object:
        KeyBinding *pNewBinding;
        pNewBinding = new KeyBinding(theKey, pSCString_ToBind);

        // Feedback:
        {
            SCString *pSCString_1 = new SCString("BOUND \"");
            // LOCALISEME

            SCString *pSCString_2 = new SCString("\" TO ");

            SCString *pSCString_3 = MakeStringForKey(theKey);

            SCString *pSCString_Feedback
                = new SCString(pSCString_1, pSCString_ToBind, pSCString_2, pSCString_3);

            pSCString_Feedback->SendToScreen();

            pSCString_Feedback->R_Release();
            pSCString_3->R_Release();
            pSCString_2->R_Release();
            pSCString_1->R_Release();
        }

        pSCString_ToBind->R_Release();
    } else {
        // Can't recognise which key is to be bound to;
        // provide an error message
        // UNWRITTEN
    }
}

// static
void KeyBinding ::ParseUnbindCommand(ProjChar *pProjCh_ToParse)
{
    GLOBALASSERT(pProjCh_ToParse);

    // Iterate through leading whitespace:
    {
        while (*pProjCh_ToParse) {
            // LOCALISEME:
            if (!isspace(*pProjCh_ToParse)) {
                break;
            }
            pProjCh_ToParse++;
        }
    }

    // Scan through the string, trying to find matches against strings for keys
    // We will use the longest match:
    {
        OurBool bGotMatch = false;
        unsigned int LongestMatch = 0;
        BindableKey theKey_ToUnbind = (BindableKey) 0;

        for (int i = 0; i < MAX_VALUE_BINDABLE_KEY; i++) {
            BindableKey theKey = (BindableKey) i;

            SCString *pSCString_TestKey = MakeStringForKey(theKey);

            unsigned int LengthOfTestString = pSCString_TestKey->GetNumChars();

            if (LengthOfTestString > 0) {
                if (0 == _strnicmp(pSCString_TestKey->pProjCh(), pProjCh_ToParse, LengthOfTestString)
                    // LOCALISEME
                ) {
                    // Then we have a match; see if it's longer than
                    // what's come before...
                    if (LengthOfTestString > LongestMatch) {
                        LongestMatch = LengthOfTestString;

                        theKey_ToUnbind = theKey;
                        bGotMatch = true;
                    }
                }
            }

            pSCString_TestKey->R_Release();
        }

        if (bGotMatch) {
            // Then we must get rid of all bindings with this as their key:
            for (auto oi = List_pKeyBindings.begin(); oi != List_pKeyBindings.end();) {
                GLOBALASSERT(oi());
                if ((*oi)->theKey == theKey_ToUnbind) {
                    oi = List_pKeyBindings.erase(oi);
                } else {
                    ++oi;
                }
            }
        }
    }
}

#if 0
// static
void
KeyBinding :: AttemptToBind
(
	SCString* pSCString_Key, // description of key
	SCString* pSCString_ToBind // string to be bound
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pSCString_Key );
		GLOBALASSERT( pSCString_ToBind );
	}

	/* CODE */
	{
		BindableKey theBindableKey_ToUse;

		if
		(
			!bGetKeyForString
			(
				theBindableKey_ToUse, // BindableKey& theKey_Out,
				pSCString_Key -> pProjCh()
			)
		)
		{
			// Can't recognise which key is to be bound to;
			// provide an error message
			ErrorDontRecogniseKey(pSCString_Key);

			return;
		}

		// Create the KeyBinding object:
		new KeyBinding
		(
			theBindableKey_ToUse,
			pSCString_ToBind
		);
	}
}

// static
void
KeyBinding :: AttemptToUnbind
(			
	SCString* pSCString_Key // description of key
)
{
	/* PRECONDITION */
	{
		GLOBALASSERT( pSCString_Key );
	}

	/* CODE */
	{
		BindableKey theBindableKey_ToUse;

		if
		(
			!bGetKeyForString
			(
				theBindableKey_ToUse, // BindableKey& theKey_Out,
				pSCString_Key -> pProjCh()
			)
		)
		{
			// Can't recognise which key is to be bound to;
			// provide an error message
			ErrorDontRecogniseKey(pSCString_Key);

			return;
		}

		// Find and remove any bindings to that key; note the number:

		// Provide feedback:
	}
}
#endif

// static
void KeyBinding ::UnbindAll(void)
{
    SCString *pSCString_Feedback = new SCString("DESTROYING ALL KEY BINDINGS");
    // LOCALISEME

    pSCString_Feedback->SendToScreen();

    pSCString_Feedback->R_Release();

    // while (List_pKeyBindings.size() > 0) {
    //     delete *List_pKeyBindings.begin();
    //     List_pKeyBindings.erase(List_pKeyBindings.begin());
    //     // The destructor for the KeyBinding will remove
    //     // it from the list and hence the list will shrink.
    // }
    List_pKeyBindings.clear();
}

// static
void KeyBinding ::ListAllBindings(void)
{
    SCString *pSCString_Feedback = new SCString("LIST OF ALL KEY BINDINGS:");
    // LOCALISEME

    pSCString_Feedback->SendToScreen();

    pSCString_Feedback->R_Release();

    for (auto &oi : List_pKeyBindings) {
        GLOBALASSERT(oi());
        oi->ListThis();
    }
}

// static
void KeyBinding ::WriteToConfigFile(char *Filename)
{
    // overwrites the file with a batch file that'll
    // restore current bindings

    GLOBALASSERT(Filename);

    FILE *pFile = OpenGameFile(Filename, FILEMODE::WRITEONLY, FILETYPE::CONFIG);

    if (!pFile) {
        return;
        // and don't destroy the bindings, since we won't
        // restore them next time into game
    }

    fprintf(pFile, "#This file generated by AVP\n");
    // LOCALISEME

    for (auto &oi : List_pKeyBindings) {
        SCString *pSCString_Key = MakeStringForKey(oi->theKey);

        fprintf(pFile, "BIND %s %s\n", pSCString_Key->pProjCh(), oi->pSCString_ToOutput->pProjCh());

        pSCString_Key->R_Release();
    }

    fclose(pFile);

    // Destroy all the current bindings so we don't get a duplicate
    // set next time the batch file fires:
    {
        // while (List_pKeyBindings.size() > 0) {
        //     delete *List_pKeyBindings.begin();
        //     List_pKeyBindings.erase(List_pKeyBindings.begin());
        //     // The destructor for the KeyBinding will remove
        //     // it from the list and hence the list will shrink.
        // }
        List_pKeyBindings.clear();
    }
}

#if KeyBindingUses_WM_KEYDOWN
// static
void KeyBinding ::Process_WM_KEYDOWN(WPARAM wParam)
{
    // Iterate through the list, finding matches.
    // We must process the matching objects later, in case they are bound to things
    // which modify the list.  So we built a list of pending SCString*'s.
    // (so BIND X UNBIND X won't kill the program.  I hope)

    // The list of pending SCStrings has been made static to the class in an attempt to
    // optimise this function

    // Ensure it starts off empty (which it would if it was a local):
    GLOBALASSERT(0 == PendingList.NumEntries());

    for (LIF<KeyBinding *> oi(&List_pKeyBindings); !oi.done(); oi.next()) {
        // Note that the BindableKey type is type-equal to WPARAM if this function
        // exists:
        if (oi()->theKey == wParam) {
            // Add _the_string_ to the pending list (with a reference)
            // The reference is added by the RefList template, and this ensures
            // the string stays alive whatever that pesky user does:
            GLOBALASSERT(oi()->pSCString_ToOutput);
            PendingList.AddToEnd(*(oi()->pSCString_ToOutput));

            if (bEcho) {
                oi()->pSCString_ToOutput->SendToScreen();
            }
        }
    }

    // Iterate through the pending list, destructively reading the
    // "references" from the front:
    {
        SCString *pSCString;

        // The assignment in this boolean expression is deliberate:
        while (NULL != (pSCString = PendingList.GetYourFirst())) {
            pSCString->ProcessAnyCheatCodes();
            pSCString->R_Release();
        }
    }

    // Ensure the pending list finishes off empty
    // (since we're pretending it's a local variable):
    GLOBALASSERT(0 == PendingList.NumEntries());
}
#endif

#if KeyBindingUses_KEY_ID
// static
void KeyBinding ::Maintain(void)
{
    // Only process if we're in a running-around type of mode
    // rather than typing at the console:
    if (IOFOCUS_AcceptControls()) {
        // Iterate through the list, finding matches.
        // We must process the matching objects later, in case they are bound to things
        // which modify the list.  So we built a list of pending SCString*'s.
        // (so BIND X UNBIND X won't kill the program.  I hope)

        // The list of pending SCStrings has been made static to the class in an attempt to
        // optimise this function

        // Ensure it starts off empty (which it would if it was a local):
        GLOBALASSERT(0 == PendingList.NumEntries());

        for (auto &oi : List_pKeyBindings) {
            // Note that the BindableKey type is type-equal to enum KEY_ID if this function
            // exists:
            if (DebouncedKeyboardInput[oi->theKey]) {
                // Add _the_string_ to the pending list (with a reference)
                // The reference is added by the RefList template, and this ensures
                // the string stays alive whatever that pesky user does:
                GLOBALASSERT(oi()->pSCString_ToOutput);
                PendingList.AddToEnd(*(oi->pSCString_ToOutput));

                if (bEcho) {
                    oi->pSCString_ToOutput->SendToScreen();
                }
            }
        }

        // Iterate through the pending list, destructively reading the
        // "references" from the front:
        {
            SCString *pSCString;

            // The assignment in this boolean expression is deliberate:
            while (NULL != (pSCString = PendingList.GetYourFirst())) {
                pSCString->ProcessAnyCheatCodes();
                pSCString->R_Release();
            }
        }

        // Ensure the pending list finishes off empty
        // (since we're pretending it's a local variable):
        GLOBALASSERT(0 == PendingList.NumEntries());
    }
}
#endif

// private:
// Private ctor/dtor; to be called only by static fns of the class:
KeyBinding ::KeyBinding(BindableKey theKey_ToUse, SCString *pSCString_ToBind)
    : theKey(theKey_ToUse)
    , pSCString_ToOutput(pSCString_ToBind)
{
    GLOBALASSERT(pSCString_ToOutput);

    pSCString_ToOutput->R_AddRef();

    List_pKeyBindings.emplace_back(this);
}

KeyBinding ::~KeyBinding()
{
    // List_pKeyBindings.erase(std::ranges::find(List_pKeyBindings, this));

    pSCString_ToOutput->R_Release();
}

#if 0
// static
OurBool
KeyBinding :: bGetKeyForString
(
	BindableKey& theKey_Out,
	const ProjChar* const pProjCh_In
)
{
#if KeyBindingUses_WM_KEYDOWN
	{
		// takes an input string and tries to figure out
		// what the corresponding WPARAM would be...
		// Returns truth if it manages to get a sensible value
		// into the output area.

		GLOBALASSERT( pProjCh_In );

#if 1
		theKey_Out = pProjCh_In[0];
		return true;
			// LOCALISEME
#else

		return false;
			// for now
#endif
	}
#endif

#if KeyBindingUses_KEY_ID
	{
		theKey_Out = KEY_NUMPADDEL;
		return true;
			// for now
	}
#endif
}
#endif

// static
void KeyBinding ::ErrorDontRecogniseKey(SCString *pSCString_Key)
{
    GLOBALASSERT(pSCString_Key);

    SCString *pSCString_1 = new SCString("UNRECOGNISED KEY: \"");
    SCString *pSCString_2 = new SCString("\"");
    // LOCALISEME

    SCString *pSCString_Feedback = new SCString(pSCString_1, pSCString_Key, pSCString_2);

    pSCString_Feedback->SendToScreen();

    pSCString_Feedback->R_Release();
    pSCString_2->R_Release();
    pSCString_1->R_Release();
}

void KeyBinding ::ListThis(void) const
{
    // used by ListAllBindings()
    SCString *pSCString_1 = MakeStringForKey(theKey);
    SCString *pSCString_2 = new SCString(" -> \"");
    // LOCALISEME
    SCString *pSCString_3 = new SCString("\"");

    SCString *pSCString_Feedback
        = new SCString(pSCString_1, pSCString_2, pSCString_ToOutput, pSCString_3);

    pSCString_Feedback->SendToScreen();

    pSCString_Feedback->R_Release();
    pSCString_3->R_Release();
    pSCString_2->R_Release();
    pSCString_1->R_Release();
}

static int GetKeyLabel(int inPhysicalKey, TextID &outTextID)
{
    // takes a physical method key and attempts to find a text
    // string to use for it, returning whether it does.
    // If it fails, output area is untouched
    if (inPhysicalKey >= KEY_LEFT && inPhysicalKey <= KEY_MOUSEWHEELDOWN) {
        outTextID = (enum TEXTSTRING_ID)(TEXTSTRING_KEYS_LEFT + (inPhysicalKey - KEY_LEFT));
        return true;
    } else
        return false;

#if 0
	switch (inPhysicalKey)
	{
		case KEY_UP: outTextID = TEXTSTRING_KEYS_UP; return true;
		case KEY_DOWN: outTextID = TEXTSTRING_KEYS_DOWN; return true;
		case KEY_LEFT: outTextID = TEXTSTRING_KEYS_LEFT; return true;
		case KEY_RIGHT: outTextID = TEXTSTRING_KEYS_RIGHT; return true;
		case KEY_CR: outTextID = TEXTSTRING_KEYS_RETURN; return true;
		case KEY_TAB: outTextID = TEXTSTRING_KEYS_TAB; return true;
		case KEY_INS: outTextID = TEXTSTRING_KEYS_INSERT; return true;
		case KEY_DEL: outTextID = TEXTSTRING_KEYS_DELETE; return true;
		case KEY_END: outTextID = TEXTSTRING_KEYS_END; return true;
		case KEY_HOME: outTextID = TEXTSTRING_KEYS_HOME; return true;
		case KEY_PAGEUP: outTextID = TEXTSTRING_KEYS_PGUP; return true;
		case KEY_PAGEDOWN: outTextID = TEXTSTRING_KEYS_PGDOWN; return true;
		case KEY_BACKSPACE: outTextID = TEXTSTRING_KEYS_BACKSP; return true;
		case KEY_COMMA: outTextID = TEXTSTRING_KEYS_COMMA; return true;
		case KEY_FSTOP: outTextID = TEXTSTRING_KEYS_PERIOD; return true;
		case KEY_SPACE: outTextID = TEXTSTRING_KEYS_SPACE; return true;
		case KEY_LMOUSE: outTextID = TEXTSTRING_KEYS_LMOUSE; return true;
		case KEY_RMOUSE: outTextID = TEXTSTRING_KEYS_RMOUSE; return true;
		case KEY_LEFTALT: outTextID = TEXTSTRING_KEYS_LALT; return true;
		case KEY_RIGHTALT: outTextID = TEXTSTRING_KEYS_RALT; return true;
		case KEY_LEFTCTRL: outTextID = TEXTSTRING_KEYS_LCTRL; return true;
		case KEY_RIGHTCTRL: outTextID = TEXTSTRING_KEYS_RCTRL; return true;
		case KEY_LEFTSHIFT: outTextID = TEXTSTRING_KEYS_LSHIFT; return true;
		case KEY_RIGHTSHIFT: outTextID = TEXTSTRING_KEYS_RSHIFT; return true;
		case KEY_CAPS: outTextID = TEXTSTRING_KEYS_CAPS; return true;
		case KEY_NUMLOCK: outTextID = TEXTSTRING_KEYS_NUMLOCK; return true;
		case KEY_SCROLLOK: outTextID = TEXTSTRING_KEYS_SCRLOCK; return true;
		case KEY_NUMPAD0: outTextID = TEXTSTRING_KEYS_PAD0; return true;
		case KEY_NUMPAD1: outTextID = TEXTSTRING_KEYS_PAD1; return true;
		case KEY_NUMPAD2: outTextID = TEXTSTRING_KEYS_PAD2; return true;
		case KEY_NUMPAD3: outTextID = TEXTSTRING_KEYS_PAD3; return true;
		case KEY_NUMPAD4: outTextID = TEXTSTRING_KEYS_PAD4; return true;
		case KEY_NUMPAD5: outTextID = TEXTSTRING_KEYS_PAD5; return true;
		case KEY_NUMPAD6: outTextID = TEXTSTRING_KEYS_PAD6; return true;
		case KEY_NUMPAD7: outTextID = TEXTSTRING_KEYS_PAD7; return true;
		case KEY_NUMPAD8: outTextID = TEXTSTRING_KEYS_PAD8; return true;
		case KEY_NUMPAD9: outTextID = TEXTSTRING_KEYS_PAD9; return true;
		case KEY_NUMPADSUB: outTextID = TEXTSTRING_KEYS_PADSUB; return true;
		case KEY_NUMPADADD: outTextID = TEXTSTRING_KEYS_PADADD; return true;
		case KEY_NUMPADDEL: outTextID = TEXTSTRING_KEYS_PADDEL; return true;
		default: return false;
	}
#endif
}

static SCString *GetMethodString(BindableKey inPhysicalKey)
{
    TextID theTextID;

    if (GetKeyLabel(
            inPhysicalKey,
            theTextID // TextID& outTextID
            )) {
        return &StringTable ::GetSCString(theTextID);
    } else {
        ProjChar theProjChar[2];

        if (inPhysicalKey >= KEY_A && inPhysicalKey <= KEY_Z) {
            theProjChar[0] = ProjChar(int(inPhysicalKey) - KEY_A + 'A');
        } else if (inPhysicalKey >= KEY_0 && inPhysicalKey <= KEY_9) {
            theProjChar[0] = ProjChar(int(inPhysicalKey) - KEY_0 + '0');
        } else {
            theProjChar[0] = 0;
        }

        theProjChar[1] = 0;

        return new SCString(theProjChar);
    }
}

// static
SCString *KeyBinding ::MakeStringForKey(BindableKey theKey)
{
#if KeyBindingUses_WM_KEYDOWN
    {
        ProjChar tempProjCh[2];

        tempProjCh[0] = (ProjChar) theKey;
        tempProjCh[1] = 0;
        // LOCALISEME

        return new SCString(&tempProjCh[0]);
    }
#endif

#if KeyBindingUses_KEY_ID
    {
        return GetMethodString(theKey);
    }
#endif
}

// static
OurBool KeyBinding ::ParseBindCommand(
    BindableKey &theKey_Out,
    ProjChar **ppProjCh_Out,
    // returns where in the input string to continue processing

    ProjChar *pProjCh_In)
{
    // returns Yes if it understands the binding and fills out the output

    GLOBALASSERT(ppProjCh_Out);
    GLOBALASSERT(pProjCh_In);

    // Iterate through leading whitespace:
    {
        while (*pProjCh_In) {
            // LOCALISEME:
            if (!isspace(*pProjCh_In)) {
                break;
            }
            pProjCh_In++;
        }
    }

    // Scan through the string, trying to find matches against strings for keys
    // We will use the longest match:
    {
        OurBool bGotMatch = false;
        unsigned int LongestMatch = 0;

        for (int i = 0; i < MAX_VALUE_BINDABLE_KEY; i++) {
            BindableKey theKey = (BindableKey) i;

            SCString *pSCString_TestKey = MakeStringForKey(theKey);

            unsigned int LengthOfTestString = pSCString_TestKey->GetNumChars();

            if (LengthOfTestString > 0) {
                if (0 == _strnicmp(pSCString_TestKey->pProjCh(), pProjCh_In, LengthOfTestString)
                    // LOCALISEME
                ) {
                    // Then we have a match; see if it's longer than
                    // what's come before...
                    if (LengthOfTestString > LongestMatch) {
                        LongestMatch = LengthOfTestString;

                        theKey_Out = theKey;
                        *ppProjCh_Out = pProjCh_In + LengthOfTestString;
                        // Continue processing after the string
                        bGotMatch = true;
                    }
                }
            }

            pSCString_TestKey->R_Release();
        }

        if (bGotMatch) {
            // Strip away whitespace following the key string:
            {
                while (**ppProjCh_Out) {
                    // LOCALISEME:
                    if (!isspace(**ppProjCh_Out)) {
                        break;
                    }
                    (*ppProjCh_Out)++;
                }
            }
        }

        return bGotMatch;
    }
}

// private:
// Maintain a static list of all of objects of the class:
// static
std::vector<std::unique_ptr<KeyBinding>> KeyBinding ::List_pKeyBindings;

// static
RefList<SCString> KeyBinding ::PendingList;

// public:
// static
int KeyBinding ::bEcho = false;

void CONSBIND_WriteKeyBindingsToConfigFile(void)
{
#if !(PREDATOR_DEMO | MARINE_DEMO || ALIEN_DEMO || DEATHMATCH_DEMO)
    KeyBinding ::WriteToConfigFile("config.cfg");
#endif
}

/* Internal function definitions ***********************************/
