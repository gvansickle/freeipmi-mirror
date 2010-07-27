/*
 * Copyright (C) 2003-2010 FreeIPMI Core Team
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <stdlib.h>
#ifdef STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#include <errno.h>

#include "freeipmi/api/ipmi-chassis-cmds-api.h"
#include "freeipmi/cmds/ipmi-chassis-cmds.h"
#include "freeipmi/fiid/fiid.h"
#include "freeipmi/spec/ipmi-chassis-boot-options-parameter-spec.h"
#include "freeipmi/spec/ipmi-cmd-spec.h"
#include "freeipmi/spec/ipmi-comp-code-spec.h"
#include "freeipmi/spec/ipmi-ipmb-lun-spec.h"
#include "freeipmi/spec/ipmi-netfn-spec.h"

#include "ipmi-api-defs.h"
#include "ipmi-api-trace.h"
#include "ipmi-api-util.h"

#include "libcommon/ipmi-fiid-util.h"

#include "freeipmi-portability.h"

int
ipmi_cmd_get_chassis_capabilities (ipmi_ctx_t ctx,
                                   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_chassis_capabilities_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_chassis_capabilities_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_chassis_capabilities (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_chassis_status (ipmi_ctx_t ctx,
                             fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_chassis_status_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_chassis_status_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_chassis_status (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_chassis_control (ipmi_ctx_t ctx,
                          uint8_t chassis_control,
                          fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!IPMI_CHASSIS_CONTROL_VALID (chassis_control)
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_chassis_control_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_chassis_control_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_chassis_control (chassis_control, obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_chassis_identify (ipmi_ctx_t ctx,
                           const uint8_t *identify_interval,
                           const uint8_t *force_identify,
                           fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if ((force_identify
       && !IPMI_CHASSIS_FORCE_IDENTIFY_VALID (*force_identify))
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_chassis_identify_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_chassis_identify_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_chassis_identify (identify_interval,
                                 force_identify,
                                 obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_front_panel_enables (ipmi_ctx_t ctx,
                                  uint8_t disable_power_off_button_for_power_off_only,
                                  uint8_t disable_reset_button,
                                  uint8_t disable_diagnostic_interrupt_button,
                                  uint8_t disable_standby_button_for_entering_standby,
                                  fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!IPMI_CHASSIS_BUTTON_VALID (disable_power_off_button_for_power_off_only)
      || !IPMI_CHASSIS_BUTTON_VALID (disable_reset_button)
      || !IPMI_CHASSIS_BUTTON_VALID (disable_diagnostic_interrupt_button)
      || !IPMI_CHASSIS_BUTTON_VALID (disable_standby_button_for_entering_standby)
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_front_panel_enables_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_front_panel_enables_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_front_panel_enables (disable_power_off_button_for_power_off_only,
                                        disable_reset_button,
                                        disable_diagnostic_interrupt_button,
                                        disable_standby_button_for_entering_standby,
                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_power_restore_policy (ipmi_ctx_t ctx,
                                   uint8_t power_restore_policy,
                                   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!IPMI_POWER_RESTORE_POLICY_VALID (power_restore_policy)
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_power_restore_policy_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_power_restore_policy_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_power_restore_policy (power_restore_policy,
                                         obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_power_cycle_interval (ipmi_ctx_t ctx,
                                   uint8_t interval,
                                   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_power_cycle_interval_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_power_cycle_interval_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_power_cycle_interval (interval, obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_system_restart_cause (ipmi_ctx_t ctx,
                                   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_system_restart_cause_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_system_restart_cause_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_restart_cause (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_system_boot_options (ipmi_ctx_t ctx,
                                  uint8_t parameter_selector,
                                  const void *configuration_parameter_data,
                                  unsigned int configuration_parameter_data_len,
                                  fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if ((!IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_SELECTOR_VALID (parameter_selector)
       && !IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_SELECTOR_IS_OEM (parameter_selector))
      || !configuration_parameter_data
      || !configuration_parameter_data_len
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_system_boot_options_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_system_boot_options_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_system_boot_options (parameter_selector,
                                        configuration_parameter_data,
                                        configuration_parameter_data_len,
                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_system_boot_options_set_in_progress (ipmi_ctx_t ctx,
                                                  uint8_t value,
                                                  fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_system_boot_options_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_system_boot_options_set_in_progress_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_system_boot_options_set_in_progress (value, obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_system_boot_options_service_partition_selector (ipmi_ctx_t ctx,
                                                             uint8_t service_partition_selector,
                                                             fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }
  
  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_system_boot_options_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_system_boot_options_service_partition_selector_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_system_boot_options_service_partition_selector (service_partition_selector,
                                                                   obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }
  
  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }
  
  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_system_boot_options_service_partition_scan (ipmi_ctx_t ctx,
                                                         uint8_t service_partition_discovered,
                                                         uint8_t service_partition_scan,
                                                         fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;
  
  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }
  
  if (!IPMI_CHASSIS_BOOT_OPTIONS_SERVICE_PARTITION_DISCOVERED_VALID (service_partition_discovered)
      || !IPMI_CHASSIS_BOOT_OPTIONS_SERVICE_PARTITION_SCAN_VALID (service_partition_scan)
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_system_boot_options_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }
  
  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_system_boot_options_service_partition_scan_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_system_boot_options_service_partition_scan (service_partition_discovered,
                                                               service_partition_scan,
                                                               obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }
  
  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }
  
  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_system_boot_options_BMC_boot_flag_valid_bit_clearing (ipmi_ctx_t ctx,
                                                                   uint8_t dont_clear_on_power_up,
                                                                   uint8_t dont_clear_on_pushbutton_rest_soft_reset,
                                                                   uint8_t dont_clear_on_watchdog_timeout,
                                                                   uint8_t dont_clear_on_chassis_control,
                                                                   uint8_t dont_clear_on_PEF,
                                                                   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!IPMI_CHASSIS_BOOT_OPTIONS_CLEAR_VALID_BIT_VALID (dont_clear_on_power_up)
      || !IPMI_CHASSIS_BOOT_OPTIONS_CLEAR_VALID_BIT_VALID (dont_clear_on_pushbutton_rest_soft_reset)
      || !IPMI_CHASSIS_BOOT_OPTIONS_CLEAR_VALID_BIT_VALID (dont_clear_on_watchdog_timeout)
      || !IPMI_CHASSIS_BOOT_OPTIONS_CLEAR_VALID_BIT_VALID (dont_clear_on_chassis_control)
      || !IPMI_CHASSIS_BOOT_OPTIONS_CLEAR_VALID_BIT_VALID (dont_clear_on_PEF)
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_system_boot_options_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_system_boot_options_BMC_boot_flag_valid_bit_clearing_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_system_boot_options_BMC_boot_flag_valid_bit_clearing (dont_clear_on_power_up,
                                                                         dont_clear_on_pushbutton_rest_soft_reset,
                                                                         dont_clear_on_watchdog_timeout,
                                                                         dont_clear_on_chassis_control,
                                                                         dont_clear_on_PEF,
                                                                         obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_system_boot_options_boot_info_acknowledge (ipmi_ctx_t ctx,
                                                        const uint8_t *bios_or_post_handled_boot_info,
                                                        const uint8_t *os_loader_handled_boot_info,
                                                        const uint8_t *os_or_service_partition_handled_boot_info,
                                                        const uint8_t *sms_handled_boot_info,
                                                        const uint8_t *oem_handled_boot_info,
                                                        fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if ((bios_or_post_handled_boot_info
       && !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (*bios_or_post_handled_boot_info))
      || (os_loader_handled_boot_info
          && !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (*os_loader_handled_boot_info))
      || (os_or_service_partition_handled_boot_info
          && !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (*os_or_service_partition_handled_boot_info))
      || (sms_handled_boot_info
          && !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (*sms_handled_boot_info))
      || (oem_handled_boot_info
          && !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (*oem_handled_boot_info))
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_system_boot_options_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_system_boot_options_boot_info_acknowledge_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_system_boot_options_boot_info_acknowledge (bios_or_post_handled_boot_info,
                                                              os_loader_handled_boot_info,
                                                              os_or_service_partition_handled_boot_info,
                                                              sms_handled_boot_info,
                                                              oem_handled_boot_info,
                                                              obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_set_system_boot_options_boot_flags (ipmi_ctx_t ctx,
                                             uint8_t bios_boot_type,
                                             uint8_t boot_flags_persistent,
                                             uint8_t boot_flags_valid,
                                             uint8_t lock_out_reset_button,
                                             uint8_t screen_blank,
                                             uint8_t boot_device,
                                             uint8_t lock_keyboard,
                                             uint8_t cmos_clear,
                                             uint8_t console_redirection,
                                             uint8_t lock_out_sleep_button,
                                             uint8_t user_password_bypass,
                                             uint8_t force_progress_event_traps,
                                             uint8_t firmware_bios_verbosity,
                                             uint8_t lock_out_via_power_button,
                                             uint8_t bios_mux_control_override,
                                             uint8_t bios_shared_mode_override,
                                             uint8_t device_instance_selector,
                                             fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (boot_flags_valid)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (boot_flags_persistent)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (cmos_clear)
      || !IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BIOS_BOOT_TYPE_VALID (bios_boot_type)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (lock_keyboard)
      || !IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_VALID (boot_device)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (screen_blank)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (lock_out_reset_button)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (lock_out_via_power_button)
      || !IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_FIRMWARE_BIOS_VERBOSITY_VALID (firmware_bios_verbosity)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (force_progress_event_traps)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (user_password_bypass)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (lock_out_sleep_button)
      || !IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_CONSOLE_REDIRECTION_VALID (console_redirection)
      || !IPMI_CHASSIS_BOOT_OPTIONS_ENABLE_VALID (bios_shared_mode_override)
      || !IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAGS_BIOS_MUX_CONTROL_OVERRIDE_VALID (bios_mux_control_override)
      || !IPMI_CHASSIS_BOOT_OPTIONS_DEVICE_INSTANCE_SELECTOR_VALID (device_instance_selector)
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_set_system_boot_options_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_set_system_boot_options_boot_flags_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_set_system_boot_options_boot_flags (bios_boot_type,
                                                   boot_flags_persistent,
                                                   boot_flags_valid,
                                                   lock_out_reset_button,
                                                   screen_blank,
                                                   boot_device,
                                                   lock_keyboard,
                                                   cmos_clear,
                                                   console_redirection,
                                                   lock_out_sleep_button,
                                                   user_password_bypass,
                                                   force_progress_event_traps,
                                                   firmware_bios_verbosity,
                                                   lock_out_via_power_button,
                                                   bios_mux_control_override,
                                                   bios_shared_mode_override,
                                                   device_instance_selector,
                                                   obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_system_boot_options (ipmi_ctx_t ctx,
                                  uint8_t parameter_selector,
                                  uint8_t set_selector,
                                  uint8_t block_selector,
                                  fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if ((!IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_SELECTOR_VALID (parameter_selector)
       && !IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_SELECTOR_IS_OEM (parameter_selector))
      || !fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_system_boot_options_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_system_boot_options_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_boot_options (parameter_selector,
                                        set_selector,
                                        block_selector,
                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_system_boot_options_service_partition_selector (ipmi_ctx_t ctx,
                                                             uint8_t set_selector,
                                                             uint8_t block_selector,
                                                             fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_system_boot_options_service_partition_selector_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_system_boot_options_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_boot_options (IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_SERVICE_PARTITION_SELECTOR,
                                        set_selector,
                                        block_selector,
                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_system_boot_options_service_partition_scan (ipmi_ctx_t ctx,
                                                         uint8_t set_selector,
                                                         uint8_t block_selector,
                                                         fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_system_boot_options_service_partition_scan_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_system_boot_options_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_boot_options (IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_SERVICE_PARTITION_SCAN,
                                        set_selector,
                                        block_selector,
                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_system_boot_options_BMC_boot_flag_valid_bit_clearing (ipmi_ctx_t ctx,
                                                                   uint8_t set_selector,
                                                                   uint8_t block_selector,
                                                                   fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_system_boot_options_BMC_boot_flag_valid_bit_clearing_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_system_boot_options_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_boot_options (IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_BMC_BOOT_FLAG_VALID_BIT_CLEARING,
                                        set_selector,
                                        block_selector,
                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_system_boot_options_boot_info_acknowledge (ipmi_ctx_t ctx,
                                                        uint8_t set_selector,
                                                        uint8_t block_selector,
                                                        fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_system_boot_options_boot_info_acknowledge_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_system_boot_options_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_boot_options (IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_BOOT_INFO_ACKNOWLEDGE,
                                        set_selector,
                                        block_selector,
                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_system_boot_options_boot_flags (ipmi_ctx_t ctx,
                                             uint8_t set_selector,
                                             uint8_t block_selector,
                                             fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_system_boot_options_boot_flags_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_system_boot_options_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_system_boot_options (IPMI_CHASSIS_BOOT_OPTIONS_PARAMETER_BOOT_FLAGS,
                                        set_selector,
                                        block_selector,
                                        obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}

int
ipmi_cmd_get_power_on_hours_counter (ipmi_ctx_t ctx,
                                     fiid_obj_t obj_cmd_rs)
{
  fiid_obj_t obj_cmd_rq = NULL;
  int rv = -1;

  if (!ctx || ctx->magic != IPMI_CTX_MAGIC)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      return (-1);
    }

  if (!fiid_obj_valid (obj_cmd_rs))
    {
      API_SET_ERRNUM (ctx, IPMI_ERR_PARAMETERS);
      return (-1);
    }

  if (FIID_OBJ_TEMPLATE_COMPARE (obj_cmd_rs,
                                 tmpl_cmd_get_power_on_hours_counter_rs) < 0)
    {
      API_FIID_OBJECT_ERROR_TO_API_ERRNUM (ctx, obj_cmd_rs);
      return (-1);
    }

  if (!(obj_cmd_rq = fiid_obj_create (tmpl_cmd_get_power_on_hours_counter_rq)))
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (fill_cmd_get_power_on_hours_counter (obj_cmd_rq) < 0)
    {
      API_ERRNO_TO_API_ERRNUM (ctx, errno);
      goto cleanup;
    }

  if (api_ipmi_cmd (ctx,
                    IPMI_BMC_IPMB_LUN_BMC,
                    IPMI_NET_FN_CHASSIS_RQ,
                    obj_cmd_rq,
                    obj_cmd_rs) < 0)
    {
      ERR_TRACE (ipmi_ctx_errormsg (ctx), ipmi_ctx_errnum (ctx));
      goto cleanup;
    }

  rv = 0;
 cleanup:
  fiid_obj_destroy (obj_cmd_rq);
  return (rv);
}
