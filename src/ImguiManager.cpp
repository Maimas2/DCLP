#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ImGuiFileDialog.h>

#include "Saves.h"
#include "utils.h"
#include "main.h"
#include "OSspecifics.h"
#include "artworks.h"

#include "clip.h"

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

char* expansionIconListImage[] = {
	(char*)"expansion-images/adventures.png",
	(char*)"expansion-images/alchemy.png",
	(char*)"expansion-images/allies.png",
	(char*)"expansion-images/base-cards.png",
	(char*)"expansion-images/base-set.png",
	(char*)"expansion-images/base-set-old.png",
	(char*)"expansion-images/cornucopia.png",
	(char*)"expansion-images/dark-ages.png",
	(char*)"expansion-images/empires.png",
	(char*)"expansion-images/guilds.png",
	(char*)"expansion-images/hinterlands.png",
	(char*)"expansion-images/hinterlands-old.png",
	(char*)"expansion-images/intrigue.png",
	(char*)"expansion-images/intrigue-old.png",
	(char*)"expansion-images/menagerie.png",
	(char*)"expansion-images/nocturne.png",
	(char*)"expansion-images/plunder.png",
	(char*)"expansion-images/promo.png",
	(char*)"expansion-images/prosperity.png",
	(char*)"expansion-images/prosperity-old.png",
	(char*)"expansion-images/renaissance.png",
	(char*)"expansion-images/seaside.png",
	(char*)"expansion-images/seaside-old.png"
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
	"Bane Marker",
	"Examples of many options"
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
	"examples/bane.dclp",
	"examples/all.dclp"
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
const char* secondaryChoices[] = {"Same", "Action/Event", "Treasure", "Victory", "Reaction", "Duration", "Reserve", "Curse", "Shelter", "Ruins", "Landmark", "Night", "Boon", "Hex", "State", "Artifact", "Project", "Way", "Ally", "Trait", "Custom"};
const char* layoutChoices[] = {"Normal", "Landscape", "Base Card", "Pile Marker", "Player Mat"};
const char* matColorChoices[] = {"Black", "Red", "Green", "Brown", "Blue"};

char* artSearch   = (char*)malloc(128);
char** shownArtworks = (char**)malloc(sizeof(char*) * 700);
int   imageToLoad = 0;
int p = 0;

const char* menuTypes[] {
	(char*)"Tab List",
	(char*)"Accordian",
	(char*)"No Orginization"
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

void doImguiWindow() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    {
		ImGui::SetNextWindowBgAlpha(1.f);

		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 20, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(pixelWidth / 4, pixelHeight - 40), ImGuiCond_FirstUseEver);
        
        ImGui::Begin("Options", NULL);

		if(ImGui::BeginMainMenuBar()) {
			if(ImGui::BeginMenu("Save/Export")) {
				if(ImGui::MenuItem("Save")) {
					Saves::save();
					//free(cardText);
					cardText = (char*)malloc(512);
					Saves::read();
				}
				if(ImGui::MenuItem("Save as...")) {
					ImGuiFileDialog::Instance()->OpenDialog("Choose DCLP File", "Choose DCLP File", ".dclp", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
				}
				ImGui::Text("%s", ("Current save file: " + currentFile).c_str());

				if(ImGui::MenuItem("Load .DCLP file")) {
					ImGuiFileDialog::Instance()->OpenDialog("Choose DCLP File", "Choose DCLP File", ".dclp", ".", 1, nullptr, ImGuiFileDialogFlags_Modal);
				}

				if(ImGui::MenuItem("Save image to out.jpg")) 		screenShot();
		
				if(ImGui::MenuItem("Copy image to Clipboard")) 		copyToClipboard();

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		if(ImGuiFileDialog::Instance()->Display("Choose DCLP File")) {
			
			if (ImGuiFileDialog::Instance()->IsOk()) {
				std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
				std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
				isExample = false;
				Saves::read(filePathName);
				if(!isExample) currentFile = filePathName;
				reloadPictures();
			}
			ImGuiFileDialog::Instance()->Close();
		}

		currentMenuType = currentMenuTypee;

		if(currentMenuType == 0) {
			ImGui::BeginTabBar("Settings");
		} else if(currentMenuType == 1) {
			
		}

		if(doMenuItem("Non-Card")) {
			ImGui::Combo("UI Menu Type", &currentMenuTypee, menuTypes, 3);
			if(ImGui::Button("Load Example")) {
				ImGui::OpenPopup("Load Example");
			}
			if(ImGui::Button("Notes & Credits")) {
				ImGui::OpenPopup("Notes");
			}

			if(ImGui::Checkbox("Are Images Low Res?", &isLowRes)) {
				ImGui::OpenPopup("Changing Res");
			}

			if(ImGui::Button("Click To Reset All")) ImGui::OpenPopup("Reset All");

			if(ImGui::TreeNode("Legend for Card Text Symbols")) {
				ImGui::Text("$1: Money symbol (supports numbers, '?', and '*')");
				ImGui::Text("@1: Debt symbol  (same as money symbol)");
				ImGui::Text("2%%: VP symbol; number must be before '%%'");
				ImGui::Text("^: Potion symbol (supports numbers)");
				ImGui::Text("'-' alone on line: horizontal dividing line");
				ImGui::Text("[i] at start of line: italicise the line");

				ImGui::TreePop();
			}

			if(ImGui::BeginPopupModal("Load Example", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("THIS WILL OVERWRITE ALL PREVIOUS WORk");
				ImGui::ListBox("Load Example", &exampleSelected, examplesNames, IM_ARRAYSIZE(examplesNames), 15);
				if(ImGui::Button("Load Example")) {
					Saves::read(string(examplesUrls[exampleSelected]));
					reloadPictures();
					ImGui::CloseCurrentPopup();
				}
				if(ImGui::Button("Close Window")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if(ImGui::BeginPopupModal("Reset All", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Are you sure you want to delete everything?\nThis will DELETE EVERYTHING.");
				if (ImGui::Button("OK", ImVec2(120, 0))) {
					ImGui::CloseCurrentPopup();
					Saves::read("base-save.dclp");
				}
				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();
				if(ImGui::Button("Cancel", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}
			if(ImGui::BeginPopupModal("Notes", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("---- Notes and Credits ----");
				ImGui::Text("This program is licensed under the GNU General Public License v3.0.\nPlease refer to the following website for more info:");
				ImGui::Text("https://www.gnu.org/licenses/gpl-3.0.en.html");
				ImGui::NewLine();
				ImGui::Text("List of libraries used in this program:");
				ImGui::BulletText("ImGui (GUI) (MIT License)");
				ImGui::BulletText("GLFW (windowing) (zlib/libpng license)");
				ImGui::BulletText("Clip (Clipboard management, see 'clip' subfolder) (check clip/LICENSE.txt for license)");
				ImGui::BulletText("Curl (Image downloading) (See curl-license.txt for its license)");
				ImGui::BulletText("Freetype (font loading) (FTL License)");
				ImGui::BulletText("stb_image (Image management) (MIT/Public Domain)");
				ImGui::BulletText("OpenGL (Rendering) (No License due to the nature of OpenGL)");
				ImGui::BulletText("ImGuiFileDialog (File input) (MIT License)");
				ImGui::NewLine();
				ImGui::Text("Example image By National Institute of Standards and Technology - National Institute of Standards and Technology, \n\
				Public Domain, https://commons.wikimedia.org/w/index.php?curid=31557618");
				if(ImGui::Button("Close Window")) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if(ImGui::BeginPopupModal("Changing Res", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Are you sure you want to switch to higher resolution images?");
				ImGui::Text("Your work will be saved, but the window will close and reopen.");
				ImGui::NewLine();
				ImGui::Text("Higher resolution images look better,\nbut increase load times significantly.");
				ImGui::NewLine();
				if(ImGui::Button("Change resolution")) {
					ImGui::CloseCurrentPopup();
					Saves::save();
					int pid = forkNew();
					exit(0);
				}
				ImGui::SameLine();
				if(ImGui::Button("Leave it as is")) {
					isLowRes = !isLowRes;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}

			endMenuItem();
		}

		if(doMenuItem("Layout")) {
			ImGui::ListBox("Layout", &cardLayout, layoutChoices, IM_ARRAYSIZE(layoutChoices), 5);
		
			if(cardLayout == 0) ImGui::Checkbox("Is Supply Card?", &isSupply);
			if(cardLayout == 0) ImGui::Checkbox("Traveller?", &isTraveler);
			if(cardLayout == 1) ImGui::Checkbox("Trait?", &isTrait);

			endMenuItem();
		}

		if(doMenuItem("Text")) {
			ImGui::InputText("Title", cardTitle, 100);
			
			if(cardLayout <= 2) {
				ImGui::InputText("Type", cardType, 100);
				ImGui::Checkbox("Split Type Over Two Lines?", &twoLinedType);
				if(ImGui::IsItemHovered()) ImGui::SetTooltip("Used for splitting up a long type over two lines as opposed to squishing it on one.");
			}
			if(cardLayout == 2 || cardLayout == 0 || (cardLayout == 1 && !isTrait)) ImGui::InputText("Cost", cardCost, 30);
			
			if(cardLayout < 3) {
				ImGui::InputText("Art Credit", cardCredit, 120);
				if(ImGui::Button("Copy Copyright Symbol")) {
					clip::set_text("©");
				}
			}
			if(cardLayout < 3) ImGui::InputText("Card Version and Creator", cardVersion, 120);
			if(cardLayout < 2) ImGui::InputText("Heirloom", heirloomText, 120);
			
			if(cardLayout == 0 || cardLayout == 2) ImGui::InputText("Preview (Top left & right)", cardPreview, 30);

			if(cardLayout <= 2 || cardLayout == 4) {
				ImGui::Text("Card Text");
				ImGui::InputTextMultiline("Card Text", cardText, 500, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_None);
			}

			endMenuItem();
		}

		if(doMenuItem("Color")) {
			if(cardLayout <= 3) ImGui::ListBox("Color", &cardColor, mainChoices, IM_ARRAYSIZE(mainChoices), 6);
			if(cardLayout <= 2) {
				if(cardLayout == 1) {
					if(!isTrait) ImGui::ListBox("Secondary Color", &cardSecondary, secondaryChoices, IM_ARRAYSIZE(secondaryChoices), 6);
				} else {
					ImGui::ListBox("Secondary Color", &cardSecondary, secondaryChoices, IM_ARRAYSIZE(secondaryChoices), 6);
				}
			}
			if(cardLayout == 4) ImGui::ListBox("Mat Color", &matColor, matColorChoices, IM_ARRAYSIZE(matColorChoices), 6);

			if(cardColor >= NUMBER_OF_CHOICES-2 && cardLayout < 4) {
				ImGui::ColorEdit3("Card Base Color", customCardColor);
				if(cardColor == NUMBER_OF_CHOICES-1) {
					ImGui::ColorEdit3("Card Embellishment Color", customEmbellishmentColor);
					if(cardLayout != 3) ImGui::ColorEdit3("Card Side Color", customSideColor);
				}
			}
			if(cardSecondary+1 == NUMBER_OF_CHOICES && cardLayout < 2 && !isTrait) {
				ImGui::ColorEdit3("Secondary Card Base Color", secondCustomCardColor);
			}
			if(cardColor >= NUMBER_OF_CHOICES-2 && cardLayout < 4) {
				if(ImGui::Button("Reset Colors")) {
					for(int i = 0; i < 12; i++) {
						customCardColor[i] = 1.f;
						customEmbellishmentColor[i] = 1.f;
						customSideColor[i] = allBrown[i];
						secondCustomCardColor[i] = 1.f;
					}
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
			ImGui::SameLine();
			ImGui::Text("(Will probably freeze as image loads)");
			if(ImGui::Button("Remove Image")) {
				hasImage = false;
				res::tempIcon.id = 0;
			}
			if(ImGui::Button("Choose From Official Images")) {
				ImGui::OpenPopup("Choose from Official Images");
			}
			if(cardLayout <= 2) if(ImGui::Button("Choose Official Expansion Icon")) {
				ImGui::OpenPopup("Choose Official Expansion Icon");
			}
			if(cardLayout <= 2) ImGui::SliderFloat("Tweak Expansion Icon X Size", &expansionIconXSizeTweak, 0.5f, 2.f, "%.2f");
			if(cardLayout <= 2) ImGui::SliderFloat("Tweak Expansion Icon Y Size", &expansionIconYSizeTweak, 0.5f, 2.f, "%.2f");
			if(cardLayout <= 2) ImGui::NewLine();
			ImGui::SliderFloat("Image X Adjust", &xMove, -1.f, 1.f, "%.3f");
			ImGui::SliderFloat("Image Y Adjust", &yMove, -1.f, 1.f, "%.3f");
			ImGui::SliderFloat("Image Zoom", &zoom, 0.25f, 4.f, "%.2f");
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
					string tem = string(expansionIconListImage[imageToLoad]);
					string exePath = getPathToPwd();
					string toLoadS = "file://" + exePath + "/" + tem;
					int i = 0;
					for(; i < toLoadS.size(); i++) {
						expansionUrl[i] = toLoadS.at(i);
					}
					expansionUrl[i] = 0;
					expansionIconXSizeTweak = expansionAspects[imageToLoad];
					if(imageToLoad == 11) {
						expansionIconXSizeTweak = 1.f;
						expansionIconYSizeTweak = 0.77f;
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
			ImGui::Text("May only be needed for large amounts of text.");
			if(cardLayout <= 2) ImGui::Checkbox("Large Single Line Vanilla Bonuses", &largeSingleLineVanillaBonuses);
			if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Border Width", &textXTweak, 0.3f, 4.f, "%.2f");
			if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text X Position", &textXPosTweak, -1.f, 1.f, "%.2f");
			if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Y Position", &textYPosTweak, -1.f, 1.f, "%.2f");
			if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Size", &textSizeTweak, 0.3f, 4.f, "%.2f");
			if(cardLayout <= 2) ImGui::SliderFloat("Tweak Vanilla Bonus Size", &bonusSizeTweak, 0.3f, 4.f, "%.2f");
			if(cardLayout <= 2) ImGui::Checkbox("Split Type Over Two Lines?", &twoLinedType);
			if(cardLayout <  2) ImGui::SliderFloat("Newline Height Tweak", &newlineSizeTweak, 0.25f, 4.f);

			//if(cardLayout <= 1) ImGui::SliderFloat("Tweak Dividing Line Y Position", &tweakDividingLineY, -1.f, 1.f, "%.2f"); Unneeded
			if(cardLayout <= 2) ImGui::SliderFloat("Bottom Text Size Tweak", &bottomTextSizeTweak, 0.3f, 2.f, "%.2f");
			if(cardLayout == 4) ImGui::SliderFloat("Mat Width Tweak", &matWidthTweak, 0.25f, 4.f);

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