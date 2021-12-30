
#include <fjtag.H>

int cfi_detect(Bus *b)
{
 if (b->flash) // already detected
  return -1;
 if ((b->width() != 8) && (b->width() != 16) && (b->width() != 32)) {
  message("unsupported bus width %d", b->width());
  return -1;
 }
 b->prepare();
 b->write(0, CFI_CMD_READ_ARRAY1);
 b->write(CFI_CMD_QUERY_OFFSET, CFI_CMD_QUERY);
 int q, r, y;
 q = b->read(CFI_QUERY_ID_OFFSET);
 r = b->read(CFI_QUERY_ID_OFFSET + 1);
 y = b->read(CFI_QUERY_ID_OFFSET + 2);
 if ((q == 'Q') && (r == 'R') && (y == 'Y'))
  message("CFI capable device detected ...");
 else
  return 0;
 cfi_query_structure_t *cfi = new cfi_query_structure_t;
 memset(cfi, 0, sizeof(cfi_query_structure_t));
 cfi->identification_string.pri_id_code = b->read(PRI_VENDOR_ID_OFFSET);
 cfi->identification_string.pri_vendor_tbl = (void *)
  (b->read(PRI_VENDOR_TABLE_ADR_OFFSET) |
  (b->read(PRI_VENDOR_TABLE_ADR_OFFSET + 1) << 8));
 cfi->identification_string.alt_id_code = b->read(ALT_VENDOR_ID_OFFSET);
 cfi->identification_string.alt_vendor_tbl = (void *)
  (b->read(ALT_VENDOR_TABLE_ADR_OFFSET) |
  (b->read(ALT_VENDOR_TABLE_ADR_OFFSET + 1) << 8));
 int tmp = b->read(VCC_MIN_WEV_OFFSET);
 cfi->system_interface_info.vcc_min_wev =
  ((tmp >> 4) & 0xF) * 1000 + (tmp & 0xF) * 100;
 tmp = b->read(VCC_MAX_WEV_OFFSET);
 cfi->system_interface_info.vcc_max_wev =
  ((tmp >> 4) & 0xF) * 1000 + (tmp & 0xF) * 100;
 tmp = b->read(VPP_MIN_WEV_OFFSET);
 cfi->system_interface_info.vpp_min_wev =
  ((tmp >> 4) & 0xF) * 1000 + (tmp & 0xF) * 100;
 tmp = b->read(VPP_MAX_WEV_OFFSET);
 cfi->system_interface_info.vpp_max_wev =
  ((tmp >> 4) & 0xF) * 1000 + (tmp & 0xF) * 100;
 tmp = b->read(TYP_SINGLE_WRITE_TIMEOUT_OFFSET);
 cfi->system_interface_info.typ_single_write_timeout = tmp ? (1 << tmp) : 0;
 tmp = b->read(TYP_BUFFER_WRITE_TIMEOUT_OFFSET);
 cfi->system_interface_info.typ_buffer_write_timeout = tmp ? (1 << tmp) : 0;
 tmp = b->read(TYP_BLOCK_ERASE_TIMEOUT_OFFSET);
 cfi->system_interface_info.typ_block_erase_timeout = tmp ? (1 << tmp) : 0;
 tmp = b->read(TYP_CHIP_ERASE_TIMEOUT_OFFSET);
 cfi->system_interface_info.typ_chip_erase_timeout = tmp ? (1 << tmp) : 0;
 tmp = b->read(MAX_SINGLE_WRITE_TIMEOUT_OFFSET);
 cfi->system_interface_info.max_single_write_timeout =
  (tmp ? (1 << tmp) : 0) * cfi->system_interface_info.typ_single_write_timeout;
 tmp = b->read(MAX_BUFFER_WRITE_TIMEOUT_OFFSET);
 cfi->system_interface_info.max_buffer_write_timeout =
  (tmp ? (1 << tmp) : 0) * cfi->system_interface_info.typ_buffer_write_timeout;
 tmp = b->read(MAX_BLOCK_ERASE_TIMEOUT_OFFSET);
 cfi->system_interface_info.max_block_erase_timeout =
  (tmp ? (1 << tmp) : 0) * cfi->system_interface_info.typ_block_erase_timeout;
 tmp = b->read(MAX_CHIP_ERASE_TIMEOUT_OFFSET);
 cfi->system_interface_info.max_chip_erase_timeout =
  (tmp ? (1 << tmp) : 0) * cfi->system_interface_info.typ_chip_erase_timeout;
 // Device geometry
 cfi->device_geometry.device_size = 1 << b->read(DEVICE_SIZE_OFFSET);
 cfi->device_geometry.device_interface =
  b->read(FLASH_DEVICE_INTERFACE_OFFSET) |
  (b->read(FLASH_DEVICE_INTERFACE_OFFSET + 1) << 8);
 cfi->device_geometry.max_bytes_write =
  1 << (b->read(MAX_BYTES_WRITE_OFFSET) |
  (b->read(MAX_BYTES_WRITE_OFFSET + 1) << 8));
 tmp = cfi->device_geometry.number_of_erase_regions =
  b->read(NUMBER_OF_ERASE_REGIONS_OFFSET);
 cfi->device_geometry.erase_block_regions = new cfi_erase_block_region_t[tmp];
 int a;
 for (int i = 0; i < tmp; i++) {
  a = ERASE_BLOCK_REGION_OFFSET + i * 4;
  uint32_t y = b->read(a) | (b->read(a + 1) << 8);
  uint32_t z = b->read(a + 2) | (b->read(a + 3) << 8);
  if (!z)
    z = 128; // 0 for 128-byte block size, else 256-byte block size
  else
    z *= 256;
  cfi->device_geometry.erase_block_regions[i].erase_block_size = z;
  cfi->device_geometry.erase_block_regions[i].number_of_erase_blocks = y + 1;
 }
 b->write(0, CFI_CMD_READ_ARRAY1);
 switch (cfi->identification_string.pri_id_code) {
  case CFI_VENDOR_INTEL_ECS:
    // Intel/Sharp Extended Command Set
    b->flash = new intel_ecs_flash_driver(b, cfi);
    break;
  case CFI_VENDOR_AMD_SCS:
     // AMD/Fujitsu Standard Command Set
    b->flash = new amd_scs_flash_driver(b, cfi);
    break;
  case CFI_VENDOR_INTEL_SCS:
    // Intel Standard Command Set
    b->flash = new intel_scs_flash_driver(b, cfi);
    break;
  case CFI_VENDOR_AMD_ECS:
    // AMD/Fujitsu Extended Command Set
    b->flash = new amd_ecs_flash_driver(b, cfi);
    break;
  case CFI_VENDOR_MITSUBISHI_SCS:
    // Mitsubishi Standard Command Set
    b->flash = new mitsubishi_scs_flash_driver(b, cfi);
    break;
  case CFI_VENDOR_MITSUBISHI_ECS:
    // Mitsubishi Extended Command Set
    b->flash = new mitsubishi_ecs_flash_driver(b, cfi);
    break;
  case CFI_VENDOR_SST_PWCS:
    // Page Write Command Set
    b->flash = new sst_pwcs_flash_driver(b, cfi);
    break;
  case CFI_VENDOR_NULL:
  default:
    b->flash = new null_flash_driver(b, cfi);
    break;
  }
  return 1;
}
