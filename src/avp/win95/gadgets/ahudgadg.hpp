/*
	
	ahudgadg.hpp

	Alien HUD gadget; a concrete derived class of abstract class "HUDGadget"

*/

#ifndef _ahudgadg
#define _ahudgadg 1

#ifndef _hudgadg
#include "hudgadg.hpp"
#endif

#ifdef __cplusplus
/* Version settings *****************************************************/

/* Constants  ***********************************************************/

/* Macros ***************************************************************/

/* Type definitions *****************************************************/
#if UseGadgets
class TextEntryGadget; // fully declared in TEXTIN.HPP

class AlienHUDGadget : public HUDGadget
{
public:
    void Render(const struct r2pos &R2Pos, const struct r2rect &R2Rect_Clip, int FixP_Alpha);

    AlienHUDGadget();
    ~AlienHUDGadget();

    void AddTextReport(
        SCString *pSCString_ToAdd
        // ultimately turn into an MCString
    );
    void ClearTheTextReportQueue(void);

#if EnableStatusPanels
    void RequestStatusPanel(enum StatusPanelIndex I_StatusPanel);

    void NoRequestedPanel(void);
#endif

    void CharTyped(
        char Ch
        // note that this _is _ a char
    );
    void Key_Backspace(void) override;
    void Key_End(void) override;
    void Key_Home(void) override;
    void Key_Left(void) override;
    void Key_Up(void) override;
    void Key_Right(void) override;
    void Key_Down(void) override;
    void Key_Delete(void) override;
    void Key_Tab(void) override;
    void Key_Return(void) override;

    void SetString(const char *text);

    void Jitter(int FixP_Magnitude);

    TextReportGadget *pTextReportGadg;

private:
    // not allowed to be NULL

    TextEntryGadget *pTextEntryGadg;
    // not allowed to be NULL
};
#endif // UseGadgets
#endif

/* Exported globals *****************************************************/

/* Function prototypes **************************************************/
void BringDownConsoleWithSayTypedIn();
void BringDownConsoleWithSaySpeciesTypedIn();

/* End of the header ****************************************************/

#endif
