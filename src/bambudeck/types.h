#ifndef _BAMBUDECK_TYPES
#define _BAMBUDECK_TYPES

#include <stdbool.h>
#include <stdint.h>

#define BAMBUDECK_LCD_MIN_SLEEP_TIME 5

#ifdef __cplusplus
extern "C"
{
#endif

    enum BambuDeckPrinterSeries
    {
        SERIES_X1 = 0,
        SERIES_P1P,
        SERIES_UNKNOWN,
    };

    struct BambuDeckPanelConfig
    {
        float xCalM;
        float yCalM;
        float xCalC;
        float yCalC;
    };

    enum BambuDeckPrintStatus
    {
        BAMBUDECK_PRINT_STATUS_IDLE,
        BAMBUDECK_PRINT_STATUS_RUNNING,
        BAMBUDECK_PRINT_STATUS_PAUSED,
        BAMBUDECK_PRINT_STATUS_FINISHED,
        BAMBUDECK_PRINT_STATUS_PREPARE,
        BAMBUDECK_PRINT_STATUS_FAILED,
    };

    enum BambuDeckPrintingSpeedLevel
    {
        BAMBUDECK_SPEED_LEVEL_INVALID = 0,
        BAMBUDECK_SPEED_LEVEL_SILENCE = 1,
        BAMBUDECK_SPEED_LEVEL_NORMAL = 2,
        BAMBUDECK_SPEED_LEVEL_RAPID = 3,
        BAMBUDECK_SPEED_LEVEL_RAMPAGE = 4,
        BAMBUDECK_SPEED_LEVEL_COUNT
    };

    enum BambuDeckAmsStatusMain
    {
        AMS_STATUS_MAIN_IDLE = 0x00,
        AMS_STATUS_MAIN_FILAMENT_CHANGE = 0x01,
        AMS_STATUS_MAIN_RFID_IDENTIFYING = 0x02,
        AMS_STATUS_MAIN_ASSIST = 0x03,
        AMS_STATUS_MAIN_CALIBRATION = 0x04,
        AMS_STATUS_MAIN_SELF_CHECK = 0x10,
        AMS_STATUS_MAIN_DEBUG = 0x20,
        AMS_STATUS_MAIN_UNKNOWN = 0xFF,
    };

    typedef struct BambuDeckPrinterStatus
    {
        char print_type[32];
        int home_flag;
        int hw_switch_state;
        int mc_left_time;
        int mc_print_percent;
        int mc_print_sub_stage;
        int mc_print_stage;
        int mc_print_error_code;
        int mc_print_line_number;
        int print_error;
        char printer_type[32];
        char subtask_name[32];
        int current_layer;
        int total_layers;
        int print_status;
        int queue_number;
        int gcode_file_prepare_percent;
        char obj_subtask_id[32];
        char project_id_[32];
        char profile_id_[32];
        char subtask_id_[32];
        char gcode_file[128];
        int plate_index;
        char task_id[32];
        double bed_temper;
        double bed_target_temper;
        double frame_temp;
        double nozzle_temper;
        double nozzle_target_temper;
        double nozzle_temper_2;
        double nozzle_target_temper_2;
        int active_nozzle;
        char nozzle_type[32];
        double chamber_temper;
        int wifi_signal;
        int cooling_fan_speed;
        int big_fan1_speed;
        int big_fan2_speed;
        int printing_speed_lvl;
        int printing_speed_mag;
        bool chamberLed;
        float nozzle_diameter;
        bool camera_recording_when_printing;
        bool camera_timelapse;
        bool has_ipcam;
        long int ams_exist_bits;
        bool ams;
        int ams_status_sub;
        int ams_status_main;
        long int ams_version;
        bool ams_support_use_ams;
        int ams_rfid_status;
        int ams_humidity;
        int ams_user_setting_hold_count;
        bool ams_insert_flag;
        bool ams_power_on_flag;
        bool ams_calibrate_remain_flag;
        bool ams_support_virtual_tray;
        bool is_ams_need_update;
        long tray_exist_bits;
        long tray_is_bbl_bits;
        long tray_read_done_bits;
        long tray_reading_bits;
        int m_ams_id;   // local ams  : "0" ~ "3"
        int m_tray_id;  // local tray id : "0" ~ "3"
        int m_tray_now; // tray_now : "0" ~ "15" or "254", "255"
        int m_tray_tar; // tray_tar : "0" ~ "15" or "255"
    } BambuDeckBambuStatus;

    extern BambuDeckBambuStatus bambuStatus;

    enum BambuDeckControlAxis
    {
        ControlAxisXY,
        ControlAxisZ,
    };

    typedef struct BambuDeckControlModeStruct
    {
        int inc;
        int axis;
        int target_bed_temper;
        int target_nozzle_temper;
    } BambuDeckControlMode;

    extern BambuDeckControlMode controlMode;

    typedef struct BambuDeckTouchConfigStruct
    {
        bool auxFanEnabled;
        bool chamberFanEnabled;
        bool chamberSensorEnabled;
        int8_t chamberSensorReadingDiff;
        bool otaEnabled;
        bool tftFlip;
        bool tftInvert;
        char wifiSsid[64];
        char wifiPassword[64];
        char printerModel[32];
        char printerName[32];
        char haUrl[128];
        char haToken[384];
        char localIP[24];
        int backlightLevel;
        int tftOffValue;
        bool wakeOnPrint;
        bool lightMode;
        int currentScreenIndex;
    } BambuDeckConfig;

    extern BambuDeckConfig bambuDeckConfig;

    /* HMS */
    enum ModuleID
    {
        MODULE_UKNOWN = 0x00,
        MODULE_01 = 0x01,
        MODULE_02 = 0x02,
        MODULE_MC = 0x03,
        MODULE_04 = 0x04,
        MODULE_MAINBOARD = 0x05,
        MODULE_06 = 0x06,
        MODULE_AMS = 0x07,
        MODULE_TH = 0x08,
        MODULE_09 = 0x09,
        MODULE_10 = 0x0A,
        MODULE_11 = 0x0B,
        MODULE_XCAM = 0x0C,
        MODULE_13 = 0x0D,
        MODULE_14 = 0x0E,
        MODULE_15 = 0x0F,
        MODULE_MAX = 0x10
    };

    enum HMSMessageLevel
    {
        HMS_UNKNOWN = 0,
        HMS_FATAL = 1,
        HMS_SERIOUS = 2,
        HMS_COMMON = 3,
        HMS_INFO = 4,
        HMS_MSG_LEVEL_MAX,
    };

    typedef struct
    {
        int module_id;
        unsigned module_num;
        unsigned part_id;
        unsigned reserved;
        int msg_level;
        int msg_code;
    } HMSItem;

    typedef struct
    {
        char subtask_id[32];
        int print_error;
    } ClearErrorMessage;

#ifdef __cplusplus
}
#endif

#endif
