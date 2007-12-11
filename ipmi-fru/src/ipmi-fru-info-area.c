/*****************************************************************************\
 *  $Id: ipmi-fru-info-area.c,v 1.6.2.1 2007-12-11 19:04:34 chu11 Exp $
 *****************************************************************************
 *  Copyright (C) 2007 Lawrence Livermore National Security, LLC.
 *  Copyright (C) 2007 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Albert Chu <chu11@llnl.gov>
 *  UCRL-CODE-232183
 *
 *  This file is part of Ipmi-fru, a tool used for retrieving
 *  motherboard field replaceable unit (FRU) information. For details,
 *  see http://www.llnl.gov/linux/.
 *
 *  Ipmi-fru is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  Ipmi-fru is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Ipmi-fru.  If not, see <http://www.gnu.org/licenses/>.
\*****************************************************************************/

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#if STDC_HEADERS
#include <string.h>
#endif /* STDC_HEADERS */
#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else  /* !TIME_WITH_SYS_TIME */
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else /* !HAVE_SYS_TIME_H */
#include <time.h>
#endif  /* !HAVE_SYS_TIME_H */
#endif /* !TIME_WITH_SYS_TIME */
#include <assert.h>

#include <freeipmi/freeipmi.h>
#include <freeipmi/api/api.h>

#include "ipmi-fru.h"
#include "ipmi-fru-fiid.h"
#include "ipmi-fru-info-area.h"
#include "ipmi-fru-util.h"

fru_err_t
ipmi_fru_output_chassis_info_area(ipmi_fru_state_data_t *state_data,
                                  uint8_t *frubuf,
                                  unsigned int frusize,
                                  unsigned int offset)
{
  uint64_t chassis_info_area_length;
  fru_err_t rv = FRU_ERR_FATAL_ERROR;
  fru_err_t ret;
  uint8_t chassis_type;
  uint32_t chassis_offset = 0;
  uint32_t chassis_max_offset = 0;
  unsigned int len_parsed;

  assert(state_data);
  assert(frubuf);
  assert(frusize);
  assert(offset);

  if ((ret = ipmi_fru_get_info_area_length(state_data,
                                           frubuf,
                                           frusize,
                                           offset,
                                           "Chassis",
                                           &chassis_info_area_length)) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }

  if (!chassis_info_area_length)
    {
      rv = FRU_ERR_NON_FATAL_ERROR;
      goto cleanup;
    }

  if ((ret = ipmi_fru_dump_hex(state_data,
                               frubuf,
                               frusize,
                               offset*8,
                               chassis_info_area_length*8,
                               "Chassis Info Area")) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }

  if ((ret = ipmi_fru_check_checksum(state_data,
                                     frubuf,
                                     frusize,
                                     offset*8,
                                     chassis_info_area_length*8,
                                     0,
                                     "Chassis Info Header")) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }

  chassis_max_offset = offset*8 + chassis_info_area_length*8;

  chassis_offset = offset*8 + 2;
  chassis_type = frubuf[chassis_offset];

  if (!IPMI_FRU_CHASSIS_TYPE_VALID(chassis_type))
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Chassis Type Unknown: 0x%02X\n", 
                      chassis_type);
      rv = FRU_ERR_NON_FATAL_ERROR;
      goto cleanup;
    }

  pstdout_printf(state_data->pstate, 
                 "  Chassis Info Area Type: %s\n",
                 ipmi_fru_chassis_types[chassis_type]);

  chassis_offset++;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               chassis_offset,
                                               chassis_max_offset,
                                               NULL,
                                               &len_parsed,
                                               "Chassis Part Number")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Chassis Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  chassis_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               chassis_offset,
                                               chassis_max_offset,
                                               NULL,
                                               &len_parsed,
                                               "Chassis Serial Number")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Chassis Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  chassis_offset += len_parsed;

  if (state_data->prog_data->args->verbose_count)
    {
      while (chassis_offset < frusize
             && chassis_offset < chassis_max_offset
             && frubuf[chassis_offset] != IPMI_FRU_SENTINEL_VALUE)
        {
          if ((ret = ipmi_fru_output_type_length_field(state_data,
                                                       frubuf,
                                                       frusize,
                                                       chassis_offset,
                                                       chassis_max_offset,
                                                       NULL,
                                                       &len_parsed,
                                                       "Chassis Custom Info")) != FRU_ERR_SUCCESS)
            {
              pstdout_fprintf(state_data->pstate, 
                              stderr,
                              "  FRU Chassis Info: Remaining Area Cannot Be Parsed\n");
              rv = ret;
              goto cleanup;
            }
          
          chassis_offset += len_parsed;
        }
      
      if (state_data->prog_data->args->verbose_count >= 2
          && (chassis_offset >= frusize
              || chassis_offset >= chassis_max_offset))
        {
          pstdout_fprintf(state_data->pstate,
                          stderr,
                          "  FRU Missing Sentinel Value\n");
          rv = FRU_ERR_NON_FATAL_ERROR;
          goto cleanup;
        }
    }

  rv = FRU_ERR_SUCCESS;
 cleanup:
  return (rv);
}
                         
fru_err_t
ipmi_fru_output_board_info_area(ipmi_fru_state_data_t *state_data,
                                uint8_t *frubuf,
                                unsigned int frusize,
                                unsigned int offset)
{
  uint64_t board_info_area_length;
  fru_err_t rv = FRU_ERR_FATAL_ERROR;
  fru_err_t ret;
  uint8_t language_code;
  uint32_t mfg_date_time = 0;
  time_t mfg_date_time_tmp = 0;
  struct tm mfg_date_time_tm;
  char mfg_date_time_buf[FRU_BUF_LEN+1];
  uint32_t board_offset = 0;
  uint32_t board_max_offset = 0;
  unsigned int len_parsed;

  assert(state_data);
  assert(frubuf);
  assert(frusize);
  assert(offset);

  if ((ret = ipmi_fru_get_info_area_length(state_data,
                                           frubuf,
                                           frusize,
                                           offset,
                                           "Board",
                                           &board_info_area_length)) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }

  if (!board_info_area_length)
    {
      rv = FRU_ERR_NON_FATAL_ERROR;
      goto cleanup;
    }

  if ((ret = ipmi_fru_dump_hex(state_data,
                               frubuf,
                               frusize,
                               offset*8,
                               board_info_area_length*8,
                               "Board Info Area")) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }

  if ((ret = ipmi_fru_check_checksum(state_data,
                                     frubuf,
                                     frusize,
                                     offset*8,
                                     board_info_area_length*8,
                                     0,
                                     "Board Info Header")) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }

  board_max_offset = offset*8 + board_info_area_length*8;

  board_offset = offset*8 + 2;
  language_code = frubuf[board_offset];

  if (state_data->prog_data->args->verbose_count >= 2)
    {
      pstdout_printf(state_data->pstate, 
                     "  FRU Board Info Area Language Code: 0x%02X\n",
                     language_code);
    }

  if (state_data->prog_data->args->verbose_count)
    {
      /* mfg_date_time is little endian - see spec */
      board_offset++;
      mfg_date_time |= frubuf[board_offset];
      board_offset++;
      mfg_date_time |= (frubuf[board_offset] << 8);
      board_offset++;
      mfg_date_time |= (frubuf[board_offset] << 16);
      
      /* Here, epoch is 0:00 hrs 1/1/96 
       *
       * mfg_date_time is in minutes
       *
       * So convert into ansi epoch to output date/time
       *
       * 26 years difference in epoch
       * 365 days/year
       * etc.
       * 
       */
      mfg_date_time_tmp = 26 * 365 * 24 * 60 * 60;
      mfg_date_time_tmp += (mfg_date_time * 60);
      
      localtime_r(&mfg_date_time_tmp, &mfg_date_time_tm);
      
      memset(mfg_date_time_buf, '\0', FRU_BUF_LEN+1);
      strftime(mfg_date_time_buf, FRU_BUF_LEN, "%D - %T", &mfg_date_time_tm);
      
      pstdout_printf(state_data->pstate,
                     "  FRU Board Info Area Manufacturing Date/Time: %s\n",
                     mfg_date_time_buf);
    }
  else
    board_offset += 3;

  board_offset++;
  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               board_offset,
                                               board_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Board Manufacturer")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Board Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  board_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               board_offset,
                                               board_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Board Product Name")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Board Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  board_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               board_offset,
                                               board_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Board Serial Number")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Board Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  board_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               board_offset,
                                               board_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Board Part Number")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Board Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  board_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               board_offset,
                                               board_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Board FRU File ID")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Board Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  board_offset += len_parsed;

  if (state_data->prog_data->args->verbose_count)
    {
      while (board_offset < frusize
             && board_offset < board_max_offset
             && frubuf[board_offset] != IPMI_FRU_SENTINEL_VALUE)
        {
          if ((ret = ipmi_fru_output_type_length_field(state_data,
                                                       frubuf,
                                                       frusize,
                                                       board_offset,
                                                       board_max_offset,
                                                       &language_code,
                                                       &len_parsed,
                                                       "Board Custom Info")) != FRU_ERR_SUCCESS)
            {
              pstdout_fprintf(state_data->pstate, 
                              stderr,
                              "  FRU Board Info: Remaining Area Cannot Be Parsed\n");
              rv = ret;
              goto cleanup;
            }
          
          board_offset += len_parsed;
        }
      
      if (state_data->prog_data->args->verbose_count >= 2
          && (board_offset >= frusize
              || board_offset >= board_max_offset))
        {
          pstdout_fprintf(state_data->pstate,
                          stderr,
                          "  FRU Missing Sentinel Value\n");
          rv = FRU_ERR_NON_FATAL_ERROR;
          goto cleanup;
        }
    }

  rv = FRU_ERR_SUCCESS;
 cleanup:
  return (rv);
}

fru_err_t
ipmi_fru_output_product_info_area(ipmi_fru_state_data_t *state_data,
                                  uint8_t *frubuf,
                                  unsigned int frusize,
                                  unsigned int offset)
{
  uint64_t product_info_area_length;
  fru_err_t rv = FRU_ERR_FATAL_ERROR;
  fru_err_t ret;
  uint8_t language_code;
  uint32_t product_offset = 0;
  uint32_t product_max_offset = 0;
  unsigned int len_parsed;

  assert(state_data);
  assert(frubuf);
  assert(frusize);
  assert(offset);

  if ((ret = ipmi_fru_get_info_area_length(state_data,
                                           frubuf,
                                           frusize,
                                           offset,
                                           "Product",
                                           &product_info_area_length)) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }
  
  if (!product_info_area_length)
    {
      rv = FRU_ERR_NON_FATAL_ERROR;
      goto cleanup;
    }

  if ((ret = ipmi_fru_dump_hex(state_data,
                               frubuf,
                               frusize,
                               offset*8,
                               product_info_area_length*8,
                               "Product Info Area")) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }

  if ((ret = ipmi_fru_check_checksum(state_data,
                                     frubuf,
                                     frusize,
                                     offset*8,
                                     product_info_area_length*8,
                                     0,
                                     "Product Info Header")) != FRU_ERR_SUCCESS)
    {
      rv = ret;
      goto cleanup;
    }

  product_max_offset = offset*8 + product_info_area_length*8;

  product_offset = offset*8 + 2;
  language_code = frubuf[product_offset];

  if (state_data->prog_data->args->verbose_count >= 2)
    {
      pstdout_printf(state_data->pstate, 
                     "  FRU Product Info Area Language Code: 0x%02X\n",
                     language_code);
    }

  product_offset++;
  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               product_offset,
                                               product_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Product Manufacturer Name")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Product Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  product_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               product_offset,
                                               product_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Product Product Name")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Product Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  product_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               product_offset,
                                               product_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Product Part/Model Number")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Product Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  product_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               product_offset,
                                               product_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Product Version Type")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Product Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  product_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               product_offset,
                                               product_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Product Serial Number")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Product Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  product_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               product_offset,
                                               product_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Product Asset Tag")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Product Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  product_offset += len_parsed;

  if ((ret = ipmi_fru_output_type_length_field(state_data,
                                               frubuf,
                                               frusize,
                                               product_offset,
                                               product_max_offset,
                                               &language_code,
                                               &len_parsed,
                                               "Product FRU File ID")) != FRU_ERR_SUCCESS)
    {
      pstdout_fprintf(state_data->pstate, 
                      stderr,
                      "  FRU Product Info: Remaining Area Cannot Be Parsed\n");
      rv = ret;
      goto cleanup;
    }
    
  product_offset += len_parsed;

  if (state_data->prog_data->args->verbose_count)
    {
      while (product_offset < frusize
             && product_offset < product_max_offset
             && frubuf[product_offset] != IPMI_FRU_SENTINEL_VALUE)
        {
          if ((ret = ipmi_fru_output_type_length_field(state_data,
                                                       frubuf,
                                                       frusize,
                                                       product_offset,
                                                       product_max_offset,
                                                       &language_code,
                                                       &len_parsed,
                                                       "Product Custom Info")) != FRU_ERR_SUCCESS)
            {
              pstdout_fprintf(state_data->pstate, 
                              stderr,
                              "  FRU Product Info: Remaining Area Cannot Be Parsed\n");
              rv = ret;
              goto cleanup;
            }
          
          product_offset += len_parsed;
        }
      
      if (state_data->prog_data->args->verbose_count >= 2
          && (product_offset >= frusize
              || product_offset >= product_max_offset))
        {
          pstdout_fprintf(state_data->pstate,
                          stderr,
                          "  FRU Missing Sentinel Value\n");
          rv = FRU_ERR_NON_FATAL_ERROR;
          goto cleanup;
        }
    }

  rv = FRU_ERR_SUCCESS;
 cleanup:
  return (rv);
}

