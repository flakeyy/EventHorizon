#define FC2_TEAM_UNIVERSE4
#include <chrono>
#include <vector>
#include <iostream>
#include <windows.h>
#include <unistd.h>
#include <thread>
#include "json.hpp"
#include "dx11.h"
#include "fc2.hpp"


using std::vector;
using std::string;
using std::thread;
using std::chrono::seconds;
using std::chrono::days;
using std::chrono::day;
using std::chrono::month;
using std::chrono::year;
using std::chrono::system_clock;
using std::chrono::year_month_day;
using std::chrono::high_resolution_clock;
using json = nlohmann::json;

// utility vars
auto session = fc2::get_session();
bool ranFirstTimeChecks = false, ready = true, asyncFinished = false, loadingFinished = false, scriptChangesMade = false, projectChangesMade = false;
const char* themes[] = {"Event Horizon","Classic", "2003 Steam", "Comfy", "Dark", "Future Dark", "Cherry"};
static const char* themeSelected = themes[0];
const char* dateFormats[] = {"MM/DD/YYYY", "DD/MM/YYYY"};
static const char* dateFormatSelected = dateFormats[0];
static int scriptSelectedIndex = 0;
static seconds duration(3);
static auto startTimer = std::chrono::high_resolution_clock::now(), endTimer = startTimer + duration;

// information vars
// some of these could be const but who really needs them to be
static string astrology, protection, session_directory, level;
static int xp, perk_points, posts, score, unread_conversations, unread_alerts, session_started, session_expiry;
year_month_day session_start_ymd, session_expiry_ymd;

// perks
static int perks_amount, owned_perks_amount, perk_purchased;
static string roll_api_message;
bool lootRolled;
vector<int> perk_ids, owned_perk_ids;
vector<string> perk_names, perk_descriptions;
vector<bool> perk_is_owned;

// scripts
static int scripts_amount, enabled_scripts_amount;
vector<string> script_names, script_ids, script_authors, script_last_updated, script_update_notes, enabled_script_ids;
vector<year_month_day> script_last_updated_ymd;
vector<bool> script_is_active;

// fc2t projects
static int projects_amount, enabled_projects_amount;
vector<string> project_names, project_authors;
vector<int> project_ids, project_last_updated, enabled_project_ids;
vector<year_month_day> project_last_updated_ymd;
vector<bool> project_is_active;

// team scripts
static int team_scripts_amount;
vector<string> team_script_names, team_script_ids;

// configuration
static string configuration;

// steam
static int steam_amount;
string steam_api_response;
vector<string> steam_names, steam_personas;

// function declarations
void renderGeneralTab(), renderScriptsTab(), renderFC2TTab(), renderTeamsTab(), renderConfigurationTab(), renderSteamTab(), renderPerksTab(), renderSettingsTab(),
showApplyButton(), refreshCache(), asyncCacheTasks(), updateActiveScripts(), updateActiveProjects();
string getYMDAsFormatted(day day, month month, year year);

void setStyleEventHorizon() {
  // Event Horizon style by flakey from ImThemes
  ImGuiStyle& style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(6.0f, 3.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(5.0f, 1.0f);
  style.FrameRounding = 2.0f;
  style.FrameBorderSize = 1.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 13.0f;
  style.ScrollbarRounding = 10.0f;
  style.GrabMinSize = 20.0f;
  style.GrabRounding = 2.0f;
  style.TabRounding = 1.0f;
  style.TabBorderSize = 1.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.8799999952316284f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.2784313857555389f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05490196123719215f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2156862765550613f, 0.9019607901573181f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.8470588326454163f, 0.6627451181411743f, 0.1176470592617989f, 0.1607843190431595f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.125490203499794f, 0.125490203499794f, 0.125490203499794f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.3921568691730499f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.7843137383460999f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2313725501298904f, 0.2000000029802322f, 0.2705882489681244f, 0.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 0.75f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 0.4699999988079071f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2431372553110123f, 0.2431372553110123f, 0.2352941185235977f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09803921729326248f, 0.1490196138620377f, 0.1568627506494522f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.6274510025978088f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.6274510025978088f, 0.6274510025978088f, 0.6274510025978088f, 0.7843137383460999f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6274510025978088f, 0.6274510025978088f, 0.6274510025978088f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.6274510025978088f, 0.6274510025978088f, 0.6274510025978088f, 0.1411764770746231f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.8588235378265381f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.7607843279838562f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.8588235378265381f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.6274510025978088f, 0.6274510025978088f, 0.6274510025978088f, 0.7803921699523926f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.6274510025978088f, 0.6274510025978088f, 0.6274510025978088f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.6274510025978088f, 0.6274510025978088f, 0.6274510025978088f, 0.03921568766236305f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.7803921699523926f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.8627451062202454f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.6666666865348816f, 0.4509803950786591f, 0.09803921729326248f, 0.9215686321258545f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.6666666865348816f, 0.4509803950786591f, 0.09803921729326248f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.6299999952316284f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 0.6299999952316284f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.5803921818733215f, 0.3725490272045135f, 0.05882352963089943f, 0.4313725531101227f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}
void setStyleClassic() {
  // Classic style by ocornut from ImThemes
  ImGuiStyle& style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(0.8980392217636108f, 0.8980392217636108f, 0.8980392217636108f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8500000238418579f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1098039224743843f, 0.1098039224743843f, 0.1372549086809158f, 0.9200000166893005f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4274509847164154f, 0.3899999856948853f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4666666686534882f, 0.4666666686534882f, 0.686274528503418f, 0.4000000059604645f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4196078479290009f, 0.407843142747879f, 0.6392157077789307f, 0.6899999976158142f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2666666805744171f, 0.2666666805744171f, 0.5372549295425415f, 0.8299999833106995f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.3176470696926117f, 0.3176470696926117f, 0.6274510025978088f, 0.8700000047683716f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.5490196347236633f, 0.800000011920929f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2000000029802322f, 0.2470588237047195f, 0.2980392277240753f, 0.6000000238418579f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.800000011920929f, 0.300000011920929f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.800000011920929f, 0.4000000059604645f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.407843142747879f, 0.3882353007793427f, 0.800000011920929f, 0.6000000238418579f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.8980392217636108f, 0.8980392217636108f, 0.8980392217636108f, 0.5f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 1.0f, 1.0f, 0.300000011920929f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.407843142747879f, 0.3882353007793427f, 0.800000011920929f, 0.6000000238418579f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.3490196168422699f, 0.4000000059604645f, 0.6078431606292725f, 0.6200000047683716f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4000000059604645f, 0.47843137383461f, 0.7098039388656616f, 0.7900000214576721f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4588235318660736f, 0.5372549295425415f, 0.800000011920929f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.8980392217636108f, 0.449999988079071f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.4470588266849518f, 0.4470588266849518f, 0.8980392217636108f, 0.800000011920929f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.529411792755127f, 0.529411792755127f, 0.8666666746139526f, 0.800000011920929f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 0.6000000238418579f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6980392336845398f, 1.0f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.8980392217636108f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.1000000014901161f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.7764706015586853f, 0.8196078538894653f, 1.0f, 0.6000000238418579f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.7764706015586853f, 0.8196078538894653f, 1.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.3333333432674408f, 0.3333333432674408f, 0.6823529601097107f, 0.7860000133514404f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.4470588266849518f, 0.4470588266849518f, 0.8980392217636108f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.4039215743541718f, 0.4039215743541718f, 0.7254902124404907f, 0.8420000076293945f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.2823529541492462f, 0.2823529541492462f, 0.5686274766921997f, 0.8212000131607056f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3490196168422699f, 0.3490196168422699f, 0.6509804129600525f, 0.8371999859809875f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.2666666805744171f, 0.2666666805744171f, 0.3764705955982208f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.4470588266849518f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2588235437870026f, 0.2588235437870026f, 0.2784313857555389f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.07000000029802322f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.0f, 1.0f, 0.3499999940395355f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4470588266849518f, 0.4470588266849518f, 0.8980392217636108f, 0.800000011920929f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.2000000029802322f, 0.2000000029802322f, 0.2000000029802322f, 0.3499999940395355f);
}
void setStyleClassicSteam() {
  // Classic Steam style by metasprite from ImThemes
  ImGuiStyle& style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 1.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 0.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 0.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2862745225429535f, 0.3372549116611481f, 0.2588235437870026f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.2862745225429535f, 0.3372549116611481f, 0.2588235437870026f, 1.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.1372549086809158f, 0.1568627506494522f, 0.1098039224743843f, 0.5199999809265137f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2666666805744171f, 0.2980392277240753f, 0.2274509817361832f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2980392277240753f, 0.3372549116611481f, 0.2588235437870026f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2862745225429535f, 0.3372549116611481f, 0.2588235437870026f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.2784313857555389f, 0.3176470696926117f, 0.239215686917305f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2470588237047195f, 0.2980392277240753f, 0.2196078449487686f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.2274509817361832f, 0.2666666805744171f, 0.2078431397676468f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.5f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.2862745225429535f, 0.3372549116611481f, 0.2588235437870026f, 0.4000000059604645f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.5f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 0.6000000238418579f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.5f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.1372549086809158f, 0.1568627506494522f, 0.1098039224743843f, 1.0f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1882352977991104f, 0.2274509817361832f, 0.1764705926179886f, 0.0f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.5372549295425415f, 0.5686274766921997f, 0.5098039507865906f, 0.7799999713897705f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.239215686917305f, 0.2666666805744171f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3490196168422699f, 0.4196078479290009f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.7764706015586853f, 0.2784313857555389f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.729411780834198f, 0.6666666865348816f, 0.239215686917305f, 1.0f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.5882353186607361f, 0.5372549295425415f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}
void setStyleComfy() {
  // Comfy style by Giuseppe from ImThemes
  ImGuiStyle& style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.1000000014901161f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 10.0f;
  style.WindowBorderSize = 0.0f;
  style.WindowMinSize = ImVec2(30.0f, 30.0f);
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Right;
  style.ChildRounding = 5.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 10.0f;
  style.PopupBorderSize = 0.0f;
  style.FramePadding = ImVec2(5.0f, 3.5f);
  style.FrameRounding = 5.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(5.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(5.0f, 5.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 5.0f;
  style.ColumnsMinSpacing = 5.0f;
  style.ScrollbarSize = 15.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 15.0f;
  style.GrabRounding = 5.0f;
  style.TabRounding = 5.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(1.0f, 1.0f, 1.0f, 0.3605149984359741f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.4235294163227081f, 0.3803921639919281f, 0.572549045085907f, 0.54935622215271f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3803921639919281f, 0.4235294163227081f, 0.572549045085907f, 0.5490196347236633f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.09803921729326248f, 0.09803921729326248f, 0.09803921729326248f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2588235437870026f, 0.2588235437870026f, 0.2588235437870026f, 0.0f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 0.0f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.1568627506494522f, 0.1568627506494522f, 0.1568627506494522f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.2352941185235977f, 0.2352941185235977f, 0.2352941185235977f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.294117659330368f, 0.294117659330368f, 0.294117659330368f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.294117659330368f, 0.294117659330368f, 0.294117659330368f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8156862854957581f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.7372549176216125f, 0.6941176652908325f, 0.886274516582489f, 0.5490196347236633f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8156862854957581f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.7372549176216125f, 0.6941176652908325f, 0.886274516582489f, 0.5490196347236633f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.8156862854957581f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7372549176216125f, 0.6941176652908325f, 0.886274516582489f, 0.5490196347236633f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.8156862854957581f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.7372549176216125f, 0.6941176652908325f, 0.886274516582489f, 0.5490196347236633f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.8156862854957581f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.7372549176216125f, 0.6941176652908325f, 0.886274516582489f, 0.5490196347236633f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.8156862854957581f, 0.772549033164978f, 0.9647058844566345f, 0.5490196347236633f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0f, 0.4509803950786591f, 1.0f, 0.0f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 0.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.294117659330368f, 0.294117659330368f, 0.294117659330368f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.7372549176216125f, 0.6941176652908325f, 0.886274516582489f, 0.5490196347236633f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.6196078658103943f, 0.5764706134796143f, 0.7686274647712708f, 0.5490196347236633f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.7372549176216125f, 0.6941176652908325f, 0.886274516582489f, 0.5490196347236633f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.4235294163227081f, 0.3803921639919281f, 0.572549045085907f, 0.5490196347236633f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.4235294163227081f, 0.3803921639919281f, 0.572549045085907f, 0.2918455004692078f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03433477878570557f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.7372549176216125f, 0.6941176652908325f, 0.886274516582489f, 0.5490196347236633f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}
void setStyleDark() {
  // Dark style by dougbinks from ImThemes
  ImGuiStyle& style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.6000000238418579f;
  style.WindowPadding = ImVec2(8.0f, 8.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 1.0f;
  style.WindowMinSize = ImVec2(32.0f, 32.0f);
  style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(4.0f, 3.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(8.0f, 4.0f);
  style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
  style.CellPadding = ImVec2(4.0f, 2.0f);
  style.IndentSpacing = 21.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 14.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 10.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 0.9399999976158142f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1568627506494522f, 0.2862745225429535f, 0.47843137383461f, 0.5400000214576721f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.4000000059604645f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.6700000166893005f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.1568627506494522f, 0.2862745225429535f, 0.47843137383461f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.239215686917305f, 0.5176470875740051f, 0.8784313797950745f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.4000000059604645f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.05882352963089943f, 0.529411792755127f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3100000023841858f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.6700000166893005f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.949999988079071f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 0.8619999885559082f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.3499999940395355f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}
void setStyleFutureDark() {
  // Future Dark style by rewrking from ImThemes
  ImGuiStyle& style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 1.0f;
  style.WindowPadding = ImVec2(12.0f, 12.0f);
  style.WindowRounding = 0.0f;
  style.WindowBorderSize = 0.0f;
  style.WindowMinSize = ImVec2(20.0f, 20.0f);
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 0.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(6.0f, 6.0f);
  style.FrameRounding = 0.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(12.0f, 6.0f);
  style.ItemInnerSpacing = ImVec2(6.0f, 3.0f);
  style.CellPadding = ImVec2(12.0f, 6.0f);
  style.IndentSpacing = 20.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 12.0f;
  style.ScrollbarRounding = 0.0f;
  style.GrabMinSize = 12.0f;
  style.GrabRounding = 0.0f;
  style.TabRounding = 0.0f;
  style.TabBorderSize = 0.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5372549295425415f, 0.5529412031173706f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.2901960909366608f, 0.5960784554481506f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.9960784316062927f, 0.4745098054409027f, 0.6980392336845398f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
}
void setStyleCherry() {
  // Soft Cherry style by Patitotective from ImThemes
  ImGuiStyle& style = ImGui::GetStyle();

  style.Alpha = 1.0f;
  style.DisabledAlpha = 0.4000000059604645f;
  style.WindowPadding = ImVec2(10.0f, 10.0f);
  style.WindowRounding = 4.0f;
  style.WindowBorderSize = 0.0f;
  style.WindowMinSize = ImVec2(50.0f, 50.0f);
  style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
  style.WindowMenuButtonPosition = ImGuiDir_Left;
  style.ChildRounding = 0.0f;
  style.ChildBorderSize = 1.0f;
  style.PopupRounding = 1.0f;
  style.PopupBorderSize = 1.0f;
  style.FramePadding = ImVec2(5.0f, 3.0f);
  style.FrameRounding = 3.0f;
  style.FrameBorderSize = 0.0f;
  style.ItemSpacing = ImVec2(6.0f, 6.0f);
  style.ItemInnerSpacing = ImVec2(3.0f, 2.0f);
  style.CellPadding = ImVec2(3.0f, 3.0f);
  style.IndentSpacing = 6.0f;
  style.ColumnsMinSpacing = 6.0f;
  style.ScrollbarSize = 13.0f;
  style.ScrollbarRounding = 16.0f;
  style.GrabMinSize = 20.0f;
  style.GrabRounding = 4.0f;
  style.TabRounding = 4.0f;
  style.TabBorderSize = 1.0f;
  style.TabMinWidthForCloseButton = 0.0f;
  style.ColorButtonPosition = ImGuiDir_Right;
  style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
  style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

  style.Colors[ImGuiCol_Text] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 1.0f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.5215686559677124f, 0.5490196347236633f, 0.5333333611488342f, 1.0f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1294117718935013f, 0.1372549086809158f, 0.168627455830574f, 1.0f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.1490196138620377f, 0.1568627506494522f, 0.1882352977991104f, 1.0f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.1372549086809158f, 0.1137254908680916f, 0.1333333402872086f, 1.0f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.168627455830574f, 0.1843137294054031f, 0.2313725501298904f, 1.0f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.2313725501298904f, 0.2000000029802322f, 0.2705882489681244f, 1.0f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 1.0f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.2000000029802322f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.239215686917305f, 0.239215686917305f, 0.2196078449487686f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3882353007793427f, 0.3882353007793427f, 0.3725490272045135f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.6941176652908325f, 0.6941176652908325f, 0.686274528503418f, 1.0f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6941176652908325f, 0.6941176652908325f, 0.686274528503418f, 1.0f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.658823549747467f, 0.1372549086809158f, 0.1764705926179886f, 1.0f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.6509804129600525f, 0.1490196138620377f, 0.3450980484485626f, 1.0f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.7098039388656616f, 0.2196078449487686f, 0.2666666805744171f, 1.0f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.6509804129600525f, 0.1490196138620377f, 0.3450980484485626f, 1.0f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.6509804129600525f, 0.1490196138620377f, 0.3450980484485626f, 1.0f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.501960813999176f, 0.07450980693101883f, 0.2549019753932953f, 1.0f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 1.0f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.6509804129600525f, 0.1490196138620377f, 0.3450980484485626f, 1.0f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 1.0f);
  style.Colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 1.0f);
  style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.8588235378265381f, 0.929411768913269f, 0.886274516582489f, 1.0f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.3098039329051971f, 0.7764706015586853f, 0.196078434586525f, 1.0f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.4549019634723663f, 0.196078434586525f, 0.2980392277240753f, 1.0f);
  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
  style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.3843137323856354f, 0.6274510025978088f, 0.9176470637321472f, 1.0f);
  style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
  style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
  style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 1.0f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.300000011920929f);
}
void selectStyle(int styleIndex) {
  switch(styleIndex) {
    case 0:
      setStyleEventHorizon();
      break;
    case 1:
      setStyleClassic();
      break;
    case 2:
      setStyleClassicSteam();
      break;
    case 3:
      setStyleComfy();
      break;
    case 4:
      setStyleDark();
      break;
    case 5:
      setStyleFutureDark();
      break;
    case 6:
      setStyleCherry();
      break;
    default:
      setStyleEventHorizon();
      break;
  }
}

void renderGeneralTab() {
  ImGui::Text( "Hello, %s!", session.username);

  ImGui::SeparatorText("User Information");
  ImGui::Text("XP: %i", xp);
  ImGui::Text("Perk Points %i", perk_points);
  ImGui::Text("Posts: %i", posts);
  ImGui::Text("Reaction Score: %i", score);
  ImGui::Text("Unread Conversations: %i", unread_conversations);
  ImGui::Text("Unread Alerts: %i", unread_alerts);
  ImGui::Text("Level: %s", level.c_str());
  ImGui::Text("Astrology: %s", astrology.c_str());
  ImGui::Text("Protection: %s", protection.c_str());

  ImGui::SeparatorText("Session");
  ImGui::Text("Session Directory: %s", session_directory.c_str());
  ImGui::Text("Session Started: %s", getYMDAsFormatted(session_start_ymd.day(), session_start_ymd.month(), session_start_ymd.year()).c_str());
  ImGui::Text("Session Expiry: %s", getYMDAsFormatted(session_expiry_ymd.day(), session_expiry_ymd.month(), session_expiry_ymd.year()).c_str());

  if(!scriptChangesMade) {
    ImGui::SeparatorText("Active Cloud Scripts");
  }
  else {
    ImGui::SeparatorText("Active Cloud Scripts *");
  }

  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching cloud scripts...");
  }
  else {
    bool anyScriptsEnabled = false;
    for (int i = 0; i < scripts_amount; i++) {
      if (!script_is_active.at(i)) {
        continue;
      }
      anyScriptsEnabled = true;
      if (ImGui::CollapsingHeader(script_names.at(i).c_str(), ImGuiTreeNodeFlags_None)) {
        ImGui::Text("ID: %s", script_ids.at(i).c_str());
        ImGui::Text("Author: %s", script_authors.at(i).c_str());
        ImGui::Text("Last Update: %s", getYMDAsFormatted(script_last_updated_ymd.at(i).day(), script_last_updated_ymd.at(i).month(), script_last_updated_ymd.at(i).year()).c_str());
        ImGui::Text("Update Notes: %s", script_update_notes.at(i).c_str());
      }
    }
    if(!anyScriptsEnabled) {
      ImGui::Text("You have no cloud scripts enabled.");
    }
  }

  if(!projectChangesMade) {
    ImGui::SeparatorText("Active FC2T Projects");
  }
  else {
    ImGui::SeparatorText("Active FC2T Projects *");
  }

  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching FC2T projects...");
  }
  else {
    bool anyProjectsEnabled = false;
    for (int i = 0; i < projects_amount; i++) {
      if (!project_is_active.at(i)) {
        continue;
      }
      anyProjectsEnabled = true;
      if (ImGui::CollapsingHeader(project_names.at(i).c_str(), ImGuiTreeNodeFlags_None)) {
        ImGui::Text("ID: %i", project_ids.at(i));
        ImGui::Text("Author: %s", project_authors.at(i).c_str());
        ImGui::Text("Last Update: %s", getYMDAsFormatted(project_last_updated_ymd.at(i).day(), project_last_updated_ymd.at(i).month(), project_last_updated_ymd.at(i).year()).c_str());
      }
    }
    if(!anyProjectsEnabled) {
      ImGui::Text("You have no FC2T projects enabled.");
    }
  }

  ImGui::SeparatorText("Abundance of Jupiter");
  json apiResponse;
  if(high_resolution_clock::now() > endTimer) {
    lootRolled = false;
    roll_api_message = "";
  }
  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching perks...");
  }
  else {
    // getting the perk by a hardcoded index is extremely lazy and probably will break eventually. sucks.
    if(perk_is_owned.at(10)) {
      if(ImGui::Button("Roll Loot")) {
        startTimer = high_resolution_clock::now();
        endTimer = startTimer + duration;
        string apiCall = "rollLoot";
        apiResponse = json::parse(fc2::api(apiCall));
        roll_api_message = apiResponse.at("message").get<string>();
        printf("%s\n", apiResponse.dump().c_str());
        lootRolled = true;
      }
    }
    else {
      ImGui::Text("You do not own the \"Abundance of Jupiter\" perk. See the perks tab to buy it.");
    }
  }
  if(lootRolled) {
    ImGui::Text("%s", roll_api_message.c_str());
  }
}
void renderScriptsTab() {
  bool anyScriptsEnabled = false;
  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching cloud scripts...");
    return;
  }

  if(!scriptChangesMade) {
    ImGui::SeparatorText("Active Cloud Scripts");
  }
  else {
    ImGui::SeparatorText("Active Cloud Scripts *");
  }

  for(int i = 0; i < scripts_amount; i++) {
    if(!script_is_active.at(i)) {
      continue;
    }
    anyScriptsEnabled = true;
    string label = script_names.at(i) + "##active_scripts";
    if(ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None)) {
      bool is_active = script_is_active.at(i);
      ImGui::Text("ID: %s", script_ids.at(i).c_str());
      ImGui::Text("Author: %s", script_authors.at(i).c_str());
      ImGui::Text("Last Update: %s", getYMDAsFormatted(script_last_updated_ymd.at(i).day(), script_last_updated_ymd.at(i).month(), script_last_updated_ymd.at(i).year()).c_str());
      ImGui::Text("Update Notes: %s", script_update_notes.at(i).c_str());
      ImGui::Checkbox("Enabled##active_scripts", &is_active);
      if(is_active != script_is_active.at(i)) {
        scriptChangesMade = true;
        script_is_active.at(i) = is_active;
      }
    }
  }

  if(!anyScriptsEnabled) {
    ImGui::Text("You have no cloud scripts enabled.");
  }

  if(scriptChangesMade) {
    showApplyButton();
  }

  ImGui::SeparatorText("All Cloud Scripts");

  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching cloud scripts...");
  }
  else {
    for (int i = 0; i < scripts_amount; i++) {
      string label = script_names.at(i) + "##all_scripts";
      if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None)) {
        bool is_active = script_is_active.at(i);
        ImGui::Text("ID: %s", script_ids.at(i).c_str());
        ImGui::Text("Author: %s", script_authors.at(i).c_str());
        ImGui::Text("Last Update: %s", getYMDAsFormatted(script_last_updated_ymd.at(i).day(), script_last_updated_ymd.at(i).month(), script_last_updated_ymd.at(i).year()).c_str());
        ImGui::Text("Update Notes: %s", script_update_notes.at(i).c_str());
        ImGui::Checkbox("Enabled##all_scripts", &is_active);
        if(is_active != script_is_active.at(i)) {
          scriptChangesMade = true;
          script_is_active.at(i) = is_active;
        }
      }
    }
  }
}
void renderFC2TTab() {
  bool anyProjectsEnabled = false;
  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching FC2T projects...");
    return;
  }

  if(!projectChangesMade) {
    ImGui::SeparatorText("Active FC2T Projects");
  }
  else {
    ImGui::SeparatorText("Active FC2T Projects *");
  }

  for(int i = 0; i < projects_amount; i++) {
    if(!project_is_active.at(i)) {
      continue;
    }
    anyProjectsEnabled = true;
    string label = project_names.at(i) + "##active_projects";
    if(ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None)) {
      bool is_active = project_is_active.at(i);
      ImGui::Text("ID: %i", project_ids.at(i));
      ImGui::Text("Author: %s", project_authors.at(i).c_str());
      ImGui::Text("Last Update: %s", getYMDAsFormatted(project_last_updated_ymd.at(i).day(), project_last_updated_ymd.at(i).month(), project_last_updated_ymd.at(i).year()).c_str());
      ImGui::Checkbox("Enabled##active_projects", &is_active);
      if(is_active != project_is_active.at(i)) {
        projectChangesMade = true;
        project_is_active.at(i) = is_active;
      }
    }
  }

  if(!anyProjectsEnabled) {
    ImGui::Text("You have no FC2T projects enabled.");
  }

  if(projectChangesMade) {
    showApplyButton();
  }

  ImGui::SeparatorText("All FC2T Projects");

  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching FC2T projects...");
  }
  else {
    for (int i = 0; i < projects_amount; i++) {
      string label = project_names.at(i) + "##all_projects";
      if (ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None)) {
        bool is_active = project_is_active.at(i);
        ImGui::Text("ID: %i", project_ids.at(i));
        ImGui::Text("Author: %s", project_authors.at(i).c_str());
        ImGui::Text("Last Update: %s", getYMDAsFormatted(project_last_updated_ymd.at(i).day(), project_last_updated_ymd.at(i).month(), project_last_updated_ymd.at(i).year()).c_str());
        ImGui::Checkbox("Enabled##all_projects", &is_active);
        if(is_active != project_is_active.at(i)) {
          projectChangesMade = true;
          project_is_active.at(i) = is_active;
        }
      }
    }
  }
}
void renderTeamsTab() {
  if(true) {
    ImGui::Text("Not yet working. Use constelia.ai/members/update");
    return;
  }

  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching team scripts...");
    return;
  }

  if(team_scripts_amount <= 0) {
    ImGui::Text("You are not an author or team member on any cloud scripts.");
    return;
  }

  string scriptSelected = team_script_names.at(scriptSelectedIndex);

  if(ImGui::BeginCombo("Team Scripts", scriptSelected.c_str(), ImGuiComboFlags_None)) {
    for(int i = 0; i < team_scripts_amount; i++) {
      bool is_selected = (scriptSelected == team_script_names[i]);
      if(ImGui::Selectable(team_script_names[i].c_str(), is_selected)) {
        scriptSelectedIndex = i;
        scriptSelected = team_script_names[i];
      }
      if(is_selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }

  string luaData = "{\"value\": " + std::to_string(1) + "}";
  auto scriptSource = fc2::call<string>("eh_get_team_script_source", FC2_LUA_TYPE_STRING, luaData);

  if(ImGui::InputTextMultiline("script", const_cast<char *>(scriptSource.c_str()), 4096)) {

  }


  if(ImGui::Button("Post", ImVec2(180,20))) {
    auto script_id = fc2::http::escape("253");
    auto content = fc2::http::escape("");
    auto notes = fc2::http::escape("notes");
    // no i am not going to fix this probably ever, it works and that's all that matters.
    string postUrl = "https://constelia.ai/api.php?key="; postUrl.append(session.license); postUrl.append("&cmd=updateScript");
    auto post_response = fc2::http::post(postUrl, (std::format("script={}&content={}&notes={}", script_id, content, notes)));
    //auto output = fc2::api("updateScript&" + std::format("script={}&content={}&notes={}", script_id, content, notes));
    std::puts(post_response.c_str());
    //std::puts(output.c_str());
  }
}
void renderConfigurationTab() {
  if(true) {
    ImGui::Text("Not yet working. Use some other config solution, like pure_configuration.lua");
    ImGui::Text("#ad");
    return;
  }

  if(!ready) {
    ImGui::Text("Refreshing Universe4, please wait.");
    return;
  }

  if(ImGui::Button("Get Configuration", ImVec2(160, 20))) {
    configuration = fc2::api("getConfiguration");
    std::puts(configuration.c_str());

  }
  ImGui::SameLine();
}
void renderSteamTab() {
  if(true) {
    ImGui::Text("Steam functionality coming at a later date.");
    return;
  }

  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching steam accounts...");
    return;
  }

  ImGui::SeparatorText("Visible Steam Accounts");

  for(int i = 0; i < steam_amount; i++) {
    string label = steam_names.at(i) + "##steam";
    if(ImGui::CollapsingHeader(label.c_str(), ImGuiTreeNodeFlags_None)) {
      ImGui::Text("Nickname: %s", steam_personas.at(i).c_str());
      //ImGui::Text("Steam ID: %s", steam_ids.at(i).c_str());
      if(ImGui::Button("Hide Steam Account", ImVec2(200, 20))) {
        steam_api_response = fc2::api(std::format("https://constelia.ai/api.php?key={}&cmd=hideSteamAccount&name={}", session.license, steam_names.at(i)));
      }
    }
    if(steam_api_response.find(steam_names.at(i))) {
      ImGui::Text(std::format("{} marked for removal.", steam_names.at(i)).c_str());
    }
  }

}
void renderPerksTab() {
  if(high_resolution_clock::now() >= endTimer) {
    perk_purchased = -1;
  }

  if(!asyncFinished || !loadingFinished) {
    ImGui::Text("Fetching perks...");
    return;
  }

  ImGui::SeparatorText("Owned Perks");
  for (int i = 0; i < perks_amount; i++) {
    if(!perk_is_owned.at(i)) {
      continue;
    }
    if(ImGui::CollapsingHeader(perk_names.at(i).c_str(), ImGuiTreeNodeFlags_None)) {
      ImGui::Text("ID: %i", perk_ids.at(i));
      ImGui::TextWrapped("Description: %s", perk_descriptions.at(i).c_str());
    }
  }
  ImGui::SeparatorText("Purchasable Perks");
  for(int i = 0; i < perks_amount; i++) {
    if(perk_is_owned.at(i)) {
      continue;
    }
    if(ImGui::CollapsingHeader(perk_names.at(i).c_str(), ImGuiTreeNodeFlags_None)) {
      ImGui::Text("ID: %i", perk_ids.at(i));
      ImGui::TextWrapped("Description: %s", perk_descriptions.at(i).c_str());
      string buttonLabel = "Purchase Perk"; buttonLabel.append("##"); buttonLabel.append(std::to_string(perk_ids.at(i)));
      if(ImGui::Button(buttonLabel.c_str())) {
        startTimer = high_resolution_clock::now();
        endTimer = startTimer + duration;
        string apiCall = "buyPerk&id=";
        apiCall.append(std::to_string(perk_ids.at(i)));
        string apiResponse = fc2::api(apiCall);
        printf("%s\n", apiResponse.c_str());
        size_t found = apiResponse.find("perk added");
        if(found != string::npos) {
          perk_purchased = i;
          perk_is_owned.at(i) = true;
        }
        else {
          perk_purchased = -2;
        }
      }
    }
  }
  if(perk_purchased >= 0) {
    ImGui::Text("Perk %s purchased.", perk_names.at(perk_purchased).c_str());
  }
  else if(perk_purchased == -2) {
    ImGui::Text("Perk failed to purchase, you either don't have enough perk points or you already own it.");
  }
}
void renderSettingsTab() {
  ImGui::SeparatorText("Event Horizon Settings");
  if(ImGui::BeginCombo("Theme", themeSelected, ImGuiComboFlags_None)) {
    for(int i = 0; i < IM_ARRAYSIZE(themes); i++) {
      bool is_selected = (themeSelected == themes[i]);
      if(ImGui::Selectable(themes[i], is_selected)) {
        themeSelected = themes[i];
        selectStyle(i);
      }
      if(is_selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
  if(ImGui::BeginCombo("Date Format", dateFormatSelected, ImGuiComboFlags_None)) {
    for(int i = 0; i < IM_ARRAYSIZE(dateFormats); i++) {
      bool is_selected = (dateFormatSelected == dateFormats[i]);
      if(ImGui::Selectable(dateFormats[i], is_selected)) {
        dateFormatSelected = dateFormats[i];
      }
      if(is_selected) {
        ImGui::SetItemDefaultFocus();
      }
    }
    ImGui::EndCombo();
  }
}

string getYMDAsFormatted(day day, month month, year year) {
  string formattedString;
  int iDay = day.operator unsigned int(), iMonth = month.operator unsigned int(), iYear = year.operator int();

  if(dateFormatSelected == dateFormats[0]) {
    formattedString = std::to_string(iMonth) + "/" + std::to_string(iDay) + "/" + std::to_string(iYear);
  }
  else if(dateFormatSelected == dateFormats[1]) {
    formattedString = std::to_string(iDay) + "/" + std::to_string(iMonth) + "/" + std::to_string(iYear);
  }

  return formattedString;
}
void showApplyButton() {
  if(asyncFinished && loadingFinished) {
    if(!ready) {
      ImGui::Text("Refreshing Universe4, please wait.");
      if(fc2::call<bool>("eh_ready", FC2_LUA_TYPE_BOOLEAN)) {
        refreshCache();
        ready = true;
        projectChangesMade = false;
        scriptChangesMade = false;
      }
    }
    else if(ImGui::Button("Apply & Refresh", ImVec2(180,20))) {
      updateActiveScripts();
      updateActiveProjects();
      fc2::call<bool>("eh_refresh", FC2_LUA_TYPE_NONE);
      ready = false;
    }
  }
}
void refreshCache() {
  loadingFinished = false;
  astrology = fc2::call<string>("eh_astrology", FC2_LUA_TYPE_STRING);
  int protectionInt = session.protection;
  int levelInt = session.level;
  session_directory = fc2::call<string>("eh_directory", FC2_LUA_TYPE_STRING);
  xp = fc2::call<int>("eh_xp", FC2_LUA_TYPE_INT);
  perk_points = fc2::call<int>("eh_perk_points", FC2_LUA_TYPE_INT);
  posts = fc2::call<int>("eh_posts", FC2_LUA_TYPE_INT);
  score = fc2::call<int>("eh_score", FC2_LUA_TYPE_INT);
  unread_conversations = fc2::call<int>("eh_unread_conversations", FC2_LUA_TYPE_INT);
  unread_alerts = fc2::call<int>("eh_unread_alerts", FC2_LUA_TYPE_INT);

  session_started = fc2::call<int>("eh_session_started", FC2_LUA_TYPE_INT);
  session_start_ymd = std::chrono::floor<days>(system_clock::from_time_t(session_started));
  session_expiry = fc2::call<int>("eh_session_expiry", FC2_LUA_TYPE_INT);
  session_expiry_ymd = std::chrono::floor<days>(system_clock::from_time_t(session_expiry));

  perks_amount = fc2::call<int>("eh_perks_amount", FC2_LUA_TYPE_INT);
  owned_perks_amount = fc2::call<int>("eh_owned_perks_amount", FC2_LUA_TYPE_INT);
  perk_ids = {};
  perk_names = {};
  perk_descriptions = {};
  perk_is_owned = {};
  owned_perk_ids = {};
  perk_purchased = -1;

  scripts_amount = fc2::call<int>("eh_scripts_amount", FC2_LUA_TYPE_INT);
  script_names = {};
  script_ids = {};
  script_authors = {};
  script_last_updated = {};
  script_last_updated_ymd = {};
  script_update_notes = {};
  script_is_active = {};
  enabled_scripts_amount = fc2::call<int>("eh_enabled_scripts_amount", FC2_LUA_TYPE_INT);
  enabled_script_ids = {};

  projects_amount = fc2::call<int>("eh_fc2t_amount", FC2_LUA_TYPE_INT);
  project_names = {};
  project_ids = {};
  project_authors = {};
  project_last_updated = {};
  project_last_updated_ymd = {};
  project_is_active = {};
  enabled_projects_amount = fc2::call<int>("eh_enabled_fc2t_amount", FC2_LUA_TYPE_INT);
  enabled_project_ids = {};

  team_scripts_amount = fc2::call<int>("eh_team_scripts_amount", FC2_LUA_TYPE_INT);
  team_script_names = {};

  //steam_amount = fc2::call<int>("eh_steam_amount", FC2_LUA_TYPE_INT);
  //steam_names = {};
  //steam_personas = {};

  // do protection thing
  switch(protectionInt) {
    case 0:
      protection = "Standard";
      break;
    case 1:
      protection = "Zombie/IPC";
      break;
    case 2:
      protection = "Kernel";
      break;
    default:
      protection = "mystery protection???";
      break;
  }

  // do level thing
  switch(levelInt) {
    case 1:
      level = "Member";
      break;
    case 2:
      level = "Veteran Member";
      break;
    case 3:
      level = "VIP";
      break;
    default:
      level = "probably admin or some shit idk dawg";
      break;
  }

  // populate scripts & projects
  thread async(asyncCacheTasks);
  async.detach();
}
void asyncCacheTasks() {
  asyncFinished = false;
  for(int i = 0; i < perks_amount; i++) {
    string luaData = "{\"value\": " + std::to_string(i) + "}";
    perk_ids.push_back(fc2::call<int>("eh_get_perk_id", FC2_LUA_TYPE_INT, luaData));
    perk_names.push_back(fc2::call<string>("eh_get_perk_name", FC2_LUA_TYPE_STRING, luaData));
    perk_descriptions.push_back(fc2::call<string>("eh_get_perk_description", FC2_LUA_TYPE_STRING, luaData));
    perk_is_owned.push_back(false);
  }

  for(int i = 0; i < owned_perks_amount; i++) {
    string luaData = "{\"value\": " + std::to_string(i) + "}";
    owned_perk_ids.push_back(fc2::call<int>("eh_get_owned_perk_id", FC2_LUA_TYPE_INT, luaData));
  }

  for(int i = 0; i < owned_perks_amount; i++) {
    auto it = std::find(perk_ids.begin(), perk_ids.end(), owned_perk_ids.at(i));
    int index = it - perk_ids.begin();
    perk_is_owned.at(index) = true;
  }

  for(int i = 0; i < scripts_amount; i++) {
    // everything about scripts from a web api call is given as a string,
    // don't know why, don't really care, store everything as a string
    string luaData = "{\"value\": " + std::to_string(i) + "}";
    script_names.push_back(fc2::call<string>("eh_get_script_name", FC2_LUA_TYPE_STRING, luaData));
    script_ids.push_back(fc2::call<string>("eh_get_script_id", FC2_LUA_TYPE_STRING, luaData));
    script_authors.push_back(fc2::call<string>("eh_get_script_author", FC2_LUA_TYPE_STRING, luaData));
    script_last_updated.push_back(fc2::call<string>("eh_get_script_last_update_time", FC2_LUA_TYPE_STRING, luaData));
    script_last_updated_ymd.push_back(std::chrono::floor<days>(system_clock::from_time_t(std::stoi(script_last_updated.at(i)))));
    script_update_notes.push_back(fc2::call<string>("eh_get_script_update_notes", FC2_LUA_TYPE_STRING, luaData));
    script_is_active.push_back(false);
  }

  for(int i = 0; i < enabled_scripts_amount; i++) {
    string luaData = "{\"value\": " + std::to_string(i) + "}";
    enabled_script_ids.push_back(fc2::call<string>("eh_get_enabled_script_id", FC2_LUA_TYPE_STRING, luaData));
  }

  for(int i = 0; i < enabled_scripts_amount; i++) {
    auto it = std::find(script_ids.begin(), script_ids.end(), enabled_script_ids.at(i));
    int index = it - script_ids.begin();
    script_is_active.at(index) = true;
  }

  for(int i = 0; i < projects_amount; i++) {
    string luaData = "{\"value\": " + std::to_string(i) + "}";
    project_names.push_back(fc2::call<string>("eh_get_fc2t_name", FC2_LUA_TYPE_STRING, luaData));
    project_ids.push_back(fc2::call<int>("eh_get_fc2t_id", FC2_LUA_TYPE_INT, luaData));
    project_authors.push_back(fc2::call<string>("eh_get_fc2t_author", FC2_LUA_TYPE_STRING, luaData));
    project_last_updated.push_back(fc2::call<int>("eh_get_fc2t_last_update_time", FC2_LUA_TYPE_INT, luaData));
    project_last_updated_ymd.push_back(std::chrono::floor<days>(system_clock::from_time_t(project_last_updated.at(i))));
    project_is_active.push_back(false);
  }

  for(int i = 0; i < enabled_projects_amount; i++) {
    string luaData = "{\"value\": " + std::to_string(i) + "}";
    enabled_project_ids.push_back(fc2::call<int>("eh_get_enabled_fc2t_id", FC2_LUA_TYPE_INT, luaData));
  }

  for(int i = 0; i < enabled_projects_amount; i++) {
    auto it = std::find(project_ids.begin(), project_ids.end(), enabled_project_ids.at(i));
    int index = it - project_ids.begin();
    project_is_active.at(index) = true;
  }

  /*for(int i = 0; i < team_scripts_amount; i++) {
    string luaData = "{\"value\": " + std::to_string(i) + "}";
    team_script_names.push_back(fc2::call<string>("eh_get_team_script_name", FC2_LUA_TYPE_STRING, luaData));
    team_script_ids.push_back(fc2::call<string>("eh_get_team_script_id", FC2_LUA_TYPE_STRING, luaData));
  }*/

  /*for(int i = 0; i < steam_amount; i++) {
    string luaData = "{\"value\": " + std::to_string(i) + "}";
    steam_names.push_back(fc2::call<string>("eh_get_steam_name", FC2_LUA_TYPE_STRING, luaData));
    steam_personas.push_back(fc2::call<string>("eh_get_steam_persona", FC2_LUA_TYPE_STRING, luaData));
  }*/

  asyncFinished = true;
  loadingFinished = true;
}
void updateActiveScripts() {
  string apiCall = "setMemberScripts&scripts=[";
  for(int i = 0; i < scripts_amount; i++) {
    if(script_is_active.at(i)) {
      if(apiCall.back() != '[') {
        apiCall.append(",");
      }
      apiCall.append(script_ids.at(i));
    }
  }
  apiCall.append("]");
  string apiResponse = fc2::api(apiCall);
  printf("%s\n", apiResponse.c_str());
}
void updateActiveProjects() {
  for(int i = 0; i < projects_amount; i++) {
    bool is_active = false;
    for(int j = 0; j < enabled_projects_amount; j++) {
      auto it = std::find(project_ids.begin(), project_ids.end(), enabled_project_ids.at(j));
      if(it != std::end(project_ids)) {
        is_active = true;
      }
    }
    if((is_active && project_is_active.at(i)) || (!is_active && !project_is_active.at(i))) {
      printf("%s | %s, %s\n", "tried toggling project.",is_active ? "true" : "false", project_is_active.at(i) ? "true" : "false");
      continue;
    }
    string apiCall = "toggleProjectStatus&id=" + std::to_string(project_ids.at(i));
    string apiResponse = fc2::api(apiCall);
    printf("%s\n", apiResponse.c_str());
  }
}

auto dx11::on_render(ImGuiIO &io, HWND window) -> void {
  if(fc2::get_error() != FC2_TEAM_ERROR_NO_ERROR) {
    puts("Universe4 is not running anymore. Terminating.");
    sleep(3);
    std::exit(1);
  }

  if(!ranFirstTimeChecks) {
    fc2::call<bool>("eh_refresh_cache", FC2_LUA_TYPE_NONE);
    refreshCache();
    setStyleEventHorizon();
    ranFirstTimeChecks = true;
  }

#ifdef IMGUI_HAS_VIEWPORT
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->GetWorkPos());
  ImGui::SetNextWindowSize(viewport->GetWorkSize());
  ImGui::SetNextWindowViewport(viewport->ID);
#else
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
  ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  if(!ImGui::Begin("Event Horizon", nullptr, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration)) {
    std::cout << "Couldn't start the main window! Terminating.";
    sleep(3);
    std::exit(1);
  }
  if(!ImGui::BeginTabBar("TabBar", ImGuiTabBarFlags_None)) {
    std::cout << "Couldn't create TabBar! Terminating.";
    sleep(3);
    std::exit(1);
  }

  // General Tab
  if(ImGui::BeginTabItem("General")) {
    renderGeneralTab();
    ImGui::EndTabItem();
  }

  // Scripts Tab
  if(ImGui::BeginTabItem("Scripts")) {
    renderScriptsTab();
    ImGui::EndTabItem();
  }

  // FC2T Projects Tab
  if(ImGui::BeginTabItem("FC2T Projects")) {
    renderFC2TTab();
    ImGui::EndTabItem();
  }

  // Perks Tab
  if(ImGui::BeginTabItem("Perks")) {
    renderPerksTab();
    ImGui::EndTabItem();
  }

  // Teams Tab
  if(ImGui::BeginTabItem("Teams")) {
    renderTeamsTab();
    ImGui::EndTabItem();
  }

  // Configuration Tab
  if(ImGui::BeginTabItem("Configuration")) {
    renderConfigurationTab();
    ImGui::EndTabItem();
  }

  // Steam Tab
  if(ImGui::BeginTabItem("Steam")) {
    renderSteamTab();
    ImGui::EndTabItem();
  }

  // Settings Tab
  if(ImGui::BeginTabItem("Settings")) {
    renderSettingsTab();
    ImGui::EndTabItem();
  }

  ImGui::EndTabBar();
  ImGui::End();
}