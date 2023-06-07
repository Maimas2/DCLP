#include "Resources.h"
#include "imgui.h"
#include "main.h"
#include "utils.h"
#include "Text.h"
#include "ImguiManager.h"
#include "Saves-class.h"
#include "Log.h"
#include "Saves.h"
#include "handler.h"
#include "OSspecifics.h"

#include <filesystem>
#include <iostream>
#include <string>

extern char* expansionName;

Save* expansionSave = nullptr;
string currentExpansionSave = "";
vector<string> savePaths;
vector<string> cardNames;

int currentImagePreview = 0;

void initExpansion() {
    if(expansionSave != nullptr) {
        //expansionSave->save(currentExpansionSave + "master.mdclp"); Commented out because it would overwrite last expansion loaded when creating a new one.
        Log::debug("Saved expansion to " + currentExpansionSave + "master.mdclp");
        delete expansionSave;
        savePaths.clear();
        cardNames.clear();
    }
}

string createExpansion(string path) {
    initExpansion();
    if(filesystem::exists(path)) {
        Log::log("Expansion already exists!");
        return "Expansion already exists!";
    }
    if(!filesystem::create_directories(path)) {
        return "Failed to create directory!";
    }
    expansionSave = new Save;
    currentExpansionSave = path;

    expansionSave->charPointers["name"] = &expansionName;

    return "";
}

void reloadNames(string path) {
    vector<string> paths;
    savePaths.clear();
    cardNames.clear();
    for(filesystem::directory_entry file : filesystem::directory_iterator(path)) {
        if(!file.path().string().ends_with(".dclp")) {
            continue;
        }
        paths.push_back(file.path());
        Log::log("Found save file: " + file.path().string());
    }
    for(int i = 0; i < paths.size(); i++) {
        Save* temp = new Save;
        temp->loadDefault();
        temp->read(paths[i]);
        savePaths.push_back(paths[i]);
        cardNames.push_back(string(cardTitle));
        delete temp;
    }
}

string loadExpansion(string path) {
    initExpansion();
    Log::debug("Attempting to load expansion " + path);
    if(!filesystem::exists(path)) {
        return "Path does not exist!";
    }

    reloadNames(path);

    expansionSave = new Save;
    currentExpansionSave = path;

    expansionSave->charPointers["name"] = &expansionName;

    expansionSave->read(currentExpansionSave + "master.mdclp");

    return "";
}
void expansionExit();
bool isDrawingPreivew = false;
void expansionImgui() {
    if(ImGui::Button("Exit to main menu")) {
        expansionExit();
        uiMode = 0;
        return;
    }
    ImGui::InputText("Name", expansionName, 120);
    if(ImGui::Button("Add new card")) {
        ImGui::OpenPopup("New Card");
        expansionUrl[0] = '\0';
    }
    ImGui::NewLine();
    float windowWidth = ImGui::GetWindowSize().x;
    for(int i = 0; i < cardNames.size(); i++) {
        string baseName = (cardNames[i] == "" ? "[empty]" : cardNames[i]);
        if(ImGui::Button(("Edit " + baseName).c_str())) {
            cout << i << 1 << endl;
            Saves::read(savePaths[i]);
            currentFile = savePaths[i];
            uiMode = 3;
            reloadPictures();
        }
        ImGui::SameLine(windowWidth - (ImGui::CalcTextSize(("Preview " + baseName).c_str()).x + 20));
        if(ImGui::Button(("Preview " + baseName).c_str())) {
            cout << i << endl;
            Save temp;
            temp.loadDefault();
            temp.read(savePaths[i]);
            isDrawingPreivew = true;
            reloadPictures();
        }
    }
    if(ImGui::BeginPopupModal("New Card", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Name (optional)", expansionUrl, 120);
        if(ImGui::Button("Create and go to editor")) {
            uiMode = 3;
            RETURN_FILE_NAME_NEW:
            currentFile = currentExpansionSave + to_string(rand()) + ".dclp";
            if(filesystem::exists(currentFile)) goto RETURN_FILE_NAME_NEW; // Needed? Nope.
            deleteFile("expansionicon.png");
            deleteFile("tempicon.png");
            hasImage = false;
            char* tempChar = (char*)malloc(512);
            strcpy(tempChar, expansionUrl);
            Saves::read("base-save.dclp");
            strcpy(cardTitle, tempChar);
            expansionUrl[0] = '\0';
            free(tempChar);
        }
        if(ImGui::Button("Nope, get me out of here")) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

int lastFrameObserved = -1;

void drawExpansionExplorer() {
    if(totalFramesEver - lastFrameObserved > 1) {
        reloadNames(currentExpansionSave);
        Log::debug("Refreshed all expansion names.");
        isDrawingPreivew = false;
    }
    lastFrameObserved = totalFramesEver;
    clear(0.f, 0.f, 0.f);

    if(isDrawingPreivew) handlerExternDraw();
}
void expansionExit() {
    if(expansionSave == nullptr) {
        return;
    }
    expansionSave->save(currentExpansionSave + "master.mdclp");
    Log::debug("Saved expansion to " + currentExpansionSave + "master.mdclp");
}