/* 
   ipmi-chassis-argp.c - IPMI Chassis ARGP parser
   
   Copyright (C) 2007 FreeIPMI Core Team
   
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
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  
*/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <argp.h>

#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */

#include <freeipmi/ipmi-chassis-boot-options-param-spec.h>
#include "ipmi-chassis.h"
#include "ipmi-chassis-argp.h"
#include "argp-common.h"

static error_t parse_opt (int key, char *arg, struct argp_state *state);

const char *argp_program_version = 
"IPMI Chassis [ipmi-chassis-" PACKAGE_VERSION "]\n"
"Copyright (C) 2003-2005 FreeIPMI Core Team\n"
"This program is free software; you may redistribute it under the terms of\n"
"the GNU General Public License.  This program has absolutely no warranty.";

const char *argp_program_bug_address = "<freeipmi-devel@gnu.org>";

static char doc[] = "IPMI Chassis - Control IPMI Chassis. The tool executes in the default privilege level of ADMIN";

static char args_doc[] = "";

static struct argp_option options[] = 
  {
    ARGP_COMMON_OPTIONS_INBAND,
    ARGP_COMMON_OPTIONS_OUTOFBAND,
    ARGP_COMMON_OPTIONS_AUTHTYPE,
    {"priv-level",     PRIVILEGE_LEVEL_KEY, "PRIVILEGE-LEVEL", 0,
     "Use this PRIVILEGE-LEVEL instead of ADMIN.  "
     "Allowed values are CALLBACK, USER, OPERATOR, ADMIN and OEM.", 12},
    {"get-capabilities", 'c', NULL, 0, "Get the chassis capabilities", 13},
    {"get-status", 's', NULL, 0, "Get the chassis status", 14},
    {"chassis-control", 'C', "CONTROL", 0, "Control the chassis.Allowed values are: power-down, power-up, power-cycle, hard-reset, diagnostic-interrupt, soft-shutdown", 15},
    {"chassis-identify", 'I', "IDENTIFY", 0,"Chassis Identification.Allowed values are: turn-off to turn off identification, <interval> to turn on identification for \"interval\" seconds, force to turn on indefinitely", 16},
    {"get-system-restart-cause", 'R', NULL, 0, "Get system restart cause", 17},
    {"get-power-on-hours-counter", 'H', NULL, 0,"Get Power on Hours counter", 18},
    {"set-power-cycle-interval", 'S', "INTERVAL", 0, "Set Power cycle interval to INTERVAL", 19},
    {"set-boot-flags", 'B', NULL, 0, "Set system boot flags. Allowed flags are: clear-cmos, boot-type, lock-keyboard, boot-device, blank-screen, lock-out-reset-button, firmware-bios-verbosity, force-progress-event-traps, user-password-bypass", 20},
    {"get-boot-flags", 'G', NULL, 0, "Get system boot-flags", 21},
    {"boot-type", IPMI_CHASSIS_KEY_BOOT_TYPE, "BOOT_TYPE", OPTION_ARG_OPTIONAL, "Set BIOS boot type to BOOT_TYPE, allowed boot types: pc-compatible or EFI. Should be used with set-boot-flags", 22},
    {"lock-out-reset-button", IPMI_CHASSIS_KEY_LOCK_OUT_RESET_BUTTON, "LOCK_OUT_RESET_BUTTON", OPTION_ARG_OPTIONAL, "Lock out reset button, allowed values: yes/no. Should be used with set-boot-flags", 23},
    {"blank-screen", IPMI_CHASSIS_KEY_SCREEN_BLANK, "BLANK_SCREEN", OPTION_ARG_OPTIONAL, "Blank screen, allowed values: yes/no. Should be used with set-boot-flags", 24},
    {"boot-device", IPMI_CHASSIS_KEY_BOOT_DEVICE_SELECTOR, "BOOT_DEVICE", OPTION_ARG_OPTIONAL, "Set device to boot from to BOOT_DEVICE, allowed devices: pxe, disk, disk-safe, diag, cd-dvd, floppy, bios. Should be used with set-boot-flags", 25},
    {"lock-keyboard", IPMI_CHASSIS_KEY_LOCK_KEYBOARD, "LOCK_KEYBOARD", OPTION_ARG_OPTIONAL, "lock keyboard, allowed values: yes/no. Should be used with set-boot-flags", 26},
    {"clear-cmos", IPMI_CHASSIS_KEY_CLEAR_CMOS, "CLEAR_CMOS", OPTION_ARG_OPTIONAL, "clear-cmos: yes/no. Should be used with set-boot-flags", 27},
    {"console-redirection", IPMI_CHASSIS_KEY_CONSOLE_REDIRECTION, "REDIRECT_TO_CONSOLE", OPTION_ARG_OPTIONAL, "redirect to console, allowed values: default, suppress, enable. Should be used with set-boot-flags", 28},
    {"user-password-bypass", IPMI_CHASSIS_KEY_USER_PASSWORD_BYPASS, "BYPASS_USER_PASSWORD", OPTION_ARG_OPTIONAL, "Bypass user password, allowed values: yes/no. Should be used with set-boot-flags", 29},
    {"force-progress-event-traps", IPMI_CHASSIS_KEY_FORCE_PROGRESS_EVENT_TRAPS, "FORCE_PROGRESS_EVENT_TRAPS", OPTION_ARG_OPTIONAL, "Force progress event traps, allowed values: yes/no. Should be used with set-boot-flags", 30},
    {"firmware-bios-verbosity", IPMI_CHASSIS_KEY_FIRMWARE_BIOS_VERBOSITY, "FIRMWARE_BIOS_VERBOSITY", OPTION_ARG_OPTIONAL, "Select firmware verbosity, allowed levels: default, quiet, verbose. Should be used with set-boot-flags", 31},
    {"set-power-restore-policy", 'X', "POLICY", 0, "Set power restore policy:list, always-on, restore, always-off", 32},
    { 0 }
  };

static char *boot_argv[];
static error_t boot_flag_parse_opt (int, char *, struct argp_state *);
static error_t

parse_opt (int key, char *arg, struct argp_state *state);

static struct argp argp = { options, parse_opt, args_doc, doc };

static error_t
boot_flag_parse_opt (int key, char *arg, struct argp_state *state)
{
  struct ipmi_chassis_arguments *cmd_args = state->input;
  uint8_t value = 0;

  switch (key)
    {
    case IPMI_CHASSIS_KEY_BOOT_TYPE:
      if((strncasecmp(arg, "pc-compatible", 13) == 0) && strlen (arg) == 13)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_TYPE_PC_COMPATIBLE;
      else if((strncasecmp(arg, "efi", 3) == 0) && strlen (arg) == 3)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_TYPE_EFI;
      else
        {
          fprintf (stderr, "Invalid boot type\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.bios_boot_type = value;
      break;

    case IPMI_CHASSIS_KEY_LOCK_OUT_RESET_BUTTON:
      if((strncasecmp(arg, "yes", 3) == 0) && strlen (arg) == 3)
        value = IPMI_CHASSIS_BOOT_OPTIONS_ENABLE;
      else if((strncasecmp(arg, "no", 2) == 0) && strlen (arg) == 2)
        value = IPMI_CHASSIS_BOOT_OPTIONS_DISABLE;
      else
        {
          fprintf (stderr, "Invalid value for lock-out-reset-button\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.lock_out_reset_button = value;
      break;

    case IPMI_CHASSIS_KEY_SCREEN_BLANK:
      if((strncasecmp(arg, "yes", 3) == 0) && strlen (arg) == 3)
        value = IPMI_CHASSIS_BOOT_OPTIONS_ENABLE;
      else if((strncasecmp(arg, "no", 2) == 0) && strlen (arg) == 2)
        value = IPMI_CHASSIS_BOOT_OPTIONS_DISABLE;
      else
        {
          fprintf (stderr, "Invalid value for blank-screen\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.screen_blank = value;
      break;

    case IPMI_CHASSIS_KEY_BOOT_DEVICE_SELECTOR:
      if (arg == NULL)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_NONE;
      else if ((strncasecmp(arg, "none", 4) == 0) && strlen (arg) == 4)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_NONE;
      else if ((strncasecmp(arg, "pxe", 3) == 0) && strlen (arg) == 3)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_PXE;
      else if ((strncasecmp(arg, "disk", 4) == 0) && strlen (arg) == 4)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_DISK;
      else if ((strncasecmp(arg, "disk-safe", 9) == 0) && strlen (arg) == 9)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_DISK_SAFE_MODE;
      else if ((strncasecmp(arg, "diag", 4) == 0) && strlen (arg) == 4)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_DIAGNOSTIC_INTERRUPT;
      else if ((strncasecmp(arg, "cd-dvd", 6) == 0) && strlen (arg) == 6)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_CD_DVD;
      else if ((strncasecmp(arg, "floppy", 6) == 0) && strlen (arg) == 6)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_FLOPPY;
      else if ((strncasecmp(arg, "bios", 4) == 0) && strlen (arg) == 4)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_BOOT_DEVICE_BIOS;
      else
        {
          printf ("Invalid boot device\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.boot_device_selector = value;
      break;

    case IPMI_CHASSIS_KEY_LOCK_KEYBOARD:
      if((strncasecmp(arg, "yes", 3) == 0) && strlen (arg) == 3)
        value = IPMI_CHASSIS_BOOT_OPTIONS_ENABLE;
      else if((strncasecmp(arg, "no", 2) == 0) && strlen (arg) == 2)
        value = IPMI_CHASSIS_BOOT_OPTIONS_DISABLE;
      else
        {
          fprintf (stderr, "Invalid value for lock-keyboard\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.lock_keyboard = value;
      break;

    case IPMI_CHASSIS_KEY_CLEAR_CMOS:
      if((strncasecmp (arg, "yes", 3) == 0) && strlen (arg) == 3)
        value = IPMI_CHASSIS_BOOT_OPTIONS_ENABLE;
      else if((strncasecmp (arg, "no", 2) == 0) && strlen (arg) == 2)
        value = IPMI_CHASSIS_BOOT_OPTIONS_DISABLE;
      else
        {
          fprintf (stderr, "Invalid value for clear-cmos\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.clear_cmos = value;
      break;

    case IPMI_CHASSIS_KEY_CONSOLE_REDIRECTION:
      if((strncasecmp (arg, "default", 7) == 0) && strlen (arg) == 7)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_CONSOLE_REDIRECTION_DEFAULT;
      else if((strncasecmp (arg, "suppress", 8) == 0) && strlen (arg) == 8)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_CONSOLE_REDIRECTION_SUPRESS;
      else if ((strncasecmp (arg, "enable", 6) == 0) && strlen (arg) == 6)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_CONSOLE_REDIRECTION_ENABLE;
      else
        {
          fprintf (stderr, "Invalid value for console-redirection\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.console_redirection = value;
      break;


    case IPMI_CHASSIS_KEY_USER_PASSWORD_BYPASS:
      if((strncasecmp(arg, "yes", 3) == 0) && strlen (arg) == 3)
        value = IPMI_CHASSIS_BOOT_OPTIONS_ENABLE;
      else if((strncasecmp(arg, "no", 2) == 0) &&  strlen (arg) == 2)
        value = IPMI_CHASSIS_BOOT_OPTIONS_DISABLE;
      else
        {
          fprintf (stderr, "Invalid value for user-password-bypass\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.user_password_bypass = value;
      break;

    case IPMI_CHASSIS_KEY_FORCE_PROGRESS_EVENT_TRAPS:
      if((strncasecmp(arg, "yes", 3) == 0) && strlen (arg) == 3)
        value = IPMI_CHASSIS_BOOT_OPTIONS_ENABLE;
      else if((strncasecmp(arg, "no", 2) == 0) && strlen (arg) == 2)
        value = IPMI_CHASSIS_BOOT_OPTIONS_DISABLE;
      else
        {
          fprintf (stderr, "Invalid value for force-progress-event-traps\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.force_progress_event_traps = value;
      break;

    case IPMI_CHASSIS_KEY_FIRMWARE_BIOS_VERBOSITY:
      if((strncasecmp(arg, "quiet", 5) == 0) && strlen (arg) == 5)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_FIRMWARE_BIOS_VERBOSITY_QUIET;
      else if((strncasecmp(arg, "default", 7) == 0) && strlen (arg) == 7)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_FIRMWARE_BIOS_VERBOSITY_DEFAULT;
      else if((strncasecmp (arg, "verbose", 7) == 0) && strlen (arg) == 7)
        value = IPMI_CHASSIS_BOOT_OPTIONS_BOOT_FLAG_FIRMWARE_BIOS_VERBOSITY_VERBOSE;
      else
        {
          fprintf (stderr, "Invalid firmware verbosity level\n");
          argp_usage (state);
        }

      cmd_args->args.boot_option_args.firmware_bios_verbosity = value;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }

  if ((cmd_args->cmd != IPMI_CMD_SET_SYSTEM_BOOT_OPTIONS))
    {
      fprintf (stderr, "please specify set-boot-flags option\n");
      argp_usage (state);
    }
  return 0;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
  int rv;
  char **ptr = NULL;
  struct ipmi_chassis_arguments *cmd_args = state->input;

  switch (key)
    {
    case 'c':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }
      cmd_args->cmd = IPMI_CMD_GET_CHASSIS_CAPABILITIES;
      break;

    case 's':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }
      cmd_args->cmd = IPMI_CMD_GET_CHASSIS_STATUS;
      break;

    case 'I':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }

      cmd_args->cmd = IPMI_CMD_CHASSIS_IDENTIFY;
      if ((strncasecmp(arg, "force", 5) == 0) && strlen (arg) == 5)
        cmd_args->args.identify_args.force_identify = IPMI_CHASSIS_FORCE_IDENTIFY_ON;
      else
        cmd_args->args.identify_args.force_identify = IPMI_CHASSIS_FORCE_IDENTIFY_OFF;

      if (cmd_args->args.identify_args.force_identify == IPMI_CHASSIS_FORCE_IDENTIFY_OFF)
        {
          cmd_args->args.identify_args.interval = strtol (arg, ptr, 10);
          if (ptr && *ptr && **ptr != '\0')
            {
              fprintf (stderr, "Invalid Chassis Identify value\n");
              argp_usage (state);
            }
        }
      break;

    case 'C':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }
      cmd_args->cmd = IPMI_CMD_CHASSIS_CONTROL;
      if ((strncasecmp(arg, "power-down", 10) == 0) && strlen (arg) == 10)
        cmd_args->args.chassis_control = IPMI_CHASSIS_CONTROL_POWER_DOWN;
      else if ((strncasecmp(arg, "power-up", 8) == 0) && strlen (arg) == 8)
        cmd_args->args.chassis_control = IPMI_CHASSIS_CONTROL_POWER_UP;
      else if ((strncasecmp(arg, "power-cycle", 11) == 0) && strlen (arg) == 11)
        cmd_args->args.chassis_control = IPMI_CHASSIS_CONTROL_POWER_CYCLE;
      else if ((strncasecmp(arg, "hard-reset", 10) == 0) && strlen (arg) == 10)
        cmd_args->args.chassis_control = IPMI_CHASSIS_CONTROL_HARD_RESET;
      else if ((strncasecmp(arg, "diagnostic-interrupt", 20) == 0) && strlen (arg) == 20)
        cmd_args->args.chassis_control = IPMI_CHASSIS_CONTROL_PULSE_DIAGNOSTIC_INTERRUPT;
      else if ((strncasecmp(arg, "soft-shutdown", 13) == 0) && strlen (arg) == 13)
        cmd_args->args.chassis_control = IPMI_CHASSIS_CONTROL_INITIATE_SOFT_SHUTDOWN;
      else
        {
          fprintf (stderr, "Invalid Chassis Control value\n");
          argp_usage (state);
        }
      break;

    case 'R':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }
      cmd_args->cmd = IPMI_CMD_GET_SYSTEM_RESTART_CAUSE;
      break;

    case 'H':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }
      cmd_args->cmd = IPMI_CMD_GET_POWER_ON_HOURS_COUNTER;
      break;

    case 'X':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }
      cmd_args->cmd = IPMI_CMD_SET_POWER_RESTORE_POLICY;

      if(!strcasecmp (arg, "always-on"))
        cmd_args->args.power_restore_policy = IPMI_POWER_RESTORE_POLICY_ALWAYS_POWER_UP_AFTER_AC_IS_LOST;
      else if(!strcasecmp (arg, "always-off"))
        cmd_args->args.power_restore_policy = IPMI_POWER_RESTORE_POLICY_ALWAYS_STAY_POWERED_OFF;
      else if(!strcasecmp (arg, "restore"))
        cmd_args->args.power_restore_policy = IPMI_POWER_RESTORE_POLICY_RESTORE_POWER_TO_STATE_WHEN_AC_WAS_LOST;
      else if(!strcasecmp (arg, "list"))
        cmd_args->args.power_restore_policy = IPMI_POWER_RESTORE_POLICY_NO_CHANGE;
      else
        {
          fprintf (stderr, "Invalid power policy\n");
          argp_usage (state);
        }
      break;

    case 'S':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }

      cmd_args->cmd = IPMI_CMD_SET_POWER_CYCLE_INTERVAL;
      cmd_args->args.power_cycle_interval = strtol (arg, ptr, 10);
      if((ptr && *ptr && **ptr != '\0') || !IPMI_CHASSIS_POWER_CYCLE_INTERVAL_VALID (cmd_args->args.power_cycle_interval))
        {
          fprintf (stderr, "Invalid power cycle interval\n");
          argp_usage (state);
        }
      break;

    case 'B':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }

      cmd_args->cmd = IPMI_CMD_SET_SYSTEM_BOOT_OPTIONS;

      cmd_args->args.boot_option_args.bios_boot_type = -1;
      cmd_args->args.boot_option_args.clear_cmos = -1;
      cmd_args->args.boot_option_args.lock_keyboard = -1;
      cmd_args->args.boot_option_args.boot_device_selector = -1;
      cmd_args->args.boot_option_args.screen_blank = -1;
      cmd_args->args.boot_option_args.lock_out_reset_button = -1;
      cmd_args->args.boot_option_args.firmware_bios_verbosity = -1;
      cmd_args->args.boot_option_args.force_progress_event_traps = -1;
      cmd_args->args.boot_option_args.user_password_bypass = -1;
      cmd_args->args.boot_option_args.console_redirection = -1;
      break;

    case 'G':
      if (cmd_args->cmd != -1)
        {
          fprintf (stderr, "Error: Only one command at a time\n");
          return -1;
        }
      cmd_args->cmd = IPMI_CMD_GET_SYSTEM_BOOT_OPTIONS;

    case ARGP_KEY_ARG:
      /* Too many arguments. */
      argp_usage (state);
      break;

    case ARGP_KEY_END:
      break;

    default:
      rv = boot_flag_parse_opt (key, arg, state);

      if (rv == ARGP_ERR_UNKNOWN)
        return common_parse_opt (key, arg, state,
                                 &(cmd_args->common));
    }

  return 0;
}


void 
ipmi_chassis_argp_parse (int argc, 
                         char **argv,
			 struct ipmi_chassis_arguments *cmd_args)
{
  error_t err;
  init_common_cmd_args (&(cmd_args->common));
  init_hostrange_cmd_args (&(cmd_args->hostrange));
  cmd_args->cmd = -1;

  /* ADMIN is minimum for ipmi-chassis b/c its needed for many of the
   * ipmi cmds used
   */
  cmd_args->common.privilege_level = IPMI_PRIVILEGE_LEVEL_ADMIN;
  err = argp_parse (&argp, argc, argv, ARGP_IN_ORDER, NULL, cmd_args);
  if (err != 0)
    exit (EINVAL);
}
