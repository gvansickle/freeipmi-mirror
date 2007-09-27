#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#include <assert.h>

#include "pef-config.h"
#include "pef-config-map.h"
#include "pef-config-utils.h"
#include "pef-config-validate.h"

/* achu: presumably there is no maximum.  We could read/write blocks
   forever based on block numbers.  However, we need to have some
   artificial max for the sake of pef-config.
*/
#define PEF_ALERT_STRING_MAX_LEN 64

struct alert_string_keys {
  uint8_t event_filter_number;
  uint8_t alert_string_set;
};

static config_err_t
_get_alert_string_keys (pef_config_state_data_t *state_data,
                        const char *section_name,
                        struct alert_string_keys *ask)
{
  fiid_obj_t obj_cmd_rs = NULL;
  uint8_t string_selector;
  uint64_t val = 0;
  config_err_t rv = CONFIG_ERR_FATAL_ERROR;
  
  assert(state_data);
  assert(section_name);
  assert(ask);

  string_selector = atoi (section_name + strlen ("Alert_String_"));

  if (!(obj_cmd_rs = Fiid_obj_create(tmpl_cmd_get_pef_configuration_parameters_alert_string_keys_rs)))
    goto cleanup;

  if (ipmi_cmd_get_pef_configuration_parameters_alert_string_keys (state_data->dev,
                                                                   IPMI_GET_PEF_PARAMETER,
                                                                   string_selector,
                                                                   BLOCK_SELECTOR,
                                                                   obj_cmd_rs) < 0)
    {
      if (state_data->prog_data->args->common.flags & IPMI_FLAGS_DEBUG_DUMP)
        fprintf(stderr,
                "ipmi_cmd_get_pef_configuration_parameters_alert_string_keys: %s\n",
                ipmi_device_strerror(ipmi_device_errnum(state_data->dev)));
      rv = CONFIG_ERR_NON_FATAL_ERROR;
      goto cleanup;
    }

  if (Fiid_obj_get (obj_cmd_rs, "filter_number", &val) < 0)
    goto cleanup;
  ask->event_filter_number = val;

  if (Fiid_obj_get (obj_cmd_rs, "set_number_for_string", &val) < 0)
    goto cleanup;
  ask->alert_string_set = val;

  rv = CONFIG_ERR_SUCCESS;
 cleanup:
  Fiid_obj_destroy(obj_cmd_rs);
  return (rv);
}

static config_err_t
_set_alert_string_keys (pef_config_state_data_t *state_data,
                        const char *section_name,
                        struct alert_string_keys *ask)
{
  fiid_obj_t obj_cmd_rs = NULL;
  uint8_t string_selector;
  config_err_t rv = CONFIG_ERR_FATAL_ERROR;

  assert(state_data);
  assert(section_name);
  assert(ask);

  if (!(obj_cmd_rs = Fiid_obj_create(tmpl_cmd_set_pef_configuration_parameters_rs)))
    goto cleanup;

  if (ipmi_cmd_set_pef_configuration_parameters_alert_string_keys (state_data->dev,
                                                                   string_selector,
                                                                   ask->event_filter_number,
                                                                   ask->alert_string_set,
                                                                   obj_cmd_rs) < 0)
    {
      if (state_data->prog_data->args->common.flags & IPMI_FLAGS_DEBUG_DUMP)
        fprintf(stderr,
                "ipmi_cmd_set_pef_configuration_parameters_alert_string_keys: %s\n",
                ipmi_device_strerror(ipmi_device_errnum(state_data->dev)));
      rv = CONFIG_ERR_NON_FATAL_ERROR;
      goto cleanup;
    }

  rv = CONFIG_ERR_SUCCESS;
 cleanup:
  Fiid_obj_destroy(obj_cmd_rs);
  return (rv);
}

static config_err_t
event_filter_number_checkout (const char *section_name,
                              struct config_keyvalue *kv,
                              void *arg)
{
  pef_config_state_data_t *state_data = (pef_config_state_data_t *)arg;
  struct alert_string_keys ask;
  config_err_t ret;

  if ((ret = _get_alert_string_keys (state_data,
                                     section_name,
                                     &ask)) != CONFIG_ERR_SUCCESS)
    return ret;

  if (config_section_update_keyvalue_output_int(kv, ask.event_filter_number) < 0)
    return CONFIG_ERR_FATAL_ERROR;

  return CONFIG_ERR_SUCCESS;
}

static config_err_t
event_filter_number_commit (const char *section_name,
                            const struct config_keyvalue *kv,
                            void *arg)
{
  pef_config_state_data_t *state_data = (pef_config_state_data_t *)arg;
  struct alert_string_keys ask;
  config_err_t ret;

  if ((ret = _get_alert_string_keys (state_data,
                                     section_name,
                                     &ask)) != CONFIG_ERR_SUCCESS)
    return ret;

  ask.event_filter_number = atoi (kv->value_input);
  
  return _set_alert_string_keys (state_data,
                                 section_name,
                                 &ask);
}

static config_err_t
alert_string_set_checkout (const char *section_name,
                           struct config_keyvalue *kv,
                           void *arg)
{
  pef_config_state_data_t *state_data = (pef_config_state_data_t *)arg;
  struct alert_string_keys ask;
  config_err_t ret;

  if ((ret = _get_alert_string_keys (state_data,
                                     section_name,
                                     &ask)) != CONFIG_ERR_SUCCESS)
    return ret;

  if (config_section_update_keyvalue_output_int(kv, ask.alert_string_set) < 0)
    return CONFIG_ERR_FATAL_ERROR;

  return CONFIG_ERR_SUCCESS;
}

static config_err_t
alert_string_set_commit (const char *section_name,
                         const struct config_keyvalue *kv,
                         void *arg)
{
  pef_config_state_data_t *state_data = (pef_config_state_data_t *)arg;
  struct alert_string_keys ask;
  config_err_t ret;

  if ((ret = _get_alert_string_keys (state_data,
                                     section_name,
                                     &ask)) != CONFIG_ERR_SUCCESS)
    return ret;

  ask.alert_string_set = atoi (kv->value_input);
  
  return _set_alert_string_keys (state_data,
                                 section_name,
                                 &ask);
}

static config_err_t
alert_string_checkout (const char *section_name,
                       struct config_keyvalue *kv,
                       void *arg)
{
  pef_config_state_data_t *state_data = (pef_config_state_data_t *)arg;
  uint8_t alert_string[PEF_ALERT_STRING_MAX_LEN+1];
  uint8_t string_selector;
  fiid_obj_t obj_cmd_rs = NULL;
  config_err_t rv = CONFIG_ERR_FATAL_ERROR;
  int blocks;
  int i;

  string_selector = atoi (section_name + strlen ("Alert_String_"));

  memset(alert_string, '\0', PEF_ALERT_STRING_MAX_LEN+1);

  if (!(obj_cmd_rs = Fiid_obj_create(tmpl_cmd_get_pef_configuration_parameters_alert_strings_rs)))
    goto cleanup;

  if (!((PEF_ALERT_STRING_MAX_LEN) % 16))
    blocks = (PEF_ALERT_STRING_MAX_LEN)/16;
  else
    blocks = (PEF_ALERT_STRING_MAX_LEN)/16 + 1;

  for (i = 0; i < blocks; i++)
    {
      Fiid_obj_clear(obj_cmd_rs);
      int j;

      if (ipmi_cmd_get_pef_configuration_parameters_alert_string (state_data->dev,
                                                                  IPMI_GET_PEF_PARAMETER,
                                                                  string_selector,
                                                                  i + 1,
                                                                  obj_cmd_rs) < 0)
        {
          if (state_data->prog_data->args->common.flags & IPMI_FLAGS_DEBUG_DUMP)
            fprintf(stderr,
                    "ipmi_cmd_get_pef_configuration_parameters_alert_string: %s\n",
                    ipmi_device_strerror(ipmi_device_errnum(state_data->dev)));
          rv = CONFIG_ERR_NON_FATAL_ERROR;
          goto cleanup;
        }
      
      /* XXX: Be lazy for now, assume no strings will overflow
       * whatever is passed in, so don't check for overflow errors
       * from Fiid_obj_get_data.
       */
      if (Fiid_obj_get_data (obj_cmd_rs,
                             "string_data",
                             alert_string + (i * 16),
                             PEF_ALERT_STRING_MAX_LEN - (i * 16)) < 0)
        goto cleanup;

      /* Check if we've found a nul character */
      for (j = 0; j < 16; j++)
        {
          if (!((alert_string + (i * 16))[j]))
            goto done;
        }
    }

 done:
  if (config_section_update_keyvalue_output(kv, (char *)alert_string) < 0)
    return CONFIG_ERR_FATAL_ERROR;

  rv = CONFIG_ERR_SUCCESS;
 cleanup:
  Fiid_obj_destroy(obj_cmd_rs);
  return (rv);

}

static config_err_t
alert_string_commit (const char *section_name,
                     const struct config_keyvalue *kv,
                     void *arg)
{ 
  pef_config_state_data_t *state_data = (pef_config_state_data_t *)arg;
  uint8_t string_selector;
  fiid_obj_t obj_cmd_rs = NULL;
  config_err_t rv = CONFIG_ERR_FATAL_ERROR;
  uint8_t *alert_string_buf = NULL;
  int alert_string_len = 0;
  int alert_string_buf_len = 0;
  int blocks;
  int i;

  string_selector = atoi (section_name + strlen ("Alert_String_"));

  if (!(obj_cmd_rs = Fiid_obj_create(tmpl_cmd_set_pef_configuration_parameters_rs)))
    goto cleanup;
  
  alert_string_len = strlen((char *)kv->value_input);

  /* We need to write a nul char, so count it as part of the buflen */
  alert_string_buf_len = alert_string_len + 1;
  
  if (!(alert_string_buf = (uint8_t *)malloc(alert_string_buf_len)))
    {
      perror("strdup");
      goto cleanup;
    }
  memset(alert_string_buf, '\0', alert_string_buf_len);

  if (alert_string_len)
    memcpy(alert_string_buf, kv->value_input, alert_string_len);

  if (!((alert_string_buf_len) % 16))
    blocks = (alert_string_buf_len)/16;
  else
    blocks = (alert_string_buf_len)/16 + 1;

  for (i = 0; i < blocks; i++)
    {
      uint8_t len_to_write;

      if ((alert_string_buf_len - (i * 16)) < 16)
        len_to_write = alert_string_buf_len - (i * 16);
      else
        len_to_write = 16;

      if (ipmi_cmd_set_pef_configuration_parameters_alert_strings (state_data->dev,
                                                                   string_selector,
                                                                   i+1,
                                                                   alert_string_buf + (i * 16),
                                                                   len_to_write,
                                                                   obj_cmd_rs) < 0)
        {
          if (state_data->prog_data->args->common.flags & IPMI_FLAGS_DEBUG_DUMP)
            fprintf(stderr,
                    "ipmi_cmd_set_pef_configuration_parameters_alert_strings: %s\n",
                    ipmi_device_strerror(ipmi_device_errnum(state_data->dev)));
          rv = CONFIG_ERR_NON_FATAL_ERROR;
          goto cleanup;
        }
    }

  rv = CONFIG_ERR_SUCCESS;
 cleanup:
  if (alert_string_buf)
    free(alert_string_buf);
  Fiid_obj_destroy(obj_cmd_rs);
  return (rv);

}

static config_validate_t
alert_string_validate (const char *section_name,
                       const char *key_name,
                       const char *value)
{
  if (strlen (value) <= PEF_ALERT_STRING_MAX_LEN)
    return CONFIG_VALIDATE_VALID_VALUE;
  return CONFIG_VALIDATE_INVALID_VALUE;
}

struct config_section *
pef_config_alert_string_section_get (pef_config_state_data_t *state_data, int num)
{
  struct config_section *section = NULL;
  char buf[64];

  if (num <= 0)
    {
      fprintf(stderr, "Invalid Num = %d\n", num);
      return NULL;
    }

  snprintf(buf, 64, "Alert_String_%d", num);

  if (!(section = config_section_create (buf, 
                                         NULL, 
                                         NULL, 
                                         0)))
    goto cleanup;

  if (config_section_add_key (section,
                              "Event_Filter_Number",
                              "Give valid number",
                              0,
                              event_filter_number_checkout,
                              event_filter_number_commit,
                              config_number_range_seven_bits) < 0) 
    goto cleanup;

  if (config_section_add_key (section,
                              "Alert_String_Set",
                              "Give valid number",
                              0,
                              alert_string_set_checkout,
                              alert_string_set_commit,
                              config_number_range_seven_bits) < 0) 
    goto cleanup;

  if (config_section_add_key (section,
                              "Alert_String",
                              "Give string. Max 64 chars.",
                              0,
                              alert_string_checkout,
                              alert_string_commit,
                              alert_string_validate) < 0) 
    goto cleanup;

  return section;

 cleanup:
  if (section)
    config_section_destroy(section);
  return NULL;
}

