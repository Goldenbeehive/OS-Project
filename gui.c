#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#define GUI_PROPERTY_LIST_IMPLEMENTATION
#include "raygui.h"
#include "gui_window_file_dialog.h"
#include "style_cyber.h"
#include "style_bluish.h"
#include <string.h>
struct process
{
    int id;
    int arrivaltime;
    int runningtime;
    int priority;
    int starttime;
    int endtime;
    int remainingtime;
    int waittime;
    int responsetime;
    int finishtime;
    int turnaroundtime;
    int lasttime;
    int flag;
};
int isInteger(const char *str)
{
    int i = 0;
    while (str[i] == ' ' || str[i] == '\t')
    {
        i++;
    }
    while (str[i] >= '0' && str[i] <= '9')
    {
        i++;
    }
    return str[i] == '\0';
}

char *processToString(struct process temp)
{
    char str[100];
    sprintf(str, "P%d AT:%d RUN:%d Remaining:%d Priority:%d", temp.id, temp.arrivaltime, temp.runningtime, temp.remainingtime, temp.priority);
    return str;
}
int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 800;
    int loop = 1;
    InitWindow(screenWidth, screenHeight, "OS Project");
    /* Title Page Variables */
    bool lightDarkBool = false;                // light/dark mode boolean
    char *lightDarkMode = "Toggle Light Mode"; // light/dark mode label text
    GuiLoadStyleCyber();                       // init state is dark
    int endOfAnimWidth = 0;                    // BG animation width
    float time = 0;                            // moves the wave

    /*Choose algorithm Page Variables*/
    bool dropdownAlgo = false;
    int algoChoice = 0;
    char filePath[1025];
    char fileText[1025];
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    bool rrError = false;
    bool txtError = false;
    char rrQuantum[100] = "";
    /*Simulation Page Variables*/
    int listViewScrollIndex = 0;
    int listViewActive = -1;
    int listViewScrollIndex2 = 0;
    int listViewActive2 = -1;
    char rdyList[1024] = "P2 AT:20 RUN:30 Remaining:25 Priority:88";
    char doneList[1024] = "P0;P1;P2;P3;P4";
    char workingList[1024] = "P5";

    /*  Page List:
        0 -> Title Page
        1 -> Choose Schduler Page
        2 -> Simulation Page
        3 -> Statistics Page
    */
    int pageShifter = 2;

    while (loop)
    {

        BeginDrawing();
        if (pageShifter == 0)
        {
            GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            /* Top Right buttons*/
            if (GuiButton((Rectangle){screenWidth - 120, 30, 100, 40}, "Close"))
            {
                loop--;
            }
            if (GuiButton((Rectangle){screenWidth - 320, 30, 180, 40}, lightDarkMode))
            {
                lightDarkBool = !lightDarkBool;
                lightDarkMode = (lightDarkBool ? "Toggle Dark Mode" : "Toggle Light Mode");
                lightDarkBool ? GuiLoadStyleBluish() : GuiLoadStyleCyber();
            }
            /*Title label*/
            GuiSetStyle(DEFAULT, TEXT_SIZE, 120);
            GuiLabel((Rectangle){30, screenHeight / 2 - (120), screenWidth, 120}, "Scheduler");
            GuiLabel((Rectangle){30, screenHeight / 2 - (120) + 100, screenWidth, 120}, "Project");
            GuiSetStyle(DEFAULT, TEXT_SIZE, 25);
            if (GuiButton((Rectangle){30, screenHeight / 2 - (120) + 240, 100, 40}, "Start"))
            {
                pageShifter = 1;
            }
            /*Background Animation*/
            for (int x = 0; x < endOfAnimWidth; x++)
            {
                float normalizedX = x / (float)screenWidth;
                float angle = PI * 2 * normalizedX + time;
                float y = sin(angle) * (screenHeight / 4);
                float y2 = cos(angle + PI / 2) * (screenHeight / 4);
                if (x == endOfAnimWidth - 1)
                {
                    DrawCircle(x, screenHeight / 2 - (int)y, 20, RED);
                    DrawCircle(x, screenHeight / 2 - (int)y2, 20, BLUE);
                }
                DrawPixel(x, screenHeight / 2 - (int)y, RED);
                DrawPixel(x, screenHeight / 2 - (int)y2, BLUE);
            }

            time += GetFrameTime();
            if (endOfAnimWidth != screenWidth - 50)
            {
                endOfAnimWidth++;
            }
        }

        if (pageShifter == 1)
        {
            if (fileDialogState.windowActive)
            {
                GuiLock();
            }
            GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            /* Top buttons*/
            if (GuiButton((Rectangle){screenWidth - 120, 30, 100, 40}, "Close"))
            {
                loop--;
            }
            if (GuiButton((Rectangle){screenWidth - 320, 30, 180, 40}, lightDarkMode))
            {
                lightDarkBool = !lightDarkBool;
                lightDarkMode = (lightDarkBool ? "Toggle Dark Mode" : "Toggle Light Mode");
                lightDarkBool ? GuiLoadStyleBluish() : GuiLoadStyleCyber();
            }
            if (GuiButton((Rectangle){30, 30, 100, 40}, "Back"))
            {
                pageShifter = 0;
            }
            /*Main Content*/
            GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
            GuiLabel((Rectangle){30, 50, screenWidth, 120}, "Choose a algorithm:");
            GuiSetStyle(DEFAULT, TEXT_SIZE, 30);

            if (GuiDropdownBox((Rectangle){30, 180, 600, 60}, "Round Robin;Highest Priority First;Shortest Time Remaining Next", &algoChoice, dropdownAlgo))
            {
                dropdownAlgo = !dropdownAlgo;
            }
            if (algoChoice == 0)
            {
                GuiLabel((Rectangle){730, 130, 500, 60}, "Enter Quantum:");
                GuiTextBox((Rectangle){730, 180, 500, 60}, rrQuantum, 20, true);
                if (rrError)
                {
                    DrawText("Please enter a valid quantum.", 730, 280, 30, RED);
                }
            }
            GuiSetStyle(DEFAULT, TEXT_SIZE, 80);
            strcpy(fileText, "File :");
            strcat(fileText, fileDialogState.dirPathText);
            strcat(fileText, "/");
            strcat(fileText, fileDialogState.fileNameText);
            GuiLabel((Rectangle){30, 500, screenWidth, 120}, "Choose your process text file:");
            GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
            if (txtError)
            {
                DrawText("Input file must be a txt file.", 30, 600, 30, RED);
            }
            GuiLabel((Rectangle){30, 700, screenWidth, 120}, fileText);
            if (GuiButton((Rectangle){30, 650, 140, 30}, GuiIconText(ICON_FILE_OPEN, "Open File")))
            {
                fileDialogState.windowActive = true;
            }
            if (GuiButton((Rectangle){screenWidth - 120, screenHeight - 50, 100, 40}, "Simulate"))
            {
                txtError = false;
                rrError = false;
                if (strstr(fileDialogState.fileNameText, ".txt") == NULL)
                {
                    txtError = true;
                }
                if (algoChoice == 0)
                {
                    if (!isInteger(rrQuantum) || rrQuantum[0] == '\0')
                    {
                        rrError = true;
                    }
                }
                if (!txtError && !rrError)
                {
                    pageShifter = 2;
                }
            }

            GuiUnlock();

            GuiWindowFileDialog(&fileDialogState);
            /* Background Animation */
            for (int x = 0; x < endOfAnimWidth; x++)
            {
                float normalizedX = x / (float)screenWidth;
                float angle = PI * 2 * normalizedX + time;
                float y = sin(angle) * (screenHeight / 4);
                float y2 = cos(angle + PI / 2) * (screenHeight / 4);
                if (x == endOfAnimWidth - 1)
                {
                    DrawCircle(x, screenHeight / 2 - (int)y, 20, RED);
                    DrawCircle(x, screenHeight / 2 - (int)y2, 20, BLUE);
                }
                DrawPixel(x, screenHeight / 2 - (int)y, RED);
                DrawPixel(x, screenHeight / 2 - (int)y2, BLUE);
            }
            time += GetFrameTime();
            if (endOfAnimWidth != screenWidth - 50)
            {
                endOfAnimWidth++;
            }
        }
        if (pageShifter == 2)
        {
            GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            /* Top buttons*/
            if (GuiButton((Rectangle){screenWidth - 120, 30, 100, 40}, "Close"))
            {
                loop--;
            }
            if (GuiButton((Rectangle){screenWidth - 320, 30, 180, 40}, lightDarkMode))
            {
                lightDarkBool = !lightDarkBool;
                lightDarkMode = (lightDarkBool ? "Toggle Dark Mode" : "Toggle Light Mode");
                lightDarkBool ? GuiLoadStyleBluish() : GuiLoadStyleCyber();
            }
            GuiGroupBox((Rectangle){120, 150, 400, 200}, "Working Process");
            DrawText(workingList, 310, 235, 20, PURPLE);
            GuiGroupBox((Rectangle){120, 400, 400, 300}, "Done Processes");
            GuiListView((Rectangle){120, 420, 400, 300}, doneList, &listViewScrollIndex, &listViewActive);
            GuiGroupBox((Rectangle){screenWidth - 550, 150, 400, 300}, "RDY Queue");
            GuiListView((Rectangle){screenWidth - 550, 150 + 20, 400, 300}, rdyList, &listViewScrollIndex2, &listViewActive2);
            GuiGroupBox((Rectangle){screenWidth - 550, 500, 400, 200}, "Memory");
            DrawText("MEM WIP", screenWidth - 550 + 150, 590, 20, RED);
            if (GuiButton((Rectangle){screenWidth - 300, screenHeight - 50, 100, 40}, "Open Debug Stats"))
            {
                pageShifter = 3;
            }

            /* Background Animation */
            for (int x = 0; x < endOfAnimWidth; x++)
            {
                float normalizedX = x / (float)screenWidth;
                float angle = PI * 2 * normalizedX + time;
                float y = sin(angle) * (screenHeight / 4);
                float y2 = cos(angle + PI / 2) * (screenHeight / 4);
                if (x == endOfAnimWidth - 1)
                {
                    DrawCircle(x, screenHeight / 2 - (int)y, 20, RED);
                    DrawCircle(x, screenHeight / 2 - (int)y2, 20, BLUE);
                }
                DrawPixel(x, screenHeight / 2 - (int)y, RED);
                DrawPixel(x, screenHeight / 2 - (int)y2, BLUE);
            }
            time += GetFrameTime();
            if (endOfAnimWidth != screenWidth - 50)
            {
                endOfAnimWidth++;
            }
        }
        if (pageShifter == 3)
        {
            GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            /* Top buttons*/
            if (GuiButton((Rectangle){screenWidth - 120, 30, 100, 40}, "Close"))
            {
                loop--;
            }
            if (GuiButton((Rectangle){screenWidth - 320, 30, 180, 40}, lightDarkMode))
            {
                lightDarkBool = !lightDarkBool;
                lightDarkMode = (lightDarkBool ? "Toggle Dark Mode" : "Toggle Light Mode");
                lightDarkBool ? GuiLoadStyleBluish() : GuiLoadStyleCyber();
            }
            GuiGroupBox((Rectangle){screenWidth / 2 - 200, screenHeight / 2 - 100, 400, 270}, "Performance");
            DrawText("CPU utilization = 100%", screenWidth / 2 - 200 + 50, screenHeight / 2 - 100 + 50, 20, LIGHTGRAY);
            DrawText("Avg WTA = 1.34", screenWidth / 2 - 200 + 50, screenHeight / 2 - 100 + 100, 20, LIGHTGRAY);
            DrawText("Avg Waiting = 1.5", screenWidth / 2 - 200 + 50, screenHeight / 2 - 100 + 150, 20, LIGHTGRAY);
            DrawText("Std WTA = 0.34", screenWidth / 2 - 200 + 50, screenHeight / 2 - 100 + 200, 20, LIGHTGRAY);
            if (GuiButton((Rectangle){screenWidth / 2 - 100, screenHeight - 50, 200, 40}, "Back to Title Screen"))
            {
                pageShifter = 0;
            }
            for (int x = 0; x < endOfAnimWidth; x++)
            {
                float normalizedX = x / (float)screenWidth;
                float angle = PI * 2 * normalizedX + time;
                float y = sin(angle) * (screenHeight / 4);
                float y2 = cos(angle + PI / 2) * (screenHeight / 4);
                if (x == endOfAnimWidth - 1)
                {
                    DrawCircle(x, screenHeight / 2 - (int)y, 20, RED);
                    DrawCircle(x, screenHeight / 2 - (int)y2, 20, BLUE);
                }
                DrawPixel(x, screenHeight / 2 - (int)y, RED);
                DrawPixel(x, screenHeight / 2 - (int)y2, BLUE);
            }
            time += GetFrameTime();
            if (endOfAnimWidth != screenWidth - 50)
            {
                endOfAnimWidth++;
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
