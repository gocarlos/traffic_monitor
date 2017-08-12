

IF(CREATE_SNAP_APP)
  SET(INSTALL_DIRECTORE_SETTINGS "/snap/traffic-monitor/current/traffic-monitor/")
  MESSAGE("Going to move the settings files to ${INSTALL_DIRECTORE_SETTINGS}")
  FILE(WRITE ${CMAKE_CURRENT_LIST_DIR}/../include/traffic_monitor/paths.h
    "// Auto-generated headers file, do not edit, will be overwritten by cmake.
    // (c) 2017 Vigilatore \n
    #ifndef INCLUDE_TRAFFIC_MONITOR_CONFIG_H_
    #define INCLUDE_TRAFFIC_MONITOR_CONFIG_H_

    #include <string>

    const std::string prefix_to_settings =
    \"${INSTALL_DIRECTORE_SETTINGS}/settings/\";
    const std::string prefix_to_webserver =
    \"${INSTALL_DIRECTORE_SETTINGS}/webserver/\";

  #endif  // INCLUDE_TRAFFIC_MONITOR_CONFIG_H_\n
")
ELSE()
  SET(INSTALL_DIRECTORE_SETTINGS "${CMAKE_CURRENT_LIST_DIR}/../")
  MESSAGE("Going to move the settings files to ${INSTALL_DIRECTORE_SETTINGS}")

  FILE(WRITE ${CMAKE_CURRENT_LIST_DIR}/../include/traffic_monitor/paths.h
    "// Auto-generated headers file, do not edit, will be overwritten by cmake.
    // (c) 2017 Vigilatore \n
    #ifndef INCLUDE_TRAFFIC_MONITOR_CONFIG_H_
    #define INCLUDE_TRAFFIC_MONITOR_CONFIG_H_

    #include <string>

    const std::string prefix_to_settings =
    \"${INSTALL_DIRECTORE_SETTINGS}/settings/\";
    const std::string prefix_to_webserver =
    \"${INSTALL_DIRECTORE_SETTINGS}/webserver/\";

  #endif  // INCLUDE_TRAFFIC_MONITOR_CONFIG_H_\n
")
ENDIF(CREATE_SNAP_APP)
