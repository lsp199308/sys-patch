#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#define STBTT_STATIC
#include <tesla.hpp>    // The Tesla Header
#include <string_view>
#include "minIni/minIni.h"

namespace {

constexpr auto CONFIG_PATH = "/config/sys-patch/config.ini";
constexpr auto LOG_PATH = "/config/sys-patch/log.ini";

auto does_file_exist(const char* path) -> bool {
    Result rc{};
    FsFileSystem fs{};
    FsFile file{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(path_buf, path);
    rc = fsFsOpenFile(&fs, path_buf, FsOpenMode_Read, &file);
    fsFileClose(&file);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

// creates a directory, non-recursive!
auto create_dir(const char* path) -> bool {
    Result rc{};
    FsFileSystem fs{};
    char path_buf[FS_MAX_PATH]{};

    if (R_FAILED(fsOpenSdCardFileSystem(&fs))) {
        return false;
    }

    strcpy(path_buf, path);
    rc = fsFsCreateDirectory(&fs, path_buf);
    fsFsClose(&fs);
    return R_SUCCEEDED(rc);
}

struct ConfigEntry {
    ConfigEntry(const char* _section, const char* _key, bool default_value) :
        section{_section}, key{_key}, value{default_value} {
            this->load_value_from_ini();
        }

    void load_value_from_ini() {
        this->value = ini_getbool(this->section, this->key, this->value, CONFIG_PATH);
    }

    auto create_list_item(const char* text) {
        auto item = new tsl::elm::ToggleListItem(text, value);
        item->setStateChangedListener([this](bool new_value){
            this->value = new_value;
            ini_putl(this->section, this->key, this->value, CONFIG_PATH);
        });
        return item;
    }

    const char* const section;
    const char* const key;
    bool value;
};

class GuiOptions final : public tsl::Gui {
public:
    GuiOptions() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("Options"));
        list->addItem(config_patch_sysmmc.create_list_item("Patch_sysmmc"));
        list->addItem(config_patch_emummc.create_list_item("Patch_emummc"));
        list->addItem(config_logging.create_list_item("Logging"));
        list->addItem(config_version_skip.create_list_item("Version_skip"));

        frame->setContent(list);
        return frame;
    }

    ConfigEntry config_patch_sysmmc{"options", "patch_sysmmc", true};
    ConfigEntry config_patch_emummc{"options", "patch_emummc", true};
    ConfigEntry config_logging{"options", "enable_logging", true};
    ConfigEntry config_version_skip{"options", "version_skip", true};
};

class GuiToggle final : public tsl::Gui {
public:
    GuiToggle() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        list->addItem(new tsl::elm::CategoryHeader("FS - 0100000000000000"));
        list->addItem(config_1_9A.create_list_item("1_9A"));
        list->addItem(config_1_9B.create_list_item("1_9B"));
        list->addItem(config_1_9C.create_list_item("1_9C"));
        list->addItem(config_1_9D.create_list_item("1_9D"));
        list->addItem(config_10A.create_list_item("10A"));
        list->addItem(config_10_18A.create_list_item("10_18A"));
        list->addItem(config_19A.create_list_item("19A"));
        list->addItem(config_10D.create_list_item("10D"));
        list->addItem(config_10_14D.create_list_item("10_14D"));
        list->addItem(config_15_16D.create_list_item("15_16D"));
        list->addItem(config_17_18D.create_list_item("17_18D"));
        list->addItem(config_19D.create_list_item("19D"));

        list->addItem(new tsl::elm::CategoryHeader("LDR - 0100000000000001"));
        list->addItem(config_noacidsigchk.create_list_item("noacidsigchk"));
        list->addItem(config_debug_flag.create_list_item("debug_flag"));
        list->addItem(config_debug_flag_off.create_list_item("debug_flag_off"));

        list->addItem(new tsl::elm::CategoryHeader("ERPT - 010000000000002b"));
        list->addItem(config_erpt.create_list_item("erpt"));

        list->addItem(new tsl::elm::CategoryHeader("ES - 0100000000000033"));
        list->addItem(config_1.create_list_item("1"));
        list->addItem(config_2_8.create_list_item("2_8"));
        list->addItem(config_9_19.create_list_item("9_19"));

        list->addItem(new tsl::elm::CategoryHeader("NIFM - 010000000000000F"));
        list->addItem(config_ctest.create_list_item("ctest"));

        list->addItem(new tsl::elm::CategoryHeader("NIM - 0100000000000025"));
        list->addItem(config_nim.create_list_item("nim"));

        frame->setContent(list);
        return frame;
    }

    ConfigEntry config_1_9A{"fs", "1_9A", true};
    ConfigEntry config_1_9B{"fs", "1_9B", true};
    ConfigEntry config_1_9C{"fs", "1_9C", true};
    ConfigEntry config_1_9D{"fs", "1_9D", true};
    ConfigEntry config_10A{"fs", "10A", true};
    ConfigEntry config_10_18A{"fs", "10_18A", true};
    ConfigEntry config_19A{"fs", "19A", true};
    ConfigEntry config_10D{"fs", "10D", true};
    ConfigEntry config_10_14D{"fs", "10_14D", true};
    ConfigEntry config_15_16D{"fs", "15_16D", true};
    ConfigEntry config_17_18D{"fs", "17_18D", true};
    ConfigEntry config_19D{"fs", "19D", true};
    ConfigEntry config_noacidsigchk{"ldr", "noacidsigchk", true};
    ConfigEntry config_debug_flag{"ldr", "debug_flag", false};
    ConfigEntry config_debug_flag_off{"ldr", "debug_flag_off", false};
    ConfigEntry config_erpt{"ldr", "erpt", true};
    ConfigEntry config_1{"es", "1", true};
    ConfigEntry config_2_8{"es", "2_8", true};
    ConfigEntry config_9_19{"es", "9_19", true};
    ConfigEntry config_ctest{"nifm", "ctest", false};
    ConfigEntry config_nim{"nim", "nim", false};
};

class GuiLog final : public tsl::Gui {
public:
    GuiLog() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        if (does_file_exist(LOG_PATH)) {
            struct CallbackUser {
                tsl::elm::List* list;
                std::string last_section;
            } callback_userdata{list};

            ini_browse([](const mTCHAR *Section, const mTCHAR *Key, const mTCHAR *Value, void *UserData){
                auto user = (CallbackUser*)UserData;
                std::string_view value{Value};

                if (value == "Skipped") {
                    return 1;
                }

                if (user->last_section != Section) {
                    user->last_section = Section;
                    user->list->addItem(new tsl::elm::CategoryHeader("Log: " + user->last_section));
                }

                #define F(x) ((x) >> 4) // 8bit -> 4bit
                constexpr tsl::Color colour_syspatch{F(0), F(255), F(200), F(255)};
                constexpr tsl::Color colour_file{F(255), F(177), F(66), F(255)};
                constexpr tsl::Color colour_unpatched{F(250), F(90), F(58), F(255)};
                #undef F

                if (value.starts_with("Patched")) {
                    if (value.ends_with("(sys-patch)")) {
                        user->list->addItem(new tsl::elm::ListItem(Key, "Patched", colour_syspatch));
                    } else {
                        user->list->addItem(new tsl::elm::ListItem(Key, "Patched", colour_file));
                    }
                } else if (value.starts_with("Unpatched") || value.starts_with("Disabled")) {
                    user->list->addItem(new tsl::elm::ListItem(Key, Value, colour_unpatched));
                } else if (user->last_section == "stats") {
                    user->list->addItem(new tsl::elm::ListItem(Key, Value, tsl::style::color::ColorDescription));
                } else {
                    user->list->addItem(new tsl::elm::ListItem(Key, Value, tsl::style::color::ColorText));
                }

                return 1;
            }, &callback_userdata, LOG_PATH);
        } else {
            list->addItem(new tsl::elm::ListItem("No log found!"));
        }

        frame->setContent(list);
        return frame;
    }
};

class GuiMain final : public tsl::Gui {
public:
    GuiMain() { }

    tsl::elm::Element* createUI() override {
        auto frame = new tsl::elm::OverlayFrame("sys-patch", VERSION_WITH_HASH);
        auto list = new tsl::elm::List();

        auto options = new tsl::elm::ListItem("Options");
        auto toggle = new tsl::elm::ListItem("Toggle patches");
        auto log = new tsl::elm::ListItem("Log");

        options->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiOptions>();
                return true;
            }
            return false;
        });

        toggle->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiToggle>();
                return true;
            }
            return false;
        });

        log->setClickListener([](u64 keys) -> bool {
            if (keys & HidNpadButton_A) {
                tsl::changeTo<GuiLog>();
                return true;
            }
            return false;
        });

        list->addItem(new tsl::elm::CategoryHeader("Menu"));
        list->addItem(options);
        list->addItem(toggle);
        list->addItem(log);

        frame->setContent(list);
        return frame;
    }
};

// libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
class SysPatchOverlay final : public tsl::Overlay {
public:
    std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<GuiMain>();
    }
};

} // namespace

int main(int argc, char **argv) {
    create_dir("/config/");
    create_dir("/config/sys-patch/");
    return tsl::loop<SysPatchOverlay>(argc, argv);
}