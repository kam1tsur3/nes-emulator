#define NES_FILE_NAME   "./cassette/sample1/sample1.nes"
#define HEADER_SIZE     16
#define HEADER_MAGIC    "NES\x1A"
#define PRG_PAGE_SIZE   0x4000
#define CHR_PAGE_SIZE   0x2000

void load_rom();

struct header {
  uint32_t MAGIC;
  uint8_t size_prg_rom;
  uint8_t size_chr_rom;
  uint8_t flag6;
  uint8_t flag7;
  uint8_t flag8;
  uint8_t flag9;
  uint8_t flag10;
  uint8_t unused[5];
};