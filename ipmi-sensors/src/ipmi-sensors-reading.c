/* 
   Copyright (C) 2003-2008 FreeIPMI Core Team

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA.  

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#ifdef STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#include <errno.h>
#include <assert.h>

#include "freeipmi/api/ipmi-sensor-cmds-api.h"
#include "freeipmi/cmds/ipmi-sensor-cmds.h"
#include "freeipmi/fiid/fiid.h"
#include "freeipmi/record-format/ipmi-sdr-record-format.h"
#include "freeipmi/spec/ipmi-sensor-types-spec.h"
#include "freeipmi/spec/ipmi-system-software-id-spec.h"
#include "freeipmi/util/ipmi-sensor-and-event-code-tables-util.h"
#include "freeipmi/util/ipmi-sensor-util.h"

#include "ipmi-sensors.h"

#include "tool-fiid-wrappers.h"
#include "tool-sdr-cache-common.h"
#include "tool-sensor-common.h"

fiid_template_t l_tmpl_cmd_get_sensor_reading_threshold_rs =
  {
    {8, "cmd", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {8, "comp_code", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {8, "sensor_reading", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {5, "reserved1", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "reading_state", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "sensor_scanning", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "all_event_messages", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {6, "sensor_state", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {2, "reserved2", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    /* optional byte */
    {8, "ignore", FIID_FIELD_OPTIONAL | FIID_FIELD_LENGTH_FIXED}, 
    
    {0,  "", 0}
  };
  
fiid_template_t l_tmpl_cmd_get_sensor_reading_discrete_8_states_rs =
  {
    {8, "cmd", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {8, "comp_code", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {8, "sensor_reading", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {5, "reserved1", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "reading_state", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "sensor_scanning", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "all_event_messages", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {8, "sensor_state", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    /* optional byte */
    {8, "ignore", FIID_FIELD_OPTIONAL | FIID_FIELD_LENGTH_FIXED}, 
    
    {0,  "", 0}
  };

fiid_template_t l_tmpl_cmd_get_sensor_reading_discrete_15_states_rs =
  {
    {8, "cmd", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {8, "comp_code", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {8, "sensor_reading", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {5, "reserved1", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "reading_state", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "sensor_scanning", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "all_event_messages", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    
    {15, "sensor_state", FIID_FIELD_REQUIRED | FIID_FIELD_LENGTH_FIXED}, 
    {1, "reserved2", FIID_FIELD_OPTIONAL | FIID_FIELD_LENGTH_FIXED}, 
    
    {0,  "", 0}
  };

#define IPMI_SENSORS_BUFLEN   1024
#define IPMI_SENSORS_MAX_LIST   32

static int
_get_threshold_message_list (struct ipmi_sensors_state_data *state_data,
                             char ***event_message_list,
                             unsigned int *event_message_list_len,
                             uint8_t sensor_state)
{
  char **tmp_event_message_list = NULL;
  char *tmp_message_list[IPMI_SENSORS_MAX_LIST];
  int num_messages = 0;
  int count = 0;
  int i;
  
  assert(state_data);
  assert(event_message_list);
  assert(event_message_list_len);

  /* achu: multiple threshold flags can be set (i.e. if we pass the
   * critical threshold, we've also passed the non-critical threshold)
   * but we only want to * output one message at the max.  Luckily for
   * us (and due to smarts * by the IPMI specification authors) if we
   * go from high bits to low * bits, we will read the flags in the
   * correct order for output.
   *
   * If you're confused why were use 'ipmi_get_threshold_message'
   * instead of 'ipmi_get_generic_event_message' (b/c this is
   * presumably event_reading_type_code == 0x01), the reason is b/c we
   * actually called the get_sensor_reading command.  In other IPMI
   * subsystems (like the SEL) we don't call the get_sensor_reading
   * command, which is when you fall back on
   * 'ipmi_get_generic_event_message'.
   */

  for (i = 5; i >= 0; i--)
    {
      char buf[IPMI_SENSORS_BUFLEN];
      uint16_t bit; 

      bit = 0x1 << i;

      if (sensor_state & bit)
	{
	  if (ipmi_get_threshold_message (i,
                                          buf,
                                          IPMI_SENSORS_BUFLEN) < 0)
            continue;
	  
	  if (!(tmp_message_list[num_messages] = strdup(buf)))
            {
              pstdout_perror(state_data->pstate, "strdup");
              goto cleanup;
            }
          
          num_messages++;
          break;
	}
    }
  
  if (num_messages)
    count = num_messages;
  else
    count = 1;

  if (!(tmp_event_message_list = (char **) malloc (sizeof (char *) * (count + 1))))
    {
      pstdout_perror(state_data->pstate, "malloc");
      goto cleanup;
    }
    
  if (num_messages)
    {
      for (i = 0; i < num_messages; i++)
	tmp_event_message_list[i] = tmp_message_list[i];
    }
  else
    {
      if (!(tmp_event_message_list[0] = strdup("OK")))
        {
          pstdout_perror(state_data->pstate, "strdup");
          goto cleanup;
        }
    }
  
  tmp_event_message_list[count] = NULL;
  *event_message_list = tmp_event_message_list;
  /* achu: note, not like generic_event_message_list or
   * sensor_specific_event_message_list, max of one message output
   */
  *event_message_list_len = 1;

  return 0;

 cleanup:
  for (i = 0; i < num_messages; i++)
    free(tmp_message_list[num_messages]);
  if (tmp_event_message_list)
    free(tmp_event_message_list);
  return -1;
}

static int
_get_generic_event_message_list (struct ipmi_sensors_state_data *state_data,
                                 char ***event_message_list,
                                 unsigned int *event_message_list_len,
                                 uint8_t event_reading_type_code, 
                                 uint16_t sensor_state)  
{
  char **tmp_event_message_list = NULL;
  char *tmp_message_list[IPMI_SENSORS_MAX_LIST];
  int num_messages = 0;
  int count = 0;
  int i;
  
  assert(state_data);
  assert(event_message_list);
  assert(event_message_list_len);

  for (i = 0; i < 16; i++)
    {
      char buf[IPMI_SENSORS_BUFLEN];
      uint16_t bit; 

      bit = 0x1 << i;
      if (sensor_state & bit)
	{
	  if (ipmi_get_generic_event_message (event_reading_type_code,
					      i,
					      buf,
					      IPMI_SENSORS_BUFLEN) < 0)
            continue;

	  if (!(tmp_message_list[num_messages] = strdup(buf)))
            {
              pstdout_perror(state_data->pstate, "strdup");
              goto cleanup;
            }

          num_messages++; 
	}
    }
  
  if (num_messages)
    count = num_messages;
  else
    count = 1;

  if (!(tmp_event_message_list = (char **) malloc (sizeof (char *) * (count + 1))))
    {
      pstdout_perror(state_data->pstate, "malloc");
      goto cleanup;
    }
      
  if (num_messages)
    {
      for (i = 0; i < num_messages; i++)
	tmp_event_message_list[i] = tmp_message_list[i];
    }
  else
    {
      if (!(tmp_event_message_list[0] = strdup("OK")))
        {
          pstdout_perror(state_data->pstate, "strdup");
          goto cleanup;
        }
    }

  tmp_event_message_list[count] = NULL;
  *event_message_list = tmp_event_message_list;
  *event_message_list_len = count;
  
  return 0;

 cleanup:
  for (i = 0; i < num_messages; i++)
    free(tmp_message_list[num_messages]);
  if (tmp_event_message_list)
    free(tmp_event_message_list);
  return -1;
}

static int
_get_sensor_specific_event_message_list (struct ipmi_sensors_state_data *state_data,
                                         char ***event_message_list,
                                         unsigned int *event_message_list_len,
                                         uint8_t sensor_type, 
                                         uint16_t sensor_state)
{
  char **tmp_event_message_list = NULL;
  char *tmp_message_list[IPMI_SENSORS_MAX_LIST];
  int num_messages = 0;
  int count = 0;
  int i;
  
  assert(state_data);
  assert(event_message_list);
  assert(event_message_list_len);

  for (i = 0; i < 16; i++)
    {
      char buf[IPMI_SENSORS_BUFLEN];
      uint16_t bit; 

      bit = 0x1 << i;

      if (sensor_state & bit)
	{
	  if (ipmi_get_sensor_type_code_message (sensor_type,
						 i,
						 buf,
						 IPMI_SENSORS_BUFLEN) < 0)
            continue;

	  if (!(tmp_message_list[num_messages] = strdup(buf)))
            {
              pstdout_perror(state_data->pstate, "strdup");
              goto cleanup;
            }

          num_messages++;
	}
    }
  
  if (num_messages)
    count = num_messages;
  else
    count = 1;

  if (!(tmp_event_message_list = (char **) malloc (sizeof (char *) * (count + 1))))
    {
      pstdout_perror(state_data->pstate, "malloc");
      goto cleanup;
    }
      
  if (num_messages)
    {
      for (i = 0; i < num_messages; i++)
	tmp_event_message_list[i] = tmp_message_list[i];
    }
  else
    {
      if (!(tmp_event_message_list[0] = strdup("OK")))
        {
          pstdout_perror(state_data->pstate, "strdup");
          goto cleanup;
        }
    }

  tmp_event_message_list[count] = NULL;
  *event_message_list = tmp_event_message_list;
  *event_message_list_len = count;

  return 0;

 cleanup:
  for (i = 0; i < num_messages; i++)
    free(tmp_message_list[num_messages]);
  if (tmp_event_message_list)
    free(tmp_event_message_list);
  return -1;
}

int
sensor_reading (struct ipmi_sensors_state_data *state_data,
                uint8_t *sdr_record,
                unsigned int sdr_record_len,
                double **reading,
                char ***event_message_list,
                unsigned int *event_message_list_len)
{ 
  uint16_t record_id;
  uint8_t record_type;
  uint8_t sensor_number;
  uint8_t sensor_type;
  uint8_t event_reading_type_code;
  int sensor_class;
  fiid_obj_t obj_cmd_rs = NULL;  
  fiid_obj_t l_obj_cmd_rs = NULL;
  double *tmp_reading = NULL;
  uint64_t val;
  int rv = -1;

  assert(state_data);
  assert(sdr_record);
  assert(sdr_record_len);
  assert(reading);
  assert(event_message_list);
  assert(event_message_list_len);

  *reading = NULL;
  *event_message_list = NULL;
  *event_message_list_len = 0;

  if (sdr_cache_get_record_id_and_type(state_data->pstate,
                                       sdr_record,
                                       sdr_record_len,
                                       &record_id,
                                       &record_type) < 0)
    return -1;

  /* can't get reading for this sdr entry. don't output an error
   * though, since this isn't really an error.  The tool will output
   * something appropriate as it sees fit.
   */
  if (record_type != IPMI_SDR_FORMAT_FULL_RECORD
      && record_type != IPMI_SDR_FORMAT_COMPACT_RECORD)
    return 0;
 
  if (sdr_cache_get_sensor_number (state_data->pstate,
                                   sdr_record,
                                   sdr_record_len,
                                   &sensor_number) < 0)
    return -1;
    
  if (sdr_cache_get_sensor_type (state_data->pstate,
                                 sdr_record,
                                 sdr_record_len,
                                 &sensor_type) < 0)
    return -1;

  if (sdr_cache_get_event_reading_type_code (state_data->pstate,
                                             sdr_record,
                                             sdr_record_len,
                                             &event_reading_type_code) < 0)
    return -1;

  sensor_class = sensor_classify (event_reading_type_code);

  if (sensor_class == SENSOR_CLASS_THRESHOLD)
    {
      _FIID_OBJ_CREATE(obj_cmd_rs, tmpl_cmd_get_sensor_reading_threshold_rs);

      if (ipmi_cmd_get_sensor_reading_threshold (state_data->ipmi_ctx, 
                                                 sensor_number, 
                                                 obj_cmd_rs) < 0)
        {
          /* A sensor listed by the SDR is not present.  Skip it's
           * output, don't error out.
           */
          if (ipmi_check_completion_code(obj_cmd_rs,
                                         IPMI_COMP_CODE_REQUEST_SENSOR_DATA_OR_RECORD_NOT_PRESENT) == 1)
            {
              if (state_data->prog_data->args->common.debug)
                pstdout_fprintf(state_data->pstate,
                                stderr,
                                "Sensor number 0x%X data in record %u not present\n",
                                sensor_number,
                                record_id);
              rv = 0;
            }
          else
            pstdout_fprintf(state_data->pstate,
                            stderr,
                            "ipmi_cmd_get_sensor_reading_discrete: %s\n",
                            ipmi_ctx_strerror(ipmi_ctx_errnum(state_data->ipmi_ctx)));
          goto cleanup;
        }

      _FIID_OBJ_COPY(l_obj_cmd_rs,
                     obj_cmd_rs,
                     l_tmpl_cmd_get_sensor_reading_threshold_rs);
     
      _FIID_OBJ_GET (l_obj_cmd_rs, "sensor_reading", &val);

      if (record_type == IPMI_SDR_FORMAT_FULL_RECORD)
        {
          int8_t r_exponent, b_exponent;
          int16_t m, b;
          uint8_t linearization, analog_data_format;

          if (sdr_cache_get_sensor_decoding_data(state_data->pstate,
                                                 sdr_record,
                                                 sdr_record_len,
                                                 &r_exponent,
                                                 &b_exponent,
                                                 &m,
                                                 &b,
                                                 &linearization,
                                                 &analog_data_format) < 0)
            goto cleanup;

          /* if the sensor is not analog, this is most likely a bug in the
           * SDR, since we shouldn't be decoding a non-threshold sensor.
           */
          if (!IPMI_SDR_ANALOG_DATA_FORMAT_VALID(analog_data_format))
            {
              if (state_data->prog_data->args->common.debug)
                pstdout_fprintf(state_data->pstate,
                                stderr,
                                "Attempting to decode non-analog sensor\n");
              rv = 0;
              goto cleanup;
            }
          
          /* if the sensor is non-linear, I just don't know what to do, 
           * let the tool figure out what to output.
           */
          if (!IPMI_SDR_LINEARIZATION_IS_LINEAR(linearization))
            {
              if (state_data->prog_data->args->common.debug)
                pstdout_fprintf(state_data->pstate,
                                stderr,
                                "Cannot decode non-linear sensor\n");
              rv = 0;
              goto cleanup;
            }
             
          if (!(tmp_reading = (double *)malloc(sizeof(double))))
            {
              pstdout_perror(state_data->pstate, "malloc");
              goto cleanup;
            }
          
	  if (ipmi_sensor_decode_value (r_exponent, 
                                        b_exponent, 
                                        m, 
                                        b, 
                                        linearization, 
                                        analog_data_format, 
                                        (uint8_t) val,
                                        tmp_reading) < 0)
            {
              pstdout_fprintf (state_data->pstate,
                               stderr,
                               "ipmi_sensor_decode_value: %s\n",
                               strerror(errno));
              goto cleanup;
            }
	}
      /* else
       *
       * I guess there is a mistake, it should have been listed as a non-threshold
       * sensor?  We'll still fall through to grab event messages, since maybe we
       * can still output something.
       */
           
      _FIID_OBJ_GET (l_obj_cmd_rs, 
                     "sensor_state", 
                     &val);

      if (_get_threshold_message_list (state_data,
                                       event_message_list,
                                       event_message_list_len,
                                       val) < 0)
        goto cleanup;
      
      rv = 1;
    }
  else if (sensor_class == SENSOR_CLASS_GENERIC_DISCRETE
           || sensor_class ==  SENSOR_CLASS_SENSOR_SPECIFIC_DISCRETE
           || sensor_class == SENSOR_CLASS_OEM)
    {
      fiid_field_t *l_tmpl_cmd_get_sensor_reading_discrete_rs = NULL;
      int32_t obj_cmd_rs_len;
      int8_t sensor_state_len;

      _FIID_OBJ_CREATE(obj_cmd_rs, tmpl_cmd_get_sensor_reading_discrete_rs);

      if (ipmi_cmd_get_sensor_reading_discrete (state_data->ipmi_ctx, 
                                                sensor_number, 
                                                obj_cmd_rs) < 0)
        {
          /* A sensor listed by the SDR is not present.  Skip it's
           * output, don't error out.
           */
          if (ipmi_check_completion_code(obj_cmd_rs,
                                         IPMI_COMP_CODE_REQUEST_SENSOR_DATA_OR_RECORD_NOT_PRESENT) == 1)
            {
              if (state_data->prog_data->args->common.debug)
                pstdout_fprintf(state_data->pstate,
                                stderr,
                                "Sensor number 0x%X data in record %u not present\n",
                                sensor_number,
                                record_id);
              rv = 0;
            }
          else
            pstdout_fprintf(state_data->pstate,
                            stderr,
                            "ipmi_cmd_get_sensor_reading_discrete: %s\n",
                            ipmi_ctx_strerror(ipmi_ctx_errnum(state_data->ipmi_ctx)));
          goto cleanup;
        }
      
      _FIID_OBJ_LEN_BYTES(obj_cmd_rs, obj_cmd_rs_len);

      /* achu:
       * 
       * hack to get around optional byte, b/c sensor_state size of 15
       * bits messes up fiid_obj_copy's expectation of byte aligned
       * fields when doing a copy.
       */

      if (obj_cmd_rs_len == fiid_template_block_len_bytes(l_tmpl_cmd_get_sensor_reading_discrete_8_states_rs, 
                                                          "cmd", 
                                                          "sensor_state"))
        l_tmpl_cmd_get_sensor_reading_discrete_rs = &l_tmpl_cmd_get_sensor_reading_discrete_8_states_rs[0];
      else
        l_tmpl_cmd_get_sensor_reading_discrete_rs = &l_tmpl_cmd_get_sensor_reading_discrete_15_states_rs[0];
      
      _FIID_OBJ_CREATE(l_obj_cmd_rs, l_tmpl_cmd_get_sensor_reading_discrete_rs);
      
      _FIID_OBJ_COPY(l_obj_cmd_rs,
                     obj_cmd_rs,
                     l_tmpl_cmd_get_sensor_reading_discrete_rs);
      
      /* 
       * IPMI Workaround (achu)
       *
       * Discovered on Dell 2950.
       *
       * It seems the sensor_state may not be returned by the server
       * at all for some sensors.  Under this situation, there's not
       * much that can be done.  Since there is no sensor_state, we
       * just assume that no states have been asserted and the
       * sensor_state = 0;
       */

      _FIID_OBJ_GET_WITH_RETURN_VALUE (l_obj_cmd_rs,
                                       "sensor_state",
                                       &val,
                                       sensor_state_len);

      if (!sensor_state_len)
        val = 0;
      
      if (sensor_class == SENSOR_CLASS_GENERIC_DISCRETE)
        {
          if (_get_generic_event_message_list (state_data,
                                               event_message_list,
                                               event_message_list_len, 
                                               event_reading_type_code, 
                                               val) < 0)
            goto cleanup;

          rv = 1;
        }
      else if (sensor_class == SENSOR_CLASS_SENSOR_SPECIFIC_DISCRETE)
        {
          if (_get_sensor_specific_event_message_list (state_data,
                                                       event_message_list,
                                                       event_message_list_len,
                                                       sensor_type, 
                                                       val) < 0)
            goto cleanup;

          rv = 1;
        }
      else if (sensor_class == SENSOR_CLASS_OEM)
        {
          char *event_message = NULL;
          char **tmp_event_message_list = NULL;

          if (asprintf (&event_message, 
                        "OEM State = %04Xh", 
                        (uint16_t) val) < 0)
            {
              pstdout_perror(state_data->pstate, "asprintf");
              goto cleanup;
            }

          if (!(tmp_event_message_list = (char **) malloc (sizeof (char *) * 2)))
            {
              pstdout_perror(state_data->pstate, "malloc");
              goto cleanup;
            }
      
          tmp_event_message_list[0] = event_message;
          tmp_event_message_list[1] = NULL;

          *event_message_list = tmp_event_message_list;
          *event_message_list_len = 1;
          
          rv = 1;
        }
    }
  else
    rv = 0;

  if (rv > 0)
    *reading = tmp_reading;
 cleanup:
  _FIID_OBJ_DESTROY(obj_cmd_rs);
  _FIID_OBJ_DESTROY(l_obj_cmd_rs);
  if (rv <= 0)
    {
      if (tmp_reading)
        free(tmp_reading);
    }
  return (rv);
}
