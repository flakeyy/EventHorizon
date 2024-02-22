#ifndef FMENU_DATA_H
#define FMENU_DATA_H
#include <string>
#include <chrono>
#include <vector>

struct Member {
  int buddy;
  int levelIndex;
  int protectionIndex;
  int xp;
  int perkPoints;
  int posts;
  int score;
  int unreadConversations;
  int unreadAlerts;
  std::string username;
  std::string license;
  std::string astrology;
  std::string level;
  std::string protection;

  void getLevelFromIndex() {
    switch(levelIndex) {
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
  }

  void getProtectionFromIndex() {
    switch(protectionIndex) {
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
  }
};

struct Session {
  int started;
  int expiry;
  std::string directory;
  std::chrono::year_month_day startYmd;
  std::chrono::year_month_day expiryYmd;
};

struct Perks {
  int amount;
  int purchased;
  bool lootRolled;
  std::string rollApiMessage;
  std::vector<int> ids;
  std::vector<int> ownedIds;
  std::vector<std::string> names;
  std::vector<std::string> descriptions;
  std::vector<bool> isOwned;
};

struct Scripts {
  int amount;
  std::vector<std::string> names;
  std::vector<std::string> authors;
  std::vector<std::string> ids;
  std::vector<std::string> updateNotes;
  std::vector<std::string> lastUpdated;
  std::vector<std::chrono::year_month_day> lastUpdatedYmd;
  std::vector<bool> isActive;
};

struct Projects {
  int amount;
  std::vector<std::string> names;
  std::vector<std::string> authors;
  std::vector<int> ids;
  std::vector<int> lastUpdated;
  std::vector<std::chrono::year_month_day> lastUpdatedYmd;
  std::vector<bool> isActive;
};

struct Posts {
  int amount;
  std::vector<std::string> username;
  std::vector<std::string> title;
  std::vector<std::string> elapsed;
};

struct Information {
  Member member;
  Session session;
  Perks perks;
  Scripts scripts;
  Projects projects;
  Posts posts;

  void reset() {
    member = {};
    session = {};
    perks = {};
    scripts = {};
    projects = {};
    posts = {};
  }
};


#endif //FMENU_DATA_H
