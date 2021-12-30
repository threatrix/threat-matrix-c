
#include <fjtag.H>

int flash_driver::size()
{
 return ((cfi_->device_geometry.device_size / (bus->width() / 8)) - 1);
}

const char *flash_driver::info()
{
 if (info_.size())
  return info_.c_str();
 char buf[2048], t[128], *ts;
 buf[0] = 0;
 switch (cfi_->identification_string.pri_id_code) {
    case CFI_VENDOR_NULL:
      ts = "null";
      break;
    case CFI_VENDOR_INTEL_ECS:
      ts = "Intel/Sharp Extended Command Set";
      break;
    case CFI_VENDOR_AMD_SCS:
      ts = "AMD/Fujitsu Standard Command Set";
      break;
    case CFI_VENDOR_INTEL_SCS:
      ts = "Intel Standard Command Set";
      break;
    case CFI_VENDOR_AMD_ECS:
      ts = "AMD/Fujitsu Extended Command Set";
      break;
    case CFI_VENDOR_MITSUBISHI_SCS:
      ts = "Mitsubishi Standard Command Set";
      break;
    case CFI_VENDOR_MITSUBISHI_ECS:
      ts = "Mitsubishi Extended Command Set";
      break;
    case CFI_VENDOR_SST_PWCS:
      ts = "Page Write Command Set";
      break;
    default:
      ts = "unknown";
      break;   
 }
 sprintf(t, " Primary command set - %s (code 0x%04X)\n", ts,
  cfi_->identification_string.pri_id_code);
 strcat(buf, t);
 sprintf(t, " Primary extended table at 0x%04X\n",
  (int)(cfi_->identification_string.pri_vendor_tbl));
 strcat(buf, t);
 switch (cfi_->identification_string.alt_id_code) {
    case CFI_VENDOR_NULL:
      ts = "null";
      break;
    case CFI_VENDOR_INTEL_ECS:
      ts = "Intel/Sharp Extended Command Set";
      break;
    case CFI_VENDOR_AMD_SCS:
      ts = "AMD/Fujitsu Standard Command Set";
      break;
    case CFI_VENDOR_INTEL_SCS:
      ts = "Intel Standard Command Set";
      break;
    case CFI_VENDOR_AMD_ECS:
      ts = "AMD/Fujitsu Extended Command Set";
      break;
    case CFI_VENDOR_MITSUBISHI_SCS:
      ts = "Mitsubishi Standard Command Set";
      break;
    case CFI_VENDOR_MITSUBISHI_ECS:
      ts = "Mitsubishi Extended Command Set";
      break;
    case CFI_VENDOR_SST_PWCS:
      ts = "Page Write Command Set";
      break;
    default:
      ts = "unknown";
      break;   
 }
 sprintf(t, " Alternate command set - %s (code 0x%04X)\n", ts,
  cfi_->identification_string.alt_id_code);
 strcat(buf, t);
 sprintf(t, " Alternate extended table at 0x%04X\n",
  (int)(cfi_->identification_string.alt_vendor_tbl));
 strcat(buf, t);
 sprintf(t, " Vcc Logic Supply Minimum Write/Erase or Write voltage: %d mV\n",
  cfi_->system_interface_info.vcc_min_wev);
 strcat(buf, t);
 sprintf(t, " Vcc Logic Supply Maximum Write/Erase or Write voltage: %d mV\n",
  cfi_->system_interface_info.vcc_max_wev);
 strcat(buf, t);
 sprintf(t, " Vpp [Programming] Supply Minimum Write/Erase voltage: %d mV\n",
  cfi_->system_interface_info.vpp_min_wev);
 strcat(buf, t);
 sprintf(t, " Vpp [Programming] Supply Maximum Write/Erase voltage: %d mV\n",
  cfi_->system_interface_info.vpp_max_wev);
 strcat(buf, t);
 sprintf(t, " Typical timeout per single byte/word program: %d us\n",
  cfi_->system_interface_info.typ_single_write_timeout);
 strcat(buf, t);
 sprintf(t, " Typical timeout for maximum-size multi-byte program: %d us\n",
  cfi_->system_interface_info.typ_buffer_write_timeout);
 strcat(buf, t);
 sprintf(t, " Typical timeout per individual block erase: %d ms\n",
  cfi_->system_interface_info.typ_block_erase_timeout);
 strcat(buf, t);
 sprintf(t, " Typical timeout for full chip erase: %d ms\n",
  cfi_->system_interface_info.typ_chip_erase_timeout);
 strcat(buf, t);
 sprintf(t, " Maximum timeout for byte/word program: %d us\n",
  cfi_->system_interface_info.max_single_write_timeout);
 strcat(buf, t);
 sprintf(t, " Maximum timeout for multi-byte program: %d us\n",
  cfi_->system_interface_info.max_buffer_write_timeout);
 strcat(buf, t);
 sprintf(t, " Maximum timeout per individual block erase: %d ms\n",
  cfi_->system_interface_info.max_block_erase_timeout);
 strcat(buf, t);
 sprintf(t, " Maximum timeout for chip erase: %d ms\n",
  cfi_->system_interface_info.max_chip_erase_timeout);
 strcat(buf, t);
 strcat(buf, " Device geometry definition:\n");
 sprintf(t, " Device Size: %d bytes (%d Kbytes, %d Mbytes)\n",
  cfi_->device_geometry.device_size,
  cfi_->device_geometry.device_size / 1024,
  cfi_->device_geometry.device_size / (1024 * 1024));
 strcat(buf, t);
 switch (cfi_->device_geometry.device_interface) {
    case CFI_INTERFACE_X8:
      ts = "x8";
      break;
    case CFI_INTERFACE_X16:
      ts = "x16";
      break;
    case CFI_INTERFACE_X8_X16:
      ts = "x8/x16";
      break;
    case CFI_INTERFACE_X32:
      ts = "x32";
      break;
    case CFI_INTERFACE_X16_X32:
      ts = "x16/x32";
      break;
    default:
      ts = "unknown";
      break;
 }
 sprintf(t, " Interface Code description: %s (0x%04X)\n", ts,
  cfi_->device_geometry.device_interface);
 strcat(buf, t);
 sprintf(t, " Maximum number of bytes in multi-byte program: %d\n",
  cfi_->device_geometry.max_bytes_write);
 strcat(buf, t);
 sprintf(t, " Number of Erase Block Regions within device: %d\n",
  cfi_->device_geometry.number_of_erase_regions);
 strcat(buf, t);
 strcat(buf, " Erase Block Region Information:\n");
 for (int i = 0; i < cfi_->device_geometry.number_of_erase_regions; i++) {
  sprintf(t, "   Region %d:\n", i);
  strcat(buf, t);
  sprintf(t, "   Erase Block Size: %d bytes (%d Kbytes)\n",
    cfi_->device_geometry.erase_block_regions[i].erase_block_size,
    cfi_->device_geometry.erase_block_regions[i].erase_block_size / 1024);
  strcat(buf, t);
  sprintf(t, "   Number of Erase Blocks within region: %d\n",
    cfi_->device_geometry.erase_block_regions[i].number_of_erase_blocks);
  strcat(buf, t);
 }
 strcat(buf, pri_table_.c_str());
 strcat(buf, alt_table_.c_str());
 info_ = buf;
 return info_.c_str();
}
