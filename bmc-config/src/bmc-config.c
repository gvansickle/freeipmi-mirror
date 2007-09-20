/* 

   bmc-config - tool to configure bmc

   Copyright (C) 2006 FreeIPMI Core Team

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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <argp.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#endif
#ifdef HAVE_SYS_IO_H
#include <sys/io.h>
#endif
#include <syslog.h>
#include <assert.h>
#include <stdarg.h>
#if HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#include <errno.h>
#if HAVE_GETOPT_H
#include <getopt.h>
#endif
#include <stdint.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <assert.h>

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <string.h>
#include <sys/types.h>

#include "bmc-config.h"
#include "bmc-config-argp.h"
#include "bmc-config-commit.h"
#include "bmc-config-checkout.h"
#include "bmc-config-diff.h"
#include "bmc-config-sections.h"

#include "tool-common.h"
#include "freeipmi-portability.h"

#include "config-common.h"

void
_bmc_config_state_data_init(bmc_config_state_data_t *state_data)
{
  assert (state_data);

  memset(state_data, '\0', sizeof(bmc_config_state_data_t));
  state_data->prog_data = NULL;
  state_data->dev = NULL;
  state_data->sections = NULL;

  state_data->cipher_suite_entry_count = 0;
  state_data->cipher_suite_id_supported_set = 0;
  state_data->cipher_suite_priv_set = 0;

  state_data->lan_channel_number_initialized = 0;
  state_data->serial_channel_number_initialized = 0;
  state_data->sol_channel_number_initialized = 0;
  state_data->number_of_lan_destinations_initialized = 0;
}

static int
_bmc_config (void *arg)
{
  bmc_config_state_data_t state_data;
  bmc_config_prog_data_t *prog_data;
  ipmi_device_t dev = NULL;
  char errmsg[IPMI_DEVICE_OPEN_ERRMSGLEN];
  struct section *sections = NULL;
  int exit_code = -1;
  config_err_t ret = 0;

  prog_data = (bmc_config_prog_data_t *)arg;

  if (!(dev = ipmi_device_open(prog_data->progname,
                               prog_data->args->common.hostname,
                               &(prog_data->args->common),
                               errmsg,
                               IPMI_DEVICE_OPEN_ERRMSGLEN)))
    {
      fprintf(stderr, "%s\n", errmsg);
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }

  _bmc_config_state_data_init(&state_data);
  state_data.dev = dev;
  state_data.prog_data = prog_data;

  if (!(sections = bmc_config_sections_list_create (&state_data)))
    {
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }
  state_data.sections = sections;

  switch (prog_data->args->action) {
  case CONFIG_ACTION_CHECKOUT:
    ret = bmc_checkout (&state_data);
    break;
  case CONFIG_ACTION_COMMIT:
    ret = bmc_commit (&state_data);
    break;
  case CONFIG_ACTION_DIFF:
    ret = bmc_diff (&state_data);
    break;
  case CONFIG_ACTION_LIST_SECTIONS:
    ret = bmc_config_sections_list (&state_data);
    break;
  case CONFIG_ACTION_INFO:
    /* shutup gcc warning */
    break;
  }
  
  if (ret == CONFIG_ERR_FATAL_ERROR || ret == CONFIG_ERR_NON_FATAL_ERROR)
    {
      exit_code = EXIT_FAILURE;
      goto cleanup;
    }

  exit_code = 0;
 cleanup:
  if (dev)
    {
      ipmi_close_device (dev);
      ipmi_device_destroy (dev);
    }
  if (sections)
    bmc_config_sections_list_destroy(&state_data, sections);
  return exit_code;
}

int
main (int argc, char *argv[])
{
  bmc_config_prog_data_t prog_data;
  struct bmc_config_arguments cmd_args;
  int exit_code;

  ipmi_disable_coredump();

  prog_data.progname = argv[0];
  bmc_config_argp_parse (argc, argv, &cmd_args);

  if (bmc_config_args_validate (&cmd_args) < 0)
    return (EXIT_FAILURE);

  prog_data.args = &cmd_args;

  exit_code = _bmc_config(&prog_data);

  return (exit_code);
}
