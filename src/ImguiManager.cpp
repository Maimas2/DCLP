#include <filesystem>
#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ImGuiFileDialog.h>
#include <math.h>

#include "Saves.h"
#include "utils.h"
#include "main.h"
#include "OSspecifics.h"
#include "artworks.h"
#include "Expansion-explorer.h"
#include "Log.h"
#include "Saves-class.h"

#include "clip.h"

using namespace std;

#define NUMBER_OF_CHOICES 21

char* expansionIconList[] = {
	(char*)"Adventures",
	(char*)"Alchemy",
	(char*)"Allies",
	(char*)"Base cards",
	(char*)"Base set",
	(char*)"Base set (old)",
	(char*)"Cornucopia",
	(char*)"Dark ages",
	(char*)"Empires",
	(char*)"Guilds",
	(char*)"Hinterlands",
	(char*)"Hinterlands (old)",
	(char*)"Intrigue",
	(char*)"Intrigue (old)",
	(char*)"Menagerie",
	(char*)"Nocturne",
	(char*)"Plunder",
	(char*)"Promo",
	(char*)"Prosperity",
	(char*)"Prosperity (old)",
	(char*)"Renaissance",
	(char*)"Seaside",
	(char*)"Seaside (old)."
};
const char* examplesNames[] = {
	"Workshop",
	"Coin of the Realm",
	"Harem",
	"Pooka",
	"Werewolf",
	"Bonfire",
	"Dominate",
	"Tax",
	"Cursed (trait)",
	"Curse (base card)",
	"Bane Marker"
};
const char* examplesUrls[] = {
	"examples/workshop.dclp",
	"examples/coin.dclp",
	"examples/harem.dclp",
	"examples/pooka.dclp",
	"examples/werewolf.dclp",
	"examples/bonfire.dclp",
	"examples/dominate.dclp",
	"examples/tax.dclp",
	"examples/cursed.dclp",
	"examples/curse.dclp",
	"examples/bane.dclp"
};
float expansionAspects[] = {
	0.85f,
	0.6f,
	0.93f,
	1.f,
	1.39f,
	1.39f,
	0.94f,
	0.84f,
	1.05f,
	0.84f,
	1.37f,
	1.3f,
	0.91f,
	1.f,
	0.96f,
	0.93f,
	0.91f,
	0.99f,
	0.95f,
	1.02f,
	1.02f,
	0.84f,
	1.02f
};

char* strip(char* in) {
	int spacesAtEnd   = 0;
	for(int i = 0; i < strlen(in); i++) {
		if(isspace(in[i])) {
			in++;
		} else break;
	}
	for(int i = strlen(in)-1; i >= 0; i--) {
		if(isspace(in[i])) {
			in[i] = (char)0;
		} else break;
	}
	return in;
}

const char* mainChoices[] = {"Action/Event", "Treasure", "Victory", "Reaction", "Duration", "Reserve", "Curse", "Shelter", "Ruins", "Landmark", "Night", "Boon", "Hex", "State", "Artifact", "Project", "Way", "Ally", "Trait", "Custom", "Extra Custom"};
const char* secondaryChoices[] = {"Same as Primary", "Action/Event", "Treasure", "Victory", "Reaction", "Duration", "Reserve", "Curse", "Shelter", "Ruins", "Landmark", "Night", "Boon", "Hex", "State", "Artifact", "Project", "Way", "Ally", "Trait", "Custom"};
const char* layoutChoices[] = {"Normal", "Landscape", "Base Card", "Pile Marker", "Player Mat"};
const char* matColorChoices[] = {"Black", "Red", "Green", "Brown", "Blue"};

string officialExpansionIconList[] = {
	"https://wiki.dominionstrategy.com/images/thumb/5/5e/Adventures_icon.png/101px-Adventures_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/6/6a/Alchemy_icon.png/71px-Alchemy_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/3/3d/Allies_icon.png/112px-Allies_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/0/01/Base_Cards_icon.png/120px-Base_Cards_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/b/b0/Dominion_icon.png/120px-Dominion_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/4/48/Dominion_old_icon.png/120px-Dominion_old_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/2/20/Cornucopia_icon.png/113px-Cornucopia_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/1/1b/Dark_Ages_icon.png/101px-Dark_Ages_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/2/2a/Empires_icon.png/120px-Empires_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/3/36/Guilds_icon.png/101px-Guilds_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/2/28/Hinterlands_icon.png/120px-Hinterlands_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/9/9e/Hinterlands_old_icon.png/120px-Hinterlands_old_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/c/cf/Intrigue_icon.png/109px-Intrigue_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/9/92/Intrigue_old_icon.png/120px-Intrigue_old_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/d/d4/Menagerie_%28expansion%29_icon.png/115px-Menagerie_%28expansion%29_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/a/aa/Nocturne_icon.png/111px-Nocturne_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/b/b8/Plunder_%28expansion%29_icon.png/109px-Plunder_%28expansion%29_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/a/a0/Promo_icon.png/118px-Promo_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/f/fb/Prosperity_icon.png/114px-Prosperity_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/5/5e/Prosperity_old_icon.png/120px-Prosperity_old_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/4/4c/Renaissance_icon.png/120px-Renaissance_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/c/c5/Seaside_icon.png/101px-Seaside_icon.png",
	"https://wiki.dominionstrategy.com/images/thumb/e/e7/Seaside_old_icon.png/120px-Seaside_old_icon.png"
};

char*  artSearch   = (char*)malloc(128);
char** shownArtworks = (char**)malloc(sizeof(char*) * 700);
int   imageToLoad = 0;
int   p = 0;

string returnV;

char* expansionName = (char*)malloc(128);

int uiMode = 0;
// 0 - main choosing screen, appears at startup
// 1 - editing a card
// 2 - editing an expansion
// 3 - editing a card from an expansion

const char* menuTypes[] {
	(char*)"Tabs",
	(char*)"Accordian",
	(char*)"None"
};
bool doMenuItem(const char* l) {
	if(currentMenuType == 0) {
		return ImGui::BeginTabItem(l);
	} else if(currentMenuType == 1) {
		return ImGui::TreeNode(l);
	} else {
		
	}
	return true;
}
void endMenuItem() {
	if(currentMenuType == 0) {
		ImGui::EndTabItem();
	} else if(currentMenuType == 1) {
		ImGui::TreePop();
	} else {
		return;
	}
}

char* savePaths[128];
int selectedExpansion = 0;
int numberOfSelectableExpansions = 0;
vector<string> paths;

int menuAction = 0;

void doImguiWindow() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    {
		ImGui::SetNextWindowBgAlpha(1.f);

        if(ImGui::BeginMainMenuBar()) {
            if(ImGui::Button("S")) {
				if(uiMode == 2 || uiMode == 3) expansionExit();
                Saves::save();
				free(cardText);
                cardText = (char*)malloc(512);
                Saves::read();
				uiMode = 0;
            }
			if(ImGui::BeginMenu("File")) {
				if(uiMode < 2 && ImGui::MenuItem("New empty file")) {
					if(uiMode == 1) Saves::save();
					deleteFile("tempicon.png");
					hasImage = false;
					Saves::read("base-save.dclp");
					currentFile = "";
					uiMode = 1;
				}

				if((uiMode == 1 || uiMode == 3) && ImGui::MenuItem("Save")) {
					if(currentFile == "") {
						ImGuiFileDialog::Instance()->OpenDialog("Save DCLP File", "Save DCLP File", ".dclp", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
					} else {
						Saves::save();
						cardText = (char*)malloc(512);
						Saves::read();
					}
				}
				if(uiMode == 1 && ImGui::MenuItem("Save as...")) {
					ImGuiFileDialog::Instance()->OpenDialog("Save DCLP File", "Save DCLP File", ".dclp", "cards/", 1, nullptr, ImGuiFileDialogFlags_Modal);
				}
				if(uiMode == 3) ImGui::Text("%s", ("Current save file: " + currentFile).c_str());

				if(uiMode != 3 && ImGui::MenuItem("Load .DCLP file")) {
					ImGuiFileDialog::Instance()->OpenDialog("Choose DCLP File", "Choose DCLP File", ".dclp", "cards/", 1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				if(ImGui::BeginMenu("Load example card")) {
					if(uiMode == 1 || uiMode == 3) ImGui::Text("THIS WILL OVERWRITE ALL PREVIOUS WORK");
					ImGui::ListBox("Load Example", &exampleSelected, examplesNames, IM_ARRAYSIZE(examplesNames), 15);
					if(ImGui::Button("Load")) {
						if(uiMode == 0) {
							Saves::read("base-save.dclp");
							currentFile = "";
							uiMode = 1;
						}
						Saves::read(string(examplesUrls[exampleSelected]));
						reloadPictures();
					}

					ImGui::EndMenu();
				}

				if((uiMode == 1 || uiMode == 3) && ImGui::MenuItem("Reset All")) {
					menuAction = 3;
				}

				if((uiMode == 1 || uiMode == 3) && ImGui::MenuItem("Save image to out.jpg")) 		    screenShot();
		
				if((uiMode == 1 || uiMode == 3) && ImGui::MenuItem("Copy image to Clipboard")) 			copyToClipboard();

				ImGui::Checkbox("Are Images Low Res? (requires restart to take effect)", &isLowRes);

				ImGui::EndMenu();
			}
			if(ImGui::BeginMenu("Help")) {
				ImGui::Combo("UI Orginization", &currentMenuTypee, menuTypes, 3);
				if(ImGui::MenuItem("Legend for card text symbols")) {
					menuAction = 2;
				}
				if(ImGui::MenuItem("Notes & Credits")) {
					menuAction = 1;
				}
				
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if(menuAction == 1) {
			ImGui::OpenPopup("Notes");
			menuAction = 0;
		}
		if(menuAction == 2) {
			ImGui::OpenPopup("Legend");
			menuAction = 0;
		}
		if(menuAction == 3) {
			ImGui::OpenPopup("Reset All");
			menuAction = 0;
		}
		if(ImGui::BeginPopupModal("Notes", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("---- Notes and Credits ----");
			ImGui::Text("This program is licensed under the GNU General Public License v3.0.\nPlease refer to the following website for more info:");
			ImGui::Text("https://www.gnu.org/licenses/gpl-3.0.en.html");
			ImGui::NewLine();
			ImGui::Text("List of libraries and tools used in this program:");
			ImGui::BulletText("ImGui (GUI) (MIT License)");
			ImGui::BulletText("GLFW (windowing) (zlib/libpng license)");
			ImGui::BulletText("Clip (Clipboard management, see 'clip' subfolder) (check clip/LICENSE.txt for license)");
			ImGui::BulletText("Curl (Image downloading) (See curl-license.txt for its license)");
			ImGui::BulletText("Freetype (font loading) (FTL License)");
			ImGui::BulletText("stb_image (Image management) (MIT/Public Domain)");
			ImGui::BulletText("OpenGL (Rendering) (No License due to the nature of OpenGL)");
			ImGui::BulletText("ImGuiFileDialog (File input) (MIT License)");

			ImGui::NewLine();

			ImGui::Text("DM on Discord at Maimas2#4562 with any questions.");

			if(ImGui::Button("Exit")) ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
		if(ImGui::BeginPopupModal("Legend", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("$1: Money symbol (supports numbers, '?', and '*')");
			ImGui::Text("@1: Debt symbol  (same as money symbol)");
			ImGui::Text("2%%: VP symbol; number must be before '%%'");
			ImGui::Text("^: Potion symbol (supports numbers)");
			ImGui::Text("'-' alone on line: horizontal dividing line");
			ImGui::Text("[i] at start of line: italicise the line");

			if(ImGui::Button("Exit")) ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}
		if(ImGui::BeginPopupModal("Reset All", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Are you sure you want to discard this card in its entirety?");
			if(ImGui::Button("Do it")) {
				deleteFile("expansionicon.png");
				deleteFile("tempicon.png");
				Saves::read("base-save.dclp");
				ImGui::CloseCurrentPopup();
			}
			if(ImGui::Button("Nah I'm good")) ImGui::CloseCurrentPopup();

			ImGui::EndPopup();
		}

        if(uiMode == 0) { // Splash screen
			int ii = 0;

			int numberOfRecentFiles = 0;
			for(; numberOfRecentFiles < 6; numberOfRecentFiles++) {
				if(numberOfRecentFiles == 5) break;
				if(recentFilesBeautified[numberOfRecentFiles] == "") break;
			}

            const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
            
            ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkSize.x/2-200, main_viewport->WorkSize.y/2-200), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always);

            ImGui::Begin("Splash Screen", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);

            float windowWidth = ImGui::GetWindowSize().x;
            float textWidth = ImGui::CalcTextSize("Dominion Card Designer").x;

            ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
            ImGui::Text("Dominion Card Designer");

            ImGui::NewLine();
            ImGui::NewLine();

            if(ImGui::Button("New empty file")) {
				hasImage = false;
                Saves::read("base-save.dclp");
				currentFile = "";
                uiMode = 1;
            }
            ImGui::SameLine(windowWidth - (ImGui::CalcTextSize("Recent Files").x + 20));
            ImGui::Text("Recent Files");

            if(numberOfRecentFiles != 0 && ImGui::Button("Restore last session")) {
                Saves::read(string(recentFiles[0]));
				currentFile = string(recentFiles[0]);
                uiMode = 1;
				reloadPictures();
            }

			ImGui::SameLine(windowWidth - (ImGui::CalcTextSize(recentFilesBeautified[ii].c_str()).x + 20));
			if(recentFilesBeautified[ii] != "") {if(ImGui::Button((char*)recentFilesBeautified[ii].c_str())){
				Saves::read(string(recentFiles[ii]));
				currentFile = string(recentFiles[ii]);
                uiMode = 1;
				reloadPictures();
			}} else {
				ImGui::NewLine();
			}
			ii++;

			if(ImGui::Button("Load .dclp file")) {
				ImGuiFileDialog::Instance()->OpenDialog("Choose DCLP File", "Choose DCLP File", ".dclp", "saves/", 1, nullptr, ImGuiFileDialogFlags_Modal);
			}

			ImGui::SameLine(windowWidth - (ImGui::CalcTextSize(recentFilesBeautified[ii].c_str()).x + 20));
			if(recentFilesBeautified[ii] != "") {if(ImGui::Button((char*)recentFilesBeautified[ii].c_str())){
				Saves::read(string(recentFiles[ii]));
				currentFile = string(recentFiles[ii]);
                uiMode = 1;
				reloadPictures();
			}} else {
				ImGui::NewLine();
			}
			ii++;
			
			ImGui::NewLine();

			ImGui::SameLine(windowWidth - (ImGui::CalcTextSize(recentFilesBeautified[ii].c_str()).x + 20));
			if(recentFilesBeautified[ii] != "") {if(ImGui::Button((char*)recentFilesBeautified[ii].c_str())){
				Saves::read(string(recentFiles[ii]));
				currentFile = string(recentFiles[ii]);
                uiMode = 1;
				reloadPictures();
			}} else {
				ImGui::NewLine();
			}
			ii++;

			ImGui::NewLine();

			ImGui::SameLine(windowWidth - (ImGui::CalcTextSize(recentFilesBeautified[ii].c_str()).x + 20));
			if(recentFilesBeautified[ii] != "") {if(ImGui::Button((char*)recentFilesBeautified[ii].c_str())){
				Saves::read(string(recentFiles[ii]));
				currentFile = string(recentFiles[ii]);
                uiMode = 1;
				reloadPictures();
			}} else {
				ImGui::NewLine();
			}
			ii++;

			ImGui::NewLine();

			ImGui::SameLine(windowWidth - (ImGui::CalcTextSize(recentFilesBeautified[ii].c_str()).x + 20));
			if(recentFilesBeautified[ii] != "") {if(ImGui::Button((char*)recentFilesBeautified[ii].c_str())){
				Saves::read(string(recentFiles[ii]));
				currentFile = string(recentFiles[ii]);
                uiMode = 1;
				reloadPictures();
			}} else {
				ImGui::NewLine();
			}
			ii++;

			//if(ImGui::Button("Ascend, ascend, ascend!")) {
			if(ImGui::Button("Create new expansion")) {
				ImGui::OpenPopup("Create Expansion");
				free(expansionName);
				expansionName = (char*)malloc(128);
				for(int i = 0; i < 128; i++) {
					expansionName[i] = 0;
				}
			}

			if(ImGui::Button("Load Expansion")) {
				paths.clear();
				for(filesystem::directory_entry file : filesystem::directory_iterator("./expansions/")) {
					if(!filesystem::exists(file.path().string() + "/master.mdclp")) {
						continue;
					}
					paths.push_back(file.path());
					Log::log("Found expansion: " + file.path().string());
				}
				ImGui::OpenPopup("Choose expansion");
				numberOfSelectableExpansions = 0;
				for(int i = 0; i < paths.size(); i++) {
					Save* temp = new Save;
					temp->charPointers["name"] = &expansionName;
					temp->read(paths[i] + "/master.mdclp");
					//memcpy(savePaths[i], expansionName, strlen(expansionName));
					savePaths[i] = (char*)malloc(256);
					strcpy(savePaths[i], expansionName);
					delete temp;
					numberOfSelectableExpansions++;
				}
			}

			if(ImGui::BeginPopupModal("Create Expansion", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::InputText("Name", expansionName, 120);

				if(ImGui::Button("Create")) {
					returnV = createExpansion("./expansions/" + string(expansionName) + "/");
					if(returnV == "") {
						uiMode = 2;
						ImGui::CloseCurrentPopup();
					} else {
						ImGui::OpenPopup("Expansion ERROR");
					}
				}
				ImGui::SameLine();
				if(ImGui::Button("Exit")) {
					ImGui::CloseCurrentPopup();
				}

				if(ImGui::BeginPopupModal("Expansion ERROR", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::Text("%s", returnV.c_str());
					if(ImGui::Button("Cool")) ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}

				ImGui::EndPopup();
			}

			if(ImGui::BeginPopupModal("Choose expansion", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::ListBox("Choose expansion", &selectedExpansion, savePaths, numberOfSelectableExpansions, 5);

				if(numberOfSelectableExpansions != 0 && ImGui::Button("Load")) {
					strcpy(cardTitle, paths[selectedExpansion].c_str());
					if(string(cardTitle) == "") {
						returnV = "Path cannot be empty!";
						ImGui::OpenPopup("Expansion ERROR");
					} else {
						returnV = loadExpansion(string(cardTitle) + "/");
						cardTitle[0] = 0;
						if(returnV != "") {
							ImGui::OpenPopup("Expansion ERROR");
						} else {
							uiMode = 2;
							ImGui::CloseCurrentPopup();
						}
					}
				} else if(numberOfSelectableExpansions == 0) {
					ImGui::Text("Create an expansion first");
				}

				if(ImGui::Button("Close")) {
					ImGui::CloseCurrentPopup();
				}

				if(ImGui::BeginPopupModal("Expansion ERROR", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
					ImGui::Text("%s", returnV.c_str());
					if(ImGui::Button("Cool")) ImGui::CloseCurrentPopup();
					ImGui::EndPopup();
				}

				ImGui::EndPopup();
			}

            ImGui::End();
        }

        if(ImGuiFileDialog::Instance()->Display("Choose DCLP File")) {
			if (ImGuiFileDialog::Instance()->IsOk()) {
				string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				isExample = false;
				Saves::read(filePathName);
				if(!isExample) currentFile = filePathName;
				reloadPictures();
				uiMode = 1;
			}
			ImGuiFileDialog::Instance()->Close();
		}
        if(ImGuiFileDialog::Instance()->Display("Save DCLP File")) {
			if (ImGuiFileDialog::Instance()->IsOk()) {
				string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				
				currentFile = filePathName;
                Saves::save();
                cardText = (char*)malloc(512);
                Saves::read();
			}
			ImGuiFileDialog::Instance()->Close();
		}

		if(uiMode == 0) {
			return;
		}
		if(uiMode == 2) {
			ImGui::Begin("Expand");

			expansionImgui();

			ImGui::End();
			return;
		}

		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(pixelWidth / 4, pixelHeight - 40), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Options", NULL);

		currentMenuType = currentMenuTypee;

		if(uiMode == 3) {
			if(ImGui::Button("Exit to expansion")) {
				Saves::save();
                cardText = (char*)malloc(512);
                Saves::read();
				uiMode = 2;
			}
		}
		if(uiMode == 1) {
			if(ImGui::Button("Exit to main menu")) {
				Saves::save();
                cardText = (char*)malloc(512);
                Saves::read();
				uiMode = 0;
			}
		}

		if(currentMenuType == 0) {
			ImGui::NewLine();
			ImGui::BeginTabBar("Settings");
		}

		if(doMenuItem("Layout")) {
			ImGui::ListBox("Layout", &cardLayout, layoutChoices, IM_ARRAYSIZE(layoutChoices), 5);
		
			if(cardLayout == 0) {
				ImGui::Checkbox("Is Supply Card?", &isSupply);
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Examples: Will o' Wisp, Bat");
				ImGui::Checkbox("Traveller?", &isTraveler);
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Examples: Peasant, Page");
			}
			if(cardLayout == 1) ImGui::Checkbox("Trait?", &isTrait);

			endMenuItem();
		}

		if(doMenuItem("Text")) {
			ImGui::InputText("Title", cardTitle, 100);
			
			if(cardLayout <= 2) {
				ImGui::InputText("Type", cardType, 100);
				if(cardLayout != 1) {
					ImGui::Checkbox("Split Type Over Two Lines?", &twoLinedType);
					if(ImGui::IsItemHovered()) ImGui::SetTooltip("Used for splitting up a long type over two lines as opposed to squishing it on one. See Werewolf for an example.");
				}
			}
			if(cardLayout == 2 || cardLayout == 0 || (cardLayout == 1 && !isTrait)) ImGui::InputText("Cost", cardCost, 30);
			
			if(cardLayout < 3) {
				ImGui::InputText("Art Credit", cardCredit, 120);
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Bottom left text.");
			}
			if(cardLayout < 3) ImGui::InputText("Card Version and Creator", cardVersion, 120);
			if(ImGui::IsItemHovered()) ImGui::SetTooltip("Bottom right text.");
			if(cardLayout < 2) ImGui::InputText("Heirloom", heirloomText, 120);
			if(ImGui::IsItemHovered()) ImGui::SetTooltip("Examples: Graveyard, Pooka.");
			
			if(cardLayout == 0 || cardLayout == 2) ImGui::InputText("Preview (Top left & right)", cardPreview, 30);

			if(cardLayout <= 2 || cardLayout == 4) {
				ImGui::Text("Card Text");
				ImGui::InputTextMultiline("Card Text", cardText, 500, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_None);
			}

			if(cardLayout < 3) if(ImGui::Button("Copy Copyright Symbol")) {
				clip::set_text("©");
			}

			endMenuItem();
		}

		if(doMenuItem("Color")) {
			int h = (currentMenuTypee == 0 ? 15 : 6);
			if(cardLayout <= 3) ImGui::ListBox("Primary Color", &cardColor, mainChoices, IM_ARRAYSIZE(mainChoices), h);
			if(cardLayout <= 2) {
				if(cardLayout == 1) {
					if(!isTrait) ImGui::ListBox("Secondary Color", &cardSecondary, secondaryChoices, IM_ARRAYSIZE(secondaryChoices), h);
				} else {
					ImGui::ListBox("Secondary Color", &cardSecondary, secondaryChoices, IM_ARRAYSIZE(secondaryChoices), h);
				}
			}
			if(cardLayout == 4) ImGui::ListBox("Mat Color", &matColor, matColorChoices, IM_ARRAYSIZE(matColorChoices), h);

			if(cardColor >= NUMBER_OF_CHOICES-2 && cardLayout < 4) {
				ImGui::ColorEdit3("Card Primary Color", customCardColor);
				if(cardColor == NUMBER_OF_CHOICES-1) {
					ImGui::ColorEdit3("Card Embellishment Color", customEmbellishmentColor);
					if(cardLayout != 3) ImGui::ColorEdit3("Card Side Color", customSideColor);
				}
			}
			if(cardSecondary+1 == NUMBER_OF_CHOICES && cardLayout < 2 && !isTrait) {
				ImGui::ColorEdit3("Secondary Card Base Color", secondCustomCardColor);
			}

			if(cardColor == NUMBER_OF_CHOICES-1 && cardLayout == 1) {
				ImGui::TextDisabled("(?)");
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Color used for the bar that goes behind the art and creator lines at the bottom.");
				ImGui::SameLine();
				ImGui::ColorEdit3("Landscape Bottom Bar Color", eventColor);
			}

			if(cardColor >= NUMBER_OF_CHOICES-2 && cardLayout < 4) {
				if(ImGui::Button("Reset Colors")) {
					resetColors();
				}
			}

			endMenuItem();
		}

		if(doMenuItem("Image")) {
			ImGui::InputText("Picture URL", iconUrl, 500);
			if(cardLayout <= 2) ImGui::InputText("Expansion URL", expansionUrl, 500);
			if(ImGui::Button("Reload Pictures")) {
				reloadPictures();
			}
			if(ImGui::Button("Remove Image")) {
				hasImage = false;
				iconUrl[0] = '\0';
				res::tempIcon.id = 0;
			}
			if(ImGui::Button("Load Official Image")) {
				ImGui::OpenPopup("Choose from Official Images");
			}
			if(cardLayout <= 2) if(ImGui::Button("Load Expansion Icon")) {
				ImGui::OpenPopup("Choose Official Expansion Icon");
			}
			if(cardLayout <= 2) {
				ImGui::Text("Edit expansion icon aspect ratio");

				ImGui::SliderFloat("Horizontal Size", &expansionIconXSizeTweak, 0.5f, 2.f, "%.2f");
				ImGui::SliderFloat("Vertical Size", &expansionIconYSizeTweak, 0.5f, 2.f, "%.2f");
				ImGui::SliderFloat("Vertical Position ", &expansionIconXPosTweak, -1.f, 1.f, "%.2f");

				ImGui::NewLine();
			}

			ImGui::Text("Edit image position and zoom");
			ImGui::SliderFloat("Horizontal Position", &xMove, -1.f, 1.f, "%.3f");
			ImGui::SliderFloat("Vertical Position", &yMove, -1.f, 1.f, "%.3f");
			ImGui::SliderFloat("Zoom", &zoom, 0.25f, 4.f, "%.2f");
			if(ImGui::Button("Reset Image Position")) {
				xMove = 0;
				yMove = 0;
				zoom  = 1;
			}

			if(ImGui::BeginPopupModal("Choose from Official Images", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				if(ImGui::InputText("Search (CASE SENSITIVE)", artSearch, 100) || isFirstFrame) {
					int i = 0;
					p = 0;
					for(; i < sizeof(artworkNames) / sizeof(char*); i++) {
						if(strstr(artworkNames[i], strip(artSearch)) != nullptr) {
							shownArtworks[p++] = artworkNames[i];
						}
					}
					shownArtworks[p] = NULL;
				}

				ImGui::ListBox("Official Images", &imageToLoad, shownArtworks, p, 15);

				if(ImGui::Button("Load Selected Image")) {
					int pos = 0;
					for(int i = 0; i < IM_ARRAYSIZE(artworkNames); i++) {
						if(strcmp(shownArtworks[imageToLoad], artworkNames[i]) == 0) {
							pos = i;
							break;
						}
					}
					for(int i = 0; ; i++) {
						iconUrl[i] = artworkUrls[pos][i];
						if(artworkUrls[pos][i] == 0) break;
					}
					reloadPictures();
					ImGui::CloseCurrentPopup();
				}

				if(ImGui::Button("Close Window")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if(ImGui::BeginPopupModal("Choose Official Expansion Icon", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::ListBox("Official Icons", &imageToLoad, expansionIconList, IM_ARRAYSIZE(expansionIconList), 15);

				if(ImGui::Button("Load Selected Image")) {
					string toLoadS = officialExpansionIconList[imageToLoad];
					int i = 0;
					for(; i < toLoadS.size(); i++) {
						expansionUrl[i] = toLoadS.at(i);
					}
					expansionUrl[i] = 0;
					expansionIconXSizeTweak = expansionAspects[imageToLoad];
					expansionIconYSizeTweak = 1.f;
					expansionIconXPosTweak  = 0.f;
					if(imageToLoad == 11) {
						expansionIconXSizeTweak = 1.f;
						expansionIconYSizeTweak = 0.77f;
					}
					if(imageToLoad == 4 || imageToLoad == 5) {
						expansionIconXPosTweak = -0.8f;
					}
					if(imageToLoad == 10) {
						expansionIconXPosTweak = -1.f;
					}
					if(imageToLoad == 8) {
						expansionIconXPosTweak = -0.5f;
					}
					reloadPictures();
					ImGui::CloseCurrentPopup();
				}

				if(ImGui::Button("Close Window")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			endMenuItem();
		}

		if((cardLayout < 3 || cardLayout == 4) && doMenuItem("Tweaks")) {
			ImGui::Text("Not required for every card, but may be needed in some cases.");
			if(cardLayout <= 2) {
				ImGui::Checkbox("Force Large Single Line Vanilla Bonuses?", &largeSingleLineVanillaBonuses);
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("When turned on, vanilla bonuses (eg. '+1 Action') are given a forced size");
				if(largeSingleLineVanillaBonuses) {
					ImGui::SliderFloat("Vanilla Bonus Size", &bonusSizeTweak, 0.3f, 4.f, "%.2f");
					if(ImGui::IsItemHovered()) ImGui::SetTooltip("Change the forced size of vanilla bonuses.");
				}
				
				ImGui::SliderFloat("Tweak Text Border Width", &textXTweak, 0.3f, 4.f, "%.2f");
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Due to my lack of effort, text is squished inwards into a rectangle in cards with high word counts. This undoes the squishing.");
				ImGui::SliderFloat("Horizontal Text Offset", &textXPosTweak, -1.f, 1.f, "%.2f");
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Set the text's horizontal offset");
				ImGui::SliderFloat("Vertical Text Offset", &textYPosTweak, -1.f, 1.f, "%.2f");
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Set the text's vertical offset");
				ImGui::SliderFloat("Text Size", &textSizeTweak, 0.3f, 4.f, "%.2f");
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Scale the entire text. May work unexpectedly with large word counts.");
			}
			
			if(cardLayout <  2) ImGui::SliderFloat("Newline Height", &newlineSizeTweak, 0.25f, 4.f, "%.2f");
			if(ImGui::IsItemHovered()) ImGui::SetTooltip("Change distance between new lines. *Works unexpectedly. Use at your own risk.**");

			if(cardLayout <= 2) ImGui::SliderFloat("Bottom Text Size Tweak", &bottomTextSizeTweak, 0.3f, 2.f, "%.2f");
			if(ImGui::IsItemHovered()) ImGui::SetTooltip("Scale the two bottom lines of text. Only needed with exceptionally long lines.");

			if(cardLayout == 4) ImGui::SliderFloat("Mat Width Tweak", &matWidthTweak, 0.25f, 4.f);
			if(ImGui::IsItemHovered()) ImGui::SetTooltip("Change the space given until the text overflows to the next line.");

			endMenuItem();
		}

		if(doMenuItem("Debug")) {
			if(ImGui::Button((isDemoShown ? "Hide ImGui Demo" : "Show ImGui Demo"))) {
				isDemoShown = !isDemoShown;
			}
			ImGui::Text("Compiled on");
			ImGui::SameLine();
			ImGui::Text(__DATE__);
			ImGui::SameLine();
			ImGui::Text("at");
			ImGui::SameLine();
			ImGui::Text(__TIME__);

			endMenuItem();
		}

		if(currentMenuType == 0) {
			ImGui::EndTabBar();
		} else if(currentMenuType == 1) {
			
		}

    	ImGui::End();

		
    	if(isDemoShown) ImGui::ShowDemoWindow();
    }
}