#include "bmc-config.h"
#include "bmc-common.h"
#include "bmc-config-api.h"
#include "bmc-diff.h"
#include "bmc-map.h"
#include "bmc-sections.h"

static int
serial_conf_checkout (ipmi_device_t dev,
		      uint8_t *basic_mode,
		      uint8_t *ppp_mode,
		      uint8_t *terminal_mode,
		      uint8_t *connect_mode)
{
  uint8_t tmp_basic_mode;
  uint8_t tmp_ppp_mode;
  uint8_t tmp_terminal_mode;
  uint8_t tmp_connect_mode;
  int ret;

  ret = get_bmc_serial_conf_connection_mode (dev,
					     &tmp_basic_mode,
					     &tmp_ppp_mode,
					     &tmp_terminal_mode,
					     &tmp_connect_mode);

  if (ret != 0)
    return -1;

  if (basic_mode)
    *basic_mode = tmp_basic_mode;

  if (ppp_mode)
    *ppp_mode = tmp_ppp_mode;

  if (terminal_mode)
    *terminal_mode = tmp_terminal_mode;

  if (connect_mode)
    *connect_mode = tmp_connect_mode;

  return 0;
}


static int
serial_conf_commit (ipmi_device_t dev,
		    uint8_t *basic_mode,
		    uint8_t *ppp_mode,
		    uint8_t *terminal_mode,
		    uint8_t *connect_mode)
{
  uint8_t tmp_basic_mode;
  uint8_t tmp_ppp_mode;
  uint8_t tmp_terminal_mode;
  uint8_t tmp_connect_mode;
  int ret;

  ret = get_bmc_serial_conf_connection_mode (dev,
					     &tmp_basic_mode,
					     &tmp_ppp_mode,
					     &tmp_terminal_mode,
					     &tmp_connect_mode);

  if (ret != 0)
    return -1;

  if (basic_mode)
    tmp_basic_mode = *basic_mode;

  if (ppp_mode)
    tmp_ppp_mode = *ppp_mode;

  if (terminal_mode)
    tmp_terminal_mode = *terminal_mode;

  if (connect_mode)
    tmp_connect_mode = *connect_mode;

  ret = set_bmc_serial_conf_connection_mode (dev,
					     tmp_basic_mode,
					     tmp_ppp_mode,
					     tmp_terminal_mode,
					     tmp_connect_mode);
  return 0;
}

static int
enable_basic_mode_checkout (const struct bmc_config_arguments *args,
			    const struct section *sect,
			    struct keyvalue *kv)
{
  int ret;
  uint8_t value;

  ret = serial_conf_checkout (args->dev,
			      &value,
			      NULL,
			      NULL,
			      NULL);

  if (ret != 0)
    return -1;

  if (kv->value)
    free (kv->value);

  if (value)
    {
      if (!(kv->value = strdup ("Yes")))
        {
          perror("strdup");
          exit(1);
        }
    }
  else
    {
      if (!(kv->value = strdup ("No")))
        {
          perror("strdup");
          exit(1);
        }
    }

  return 0;
}

static int
enable_basic_mode_commit (const struct bmc_config_arguments *args,
			  const struct section *sect,
			  const struct keyvalue *kv)
{
  uint8_t value;
  value = (same (kv->value, "yes") ? 1 : 0);

  return serial_conf_commit (args->dev,
			     &value, NULL, NULL, NULL);
}

static int
enable_basic_mode_diff (const struct bmc_config_arguments *args,
			const struct section *sect,
			const struct keyvalue *kv)
{
  int ret;
  uint8_t get_value;
  uint8_t passed_value;

  ret = serial_conf_checkout (args->dev,
			      &get_value,
			      NULL,
			      NULL,
			      NULL);

  if (ret != 0)
    return -1;

  passed_value = same (kv->value, "yes");

  if (passed_value == get_value)
    ret = 0;
  else 
    {
      ret = 1;
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   get_value ? "Yes" : "No");
    }
  return ret;
}

static int
enable_basic_mode_validate (const struct bmc_config_arguments *args,
			    const struct section *sect,
			    const char *value)
{
  return (value && (same (value, "yes") || same (value, "no"))) ? 0 : 1;
}


/* ppp */

static int
enable_ppp_mode_checkout (const struct bmc_config_arguments *args,
			  const struct section *sect,
			  struct keyvalue *kv)
{
  int ret;
  uint8_t value;

  ret = serial_conf_checkout (args->dev,
			      NULL,
			      &value,
			      NULL,
			      NULL);

  if (ret != 0)
    return -1;

  if (kv->value)
    free (kv->value);

  if (value)
    {
      if (!(kv->value = strdup ("Yes")))
        {
          perror("strdup");
          exit(1);
        }
    }
  else
    {
      if (!(kv->value = strdup ("No")))
        {
          perror("strdup");
          exit(1);
        }
    }

  return 0;
}

static int
enable_ppp_mode_commit (const struct bmc_config_arguments *args,
			const struct section *sect,
			const struct keyvalue *kv)
{
  uint8_t value;
  value = (same (kv->value, "yes") ? 1 : 0);

  return serial_conf_commit (args->dev,
			     NULL, &value, NULL, NULL);
}

static int
enable_ppp_mode_diff (const struct bmc_config_arguments *args,
		      const struct section *sect,
		      const struct keyvalue *kv)
{
  int ret;
  uint8_t get_value;
  uint8_t passed_value;

  ret = serial_conf_checkout (args->dev,
			      NULL,
			      &get_value,
			      NULL,
			      NULL);

  if (ret != 0)
    return -1;

  passed_value = same (kv->value, "yes");

  if (passed_value == get_value)
    ret = 0;
  else 
    {
      ret = 1;
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   get_value ? "Yes" : "No");
    }
  return ret;
}

static int
enable_ppp_mode_validate (const struct bmc_config_arguments *args,
			  const struct section *sect,
			  const char *value)
{
  return (value && (same (value, "yes") || same (value, "no"))) ? 0 : 1;
}

/* terminal */

static int
enable_terminal_mode_checkout (const struct bmc_config_arguments *args,
			       const struct section *sect,
			       struct keyvalue *kv)
{
  int ret;
  uint8_t value;

  ret = serial_conf_checkout (args->dev,
			      NULL,
			      NULL,
			      &value,
			      NULL);

  if (ret != 0)
    return -1;

  if (kv->value)
    free (kv->value);

  if (value)
    {
      if (!(kv->value = strdup ("Yes")))
        {
          perror("strdup");
          exit(1);
        }
    }
  else
    {
      if (!(kv->value = strdup ("No")))
        {
          perror("strdup");
          exit(1);
        }
    }

  return 0;
}

static int
enable_terminal_mode_commit (const struct bmc_config_arguments *args,
			     const struct section *sect,
			     const struct keyvalue *kv)
{
  uint8_t value;
  value = (same (kv->value, "yes") ? 1 : 0);

  return serial_conf_commit (args->dev,
			     NULL, NULL, &value, NULL);
}

static int
enable_terminal_mode_diff (const struct bmc_config_arguments *args,
			   const struct section *sect,
			   const struct keyvalue *kv)
{
  int ret;
  uint8_t get_value;
  uint8_t passed_value;

  ret = serial_conf_checkout (args->dev,
			      NULL,
			      NULL,
			      &get_value,
			      NULL);

  if (ret != 0)
    return -1;

  passed_value = same (kv->value, "yes");

  if (passed_value == get_value)
    ret = 0;
  else 
    {
      ret = 1;
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   get_value ? "Yes" : "No");
    }
  return ret;
}

static int
enable_terminal_mode_validate (const struct bmc_config_arguments *args,
			       const struct section *sect,
			       const char *value)
{
  return (value && (same (value, "yes") || same (value, "no"))) ? 0 : 1;
}



static int
connect_mode_checkout (const struct bmc_config_arguments *args,
		       const struct section *sect,
		       struct keyvalue *kv)
{
  int ret;
  uint8_t value;

  ret = serial_conf_checkout (args->dev,
			      NULL,
			      NULL,
			      NULL,
			      &value);

  if (ret != 0)
    return -1;

  if (kv->value)
    free (kv->value);

  kv->value = connect_mode_string (value);

  return 0;
}

static int
connect_mode_commit (const struct bmc_config_arguments *args,
		     const struct section *sect,
		     const struct keyvalue *kv)
{
  uint8_t value;
  value = connect_mode_number (kv->value);

  return serial_conf_commit (args->dev,
			     NULL, NULL, NULL, &value);
}

static int
connect_mode_diff (const struct bmc_config_arguments *args,
		   const struct section *sect,
		   const struct keyvalue *kv)
{
  int ret;
  uint8_t get_value;
  uint8_t passed_value;

  ret = serial_conf_checkout (args->dev,
			      NULL,
			      NULL,
			      NULL,
			      &get_value);

  if (ret != 0)
    return -1;

  passed_value = connect_mode_number (kv->value);
  if (passed_value == get_value)
    ret = 0;
  else 
    {
      ret = 1;
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   connect_mode_string (get_value));
    }
  return ret;
}

static int
connect_mode_validate (const struct bmc_config_arguments *args,
		       const struct section *sect,
		       const char *value)
{
  return (connect_mode_number (value) != -1) ? 0 : 1;
}


static int
page_blackout_interval_checkout (const struct bmc_config_arguments *args,
				 const struct section *sect,
				 struct keyvalue *kv)
{
  int ret;
  uint8_t interval;

  ret = get_bmc_serial_conf_page_blackout_interval (args->dev,
						    &interval);

  if (ret != 0)
    return -1;
  
  if (kv->value)
    free (kv->value);

  asprintf (&kv->value, "%d", interval);

  return 0;
}

static int
page_blackout_interval_commit (const struct bmc_config_arguments *args,
			       const struct section *sect,
			       const struct keyvalue *kv)
{
  return set_bmc_serial_conf_page_blackout_interval (args->dev,
						     atoi (kv->value));
}

static int
page_blackout_interval_diff (const struct bmc_config_arguments *args,
			     const struct section *sect,
			     const struct keyvalue *kv)
{
  uint8_t interval;
  int passed_interval;
  int ret;

  ret = get_bmc_serial_conf_page_blackout_interval (args->dev,
						    &interval);

  if (ret != 0)
    return -1;

  passed_interval = atoi (kv->value);

  if (passed_interval == interval)
    ret = 0;
  else 
    {
      char num[32];
      ret = 1;
      sprintf (num, "%d", interval);
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   num);
    }
  return ret;
}

static int
page_blackout_interval_validate (const struct bmc_config_arguments *args,
				 const struct section *sect,
				 const char *value)
{
  char *endptr;
  int num =  strtol (value, &endptr, 0);
  if (*endptr)
    return 1;
  if (num < 0 || num > 255)
    return 1;
  return 0;
}


/* retry time */

static int
call_retry_interval_checkout (const struct bmc_config_arguments *args,
			      const struct section *sect,
			      struct keyvalue *kv)
{
  int ret;
  uint8_t interval;

  ret = get_bmc_serial_conf_call_retry_interval (args->dev,
                                                 &interval);

  if (ret != 0)
    return -1;
  
  if (kv->value)
    free (kv->value);

  asprintf (&kv->value, "%d", interval);

  return 0;
}

static int
call_retry_interval_commit (const struct bmc_config_arguments *args,
			    const struct section *sect,
			    const struct keyvalue *kv)
{
  return set_bmc_serial_conf_call_retry_interval (args->dev,
                                                  atoi (kv->value));
}

static int
call_retry_interval_diff (const struct bmc_config_arguments *args,
			  const struct section *sect,
			  const struct keyvalue *kv)
{
  uint8_t interval;
  int passed_interval;
  int ret;

  ret = get_bmc_serial_conf_call_retry_interval (args->dev,
                                                 &interval);

  if (ret != 0)
    return -1;

  passed_interval = atoi (kv->value);

  if (passed_interval == interval)
    ret = 0;
  else 
    {
      char num[32];
      ret = 1;
      sprintf (num, "%d", interval);
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   num);
    }
  return ret;
}

static int
call_retry_interval_validate (const struct bmc_config_arguments *args,
			      const struct section *sect,
			      const char *value)
{
  char *endptr;
  int num =  strtol (value, &endptr, 0);
  if (*endptr)
    return 1;
  if (num < 0 || num > 255)
    return 1;
  return 0;
}

static int
serial_conf_comm_checkout (ipmi_device_t dev,
			   uint8_t *dtr_hangup,
			   uint8_t *flow_control,
			   uint8_t *bit_rate)
{
  int ret;
  uint8_t tmp_dtr_hangup;
  uint8_t tmp_flow_control;
  uint8_t tmp_bit_rate;

  ret = get_bmc_serial_conf_ipmi_messaging_comm_settings (dev,
							  &tmp_dtr_hangup,
							  &tmp_flow_control,
							  &tmp_bit_rate);

  if (ret != 0)
    return -1;

  if (dtr_hangup)
    *dtr_hangup = tmp_dtr_hangup;

  if (flow_control)
    *flow_control = tmp_flow_control;

  if (bit_rate)
    *bit_rate = tmp_bit_rate;

  return 0;
}

static int
serial_conf_comm_commit (ipmi_device_t dev,
			 uint8_t *dtr_hangup,
			 uint8_t *flow_control,
			 uint8_t *bit_rate)
{
  int ret;
  uint8_t tmp_dtr_hangup;
  uint8_t tmp_flow_control;
  uint8_t tmp_bit_rate;

  ret = get_bmc_serial_conf_ipmi_messaging_comm_settings (dev,
							  &tmp_dtr_hangup,
							  &tmp_flow_control,
							  &tmp_bit_rate);

  if (ret != 0)
    return -1;

  if (dtr_hangup)
    tmp_dtr_hangup = *dtr_hangup;
  if (flow_control)
    tmp_flow_control = *flow_control;
  if (bit_rate)
    tmp_bit_rate = *bit_rate;

  return set_bmc_serial_conf_ipmi_messaging_comm_settings (dev,
							   tmp_dtr_hangup,
							   tmp_flow_control,
							   tmp_bit_rate);
}

static int
enable_dtr_hangup_checkout (const struct bmc_config_arguments *args,
			    const struct section *sect,
			    struct keyvalue *kv)
{
  int ret;
  uint8_t value;
  
  ret = serial_conf_comm_checkout (args->dev,
				   &value,
				   NULL,
				   NULL);

  if (ret != 0)
    return -1;

  if (kv->value)
    free (kv->value);

  if (value)
    {
      if (!(kv->value = strdup ("Yes")))
        {
          perror("strdup");
          exit(1);
        }
    }
  else
    {
      if (!(kv->value = strdup ("No")))
        {
          perror("strdup");
          exit(1);
        }
    }

  return 0;
}

static int
enable_dtr_hangup_commit (const struct bmc_config_arguments *args,
			  const struct section *sect,
			  const struct keyvalue *kv)
{
  uint8_t value = same (kv->value, "yes");

  return serial_conf_comm_commit (args->dev,
				  &value,
				  NULL,
				  NULL);
}

static int
enable_dtr_hangup_diff (const struct bmc_config_arguments *args,
			const struct section *sect,
			const struct keyvalue *kv)
{
  uint8_t passed_value;
  uint8_t got_value;
  int ret;

  ret = serial_conf_comm_checkout (args->dev,
				   &got_value,
				   NULL,
				   NULL);

  if (ret != 0)
    return -1;

  passed_value = same (kv->value, "yes") ? 1 : 0;

  if (passed_value == got_value)
    ret = 0;
  else 
    {
      ret = 1;
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   got_value ? "Yes" : "No");
    }
  return ret;
}

static int
enable_dtr_hangup_validate (const struct bmc_config_arguments *args,
			    const struct section *sect,
			    const char *value)
{
  return (value && (same (value, "yes") || same (value, "no"))) ? 0 : 1;
}

static int
flow_control_checkout (const struct bmc_config_arguments *args,
		       const struct section *sect,
		       struct keyvalue *kv)
{
  int ret;
  uint8_t value;
  
  ret = serial_conf_comm_checkout (args->dev,
				   NULL,
				   &value,
				   NULL);

  if (ret != 0)
    return -1;

  if (kv->value)
    free (kv->value);

  if (!(kv->value = strdup (flow_control_string (value))))
    {
      perror("strdup");
      exit(1);
    }

  return 0;
}

static int
flow_control_commit (const struct bmc_config_arguments *args,
		     const struct section *sect,
		     const struct keyvalue *kv)
{
  uint8_t value = flow_control_number (kv->value);
  return serial_conf_comm_commit (args->dev,
				  NULL,
				  &value,
				  NULL);
}

static int
flow_control_diff (const struct bmc_config_arguments *args,
		   const struct section *sect,
		   const struct keyvalue *kv)
{
  uint8_t passed_value;
  uint8_t got_value;
  int ret;

  ret = serial_conf_comm_checkout (args->dev,
				   NULL,
				   &got_value,
				   NULL);

  if (ret != 0)
    return -1;

  passed_value = flow_control_number (kv->value);

  if (passed_value == got_value)
    ret = 0;
  else 
    {
      ret = 1;
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   flow_control_string (got_value));
    }
  return ret;
}

static int
flow_control_validate (const struct bmc_config_arguments *args,
		       const struct section *sect,
		       const char *value)
{
  return (flow_control_number (value) > -1 ) ? 0 : 1;
}

static int
bit_rate_checkout (const struct bmc_config_arguments *args,
		   const struct section *sect,
		   struct keyvalue *kv)
{
  int ret;
  uint8_t value;
  
  ret = serial_conf_comm_checkout (args->dev,
				   NULL,
				   NULL,
				   &value);

  if (ret != 0)
    return -1;

  if (kv->value)
    free (kv->value);

  if (!(kv->value = strdup (bit_rate_string (value))))
    {
      perror("strdup");
      exit(1);
    }

  return 0;
}

static int
bit_rate_commit (const struct bmc_config_arguments *args,
		 const struct section *sect,
		 const struct keyvalue *kv)
{
  uint8_t value = bit_rate_number (kv->value);
  return serial_conf_comm_commit (args->dev,
				  NULL,
				  NULL,
				  &value);
}

static int
bit_rate_diff (const struct bmc_config_arguments *args,
	       const struct section *sect,
	       const struct keyvalue *kv)
{
  uint8_t passed_value;
  uint8_t got_value;
  int ret;

  ret = serial_conf_comm_checkout (args->dev,
				   NULL,
				   NULL,
				   &got_value);

  if (ret != 0)
    return -1;

  passed_value = bit_rate_number (kv->value);

  if (passed_value == got_value)
    ret = 0;
  else 
    {
      ret = 1;
      report_diff (sect->section,
                   kv->key,
                   kv->value,
                   bit_rate_string (got_value));
    }
  return ret;
}

static int
bit_rate_validate (const struct bmc_config_arguments *args,
		   const struct section *sect,
		   const char *value)
{
  return (bit_rate_number (value) > -1 ) ? 0 : 1;
}

struct section *
bmc_serial_conf_section_get (struct bmc_config_arguments *args)
{
  struct section *bmc_serial_conf_section = NULL;

  if (!(bmc_serial_conf_section = (void *) calloc (1, sizeof (struct section))))
    {
      perror("calloc");
      exit(1);
    }

  if (!(bmc_serial_conf_section->section = strdup ("Serial_Conf")))
    {
      perror("strdup");
      exit(1);
    }

  add_keyvalue (bmc_serial_conf_section,
		"Enable_Basic_Mode",
		"Possible values: Yes/No",
                0,
		enable_basic_mode_checkout,
		enable_basic_mode_commit,
		enable_basic_mode_diff,
		enable_basic_mode_validate);

  add_keyvalue (bmc_serial_conf_section,
		"Enable_PPP_Mode",
		"Possible values: Yes/No",
                0,
		enable_ppp_mode_checkout,
		enable_ppp_mode_commit,
		enable_ppp_mode_diff,
		enable_ppp_mode_validate);

  add_keyvalue (bmc_serial_conf_section,
		"Enable_Terminal_Mode",
		"Possible values: Yes/No",
                0,
		enable_terminal_mode_checkout,
		enable_terminal_mode_commit,
		enable_terminal_mode_diff,
		enable_terminal_mode_validate);

  add_keyvalue (bmc_serial_conf_section,
		"Connect_Mode",
		"Possible values: Modem_Connect/Direct_Mode",
                0,
		connect_mode_checkout,
		connect_mode_commit,
		connect_mode_diff,
		connect_mode_validate);

  add_keyvalue (bmc_serial_conf_section,
		"Page_Blackout_Interval",
		"Give a valid number",
                0,
		page_blackout_interval_checkout,
		page_blackout_interval_commit,
		page_blackout_interval_diff,
		page_blackout_interval_validate);


  add_keyvalue (bmc_serial_conf_section,
		"Call_Retry_Interval",
		"Give a valid number",
                0,
		call_retry_interval_checkout,
		call_retry_interval_commit,
		call_retry_interval_diff,
		call_retry_interval_validate);

  /* achu: For backwards compatability to bmc-config in 0.2.0 */
  add_keyvalue (bmc_serial_conf_section,
		"Call_Retry_Time",
		"Give a valid number",
                BMC_DO_NOT_CHECKOUT,
		call_retry_interval_checkout,
		call_retry_interval_commit,
		call_retry_interval_diff,
		call_retry_interval_validate);

  add_keyvalue (bmc_serial_conf_section,
		"Enable_DTR_Hangup",
		"Possible values: Yes/No",
                0,
		enable_dtr_hangup_checkout,
		enable_dtr_hangup_commit,
		enable_dtr_hangup_diff,
		enable_dtr_hangup_validate);

  add_keyvalue (bmc_serial_conf_section,
		"Flow_Control",
		"Possible values: No_Flow_Control/RTS_CTS/XON_XOFF",
                0,
		flow_control_checkout,
		flow_control_commit,
		flow_control_diff,
		flow_control_validate);

  add_keyvalue (bmc_serial_conf_section,
		"Bit_Rate",
		"Possible values: 9600/19200/38400/57600/115200",
                0,
		bit_rate_checkout,
		bit_rate_commit,
		bit_rate_diff,
		bit_rate_validate);

  return bmc_serial_conf_section;
}
