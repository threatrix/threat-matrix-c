
#ifndef	_FLASH_H_
#define	_FLASH_H_

#include <cfi.H>
#include <bus.H>

#include <string>

using namespace std;

class Bus;

class flash_driver {
  public:
    string name_, manufacturer_, info_, pri_table_, alt_table_;
    Bus *bus;
    cfi_query_structure_t *cfi_;
    int protected_, mid_, cid_, max_sector;
    flash_driver(Bus *b, cfi_query_structure_t *c) : bus(b), cfi_(c),
      max_sector(0) { }
    virtual ~flash_driver() {
      if (cfi_) {
       if (cfi_->device_geometry.erase_block_regions)
        delete [] cfi_->device_geometry.erase_block_regions;
       delete cfi_;
      }
   }
    virtual int read(uint32_t adr) = 0;
    virtual int burn(uint32_t adr, uint32_t data) = 0;
    virtual int burn(uint32_t adr, unsigned char *data, uint32_t len) = 0;
    virtual int chip_erase() = 0;
    virtual int sector_erase(uint32_t sector) = 0;
    virtual int protect(uint32_t sector) = 0;
    virtual int unprotect(uint32_t sector) = 0;
    virtual int isprotected(uint32_t sector) = 0;
    virtual int reset() = 0;

    const char *name() { return name_.c_str(); }
    const char *manufacturer() { return manufacturer_.c_str(); }
    const char *info();
    cfi_query_structure_t *cfi() { return cfi_; }
    int cid() { return cid_; }
    int mid() { return mid_; }
    int isprotected() { return protected_; }
    int sectors() { return max_sector; }
    int size();
};

class intel_ecs_flash_driver : public flash_driver {
  public:
    intel_ecs_flash_driver(Bus *b, cfi_query_structure_t *c) :
      flash_driver(b, c) { name_ = "unsupported intel ecs flash chip"; }
    int read(uint32_t adr) { return 0; }
    int burn(uint32_t adr, uint32_t data) { return 0; }
    int burn(uint32_t adr, unsigned char *data, uint32_t len) { return 0; }
    int sector_erase(uint32_t sec) { return 0; }
    int chip_erase() { return 0; }
    int protect(uint32_t sec) { return 0; }
    int unprotect(uint32_t sec) { return 0; }
    int isprotected(uint32_t sec) { return 0; }
    int reset() { return 0; }
};

class intel_scs_flash_driver : public flash_driver {
  public:
    intel_scs_flash_driver(Bus *b, cfi_query_structure_t *c) :
      flash_driver(b, c) { name_ = "unsupported intel scs flash chip"; }
    int read(uint32_t adr) { return 0; }
    int burn(uint32_t adr, uint32_t data) { return 0; }
    int burn(uint32_t adr, unsigned char *data, uint32_t len) { return 0; }
    int sector_erase(uint32_t sec) { return 0; }
    int chip_erase() { return 0; }
    int protect(uint32_t sec) { return 0; }
    int unprotect(uint32_t sec) { return 0; }
    int isprotected(uint32_t sec) { return 0; }
    int reset() { return 0; }
};

class amd_ecs_flash_driver : public flash_driver {
  public:
    amd_ecs_flash_driver(Bus *b, cfi_query_structure_t *c) :
      flash_driver(b, c) { name_ = "unsupported amd ecs flash chip"; }
    int read(uint32_t adr) { return 0; }
    int burn(uint32_t adr, uint32_t data) { return 0; }
    int burn(uint32_t adr, unsigned char *data, uint32_t len) { return 0; }
    int sector_erase(uint32_t sec) { return 0; }
    int chip_erase() { return 0; }
    int protect(uint32_t sec) { return 0; }
    int unprotect(uint32_t sec) { return 0; }
    int isprotected(uint32_t sec) { return 0; }
    int reset() { return 0; }
};

class amd_scs_flash_driver : public flash_driver {
    int revision;
    int status(uint32_t adr = 0);
  public:
    amd_scs_flash_driver(Bus *b, cfi_query_structure_t *c);
    int read(uint32_t adr);
    int burn(uint32_t adr, uint32_t data);
    int burn(uint32_t adr, unsigned char *data, uint32_t len);
    int sector_erase(uint32_t sec);
    int chip_erase();
    int protect(uint32_t sec);
    int unprotect(uint32_t sec);
    int isprotected(uint32_t sec);
    int reset();
};

class mitsubishi_ecs_flash_driver : public flash_driver {
  public:
    mitsubishi_ecs_flash_driver(Bus *b, cfi_query_structure_t *c) :
      flash_driver(b, c) { name_ = "unsupported mitsubishi ecs flash chip"; }
    int read(uint32_t adr) { return 0; }
    int burn(uint32_t adr, uint32_t data) { return 0; }
    int burn(uint32_t adr, unsigned char *data, uint32_t len) { return 0; }
    int sector_erase(uint32_t sec) { return 0; }
    int chip_erase() { return 0; }
    int protect(uint32_t sec) { return 0; }
    int unprotect(uint32_t sec) { return 0; }
    int isprotected(uint32_t sec) { return 0; }
    int reset() { return 0; }
};

class mitsubishi_scs_flash_driver : public flash_driver {
  public:
    mitsubishi_scs_flash_driver(Bus *b, cfi_query_structure_t *c) :
      flash_driver(b, c) { name_ = "unsupported mitsubishi scs flash chip"; }
    int read(uint32_t adr) { return 0; }
    int burn(uint32_t adr, uint32_t data) { return 0; }
    int burn(uint32_t adr, unsigned char *data, uint32_t len) { return 0; }
    int sector_erase(uint32_t sec) { return 0; }
    int chip_erase() { return 0; }
    int protect(uint32_t sec) { return 0; }
    int unprotect(uint32_t sec) { return 0; }
    int isprotected(uint32_t sec) { return 0; }
    int reset() { return 0; }
};

class sst_pwcs_flash_driver : public flash_driver {
  public:
    sst_pwcs_flash_driver(Bus *b, cfi_query_structure_t *c) :
      flash_driver(b, c) { name_ = "unsupported sst pwcs flash chip"; }
    int read(uint32_t adr) { return 0; }
    int burn(uint32_t adr, uint32_t data) { return 0; }
    int burn(uint32_t adr, unsigned char *data, uint32_t len) { return 0; }
    int sector_erase(uint32_t sec) { return 0; }
    int chip_erase() { return 0; }
    int protect(uint32_t sec) { return 0; }
    int unprotect(uint32_t sec) { return 0; }
    int isprotected(uint32_t sec) { return 0; }
    int reset() { return 0; }
};

class null_flash_driver : public flash_driver {
  public:
    null_flash_driver(Bus *b, cfi_query_structure_t *c) :
      flash_driver(b, c) { name_ = "Null Flash Chip"; }
    int read(uint32_t adr) { return 0; }
    int burn(uint32_t adr, uint32_t data) { return 0; }
    int burn(uint32_t adr, unsigned char *data, uint32_t len) { return 0; }
    int sector_erase(uint32_t sec) { return 0; }
    int chip_erase() { return 0; }
    int protect(uint32_t sec) { return 0; }
    int unprotect(uint32_t sec) { return 0; }
    int isprotected(uint32_t sec) { return 0; }
    int reset() { return 0; }
};

extern int cfi_detect(Bus *b);
extern int jedec_detect(Bus *b);

#endif
