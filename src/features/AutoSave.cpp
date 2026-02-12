#include <chrono>

#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/LocalLevelManager.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/loader/Dirs.hpp>

#include <cvolton.level-id-api/include/EditorIDs.hpp>

using namespace geode::prelude;
namespace fs = std::filesystem;


namespace Time {

    time_t getNow() { // in seconds
        auto now = std::chrono::system_clock::now();
        return std::chrono::system_clock::to_time_t(now);
    }
    
    const time_t gameLoadTime = getNow();
    
    time_t getFileSaveTime(fs::path path) {
        if (!fs::exists(path)) return 0;
        auto ftime = fs::last_write_time(path);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        time_t time = std::chrono::system_clock::to_time_t(sctp);
        return time;
    }
    
    time_t getLastGDSaveTime() {
        auto gdSaveFilePath = dirs::getGeodeSaveDir() / ".." / "CCLocalLevels.dat";
        return getFileSaveTime(gdSaveFilePath);
    }
};


namespace Backups {
    // save encoded (gzipped) level string to backup file
    bool save(std::string const &levelString, fs::path path) {
        auto data = matjson::makeObject({
            {"levelString", levelString}
        });
        return utils::file::writeToJson(path, data).isOk();
    }

    fs::path getBackupsDir() {
        auto dir = Mod::get()->getSaveDir().append("backups");
        if (!fs::exists(dir)) {
            fs::create_directory(dir);
        }
        return dir;
    }

    fs::path pathForId(int levelId) {
        return getBackupsDir().append(fmt::format("{}.json", levelId));
    }

    // get encoded (gzipped) level string from backup file
    std::string getLevelString(fs::path path) {
        auto res = utils::file::readJson(path);
        if (res.isOk()) {
            auto lvl = res.unwrap()["levelString"];
            return lvl.asString().unwrapOrDefault();
        }
        return "";
    }

    // get all files in backup directory
    std::vector<fs::path> getAllBackups() {
        auto dir = getBackupsDir();
        std::vector<fs::path> ret;
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                ret.push_back(entry.path());
            }
        }
        return ret;
    }

    // clear backups that are too old (older than a week)
    void clearTooOldBackups(time_t maxLifeTime) {
        auto all = getAllBackups();
        time_t now = Time::getNow();
        for (auto& path : all) {
            time_t saveTime = Time::getFileSaveTime(path);
            if (saveTime < now - maxLifeTime) {
                fs::remove(path);
            }
        }
    }
};


$on_mod(Loaded) {
    Backups::clearTooOldBackups(1 * 7 * 24 * 60 * 60); // week
}


class $modify(AutosaveLevelEditorLayer, LevelEditorLayer) {
    struct Fields {
        int levelId;
        bool doBackupAfterPlaytest = false;
        int backupTimer = -1;
        Ref<TextAlertPopup> backupAlert;
    };

    bool init(GJGameLevel* p0, bool p1) {
        if (!LevelEditorLayer::init(p0, p1)) return false;
        if (GJLevelType::Editor == p0->m_levelType) {
            m_fields->levelId = EditorIDs::getID(p0);
            scheduleNextBackup();
        }        
        return true;
    }

    void scheduleNextBackup() {
        unschedule(schedule_selector(AutosaveLevelEditorLayer::startBackupTimer));
        if (int val = Mod::get()->getSettingValue<uint64_t>("autosave-rate")) {
            scheduleOnce(schedule_selector(AutosaveLevelEditorLayer::startBackupTimer), val * 60);
        }
    }

    void startBackupTimer(float) {
        schedule(schedule_selector(AutosaveLevelEditorLayer::timerBackup), 1);
    }

    void timerBackup(float) {
        if (m_fields->backupTimer > 1) {
            // countdown
            m_fields->backupTimer--;
            m_fields->backupAlert->m_label->setString(fmt::format("Backup in {}s", m_fields->backupTimer).c_str());
        } else if (m_fields->backupTimer == 1) {
            // do backup
            doBackup();
            m_fields->backupTimer = -1;
            m_fields->backupAlert->setVisible(false);
            m_fields->backupAlert = nullptr;
            
            auto alert = TextAlertPopup::create("Saved!", 1, 0.3, 150, "bigFont.fnt");
            addChild(alert, 199);
            alert->setPositionY(30);
            unschedule(schedule_selector(AutosaveLevelEditorLayer::timerBackup));
        } else { // timer <= 0
            // start countdown
            m_fields->backupTimer = 3;
            auto alert = TextAlertPopup::create("Backup in 3s", 4, 0.3, 150, "bigFont.fnt");
            addChild(alert, 199);
            alert->setPositionY(30);
            m_fields->backupAlert = alert;
        }
    }

    void onStopPlaytest() {
        LevelEditorLayer::onStopPlaytest();
        if (m_fields->doBackupAfterPlaytest) {
            doBackup();
        }
    }

    void doBackup(bool fromExistingSaveStr = false) {
        if (GJLevelType::Editor != m_level->m_levelType) return;

        // don't do it in playtest mode
        if (m_playbackMode == PlaybackMode::Not) {
            m_fields->doBackupAfterPlaytest = false;

            // save backup
            if (fromExistingSaveStr) {
                Backups::save(m_level->m_levelString, Backups::pathForId(m_fields->levelId));
            } else {
                std::string s = ZipUtils::compressString(getLevelString(), true, 0);
                Backups::save(s, Backups::pathForId(m_fields->levelId));
            }

            scheduleNextBackup();
        } else {
            m_fields->doBackupAfterPlaytest = true;
        }
    }
    
};


// class $modify(EditorPauseLayer) {
//     void saveLevel() {
//         EditorPauseLayer::saveLevel();
//         reinterpret_cast<AutosaveLevelEditorLayer*>(m_editorLayer)->doBackup(true); 
//     }

//     void onExitEditor(CCObject* sender) {
//         reinterpret_cast<AutosaveLevelEditorLayer*>(m_editorLayer)->doBackup(true);
//         EditorPauseLayer::onExitEditor(sender);
//     }
// };


// Here we check for existing backups
class $modify(EditLevelLayer) {
    bool init(GJGameLevel* p0) {
        if (!EditLevelLayer::init(p0)) return false;
        if (LevelEditorLayer::get() != nullptr) return true; // don't check on level exit

        // check backups
        if (levelHasNewerBackup(p0)) {
            auto popup = createQuickPopup(
                "GEAR Backup System",
                "Found backup for this level that is newer that the last game save. "
                "That likely happened because of the game crash\n<cy>Load backup?</c>",
                "Load Backup", "No",
                [this, p0] (auto, bool isBtn2) {
                    if (!isBtn2) {
                        loadLevelBackup(p0);
                        addChild(TextAlertPopup::create("Loaded!", 1, 0.6, 150, "bigFont.fnt"), 199);
                    }
                },
                false, false
            );
            popup->m_scene = this;
            popup->show();
        }
        return true;
    }

    bool levelHasNewerBackup(GJGameLevel* lvl) {
        int id = EditorIDs::getID(lvl);
        auto path = Backups::pathForId(id);
        if (fs::exists(path)) {
            auto fileSaveTime = Time::getFileSaveTime(path);
            return Time::getLastGDSaveTime() < fileSaveTime && fileSaveTime < Time::gameLoadTime;
        }
        return false;
    }

    void loadLevelBackup(GJGameLevel* lvl) {
        int id = EditorIDs::getID(lvl);
        auto path = Backups::pathForId(id);
        if (fs::exists(path)) {
            auto str = Backups::getLevelString(path);
            if (!str.empty()) lvl->m_levelString = str;
        }
    }
};
