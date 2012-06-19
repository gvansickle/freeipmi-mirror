/*
 * Copyright (C) 2008-2012 FreeIPMI Core Team
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

#ifndef IPMI_OEM_THIRDPARTY_H
#define IPMI_OEM_THIRDPARTY_H

#include "ipmi-oem.h"

/* Common functions for OEM extensions shared between multiple vendors
 * b/c they share a common third party firmware
 */

int ipmi_oem_thirdparty_get_extended_config_value (ipmi_oem_state_data_t *state_data,
						   uint8_t configuration_id,
						   uint8_t attribute_id,
						   uint8_t index,
						   unsigned int value_return_length,
						   uint32_t *value);

int ipmi_oem_thirdparty_get_extended_config_string (ipmi_oem_state_data_t *state_data,
						    uint8_t configuration_id,
						    uint8_t attribute_id,
						    uint8_t index,
						    char *buf,
						    unsigned int buflen);

int ipmi_oem_thirdparty_set_extended_config_value (ipmi_oem_state_data_t *state_data,
						   uint8_t configuration_id,
						   uint8_t attribute_id,
						   uint8_t index,
						   unsigned int value_length,
						   uint32_t value);

int ipmi_oem_thirdparty_set_extended_config_string (ipmi_oem_state_data_t *state_data,
						    uint8_t configuration_id,
						    uint8_t attribute_id,
						    uint8_t index,
						    char *buf,
						    unsigned int buflen);

#endif /* IPMI_OEM_THIRDPARTY_H */