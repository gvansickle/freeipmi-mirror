#ifndef _IPMI_PEF_WRAPPER_H
#define _IPMI_PEF_WRAPPER_H

struct pef_event_filter_table 
{
  int filter_number;
  int filter_type;
  int enable_filter;
  int event_filter_action_alert;
  int event_filter_action_power_off;
  int event_filter_action_reset;
  int event_filter_action_power_cycle;
  int event_filter_action_oem;
  int event_filter_action_diagnostic_interrupt;
  int event_filter_action_group_control_operation;
  int alert_policy_number;
  int group_control_selector;
  int event_severity;
  int generator_id_byte1;
  int generator_id_byte2;
  int sensor_type;
  int sensor_number;
  int event_trigger;
  int event_data1_offset_mask;
  int event_data1_AND_mask;
  int event_data1_compare1;
  int event_data1_compare2;
  int event_data2_AND_mask;
  int event_data2_compare1;
  int event_data2_compare2;
  int event_data3_AND_mask;
  int event_data3_compare1;
  int event_data3_compare2;
};
typedef struct pef_event_filter_table pef_event_filter_table_t;

struct pef_alert_policy_table
{
  int alert_policy_entry_number;
  int policy_type;
  int policy_enabled;
  int policy_number;
  int destination_selector;
  int channel_number;
  int alert_string_set_selector;
  int event_specific_alert_string_lookup;
};
typedef struct pef_alert_policy_table pef_alert_policy_table_t;

struct lan_alert_destination
{
  int destination_selector;
  int destination_type;
  int alert_acknowledge;
  int alert_acknowledge_timeout;
  int alert_retries;
  int gateway_selector;
  char alert_ip_address[16];
  char alert_mac_address[18];
};
typedef struct lan_alert_destination lan_alert_destination_t;

pef_err_t get_bmc_lan_conf_community_string (ipmi_pef_state_data_t *state_data,
                                             uint8_t *community_string,
                                             uint32_t community_string_len);

pef_err_t set_bmc_lan_conf_community_string (ipmi_pef_state_data_t *state_data,
                                             uint8_t *community_string);

pef_err_t get_bmc_lan_conf_destination_type(ipmi_pef_state_data_t *state_data,
                                            uint8_t destination_selector,
                                            uint8_t *alert_destination_type,
                                            uint8_t *alert_acknowledge,
                                            uint8_t *alert_acknowledge_timeout,
                                            uint8_t *alert_retries);

pef_err_t set_bmc_lan_conf_destination_type(ipmi_pef_state_data_t *state_data,
                                            uint8_t destination_selector,
                                            uint8_t alert_destination_type,
                                            uint8_t alert_acknowledge,
                                            uint8_t alert_acknowledge_timeout,
                                            uint8_t alert_retries);

pef_err_t get_bmc_lan_conf_destination_addresses(ipmi_pef_state_data_t *state_data,
                                                 uint8_t destination_selector,
                                                 uint8_t *alert_gateway,
                                                 char *alert_ip_address,
                                                 unsigned int alert_ip_address_len,
                                                 char *alert_mac_address,
                                                 unsigned int alert_mac_address_len);

pef_err_t set_bmc_lan_conf_destination_addresses(ipmi_pef_state_data_t *state_data,
                                                 uint8_t destination_selector,
                                                 uint8_t alert_gateway,
                                                 char *alert_ip_address,
                                                 char *alert_mac_address);

pef_err_t get_bmc_pef_conf_alert_policy_table (struct ipmi_pef_state_data *state_data, 
                                               uint8_t alert_policy_entry_number,
                                               uint8_t *policy_number_type,
                                               uint8_t *policy_number_enabled,
                                               uint8_t *policy_number,
                                               uint8_t *destination_selector,
                                               uint8_t *channel_number,
                                               uint8_t *alert_string_set_selector,
                                               uint8_t *event_specific_alert_string_lookup);

pef_err_t set_bmc_pef_conf_alert_policy_table (struct ipmi_pef_state_data *state_data, 
                                               uint8_t alert_policy_entry_number,
                                               uint8_t policy_number_type,
                                               uint8_t policy_number_enabled,
                                               uint8_t policy_number,
                                               uint8_t destination_selector,
                                               uint8_t channel_number,
                                               uint8_t alert_string_set_selector,
                                               uint8_t event_specific_alert_string_lookup);

pef_err_t get_bmc_pef_conf_event_filter_table (struct ipmi_pef_state_data *state_data, 
                                               uint8_t filter_number,
                                               uint8_t *filter_type,
                                               uint8_t *enable_filter,
                                               uint8_t *event_filter_action_alert,
                                               uint8_t *event_filter_action_power_off,
                                               uint8_t *event_filter_action_reset,
                                               uint8_t *event_filter_action_power_cycle,
                                               uint8_t *event_filter_action_oem,
                                               uint8_t *event_filter_action_diagnostic_interrupt,
                                               uint8_t *event_filter_action_group_control_operation,
                                               uint8_t *alert_policy_number,
                                               uint8_t *group_control_selector,
                                               uint8_t *event_severity,
                                               uint8_t *generator_id_byte1,
                                               uint8_t *generator_id_byte2,
                                               uint8_t *sensor_type,
                                               uint8_t *sensor_number,
                                               uint8_t *event_trigger,
                                               uint8_t *event_data1_offset_mask,
                                               uint8_t *event_data1_AND_mask,
                                               uint8_t *event_data1_compare1,
                                               uint8_t *event_data1_compare2,
                                               uint8_t *event_data2_AND_mask,
                                               uint8_t *event_data2_compare1,
                                               uint8_t *event_data2_compare2,
                                               uint8_t *event_data3_AND_mask,
                                               uint8_t *event_data3_compare1,
                                               uint8_t *event_data3_compare2);

pef_err_t set_bmc_pef_conf_event_filter_table (struct ipmi_pef_state_data *state_data, 
                                               uint8_t filter_number,
                                               uint8_t filter_type,
                                               uint8_t enable_filter,
                                               uint8_t event_filter_action_alert,
                                               uint8_t event_filter_action_power_off,
                                               uint8_t event_filter_action_reset,
                                               uint8_t event_filter_action_power_cycle,
                                               uint8_t event_filter_action_oem,
                                               uint8_t event_filter_action_diagnostic_interrupt,
                                               uint8_t event_filter_action_group_control_operation,
                                               uint8_t alert_policy_number,
                                               uint8_t group_control_selector,
                                               uint8_t event_severity,
                                               uint8_t generator_id_byte1,
                                               uint8_t generator_id_byte2,
                                               uint8_t sensor_type,
                                               uint8_t sensor_number,
                                               uint8_t event_trigger,
                                               uint8_t event_data1_offset_mask,
                                               uint8_t event_data1_AND_mask,
                                               uint8_t event_data1_compare1,
                                               uint8_t event_data1_compare2,
                                               uint8_t event_data2_AND_mask,
                                               uint8_t event_data2_compare1,
                                               uint8_t event_data2_compare2,
                                               uint8_t event_data3_AND_mask,
                                               uint8_t event_data3_compare1,
                                               uint8_t event_data3_compare2);

int get_bmc_community_string (struct ipmi_pef_state_data *state_data,
                              uint8_t *community_string,
                              uint32_t community_string_len);

int get_community_string (struct ipmi_pef_state_data *state_data,
                          FILE *fp,
                          uint8_t *community_string, 
                          uint32_t community_string_len);

int set_bmc_community_string (struct ipmi_pef_state_data *state_data,
                              uint8_t *community_string);

int get_alert_policy_table (struct ipmi_pef_state_data *state_data, 
			    int policy_number, 
			    pef_alert_policy_table_t *apt);

int get_alert_policy_table_list (FILE *fp, pef_alert_policy_table_t **apt_list, int *count);

int set_alert_policy_table (struct ipmi_pef_state_data *state_data, pef_alert_policy_table_t *apt);


int get_event_filter_table (struct ipmi_pef_state_data *state_data, 
			    int filter, 
			    pef_event_filter_table_t *eft);

int get_event_filter_table_list (FILE *fp, pef_event_filter_table_t **eft_list, int *count);

int set_event_filter_table (struct ipmi_pef_state_data *state_data, pef_event_filter_table_t *eft);

int get_lan_alert_destination (struct ipmi_pef_state_data *state_data, 
			       int destination_selector, 
			       lan_alert_destination_t *lad);

int get_lan_alert_destination_list (FILE *fp, lan_alert_destination_t **lad_list, int *count);

int set_lan_alert_destination (struct ipmi_pef_state_data *state_data, 
			       lan_alert_destination_t *lad);

#endif
