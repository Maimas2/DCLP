#ifndef GL_IS_INCLUDED
#define GL_IS_INCLUDED
#include "glm/geometric.hpp"
#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sys/wait.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <string>
#include <thread>
#include <signal.h>

#include "main.h"
#include "utils.h"
#include "handler.h"
#include "Text.h"
#include "Resources.h"
#include "Log.h"
#include "OSspecifics.h"
#include "Saves.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "clip.h"

using namespace std;

GLFWwindow* createWindow(bool f);
static void changeFullscreen();
void calculateFPS();

bool isInfinite = false;

float baseWindowRatio = 0.6522547652254765;

float width;
float height;

float pixelWidth;
float pixelHeight;

float xStretch;
float yStretch;

GLFWwindow* window;

int currentSecond = 0;
int framesSoFar = 0;
int currentFps = 0;
int totalFramesEver = 0;

float windowRatio, windowInverse;

float delta = 0.f;
float drawDelta = 0.f, updateDelta = 0.f;
float averageDrawDelta = 0.f, averageUpdateDelta = 0.f;
float drawDeltaAdd = 0.f;
float updateDeltaAdd = 0.f;
float progressThroughFrame = 0.f;
float timeOfLastFrame = 0.f;
float targetFixedFps = 60.f;

bool isWindowFocused = true;
string pathPrefix;

bool isPausedMaster = false;

bool isFirstFrame = true;

#define NUMBER_OF_CHOICES 21
const char* mainChoices[] = {"Action/Event", "Treasure", "Victory", "Reaction", "Duration", "Reserve", "Curse", "Shelter", "Ruins", "Landmark", "Night", "Boon", "Hex", "State", "Artifact", "Project", "Way", "Ally", "Trait", "Custom", "Extra Custom"};
const char* secondaryChoices[] = {"Same", "Action/Event", "Treasure", "Victory", "Reaction", "Duration", "Reserve", "Curse", "Shelter", "Ruins", "Landmark", "Night", "Boon", "Hex", "State", "Artifact", "Project", "Way", "Ally", "Trait", "Custom", "Extra Custom"};
const char* layoutChoices[] = {"Normal", "Landscape", "Base Card", "Pile Marker", "Player Mat"};
const char* matColorChoices[] = {"Black", "Red", "Green", "Brown", "Blue"};

float customCardColor[3] = {1.f, 1.f, 1.f};
float customEmbellishmentColor[3] = {1.f, 1.f, 1.f};
float customSideColor[3] = {1.2f, 0.8f, 0.5f};

int getChoice(char* c) {
	for(int i = 0; i < 18; i++) {
		if(strcmp(mainChoices[i], c) == 0) return i;
	}
	return 0;
}
static void enablings() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    //glDisable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
}
float lastFixedUpdate = 0.f;
static void update() {
	if(glfwGetTime()-lastFixedUpdate > 1.f/60.f) {
		handlerUpdate();
		lastFixedUpdate += 1.f/60.f;
	}
	progressThroughFrame = (glfwGetTime()-lastFixedUpdate)*targetFixedFps;
    handlerUnfixedUpdate();
}
int w, h;
bool screenshottingNextFrame = false;
GLubyte* getScreenshotPixels() { // Based off https://stackoverflow.com/questions/33757634/creating-screenshot-in-opengl-not-working
	if(!isFullscreen && !isScreenshotting) {
		changeFullscreen();
		calculateFPS();
        enablings();
		for(int i = 0; i < 10; i++) {
			resetMatrix();
			update();
			handlerDraw();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
	if(cardLayout == 0 || cardLayout == 2) {
		w = (pixelWidth/1920)*672;
		h = (pixelHeight/1080)*1080;
	} else if(cardLayout == 1 || cardLayout == 3) {
		w = (pixelWidth/1920)*1080;
		h = (pixelHeight/1080)*702;
	}
	GLubyte* pixels = (GLubyte*)malloc(3 * w * h);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadPixels((int)(pixelWidth/2)-(w/2), (int)(pixelHeight/2)-(h/2), w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	if (GL_NO_ERROR != glGetError()) throw "Error: Unable to read pixels.";

	return pixels;
}
void screenShot() {
	GLubyte* pixels = getScreenshotPixels();

	stbi_flip_vertically_on_write(true);
	stbi_write_jpg("out.jpg", w, h, 3, pixels, 100);
	
	free(pixels);
}
void copyToClipboard() {
	if(!screenshottingNextFrame) {
		screenshottingNextFrame = true;
		return;
	}
	GLubyte* pixels = getScreenshotPixels();

	uint8_t* pix = (uint8_t*)malloc(w * h * sizeof(uint8_t) * 4);

	int m = w * h * 4;
	int g = 0;
	for(int i = 0; i < m; i++) {
		if(i % 4 == 3) {
			pix[i] = 255;
			continue;
		}
		int pn = ((w * h * 3) - g++)/ 3;
		int r  = (pn / h);
		int c  = (pn % h);
		//cout << g-1 << "\t" << ((r * h) + c)*3 << endl;
		pix[i] = pixels[((r * h) + c)*3 + (i % 4)];
	}

	clip::image_spec spec;
	spec.width = w;
	spec.height = h;
	spec.bits_per_pixel = 32;
	spec.bytes_per_row = spec.width*4;
	spec.red_mask = 0xff;
	spec.green_mask = 0xff00;
	spec.blue_mask = 0xff0000;
	spec.alpha_mask = 0xff000000;
	spec.red_shift = 0;
	spec.green_shift = 8;
	spec.blue_shift = 16;
	spec.alpha_shift = 24;
	clip::image img(pix, spec);
	clip::set_image(img);

	free(pixels);
	free(pix);

	screenshottingNextFrame = false;
}
char* cardTitle   = (char*)malloc(128);
char* cardType    = (char*)malloc(128);
char* cardText    = (char*)malloc(512);
char* iconUrl     = (char*)malloc(512);
char* expansionUrl= (char*)malloc(512);
char* cardCost    = (char*)malloc(32);
char* cardPreview = (char*)malloc(32);
char* cardCredit  = (char*)malloc(128);
char* cardVersion = (char*)malloc(128);
char* artSearch   = (char*)malloc(128);
int   cardColor = 0;
int   cardSecondary = 0;
int   cardLayout= 0;
int   matColor  = 0;
float xMove     = 0.f;
float yMove     = 0.f;
float zoom      = 1.f;
bool  isLowRes  = true;
bool  showWindow= true;
bool  isTraveler= false;
bool  isTightScreenshot = false;
float textXTweak= 1.f;
float textSizeTweak = 1.f;
float textXPosTweak = 0.f;
float textYPosTweak = 0.f;
float expansionIconXSizeTweak = 1.f;
float expansionIconYSizeTweak = 1.f;
bool  largeSingleLineVanillaBonuses = true;
float tweakDividingLineY = -0.4f;
float bottomTextSizeTweak = 1.f;
bool  isSupply  = true;

float lastResetClick;
int imageToLoad = 0;

char* artworkNames[] = {(char*)"Harvest", (char*)"Horn of Plenty", (char*)"Duchess", (char*)"Oasis", (char*)"Spice Merchant", (char*)"Dame Natalie", (char*)"Mystic", (char*)"Advisor", (char*)"Soothsayer", (char*)"Page", (char*)"Treasure Hunter", (char*)"Warrior", (char*)"Hero", (char*)"Champion", (char*)"Royal Blacksmith", (char*)"Groundskeeper", (char*)"Ambassador", (char*)"Lookout", (char*)"Swindler", (char*)"Familiar", (char*)"Vault", (char*)"Survivors", (char*)"Storeroom", (char*)"Procession", (char*)"Quarry", (char*)"Tunnel", (char*)"Necropolis", (char*)"Blessed Village", (char*)"Cobbler", (char*)"Leprechaun", (char*)"Tragic Hero", (char*)"Sentinel", (char*)"Royal Galley", (char*)"Bonfire", (char*)"Ball", (char*)"Banquet", (char*)"Conquest", (char*)"Locusts", (char*)"Silos", (char*)"Academy", (char*)"Crop Rotation", (char*)"Ride", (char*)"Transport", (char*)"Invest", (char*)"Stampede", (char*)"Enclave", (char*)"Cave Dwellers", (char*)"Forest Dwellers", (char*)"Island Folk", (char*)"Mountain Folk", (char*)"Order of Astrologers", (char*)"Order of Masons", (char*)"Workshop", (char*)"Duke", (char*)"Minion", (char*)"Sea Hag", (char*)"Village", (char*)"Mine", (char*)"Coppersmith", (char*)"Mining Village", (char*)"Courtier", (char*)"Replace", (char*)"Haven", (char*)"Blockade", (char*)"Outpost", (char*)"Pirate", (char*)"Transmute", (char*)"Workers Village", (char*)"Haggler", (char*)"Abandoned Mine", (char*)"Hermit", (char*)"Madman", (char*)"Pillage", (char*)"Ranger", (char*)"Storyteller", (char*)"Hireling", (char*)"Enchantress", (char*)"Pixie", (char*)"Goat", (char*)"Imp", (char*)"Fool", (char*)"Lucky Coin", (char*)"Devil's Workshop", (char*)"Tormentor", (char*)"Border Guard", (char*)"Patron", (char*)"Treasurer", (char*)"Camel Train", (char*)"Goatherd", (char*)"Sheepdog", (char*)"Snowy Village", (char*)"Cardinal", (char*)"Cavalry", (char*)"Coven", (char*)"Falconer", (char*)"Hunting Lodge", (char*)"Sanctuary", (char*)"Animal Fair", (char*)"Horse", (char*)"Underling", (char*)"Broker", (char*)"Carpenter", (char*)"Galleria", (char*)"Skirmisher", (char*)"Specialist", (char*)"Basilica", (char*)"Tower", (char*)"Triumphal Arch", (char*)"Greed", (char*)"Lost in the Woods", (char*)"Key", (char*)"Lantern", (char*)"Potion", (char*)"Curse", (char*)"Hoard", (char*)"Saboteur", (char*)"Explorer", (char*)"Kings Court", (char*)"Stables", (char*)"Ironmonger", (char*)"Count", (char*)"Aqueduct", (char*)"Colonnade", (char*)"Obelisk", (char*)"Walled Village", (char*)"Ruined Village", (char*)"Wheelwright", (char*)"Engineer", (char*)"Legionary", (char*)"Overlord", (char*)"Bard", (char*)"Night Watchman", (char*)"Skulk", (char*)"Scholar", (char*)"Silk Merchant", (char*)"Spices", (char*)"Groom", (char*)"Hostelry", (char*)"Kiln", (char*)"Paddock", (char*)"Sycophant", (char*)"Guildmaster", (char*)"Dismantle", (char*)"Sentry", (char*)"Talisman", (char*)"Farmland", (char*)"Highway", (char*)"Forager", (char*)"Hovel", (char*)"Merchant Guild", (char*)"Guide", (char*)"Ratcatcher", (char*)"WineMerchant", (char*)"Haunted Mirror", (char*)"Ghost", (char*)"Scrap", (char*)"Barge", (char*)"Highwayman", (char*)"Prince", (char*)"Fellowship of Scribes", (char*)"Gang of Pickpockets", (char*)"Masquerade", (char*)"Pawn", (char*)"Torturer", (char*)"Island", (char*)"Pirate Ship", (char*)"Native Village", (char*)"Golem", (char*)"Charlatan", (char*)"Collection", (char*)"Marauder", (char*)"Scavenger", (char*)"Bounty Hunter", (char*)"Village Green", (char*)"Mastermind", (char*)"Swap", (char*)"Marquis", (char*)"Black Market", (char*)"Delusion", (char*)"Fear", (char*)"Farming Village", (char*)"Nomad Camp", (char*)"Cauldron", (char*)"Souk", (char*)"Beggar", (char*)"Sir Vander", (char*)"Patrician", (char*)"Emporium", (char*)"Gatekeeper", (char*)"Livery", (char*)"Destrier", (char*)"Wayfarer", (char*)"Forts", (char*)"Tent", (char*)"Garrison", (char*)"Hill Fort", (char*)"Stronghold", (char*)"Pilgrimage", (char*)"Pathfinding", (char*)"Battlefield", (char*)"Mountain Pass", (char*)"Fair", (char*)"Astrolabe", (char*)"Cargo Ship", (char*)"Druid", (char*)"Flag Bearer", (char*)"Lackeys", (char*)"Monkey", (char*)"Stockpile", (char*)"Supplies", (char*)"Werewolf", (char*)"Star Chart", (char*)"Flag", (char*)"Way of the Chameleon", (char*)"Way of the Rat", (char*)"Band of Nomads", (char*)"Circle of Witches", (char*)"Wandering Minstrel", (char*)"Augurs", (char*)"Herb Gatherer", (char*)"Acolyte", (char*)"Sorceress", (char*)"Sibyl", (char*)"Sinister Plot", (char*)"Guildhall", (char*)"Toil", (char*)"Seize the Day", (char*)"Alliance", (char*)"Populate", (char*)"Way of the Sheep", (char*)"Artisan", (char*)"Library", (char*)"Ruined Library", (char*)"Throne Room", (char*)"Bridge", (char*)"Courtyard", (char*)"Patrol", (char*)"Embargo", (char*)"Fishing Village", (char*)"Herbalist", (char*)"Trade Route", (char*)"Clerk", (char*)"Magnate", (char*)"Sage", (char*)"Squire", (char*)"Peasant", (char*)"Soldier", (char*)"Fugitive", (char*)"Disciple", (char*)"Teacher", (char*)"Temple", (char*)"Villa", (char*)"Wild Hunt", (char*)"Monastery", (char*)"Pooka", (char*)"Cursed Gold", (char*)"Mountain Village", (char*)"Clashes", (char*)"Battle Plan", (char*)"Archer", (char*)"Warlord", (char*)"Territory", (char*)"Wizards", (char*)"Student", (char*)"Conjurer", (char*)"Sorcerer", (char*)"Lich", (char*)"Governor", (char*)"Horn", (char*)"City Gate", (char*)"Citadel", (char*)"Banish", (char*)"Bargain", (char*)"Demand", (char*)"Reap", (char*)"Way of the Otter", (char*)"Rats", (char*)"Philosophers Stone", (char*)"Royal Seal", (char*)"IllGotten Gains", (char*)"Cultist", (char*)"Changeling", (char*)"Experiment", (char*)"Villain", (char*)"Bad Omens", (char*)"Envy", (char*)"Bank", (char*)"Horse Traders", (char*)"Jester", (char*)"Rogue", (char*)"Quest", (char*)"Raid", (char*)"Training", (char*)"Anvil", (char*)"Young Witch", (char*)"Fairgrounds", (char*)"Oracle", (char*)"Poor House", (char*)"Dame Sylvia", (char*)"Taxman", (char*)"Herald", (char*)"Swamp Hag", (char*)"Encampment", (char*)"Plunder", (char*)"Settlers", (char*)"Bustling Village", (char*)"Old Witch", (char*)"Sculptor", (char*)"Importer", (char*)"Emissary", (char*)"Save", (char*)"Plan", (char*)"Haunting", (char*)"Misery", (char*)"Poverty", (char*)"Delay", (char*)"March", (char*)"Way of the Horse", (char*)"Way of the Mole", (char*)"Merchant", (char*)"Poacher", (char*)"Peddler", (char*)"Noble Brigand", (char*)"Margrave", (char*)"Vagrant", (char*)"Sir Bailey", (char*)"Sacrifice", (char*)"Conclave", (char*)"Exorcist", (char*)"Cursed Village", (char*)"Seer", (char*)"Swashbuckler", (char*)"ScoutingParty", (char*)"Expedition", (char*)"LostArts", (char*)"Triumph", (char*)"Annex", (char*)"Ritual", (char*)"Wedding", (char*)"Windfall", (char*)"Keep", (char*)"Wall", (char*)"Treasure Chest", (char*)"Cathedral", (char*)"Canal", (char*)"Vassal", (char*)"Thief", (char*)"Bandit", (char*)"Laboratory", (char*)"Great Hall", (char*)"Warehouse", (char*)"Sailor", (char*)"Tide Pools", (char*)"Sea Witch", (char*)"Vineyard", (char*)"Monument", (char*)"Guard Dog", (char*)"Scheme", (char*)"Berserker", (char*)"Witch's Hut", (char*)"Graverobber", (char*)"Sir Martin", (char*)"Caravan Guard", (char*)"Lost City", (char*)"Castles", (char*)"Humble Castle", (char*)"Crumbling Castle", (char*)"Small Castle", (char*)"Haunted Castle", (char*)"Opulent Castle", (char*)"Sprawling Castle", (char*)"Grand Castle", (char*)"King's Castle", (char*)"Will-o'-Wisp", (char*)"Acting Troupe", (char*)"Hideout", (char*)"Inventor", (char*)"Priest", (char*)"Research", (char*)"Recruiter", (char*)"Townsfolk", (char*)"Town Crier", (char*)"Blacksmith", (char*)"Miller", (char*)"Elder", (char*)"Merchant Camp", (char*)"Odysseys", (char*)"Old Map", (char*)"Voyage", (char*)"Sunken Treasure", (char*)"Distant Shore", (char*)"Courier", (char*)"Innkeeper", (char*)"Town", (char*)"Barbarian", (char*)"Hunter", (char*)"Church", (char*)"Captain", (char*)"Travelling Fair", (char*)"Seaway", (char*)"Bandit Fort", (char*)"Labyrinth", (char*)"Museum", (char*)"Plague", (char*)"The Earth's Gift", (char*)"The Field's Gift", (char*)"The Flame's Gift", (char*)"The Forest's Gift", (char*)"The Moon's Gift", (char*)"The Mountain's Gift", (char*)"The River's Gift", (char*)"The Sea's Gift", (char*)"The Sky's Gift", (char*)"The Sun's Gift", (char*)"The Swamp's Gift", (char*)"The Wind's Gift", (char*)"War", (char*)"Candlestick Maker", (char*)"Masterpiece", (char*)"Royal Carriage", (char*)"Gladiator", (char*)"Fortune", (char*)"Wolf Den", (char*)"Possession", (char*)"Mountebank", (char*)"Jack of all Trades", (char*)"Mint", (char*)"Scrying Pool", (char*)"Hamlet", (char*)"Hunting Party", (char*)"Border Village", (char*)"Junk Dealer", (char*)"Rebuild", (char*)"Journeyman", (char*)"BridgeTroll", (char*)"HauntedWoods", (char*)"Chariot Race", (char*)"Defiled Shrine", (char*)"Venture", (char*)"Diplomat", (char*)"Followers", (char*)"Trader", (char*)"Market Square", (char*)"Dame Anna", (char*)"Stonemason", (char*)"Doctor", (char*)"Tiara", (char*)"War Chest", (char*)"Princess", (char*)"Mandarin", (char*)"Dame Josephine", (char*)"Dame Molly", (char*)"Baker", (char*)"Butcher", (char*)"Artificer", (char*)"Crypt", (char*)"Faithful Hound", (char*)"Capital City", (char*)"Contract", (char*)"Modify", (char*)"Borrow", (char*)"Baths", (char*)"Fountain", (char*)"Orchard", (char*)"Festival", (char*)"Secret Chamber", (char*)"Mill", (char*)"Nobles", (char*)"Secret Passage", (char*)"Lighthouse", (char*)"Caravan", (char*)"University", (char*)"Grand Market", (char*)"Nomads", (char*)"Trail", (char*)"Weaver", (char*)"Ruined Market", (char*)"Inn", (char*)"Armory", (char*)"Catacombs", (char*)"Cemetery", (char*)"Ghost Town", (char*)"Wish", (char*)"Sleigh", (char*)"Displace", (char*)"Fisherman", (char*)"Exploration", (char*)"Investment", (char*)"Crystal Ball", (char*)"City", (char*)"Develop", (char*)"Embassy", (char*)"Death Cart", (char*)"Fortress", (char*)"Sir Destry", (char*)"Sir Michael", (char*)"Gear", (char*)"Magpie", (char*)"Miser", (char*)"Transmogrify", (char*)"Giant", (char*)"Treasure Trove", (char*)"Archive", (char*)"Capital", (char*)"Charm", (char*)"Crown", (char*)"Necromancer", (char*)"Zombie Apprentice", (char*)"Zombie Mason", (char*)"Zombie Spy", (char*)"Ducat", (char*)"Improve", (char*)"Scepter", (char*)"Black Cat", (char*)"Trade", (char*)"Summon", (char*)"Way of the Butterfly", (char*)"Way of the Frog", (char*)"Way of the Goat", (char*)"Way of the Monkey", (char*)"Way of the Mouse", (char*)"Way of the Mule", (char*)"Way of the Owl", (char*)"Way of the Pig", (char*)"Way of the Seal", (char*)"Way of the Squirrel", (char*)"Way of the Turtle", (char*)"Way of the Worm", (char*)"City-state", (char*)"Coastal Haven", (char*)"League of Shopkeepers", (char*)"Market Towns", (char*)"Trappers' Lodge", (char*)"Woodworkers' Guild", (char*)"Trusty Steed", (char*)"Remake", (char*)"Cartographer", (char*)"Port", (char*)"Den of Sin", (char*)"Raider", (char*)"Ferry", (char*)"Inheritance", (char*)"Delve", (char*)"Palace", (char*)"Harbinger", (char*)"Lurker", (char*)"Ironworks", (char*)"Trading Post", (char*)"Sea Chart", (char*)"Cutpurse", (char*)"Bazaar", (char*)"Corsair", (char*)"Tactician", (char*)"Apprentice", (char*)"Forge", (char*)"Silk Road", (char*)"Overgrown Estate", (char*)"Urchin", (char*)"Mercenary", (char*)"Dungeon", (char*)"Messenger", (char*)"Guardian", (char*)"Tracker", (char*)"Pouch", (char*)"Sacred Grove", (char*)"Vampire", (char*)"Bat", (char*)"Stash", (char*)"Mission", (char*)"Advance", (char*)"Donate", (char*)"Dominate", (char*)"Arena", (char*)"Tomb", (char*)"Famine", (char*)"Capitalism", (char*)"Fleet", (char*)"Piazza", (char*)"Barracks", (char*)"Desperation", (char*)"Gamble", (char*)"Pursue", (char*)"Enhance", (char*)"Commerce", (char*)"Architects' Guild", (char*)"Crafters' Guild", (char*)"Desert Guides", (char*)"Family of Inventors", (char*)"League of Bankers", (char*)"Peaceful Cult", (char*)"Estate", (char*)"Duchy", (char*)"Province", (char*)"Colony", (char*)"Menagerie", (char*)"Cellar", (char*)"Chapel", (char*)"Moat", (char*)"Chancellor", (char*)"Woodcutter", (char*)"Bureaucrat", (char*)"Feast", (char*)"Gardens", (char*)"Militia", (char*)"Moneylender", (char*)"Remodel", (char*)"Smithy", (char*)"Council Room", (char*)"Market", (char*)"Witch", (char*)"Steward", (char*)"Conspirator", (char*)"Scout", (char*)"Tribute", (char*)"Upgrade", (char*)"Treasure Map", (char*)"Ghost Ship", (char*)"Counting House", (char*)"Crossroads", (char*)"Feodum", (char*)"Knights", (char*)"Altar", (char*)"Amulet", (char*)"Distant Lands", (char*)"City Quarter", (char*)"Catapult", (char*)"Rocks", (char*)"Shepherd", (char*)"Pasture", (char*)"Envoy", (char*)"Alms", (char*)"Sewers", (char*)"Road Network", (char*)"Way of the Camel", (char*)"Way of the Ox", (char*)"Plateau Shepherds", (char*)"Pot", (char*)"Shanty Town", (char*)"Harem", (char*)"Pearl Diver", (char*)"Navigator", (char*)"Spy", (char*)"Sauna", (char*)"Avanto", (char*)"Tax", (char*)"Salt the Earth", (char*)"Fortune Teller", (char*)"Smugglers", (char*)"Salvager", (char*)"Contraband", (char*)"Rabble", (char*)"Cache", (char*)"Band of Misfits", (char*)"Bandit Camp", (char*)"Raze", (char*)"Watchtower", (char*)"Bishop", (char*)"Adventurer", (char*)"Wishing Well", (char*)"Baron", (char*)"Merchant Ship", (char*)"Treasury", (char*)"Apothecary", (char*)"Expand", (char*)"Bag Of Gold", (char*)"Diadem", (char*)"Fools Gold", (char*)"Spoils", (char*)"Counterfeit", (char*)"Plaza", (char*)"Coin of the Realm", (char*)"Duplicate", (char*)"Relic", (char*)"Farmers' Market", (char*)"Forum", (char*)"Idol", (char*)"Secret Cave", (char*)"Magic Lamp", (char*)"Bauble", (char*)"Pageant", (char*)"Innovation", (char*)"Copper", (char*)"Gold", (char*)"Platinum", (char*)"Silver", (char*)"Wharf", (char*)"Alchemist", (char*)"Loan", (char*)"Hunting Grounds", (char*)"Tournament", (char*)"Goons"};
char* artworkUrls [] = {(char*)"http://wiki.dominionstrategy.com/images/1/15/HarvestArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8f/Horn_of_PlentyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bb/DuchessArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d3/OasisArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4b/Spice_MerchantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/1/1a/Dame_NatalieArt.jpg/354px-Dame_NatalieArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/21/MysticArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c8/AdvisorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/ff/SoothsayerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/ac/PageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c1/Treasure_HunterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/b/bf/WarriorArt.jpg/354px-WarriorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/60/HeroArt.jpg/354px-HeroArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/32/ChampionArt.jpg/354px-ChampionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/6e/Royal_BlacksmithArt.jpg/354px-Royal_BlacksmithArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/2/2f/GroundskeeperArt.jpg/354px-GroundskeeperArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/92/AmbassadorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fa/LookoutArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/27/SwindlerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/45/FamiliarArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/49/VaultArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a4/SurvivorsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1e/StoreroomArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/29/ProcessionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6d/QuarryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/a/ad/TunnelArt.jpg/354px-TunnelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fe/NecropolisArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e5/Blessed_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/3d/CobblerArt.jpg/354px-CobblerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/8/8b/LeprechaunArt.jpg/354px-LeprechaunArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/70/Tragic_HeroArt.jpg/354px-Tragic_HeroArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1c/SentinelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/a/a5/Royal_GalleyArt.jpg/354px-Royal_GalleyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/4/4c/BonfireArt.jpg/452px-BonfireArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/e/e5/BallArt.jpg/452px-BallArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/2/2b/BanquetArt.jpg/452px-BanquetArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/9/96/ConquestArt.jpg/452px-ConquestArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/38/LocustsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/35/SilosArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/38/AcademyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7a/Crop_RotationArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2f/RideArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/01/TransportArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/1/18/InvestArt.jpg/452px-InvestArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f5/StampedeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9e/EnclaveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/80/Cave_DwellersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2d/Forest_DwellersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/4/43/Island_FolkArt.jpg/452px-Island_FolkArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/11/Mountain_FolkArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/31/Order_of_AstrologersArt.jpg/452px-Order_of_AstrologersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/17/Order_of_MasonsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/24/WorkshopArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6e/DukeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/70/MinionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7b/Sea_HagArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/e/e6/VillageArt.jpg/354px-VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1d/MineArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c3/CoppersmithArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/2/22/Mining_VillageArt.jpg/354px-Mining_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b5/CourtierArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/74/ReplaceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d4/HavenArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f4/BlockadeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/5/54/OutpostArt.jpg/354px-OutpostArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cf/PirateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/9/9b/TransmuteArt.jpg/354px-TransmuteArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/e/e4/Workers_VillageArt.jpg/354px-Workers_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b5/HagglerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/ae/Abandoned_MineArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5b/HermitArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c1/MadmanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/07/PillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/94/RangerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bc/StorytellerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cf/HirelingArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f7/EnchantressArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/58/PixieArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/06/GoatArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2c/ImpArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e3/FoolArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fa/Lucky_CoinArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b3/Devil%27s_WorkshopArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e8/TormentorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/13/Border_GuardArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/26/PatronArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4f/TreasurerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4b/Camel_TrainArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0e/GoatherdArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0f/SheepdogArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d0/Snowy_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0f/CardinalArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/52/CavalryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cd/CovenArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6a/FalconerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c3/Hunting_LodgeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a9/SanctuaryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1e/Animal_FairArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/41/HorseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/44/UnderlingArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/7f/BrokerArt.jpg/354px-BrokerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/55/CarpenterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8b/GalleriaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/25/SkirmisherArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/03/SpecialistArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/64/BasilicaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2f/TowerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cb/Triumphal_ArchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f8/GreedArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fb/Lost_in_the_WoodsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a2/KeyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/70/LanternArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/da/PotionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/11/CurseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ea/HoardArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e5/SaboteurArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d7/ExplorerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2e/Kings_CourtArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/38/StablesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/96/IronmongerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f8/CountArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0b/AqueductArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/94/ColonnadeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/23/ObeliskArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ea/Walled_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/21/Ruined_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/41/WheelwrightArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2b/EngineerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/23/LegionaryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c0/OverlordArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/20/BardArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d3/Night_WatchmanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3f/SkulkArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/62/ScholarArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b2/Silk_MerchantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fb/SpicesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/22/GroomArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ea/HostelryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/06/KilnArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/be/PaddockArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/41/SycophantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ea/GuildmasterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b3/DismantleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bd/SentryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/d/de/TalismanArt.jpg/354px-TalismanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/d/dd/FarmlandArt.jpg/354px-FarmlandArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/21/HighwayArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/8/8d/ForagerArt.jpg/354px-ForagerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/4/4a/HovelArt.jpg/354px-HovelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5e/Merchant_GuildArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/b/b7/GuideArt.jpg/354px-GuideArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/8/81/RatcatcherArt.jpg/354px-RatcatcherArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/61/WineMerchantArt.jpg/354px-WineMerchantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/3d/Haunted_MirrorArt.jpg/354px-Haunted_MirrorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/2/2d/GhostArt.jpg/354px-GhostArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/01/ScrapArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fd/BargeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/67/HighwaymanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/c/ca/PrinceArt.jpg/354px-PrinceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/57/Fellowship_of_ScribesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4c/Gang_of_PickpocketsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b6/MasqueradeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d7/PawnArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6e/TorturerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/40/IslandArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/ad/Pirate_ShipArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4f/Native_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/38/GolemArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0d/CharlatanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f6/CollectionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/10/MarauderArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/21/ScavengerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f3/Bounty_HunterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/01/Village_GreenArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/03/MastermindArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/98/SwapArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9c/MarquisArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/93/Black_MarketArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/58/DelusionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/09/FearArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/a/a5/Farming_VillageArt.jpg/354px-Farming_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/e/ef/Nomad_CampArt.jpg/354px-Nomad_CampArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/84/CauldronArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7a/SoukArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e3/BeggarArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/1/1d/Sir_VanderArt.jpg/354px-Sir_VanderArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/0/00/PatricianArt.jpg/354px-PatricianArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c2/EmporiumArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/0/07/GatekeeperArt.jpg/354px-GatekeeperArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/8/8e/LiveryArt.jpg/354px-LiveryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/75/DestrierArt.jpg/354px-DestrierArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/30/WayfarerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/13/FortsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9a/TentArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/01/GarrisonArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6e/Hill_FortArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/59/StrongholdArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/a/a2/PilgrimageArt.jpg/452px-PilgrimageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a3/PathfindingArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/86/BattlefieldArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/4/43/Mountain_PassArt.jpg/452px-Mountain_PassArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a7/FairArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fe/AstrolabeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/81/Cargo_ShipArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/a/a3/DruidArt.jpg/354px-DruidArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/53/Flag_BearerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e7/LackeysArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d2/MonkeyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4c/StockpileArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1b/SuppliesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4a/WerewolfArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bc/Star_ChartArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/82/FlagArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7b/Way_of_the_ChameleonArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/83/Way_of_the_RatArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b7/Band_of_NomadsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/12/Circle_of_WitchesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fc/Wandering_MinstrelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/29/AugursArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e6/Herb_GathererArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/53/AcolyteArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b9/SorceressArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bd/SibylArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/05/Sinister_PlotArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2e/GuildhallArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/72/ToilArt.jpg/452px-ToilArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/48/Seize_the_DayArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a4/AllianceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/de/PopulateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d4/Way_of_the_SheepArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/08/ArtisanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/de/LibraryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/ff/Ruined_LibraryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f2/Throne_RoomArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7c/BridgeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/13/CourtyardArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/40/PatrolArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/96/EmbargoArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/20/Fishing_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/0/09/HerbalistArt.jpg/354px-HerbalistArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/35/Trade_RouteArt.jpg/354px-Trade_RouteArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a1/ClerkArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4b/MagnateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d6/SageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8e/SquireArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2a/PeasantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/36/SoldierArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f7/FugitiveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b9/DiscipleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8c/TeacherArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/56/TempleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/03/VillaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e6/Wild_HuntArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/64/MonasteryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/78/PookaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c2/Cursed_GoldArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c5/Mountain_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/92/ClashesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/53/Battle_PlanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4b/ArcherArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c8/WarlordArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7c/TerritoryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c3/WizardsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3c/StudentArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c8/ConjurerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f0/SorcererArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c7/LichArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e3/GovernorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/29/HornArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e2/City_GateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d4/CitadelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f8/BanishArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4f/BargainArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/60/DemandArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9d/ReapArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a0/Way_of_the_OtterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3b/RatsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/92/Philosophers_StoneArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/38/Royal_SealArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ee/IllGotten_GainsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1b/CultistArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d8/ChangelingArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/90/ExperimentArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1c/VillainArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1b/Bad_OmensArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bd/EnvyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/80/BankArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/89/Horse_TradersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/ff/JesterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fa/RogueArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b9/QuestArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/21/RaidArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/34/TrainingArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/77/AnvilArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/89/Young_WitchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e1/FairgroundsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/eb/OracleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/02/Poor_HouseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/66/Dame_SylviaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/85/TaxmanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/13/HeraldArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/35/Swamp_HagArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/63/EncampmentArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/10/PlunderArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/50/SettlersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0a/Bustling_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2a/Old_WitchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/79/SculptorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f7/ImporterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1c/EmissaryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6a/SaveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f2/PlanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/10/HauntingArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/ce/MiseryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e2/PovertyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e0/DelayArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/24/MarchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/66/Way_of_the_HorseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/62/Way_of_the_MoleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d3/MerchantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/55/PoacherArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9f/PeddlerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6b/Noble_BrigandArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f0/MargraveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0c/VagrantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d2/Sir_BaileyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d7/SacrificeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/fc/ConclaveArt.jpg/354px-ConclaveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/4/44/ExorcistArt.jpg/354px-ExorcistArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/18/Cursed_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/85/SeerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/fd/SwashbucklerArt.jpg/354px-SwashbucklerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/0/0d/ScoutingPartyArt.jpg/452px-ScoutingPartyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/c/c0/ExpeditionArt.jpg/452px-ExpeditionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/4/49/LostArtsArt.jpg/452px-LostArtsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9d/TriumphArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/46/AnnexArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cd/RitualArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/25/WeddingArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/f0/WindfallArt.jpg/452px-WindfallArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/b/b5/KeepArt.jpg/452px-KeepArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8d/WallArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f0/Treasure_ChestArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/fb/CathedralArt.jpg/452px-CathedralArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/31/CanalArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/ba/VassalArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/63/ThiefArt.jpg/354px-ThiefArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d4/BanditArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/60/LaboratoryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7e/Great_HallArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ed/WarehouseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8f/SailorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4f/Tide_PoolsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/68/Sea_WitchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c9/VineyardArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d5/MonumentArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0b/Guard_DogArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/ab/SchemeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/be/BerserkerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/21/Witch%27s_HutArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/29/GraverobberArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/ab/Sir_MartinArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/96/Caravan_GuardArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b1/Lost_CityArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/13/CastlesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/32/Humble_CastleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1b/Crumbling_CastleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/26/Small_CastleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a8/Haunted_CastleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5f/Opulent_CastleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3b/Sprawling_CastleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/30/Grand_CastleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c1/King%27s_CastleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bf/Will-o%27-WispArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bd/Acting_TroupeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6a/HideoutArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/51/InventorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8f/PriestArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0b/ResearchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3d/RecruiterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cf/TownsfolkArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fb/Town_CrierArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/49/BlacksmithArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/05/MillerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cc/ElderArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/dd/Merchant_CampArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/60/OdysseysArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/70/Old_MapArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8a/VoyageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/11/Sunken_TreasureArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c0/Distant_ShoreArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/48/CourierArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4e/InnkeeperArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/ce/TownArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c2/BarbarianArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4c/HunterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/b/bf/ChurchArt.jpg/354px-ChurchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8e/CaptainArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d4/Travelling_FairArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ec/SeawayArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/df/Bandit_FortArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8d/LabyrinthArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/69/MuseumArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1d/PlagueArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/78/The_Earth%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/77/The_Field%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/22/The_Flame%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/73/The_Forest%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cd/The_Moon%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/53/The_Mountain%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/33/The_River%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/88/The_Sea%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bc/The_Sky%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f1/The_Sun%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c5/The_Swamp%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f4/The_Wind%27s_GiftArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/23/WarArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e2/Candlestick_MakerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d9/MasterpieceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a6/Royal_CarriageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a7/GladiatorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fd/FortuneArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/09/Wolf_DenArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/fd/PossessionArt.jpg/354px-PossessionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/0/0d/MountebankArt.jpg/354px-MountebankArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/ae/Jack_of_all_TradesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b9/MintArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f8/Scrying_PoolArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/b/b0/HamletArt.jpg/354px-HamletArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/65/Hunting_PartyArt.jpg/354px-Hunting_PartyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/2/2b/Border_VillageArt.jpg/354px-Border_VillageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/8/80/Junk_DealerArt.jpg/354px-Junk_DealerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/4/4d/RebuildArt.jpg/354px-RebuildArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/a/a5/JourneymanArt.jpg/354px-JourneymanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/a/a1/BridgeTrollArt.jpg/354px-BridgeTrollArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/78/HauntedWoodsArt.jpg/354px-HauntedWoodsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c9/Chariot_RaceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cf/Defiled_ShrineArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e3/VentureArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/92/DiplomatArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/89/FollowersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/72/TraderArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/dd/Market_SquareArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/eb/Dame_AnnaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/59/StonemasonArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cc/DoctorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b6/TiaraArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/09/War_ChestArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ee/PrincessArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cf/MandarinArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/89/Dame_JosephineArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5a/Dame_MollyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/16/BakerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/56/ButcherArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/6b/ArtificerArt.jpg/354px-ArtificerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fc/CryptArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/b/b8/Faithful_HoundArt.jpg/354px-Faithful_HoundArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cd/Capital_CityArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/04/ContractArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/13/ModifyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/af/BorrowArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a1/BathsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5b/FountainArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c6/OrchardArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/dc/FestivalArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1a/Secret_ChamberArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f9/MillArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/98/NoblesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5e/Secret_PassageArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/06/LighthouseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/21/CaravanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e3/UniversityArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cc/Grand_MarketArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/26/NomadsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a3/TrailArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b3/WeaverArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0e/Ruined_MarketArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3d/InnArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/91/ArmoryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5b/CatacombsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a7/CemeteryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ed/Ghost_TownArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4d/WishArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/dd/SleighArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6e/DisplaceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/51/FishermanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6d/ExplorationArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e2/InvestmentArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/38/Crystal_BallArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/37/CityArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/0/00/DevelopArt.jpg/354px-DevelopArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/31/EmbassyArt.jpg/354px-EmbassyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/2/2e/Death_CartArt.jpg/354px-Death_CartArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/f5/FortressArt.jpg/354px-FortressArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/7f/Sir_DestryArt.jpg/354px-Sir_DestryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/5/5a/Sir_MichaelArt.jpg/354px-Sir_MichaelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/62/GearArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/b/b0/MagpieArt.jpg/354px-MagpieArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c0/MiserArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/d/dc/TransmogrifyArt.jpg/354px-TransmogrifyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/31/GiantArt.jpg/354px-GiantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/94/Treasure_TroveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/74/ArchiveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a5/CapitalArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/35/CharmArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/65/CrownArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b4/NecromancerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/35/Zombie_ApprenticeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a2/Zombie_MasonArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7a/Zombie_SpyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a3/DucatArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/51/ImproveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f3/ScepterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/15/Black_CatArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/f7/TradeArt.jpg/452px-TradeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/a/ad/SummonArt.jpg/452px-SummonArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/36/Way_of_the_ButterflyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c7/Way_of_the_FrogArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/8d/Way_of_the_GoatArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/91/Way_of_the_MonkeyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/67/Way_of_the_MouseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5b/Way_of_the_MuleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/ce/Way_of_the_OwlArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/be/Way_of_the_PigArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/28/Way_of_the_SealArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/27/Way_of_the_SquirrelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/31/Way_of_the_TurtleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e9/Way_of_the_WormArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3e/City-stateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/95/Coastal_HavenArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/31/League_of_ShopkeepersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f9/Market_TownsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9d/Trappers%27_LodgeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/23/Woodworkers%27_GuildArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3f/Trusty_SteedArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fe/RemakeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/75/CartographerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/71/PortArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/02/Den_of_SinArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cc/RaiderArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7a/FerryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/dd/InheritanceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b5/DelveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/12/PalaceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2d/HarbingerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/78/LurkerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0d/IronworksArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c3/Trading_PostArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5d/Sea_ChartArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3e/CutpurseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/7c/BazaarArt.jpg/354px-BazaarArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/79/CorsairArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/49/TacticianArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1c/ApprenticeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/35/ForgeArt.jpg/354px-ForgeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b3/Silk_RoadArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/71/Overgrown_EstateArt.jpg/354px-Overgrown_EstateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/15/UrchinArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bb/MercenaryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/4/4b/DungeonArt.jpg/354px-DungeonArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/98/MessengerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d6/GuardianArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/46/TrackerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/52/PouchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a2/Sacred_GroveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/ae/VampireArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f0/BatArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/df/StashArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/90/MissionArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/36/AdvanceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f5/DonateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e7/DominateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/74/ArenaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/54/TombArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/51/FamineArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/19/CapitalismArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bf/FleetArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/ff/PiazzaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/f9/BarracksArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fe/DesperationArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/96/GambleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/80/PursueArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e2/EnhanceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6b/CommerceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3c/Architects%27_GuildArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6c/Crafters%27_GuildArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/cb/Desert_GuidesArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3d/Family_of_InventorsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5f/League_of_BankersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/73/Peaceful_CultArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e3/EstateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/30/DuchyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4c/ProvinceArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2d/ColonyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/e/eb/MenagerieArt.jpg/354px-MenagerieArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/29/CellarArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/73/ChapelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/aa/MoatArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/3f/ChancellorArt.jpg/354px-ChancellorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/60/WoodcutterArt.jpg/354px-WoodcutterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/18/BureaucratArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/14/FeastArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/43/GardensArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6f/MilitiaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/67/MoneylenderArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/08/RemodelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d7/SmithyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bb/Council_RoomArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/24/MarketArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5c/WitchArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c3/StewardArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/26/ConspiratorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/79/ScoutArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5d/TributeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b4/UpgradeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/29/Treasure_MapArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/5e/Ghost_ShipArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9c/Counting_HouseArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7f/CrossroadsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/56/FeodumArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/7b/KnightsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4b/AltarArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/48/AmuletArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c4/Distant_LandsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/68/City_QuarterArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/bd/CatapultArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/f/fc/RocksArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/da/ShepherdArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/9e/PastureArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/f8/EnvoyArt.jpg/354px-EnvoyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/ab/AlmsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/ec/SewersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d7/Road_NetworkArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/03/Way_of_the_CamelArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/4f/Way_of_the_OxArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/72/Plateau_ShepherdsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/c/c3/Potion.jpg/200px-Potion.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/36/Shanty_TownArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/90/HaremArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/20/Pearl_DiverArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/54/NavigatorArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/83/SpyArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/d/de/SaunaArt.jpg/354px-SaunaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/f1/AvantoArt.jpg/354px-AvantoArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/21/TaxArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/32/Salt_the_EarthArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/5/52/Fortune_TellerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/64/SmugglersArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2a/SalvagerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0a/ContrabandArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1b/RabbleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/7/79/CacheArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d9/Band_of_MisfitsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6f/Bandit_CampArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c8/RazeArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e1/WatchtowerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/48/BishopArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/76/AdventurerArt.jpg/354px-AdventurerArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a7/Wishing_WellArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/dc/BaronArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/65/Merchant_ShipArt.jpg/354px-Merchant_ShipArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/7/79/TreasuryArt.jpg/354px-TreasuryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/e/e6/ApothecaryArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/0a/ExpandArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/5/5a/Bag_Of_GoldArt.jpg/354px-Bag_Of_GoldArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/6/67/DiademArt.jpg/354px-DiademArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/6b/Fools_GoldArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3a/SpoilsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/24/CounterfeitArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/a/a1/PlazaArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/4/43/Coin_of_the_RealmArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/0/09/DuplicateArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/93/RelicArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/d/d2/Farmers%27_MarketArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2c/ForumArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/2/2e/IdolArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/b/b3/Secret_CaveArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/c/c2/Magic_LampArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1a/BaubleArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3d/PageantArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/32/InnovationArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/6/62/CopperArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/93/GoldArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/1/1a/PlatinumArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/30/SilverArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/1/16/WharfArt.jpg/354px-WharfArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/9/93/AlchemistArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/3/3a/LoanArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/3/38/Hunting_GroundsArt.jpg/354px-Hunting_GroundsArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/thumb/f/f1/TournamentArt.jpg/354px-TournamentArt.jpg", (char*)"http://wiki.dominionstrategy.com/images/8/83/GoonsArt.jpg"};

char** shownArtworks = (char**)malloc(sizeof(char*) * 700);
int p = 0;
bool isOtherWindowShwon = false;

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

char *trimwhitespace(char *str) { // https://stackoverflow.com/a/122721
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
void reloadPictures() {
	loadIcon(string(iconUrl), "./tempicon.png", &(res::tempIcon), false);
	loadIcon(string(expansionUrl), "./expansionicon.png", &(res::tempExpansionIcon), false);
}
void composeDearImGuiFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    {
		ImGui::SetNextWindowBgAlpha(0.7f);
        
        ImGui::Begin("Options", NULL);
		
		ImGui::ListBox("Layout", &cardLayout, layoutChoices, IM_ARRAYSIZE(layoutChoices), 5);
		
		if(cardLayout == 0) ImGui::Checkbox("Traveller?", &isTraveler);
		if(cardLayout == 0) ImGui::Checkbox("Is Supply Pile?", &isSupply);

		//ImGui::InputText("string", title, IM_ARRAYSIZE(title));
		
		ImGui::InputText("Title", cardTitle, 100);
		
		if(cardLayout <= 2) ImGui::InputText("Type", cardType, 100);
		if(cardLayout <= 2) ImGui::InputText("Cost", cardCost, 30);
		
		if(cardLayout != 3) ImGui::InputText("Art Credit", cardCredit, 120);
		if(cardLayout != 3) ImGui::InputText("Card Version and Creator", cardVersion, 120);
		
		if(cardLayout == 0 || cardLayout == 2) ImGui::InputText("Preview (Top left & right)", cardPreview, 30);
		
		if(cardLayout <= 3) ImGui::ListBox("Color", &cardColor, mainChoices, IM_ARRAYSIZE(mainChoices), 6);
		if(cardLayout < 2)  ImGui::ListBox("Secondary Color", &cardSecondary, secondaryChoices, IM_ARRAYSIZE(secondaryChoices), 6);
		if(cardLayout == 4) ImGui::ListBox("Mat Color", &matColor, matColorChoices, IM_ARRAYSIZE(matColorChoices), 6);
		
		if(cardLayout <= 2) {
			static ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
			ImGui::InputTextMultiline("Card Text", cardText, 500, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
		}
		
		ImGui::InputText("Picture URL", iconUrl, 500);
		if(cardLayout <= 2) ImGui::InputText("Expansion URL", expansionUrl, 500);
		if(ImGui::Button("Reload Pictures")) {
			reloadPictures();
		}
		ImGui::SameLine();
		ImGui::Text("(Will probably freeze as image loads)");
		if(ImGui::Button("Choose From Official Images")) {
			ImGui::OpenPopup("Choose from Official Images");
		}
		if(ImGui::Button("Choose Official Expansion Icon")) {
			ImGui::OpenPopup("Choose Official Expansion Icon");
		}
		ImGui::SliderFloat("X Move Distance", &xMove, -1.f, 1.f, "%.3f");
		ImGui::SliderFloat("Y Move Distance", &yMove, -1.f, 1.f, "%.3f");
		ImGui::SliderFloat("Zoom", &zoom, 0.25f, 4.f, "%.2f");
		if(ImGui::Button("Reset Positions")) {
			xMove = 0;
			yMove = 0;
			zoom  = 1;
		}
		
		if(ImGui::Button("Save Screenshot")) 	screenShot();
		
		if(ImGui::Button("Copy to Clipboard")) 	copyToClipboard();
		
		if (ImGui::Button("Click To Reset All")) ImGui::OpenPopup("Reset All");

		if(ImGui::BeginPopupModal("Reset All", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Are you sure you want to delete everything?\nThis will restart the program.");
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				int pid = fork();
				if(pid == 0) {
					execv("./DCLP", (char *const[]){(char*)"./DCLP"});
				} else {
					glfwHideWindow(window);
					system("rm ./save.dclp ./tempicon.png expansionicon.png > /dev/null");
					waitpid(pid, 0, 0);
					exit(0);
				}
			}
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
		}
		if(ImGui::BeginPopupModal("Choose from Official Images", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			if(ImGui::InputText("Search (CASE SENSITIVE)", artSearch, 100) || isFirstFrame) {
				int i = 0;
				p = 0;
				for(; i < sizeof(artworkNames) / sizeof(char*); i++) {
					if(strstr(artworkNames[i], trimwhitespace(artSearch)) != nullptr) {
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
			if(ImGui::InputText("Search", artSearch, 100) || isFirstFrame) {
				int i = 0;
				p = 0;
				for(; i < sizeof(artworkNames) / sizeof(char*); i++) {
					if(strstr(artworkNames[i], trimwhitespace(artSearch)) != nullptr) {
						shownArtworks[p++] = artworkNames[i];
					}
				}
				shownArtworks[p] = NULL;
			}

			ImGui::ListBox("Official Icons", &imageToLoad, expansionIconList, IM_ARRAYSIZE(expansionIconList), 15);

			if(ImGui::Button("Load Selected Image")) {
				string tem = string(expansionIconListImage[imageToLoad]);
				string exePath = getPathToPwd();
				string toLoadS = "file://" + exePath + "/" + tem;
				cout << toLoadS << endl;
				int i = 0;
				for(; i < toLoadS.size(); i++) {
					expansionUrl[i] = toLoadS.at(i);
				}
				expansionUrl[i] = 0;
				reloadPictures();
				ImGui::CloseCurrentPopup();
			}

			if(ImGui::Button("Close Window")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		
		if(cardColor >= NUMBER_OF_CHOICES-2 && cardLayout < 4) {
			ImGui::ColorEdit3("Card Base Color", customCardColor);
			if(cardColor == NUMBER_OF_CHOICES-1) {
				ImGui::ColorEdit3("Card Embellishemnt Color", customEmbellishmentColor);
				if(cardLayout != 3) ImGui::ColorEdit3("Card Side Color", customSideColor);
			}
			ImGui::Text("Custom colors are not saved. =(");
			if(ImGui::Button("Reset Colors")) {
				for(int i = 0; i < 12; i++) {
					customCardColor[i] = 1.f;
					customEmbellishmentColor[i] = 1.f;
					customSideColor[i] = allBrown[i];
				}
			}
		}
		if(cardLayout != 3) ImGui::Text("Tweaks");
		if(cardLayout <= 2) ImGui::Checkbox("Large Single Line Vanilla Bonuses", &largeSingleLineVanillaBonuses);
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Border Width", &textXTweak, 0.3f, 4.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text X Position", &textXPosTweak, -1.f, 1.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Y Position", &textYPosTweak, -1.f, 1.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Tweak Text Size", &textSizeTweak, 0.3f, 4.f, "%.2f");

		if(cardLayout != 3) ImGui::SliderFloat("Tweak Expansion Icon X Size", &expansionIconXSizeTweak, 0.5f, 2.f, "%.2f");
		if(cardLayout != 3) ImGui::SliderFloat("Tweak Expansion Icon Y Size", &expansionIconYSizeTweak, 0.5f, 2.f, "%.2f");

		if(cardLayout <= 1) ImGui::SliderFloat("Tweak Dividing Line Y Position", &tweakDividingLineY, -1.f, 1.f, "%.2f");
		if(cardLayout <= 2) ImGui::SliderFloat("Bottom Text Size Tweak", &bottomTextSizeTweak, 0.3f, 2.f, "%.2f");

    	ImGui::End();

		
    	// ImGui::ShowDemoWindow();
    }
	
}
static void draw() {
    handlerDraw();
	
	composeDearImGuiFrame();
    ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
static void error_callback(int error, const char* description) {
    fputs(description, stderr);
}
static void changeFullscreen() {
	if(!isFullscreen) {
		const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, v->width, v->height, 0);
	} else {
		const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, nullptr, 0, 0, 1600, 900, 0);
		glfwMaximizeWindow(window);
	}
	isFullscreen = !isFullscreen;
	if(isFullscreen) {
		Log::debug("Entering fullscreen.");
	} else {
		Log::debug("Exiting fullscreen.");
	}
}
bool testBool = false;
bool isFullscreen = false;
bool hasDebugMode = false;
static void key_callback(GLFWwindow* eventWindow, int key, int scancode, int action, int mods) {
	if(key == GLFW_KEY_S && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
	   Log::log("Ctrl+Alt+S pressed, segfaulting. This will kill everything in its place, probably causing things like data loss and stuff.");
	   Log::flushFile();
		
       raise(11);
    }
    if(key == GLFW_KEY_F4 && glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        return;
    }
    if(action != GLFW_PRESS) {
        return;
    }
	if(isPausedMaster) update();
	if(key == GLFW_KEY_F4 && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		isPausedMaster = !isPausedMaster;
		Log::debug(string(isPausedMaster ? "Pausing " : "Unpausing ") + "game");
	}
	if(key == GLFW_KEY_F11) {
		changeFullscreen();
	}
    if(key == GLFW_KEY_SPACE) {
        testBool = !testBool;
		Log::debug("Setting testBool to " + to_string(testBool));
    }
    onKey(key, action, mods);
}
static void window_size_callback(GLFWwindow* window, int width, int height) {
    Log::debug("Window resized to " + doubleToString(width) + "x" + doubleToString(height));
	setFloat("pixelWidth", pixelWidth);
	setFloat("pixelHeight", pixelHeight);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(action == GLFW_PRESS) onMouse(getX(), getY(), action, button);
	if(action == GLFW_RELEASE) onMouseRelease();
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	onScroll(xoffset, yoffset);
	//Log::debug("Scrolled with xOffset of " + to_string(xoffset) + " and a yOffset of " + to_string(yoffset) + ".");
}
void window_focus_callback(GLFWwindow* window, int focused) {
    if(focused) {
        isWindowFocused = true;
		Log::debug("Window focused");
    } else {
        isWindowFocused = false;
		Log::debug("Window unfocused");
    }
}
void APIENTRY errorPassThrough(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *userParam) {
	if(hasDebugMode) {
		cout << message << endl;
	}
}
void calculateFPS() {
    totalFramesEver++;
    if(floor(glfwGetTime()) > currentSecond) {
		averageUpdateDelta = updateDeltaAdd / (float)framesSoFar;
		averageDrawDelta = drawDeltaAdd / (float)framesSoFar;
		
        currentSecond = floor(glfwGetTime());
        currentFps = framesSoFar;
        framesSoFar = 0;
		
		drawDeltaAdd = 0.f;
		updateDeltaAdd = 0.f;
		
		Log::flushFile();
    } else {
        framesSoFar++;
    }
}
float getHBorder() {
    if(windowRatio > baseWindowRatio) {
        return windowRatio;
    } else {
        return baseWindowRatio;
    }
}
float getYBorder() {
    if(windowRatio > baseWindowRatio) {
        return 1.f;
    } else {
        return baseWindowRatio * windowInverse;
    }
}
GLFWwindow* createWindow(bool f) {
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_REFRESH_RATE, 60);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
	Log::debug("Refresh rate should be " + to_string(v->refreshRate) + "hz");
	GLFWwindow* e;
	
	e =  glfwCreateWindow(v->width, v->height, "Bang", NULL, NULL);
	if (!e)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(e);
    glfwSetKeyCallback(e, key_callback);
    glfwSetMouseButtonCallback(e, mouse_button_callback);
    glfwSetWindowSizeCallback(e, window_size_callback);
	glfwSetScrollCallback(e, scroll_callback);
	glfwSetWindowFocusCallback(e, window_focus_callback);
	
	return e;
}
void resetMatrix() {
	int iwidth, iheight;
	glfwGetFramebufferSize(window, &iwidth, &iheight);
	windowRatio = (float) iwidth / (float) iheight;
	windowInverse = (float)iheight / (float)iwidth;
	glViewport(0, 0, iwidth, iheight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//glOrtho(-getHBorder(), getHBorder(), -getYBorder(), getYBorder(), 1.f, -1.f);
	//glOrtho(-baseWindowRatio, baseWindowRatio, -baseWindowRatio, baseWindowRatio, 1.f, -1.f);

	width = getHBorder()*2.f;
	height = getYBorder()*2.f;

	pixelWidth = (float)iwidth;
	pixelHeight = (float)iheight;

	if(windowRatio > baseWindowRatio) {
		xStretch = getHBorder();
		yStretch = 1.f;
	} else {
		xStretch = baseWindowRatio;
		yStretch = getYBorder();
	}
	setFloat("xStretch", xStretch);
	setFloat("yStretch", yStretch);
	
	if(screenshottingNextFrame) {
		setMat4("transMat", glm::scale(glm::vec3(-1.f, 1.f, 1.f)));
	} else {
		setMat4("transMat", glm::mat4(1.f));
	}
	
	glUseProgram(shaderProgram);
}
void imguiInit() {
	// https://marcelfischer.eu/blog/2019/imgui-in-sdl-opengl/
	
	IMGUI_CHECKVERSION();
	
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	
	ImGui::StyleColorsDark();
	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
}
bool isScreenshotting = false;
#define incrementLoop() i++;if(i>=argc) break;
void printHelp() {
	cout << "Dominion Command Line Card Creator" << endl <<
			"This program is used to generate cards for Dominion, by Donald X. Vaccarino." << endl <<
			"------- COPYRIGHT NOTICE -------" << endl <<
			"All designs of cards are property of Donald X. Vaccarino and Rio Grande Games." << endl <<
			"-- Other Notes --" << endl <<
			"All designs of cards are takes from shardofhonor's similar card creator, found at https://shardofhonor.github.io/dominion-card-generator/" << endl <<
			"This program uses Curl for downloading images. Use --curl-license for curl's license." << endl <<
			"Unless specified with --high-res, all images (excluding the coin, debt, and VP token) are rendered at half resolution." << endl << 
			"If any argument specifies a description of the card, it will automatically take a screenshot. To enter the editor, use no arguments specifying the description." << endl << endl <<
			"If a money amount is empty, try putting a backslash before the dollar signs so they are not intreted as shell variables." << endl <<
			"---- ARGUMENTS ----" << endl <<
			//"--cli\t\t\tEnter alternate CLI" << endl <<
			"--title\t\t\tSelect title of card" << endl <<
			"--type\t\t\tSelect type of card. Use --list-types to see avalible types." << endl <<
			"--cost\t\t\tSelect cost of card. Use --legend for symbols." << endl <<
			"--preview\t\tSelect preview of card (This is the symbol in the top left and right corners of the card). Use --legend for symbols." << endl <<
			"--preview\t\tSelect preview of card (This is the symbol in the top left and right corners of the card). Use --legend for symbols." << endl <<
			"--url\t\t\tSelect url of picture of card. Note: Uses curl to download images." << endl <<
			"--expansion-url\tSelect url of expansion of card. (The bottom right icon) Note: Uses curl to download images." << endl <<
			"--author\t\tSelect author and version card." << endl <<
			"--illustration-credit\tSelect illustration credit for card." << endl <<
			"--primary-color\t\tSelect primary color for card. Use --list-colors to see avalible colors." << endl <<
			"--secondary-color\tSelect secondary color for card. If unspecified, the primary color will be used. Use --list-colors to see avalible colors." << endl <<
			"--text\t\t\tSelect card text. See --legend for symbols." << endl << 
			"--layout\t\t\tSelect card layout. See --layouts for a list of layouts." << endl << 
			"--font-size\t\tSelect font size. Default is 144, using over 500 will result in warning." << endl << 
			"";
}
void printCurlLicense() {
	cout << "Curl (found at https://curl.se) license. This license can be found at https://curl.se/docs/copyright.html" << endl << "\
\
COPYRIGHT AND PERMISSION NOTICE\n\
\n\
Copyright (c) 1996 - 2022, Daniel Stenberg, daniel@haxx.se, and many contributors, see the THANKS file.\n\
\n\
All rights reserved.\n\
\n\
Permission to use, copy, modify, and distribute this software for any purpose with or without fee is hereby granted, provided that the above copyright notice and this permission notice appear in all copies.\n\
\n\
THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.\n\
\n\
Except as contained in this notice, the name of a copyright holder shall not be used in advertising or otherwise to promote the sale, use or other dealings in this Software without prior written authorization of the copyright holder." << endl;
}
void printColors() {
	for(int i = 0; i < 18; i++) {
		cout << i+1 << "." << (i < 9 ? "  " : " ") << mainChoices[i] << endl;
	}
}
void printLegend() {
	cout << "--- LEGEND ---" << endl <<
			"$\tMoney symbol (Ya know, the coin)" << endl <<
			"@\tDebt symbol" << endl <<
			"^\tPotion symbol" << endl <<
			"%\tVictory point symbol" << endl;
}
void printLayouts() {
	cout << "--- Dominion Card Layouts ---" << endl <<
			"0\tNormal type of card. Used by Actions, Reactions, etc." << endl <<
			"1\tLandscape/horizontal card. Used by Events, Projects, etc." << endl <<
			"2\tBase card. Used by Copper, Silver, Estate, Province, Curse, etc." << endl <<
			"";
}
int doSingleArgument(string arg, string next, int* i) { // Trust me on this one
	
	return 0;
}
string ReplaceAll(string str, const string& from, const string& to) { // https://stackoverflow.com/a/24315631
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}
int main(int argc, char *argv[]) {
	showWindow = false;
	bool isCli = false;
	for(int i = 1; i < argc; i++) { // Lengthy, but it works, so I don't care
		if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			printHelp();
			return 0;
		} else if(strcmp(argv[i], "--high-res") == 0) {
			cout << "Using higher resolution images, expect lower performance and longer load times." << endl;
			isLowRes = false;
		} else if(strcmp(argv[i], "--title") == 0) {
			incrementLoop();
			cardTitle = argv[i];
			cout << "Using \"" << cardTitle << "\" as title." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--type") == 0) {
			incrementLoop();
			cardType = argv[i];
			cout << "Using \"" << cardType << "\" as type." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--cost") == 0) {
			incrementLoop();
			cardCost = argv[i];
			cout << "Using \"" << cardCost << "\" as cost." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--url") == 0) {
			incrementLoop();
			iconUrl = argv[i];
			cout << "Loading \"" << iconUrl << "\" as image." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--expansion-url") == 0) {
			incrementLoop();
			expansionUrl = argv[i];
			cout << "Loading \"" << expansionUrl << "\" as expansion icon." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--primary-color") == 0) {
			incrementLoop();
			cardColor = getChoice(argv[i]);
			cout << "Using \"" << mainChoices[cardColor] << "\" as primary color." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--secondary-color") == 0) {
			incrementLoop();
			cardSecondary = getChoice(argv[i])+1;
			cout << cardSecondary << endl;
			cout << "Using \"" << secondaryChoices[cardSecondary] << "\" as secondary color." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--text") == 0) {
			incrementLoop();
			cardText = argv[i];
			cout << "Card text acquired." << endl;
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--illustration-credit") == 0) {
			incrementLoop();
			cardCredit = argv[i];
			cout << "Using \"" << cardCredit << "\" as illustration credit." << endl;
			
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--author") == 0) {
			incrementLoop();
			cardVersion = argv[i];
			cout << "Using \"" << cardVersion << "\" as version and author credit." << endl;
			
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--preview") == 0) {
			incrementLoop();
			cardPreview = argv[i];
			cout << "Using \"" << cardVersion << "\" as preview." << endl;
			
			isScreenshotting = true;
		} else if(strcmp(argv[i], "--legend") == 0) {
			printLegend();
			return 0;
		} else if(strcmp(argv[i], "--font-size") == 0) {
			incrementLoop();
			fontSize = stoi(argv[i]);
			if(fontSize == 0) {
				Log::warning("Font size was zero or unrecognisable, resetting to 144.");
				fontSize = 144;
			}
			if(fontSize > 500) {
				Log::warning("Font size is over 500! Expect longer render and load times.");
			}
		} else if(strcmp(argv[i], "--curl-license") == 0) {
			printCurlLicense();
			return 0;
		} else if(strcmp(argv[i], "--list-colors") == 0) {
			printColors();
			return 0;
		} else if(strcmp(argv[i], "--list-types") == 0) {
			printColors();
			return 0;
		}
	}
	
	for(int i = 0; i < 512; i++) {
		if(i == 1) {
			char* cardText    = (char*)malloc(512);
			char* iconUrl     = (char*)malloc(512);
			char* expansionUrl= (char*)malloc(512);
		}
		if(i < 32) {
			cardCost[i] = '\0';
			cardPreview[i] = '\0';
		}
		if(i < 128) {
			cardTitle[i] = '\0';
			cardType[i] = '\0';
			cardCredit[i] = '\0';
			cardVersion[i] = '\0';
		}
		cardText[i] = '\0';
		iconUrl[i] = '\0';
		expansionUrl[i] = '\0';
	}
	
	vector<string> splitted = split(getPathToExe(), "/");
	pathPrefix = "./";
	for(int i = 0; i < splitted.size(); i++) {
		//pathPrefix += splitted[i] + "/";
	}
	Log::log("Path prefix should be " + pathPrefix);
	
	int logReturn = Log::loggerInit();
	if(logReturn != 0) {
		cout << "Logger init failed!" << endl;
	}
	
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    const GLFWvidmode* v = glfwGetVideoMode(glfwGetPrimaryMonitor());
    window = createWindow(false);
    

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        cout << "Failed to initialize OpenGL context! Exiting" << std::endl;
        return -1;
    }
	
	if(!isScreenshotting) imguiInit();
	
   	textInit();
	loadFont("cinzel.ttf", "trajan");
	loadFont("cinzel-bold.ttf", "trajanb");
	loadFont("tnr-bold.ttf", "tnrb");
	loadFont("tnr.ttf", "tnr");
	loadFont("tnri.ttf", "tnri");
	
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	
   	res::initial();
	
	utilsSetup();
	
	textInit();
	
	handOnLoad();

	Saves::init();
	Saves::read();

	loadIcon(string(iconUrl), "./tempicon.png", &(res::tempIcon), true);
	loadIcon(string(expansionUrl), "./expansionicon.png", &(res::tempExpansionIcon), true);
	
	glfwSwapInterval(1);

	//changeFullscreen();
	
	float deltaFloat = 0.f;
	
	if(isScreenshotting) {
		loadIcon(string(iconUrl), "./tempicon.png", &(res::tempIcon), false);
		loadIcon(string(expansionUrl), "./expansionicon.png", &(res::tempExpansionIcon), false);
		calculateFPS();
      enablings();
		for(int i = 0; i < 10; i++) {
			resetMatrix();
			update();
			handlerDraw();
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
		screenShot();
		return 0;
	}
	
	glfwShowWindow(window);
	
    while (!glfwWindowShouldClose(window))
    {
		/*if(!isWindowFocused) {
			glfwPollEvents();
			continue;
		}*/
        calculateFPS();
        enablings();
		
		resetMatrix();
		
		deltaFloat = glfwGetTime();
        if(!isPausedMaster) {
			update();
			updateDelta = (glfwGetTime()-deltaFloat)*1000;
			updateDeltaAdd += updateDelta;
		} else {
			updateDelta = 0.f;
		}
		
		deltaFloat = glfwGetTime();
		if(isWindowFocused) {
			draw();
			drawDelta = (glfwGetTime() - deltaFloat)*1000;
			drawDeltaAdd += drawDelta;
		} else {
			drawDelta = 0.f;
		}
		
		delta = abs(glfwGetTime()-timeOfLastFrame);
        timeOfLastFrame = glfwGetTime();
		
		int timeToSleep = ((1.f/targetFixedFps)-delta)*1000;
		this_thread::sleep_for((chrono::milliseconds)timeToSleep);
		
        glfwSwapBuffers(window);
        glfwPollEvents();

		if(screenshottingNextFrame) {
			copyToClipboard();
		}
    }
	
	bangExit();
    return 0;
}

#ifdef _WIN64
int WinMain() {
	main(0, nullptr);
}
#endif
