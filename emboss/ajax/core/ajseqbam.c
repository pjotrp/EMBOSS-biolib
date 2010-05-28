#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>
#include <sys/file.h>

#include "ajax.h"
#include "ajseqbam.h"

static AjBool bam_is_be = ajFalse;

const char *o_hd_tags[] = {"SO","GO",NULL};
const char *r_hd_tags[] = {"VN",NULL};

const char *o_sq_tags[] = {"AS","M5","UR","SP",NULL};
const char *r_sq_tags[] = {"SN","LN",NULL};
const char *u_sq_tags[] = {"SN",NULL};

const char *o_rg_tags[] = {"LB","DS","PU","PI","CN","DT","PL",NULL};
const char *r_rg_tags[] = {"ID",NULL};
const char *u_rg_tags[] = {"ID",NULL};

const char *o_pg_tags[] = {"VN","CL",NULL};
const char *r_pg_tags[] = {"ID",NULL};

const char *types[]          = {"HD","SQ","RG","PG","CO",NULL};
const char **optional_tags[] = {o_hd_tags,o_sq_tags,o_rg_tags,o_pg_tags,NULL,NULL};
const char **required_tags[] = {r_hd_tags,r_sq_tags,r_rg_tags,r_pg_tags,NULL,NULL};
const char **unique_tags[]   = {NULL,     u_sq_tags,u_rg_tags,NULL,NULL,NULL};


unsigned char bam_nt16_table[256] = {
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	 1, 2, 4, 8, 15,15,15,15, 15,15,15,15, 15, 0 /*=*/,15,15,
	15, 1,14, 2, 13,15,15, 4, 11,15,15,12, 15, 3,15,15,
	15,15, 5, 6,  8,15, 7, 9, 15,10,15,15, 15,15,15,15,
	15, 1,14, 2, 13,15,15, 4, 11,15,15,12, 15, 3,15,15,
	15,15, 5, 6,  8,15, 7, 9, 15,10,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15,
	15,15,15,15, 15,15,15,15, 15,15,15,15, 15,15,15,15
};

const char *bam_nt16_rev_table = "=ACMGRSVTWYHKDBN";

typedef struct
{
    char key[2];
    char *value;
}
HeaderTag;

typedef struct
{
    char type[2];
    AjPList tags;
}
HeaderLine;

const char *bam_flag2char_table = "pPuUrR12sfd\0\0\0\0\0";
void bam_destroy_header_hash(bam_header_t *header);
static void sam_header_line_free(HeaderLine *hline);
void bam_view1(const bam_header_t *header, const bam1_t *b);
static const char *nextline(char **lineptr, size_t *n, const char *text);
static HeaderLine *sam_header_line_parse(const char *headerLine);
static int sam_header_line_validate(HeaderLine *hline);
static HeaderTag *header_line_has_tag(HeaderLine *hline, const char *key);
static int tag_exists(const char *tag, const char **tags);
static HeaderTag *new_tag(const char *name, const char *value_from,
                          const char *value_to);

/**************************
 * CIGAR related routines *
 **************************/

ajuint bam_calend(const bam1_core_t *c, const ajuint *cigar)
{
	ajuint k, end;
	end = c->pos;
	for (k = 0; k < c->n_cigar; ++k) {
		int op = cigar[k] & BAM_CIGAR_MASK;
		if (op == BAM_CMATCH || op == BAM_CDEL || op == BAM_CREF_SKIP)
			end += cigar[k] >> BAM_CIGAR_SHIFT;
	}
	return end;
}

ajint bam_cigar2qlen(const bam1_core_t *c, const ajuint *cigar)
{
	ajuint k;
	ajint l = 0;
	for (k = 0; k < c->n_cigar; ++k) {
		int op = cigar[k] & BAM_CIGAR_MASK;
		if (op == BAM_CMATCH || op == BAM_CINS || op == BAM_CSOFT_CLIP)
			l += cigar[k] >> BAM_CIGAR_SHIFT;
	}
	return l;
}

/********************
 * BAM I/O routines *
 ********************/

bam_header_t *bam_header_init(void)
{
	bam_is_be = ajUtilGetBigendian();
	return (bam_header_t*)calloc(1, sizeof(bam_header_t));
}

void bam_header_destroy(bam_header_t *header)
{
	ajint i;
	if (header == 0) return;
	if (header->target_name) {
		for (i = 0; i < header->n_targets; ++i)
			free(header->target_name[i]);
		free(header->target_name);
		free(header->target_len);
	}
	free(header->text);
	if (header->dict) ajListstrFree(&header->dict);
	if (header->rg2lib) ajTableFree(&header->rg2lib);
	bam_destroy_header_hash(header);
	free(header);
}

bam_header_t *bam_header_read(bamFile fp)
{
	bam_header_t *header;
	char buf[4];
	ajint i = 1, name_len;
	// check EOF
	i = bgzf_check_EOF(fp);
	if (i < 0) {
		// If the file is a pipe, checking the EOF marker will *always* fail
		// with ESPIPE.  Suppress the error message in this case.
		if (errno != ESPIPE) perror("[bam_header_read] bgzf_check_EOF");
	}
	else if (i == 0) fprintf(stderr, "[bam_header_read] EOF marker is absent.\n");
	// read "BAM1"
	if (bam_read(fp, buf, 4) != 4) return 0;
	if (strncmp(buf, "BAM\001", 4)) {
		fprintf(stderr, "[bam_header_read] wrong header\n");
		return 0;
	}
	header = bam_header_init();
	// read plain text and the number of reference sequences
	bam_read(fp, &header->l_text, 4);
	if (bam_is_be) ajByteRevInt(&header->l_text);
	header->text = (char*)calloc(header->l_text + 1, 1);
	bam_read(fp, header->text, header->l_text);
	bam_read(fp, &header->n_targets, 4);
	if (bam_is_be) ajByteRevInt(&header->n_targets);
	// read reference sequence names and lengths
	header->target_name = (char**)calloc(header->n_targets, sizeof(char*));
	header->target_len = (ajuint*)calloc(header->n_targets, 4);
	for (i = 0; i != header->n_targets; ++i) {
		bam_read(fp, &name_len, 4);
		if (bam_is_be) ajByteRevInt(&name_len);
		header->target_name[i] = (char*)calloc(name_len, 1);
		bam_read(fp, header->target_name[i], name_len);
		bam_read(fp, &header->target_len[i], 4);
		if (bam_is_be) ajByteRevUint(&header->target_len[i]);
	}
	return header;
}

int bam_header_write(bamFile fp, const bam_header_t *header)
{
	char buf[4];
	ajint i, name_len, x;
	// write "BAM1"
	strncpy(buf, "BAM\001", 4);
	bam_write(fp, buf, 4);
	// write plain text and the number of reference sequences
	if (bam_is_be) {
		x = header->l_text;
                ajByteRevInt(&x);
		bam_write(fp, &x, 4);
		if (header->l_text) bam_write(fp, header->text, header->l_text);
		x = header->n_targets;
                ajByteRevInt(&x);
		bam_write(fp, &x, 4);
	} else {
		bam_write(fp, &header->l_text, 4);
		if (header->l_text) bam_write(fp, header->text, header->l_text);
		bam_write(fp, &header->n_targets, 4);
	}
	// write sequence names and lengths
	for (i = 0; i != header->n_targets; ++i) {
		char *p = header->target_name[i];
		name_len = strlen(p) + 1;
		if (bam_is_be) {
			x = name_len;
                        ajByteRevInt(&x);
			bam_write(fp, &x, 4);
		} else bam_write(fp, &name_len, 4);
		bam_write(fp, p, name_len);
		if (bam_is_be) {
			x = header->target_len[i];
                        ajByteRevInt(&x);
			bam_write(fp, &x, 4);
		} else bam_write(fp, &header->target_len[i], 4);
	}
	return 0;
}

static void swap_endian_data(const bam1_core_t *c, int data_len, uint8_t *data)
{
	uint8_t *s;
	ajuint i, *cigar = (ajuint*)(data + c->l_qname);
	s = data + c->n_cigar*4 + c->l_qname + c->l_qseq + (c->l_qseq + 1)/2;
	for (i = 0; i < c->n_cigar; ++i) ajByteRevUint(&cigar[i]);
	while (s < data + data_len) {
		uint8_t type;
		s += 2; // skip key
		type = toupper(*s); ++s; // skip type
		if (type == 'C' || type == 'A') ++s;
		else if (type == 'S') { ajByteRevShort((ajshort*)s); s += 2; }
		else if (type == 'I' || type == 'F') { ajByteRevUint((ajuint*)s); s += 4; }
		else if (type == 'D') { ajByteRevLong((ajlong*)s); s += 8; }
		else if (type == 'Z' || type == 'H') { while (*s) ++s; ++s; }
	}
}

int bam_read1(bamFile fp, bam1_t *b)
{
	bam1_core_t *c = &b->core;
	ajint block_len, ret, i;
	ajuint x[8];

	assert(BAM_CORE_SIZE == 32);
	if ((ret = bam_read(fp, &block_len, 4)) != 4) {
		if (ret == 0) return -1; // normal end-of-file
		else return -2; // truncated
	}
	if (bam_read(fp, x, BAM_CORE_SIZE) != BAM_CORE_SIZE) return -3;
	if (bam_is_be) {
		ajByteRevInt(&block_len);
		for (i = 0; i < 8; ++i) ajByteRevUint(x + i);
	}
	c->tid = x[0]; c->pos = x[1];
	c->bin = x[2]>>16; c->qual = x[2]>>8&0xff; c->l_qname = x[2]&0xff;
	c->flag = x[3]>>16; c->n_cigar = x[3]&0xffff;
	c->l_qseq = x[4];
	c->mtid = x[5]; c->mpos = x[6]; c->isize = x[7];
	b->data_len = block_len - BAM_CORE_SIZE;
	if (b->m_data < b->data_len) {
		b->m_data = b->data_len;
		kroundup32(b->m_data);
		b->data = (uint8_t*)realloc(b->data, b->m_data);
	}
	if (bam_read(fp, b->data, b->data_len) != b->data_len) return -4;
	b->l_aux = b->data_len - c->n_cigar * 4 - c->l_qname - c->l_qseq - (c->l_qseq+1)/2;
	if (bam_is_be) swap_endian_data(c, b->data_len, b->data);
	return 4 + block_len;
}

inline int bam_write1_core(bamFile fp, const bam1_core_t *c, int data_len, uint8_t *data)
{
    ajuint x[8];
    ajuint block_len = data_len + BAM_CORE_SIZE;
    ajuint y;
	int i;
	assert(BAM_CORE_SIZE == 32);
	x[0] = c->tid;
	x[1] = c->pos;
	x[2] = (ajuint)c->bin<<16 | c->qual<<8 | c->l_qname;
	x[3] = (ajuint)c->flag<<16 | c->n_cigar;
	x[4] = c->l_qseq;
	x[5] = c->mtid;
	x[6] = c->mpos;
	x[7] = c->isize;
	if (bam_is_be) {
		for (i = 0; i < 8; ++i) ajByteRevUint(x + i);
		y = block_len;
		ajByteRevUint(&y);
                bam_write(fp, &y, 4);
		swap_endian_data(c, data_len, data);
	} else bam_write(fp, &block_len, 4);
	bam_write(fp, x, BAM_CORE_SIZE);
	bam_write(fp, data, data_len);
	if (bam_is_be) swap_endian_data(c, data_len, data);
	return 4 + block_len;
}

int bam_write1(bamFile fp, const bam1_t *b)
{
	return bam_write1_core(fp, &b->core, b->data_len, b->data);
}

char *bam_format1_core(const bam_header_t *header, const bam1_t *b, int of)
{
	uint8_t *s = bam1_seq(b), *t = bam1_qual(b);
	int i;
	const bam1_core_t *c = &b->core;
	static AjPStr str = NULL;
        char *ret = NULL;

	ajFmtPrintS(&str, "%s\t", bam1_qname(b));
	if (of == BAM_OFDEC) ajFmtPrintAppS(&str, "%d\t", c->flag);
	else if (of == BAM_OFHEX) ajFmtPrintAppS(&str, "0x%x\t", c->flag);
	else { // BAM_OFSTR
		for (i = 0; i < 16; ++i)
			if ((c->flag & 1<<i) && bam_flag2char_table[i])
                            ajStrAppendK(&str, bam_flag2char_table[i]);
		ajStrAppendK(&str,'\t');
	}
	if (c->tid < 0) ajStrAppendC(&str, "*\t");
	else ajFmtPrintAppS(&str, "%s\t", header->target_name[c->tid]);
	ajFmtPrintAppS(&str, "%d\t%d\t", c->pos + 1, c->qual);
	if (c->n_cigar == 0) ajStrAppendK(&str, '*');
	else {
		for (i = 0; i < c->n_cigar; ++i)
			ajFmtPrintAppS(&str, "%d%c", bam1_cigar(b)[i]>>BAM_CIGAR_SHIFT, "MIDNSHP"[bam1_cigar(b)[i]&BAM_CIGAR_MASK]);
	}
	ajStrAppendK(&str, '\t');
            if (c->mtid < 0) ajStrAppendC(&str, "*\t");
            else if (c->mtid == c->tid) ajStrAppendC(&str, "=\t");
	else ajFmtPrintAppS(&str, "%s\t", header->target_name[c->mtid]);
	ajFmtPrintAppS(&str, "%d\t%d\t", c->mpos + 1, c->isize);
	if (c->l_qseq) {
            for (i = 0; i < c->l_qseq; ++i) ajStrAppendK(&str, bam_nt16_rev_table[bam1_seqi(s, i)]);
            ajStrAppendK(&str, '\t');
            if (t[0] == 0xff) ajStrAppendK(&str, '*');
            else for (i = 0; i < c->l_qseq; ++i) ajStrAppendK(&str, (t[i] + 33));
	} else ajStrAppendC(&str, "*\t*");
	s = bam1_aux(b);
	while (s < b->data + b->data_len) {
		uint8_t type, key[2];
		key[0] = s[0]; key[1] = s[1];
		s += 2; type = *s; ++s;
		ajFmtPrintAppS(&str, "\t%c%c:", key[0], key[1]);
		if (type == 'A') { ajFmtPrintAppS(&str, "A:%c", *s); ++s; }
		else if (type == 'C') { ajFmtPrintAppS(&str, "i:%u", *s); ++s; }
		else if (type == 'c') { ajFmtPrintAppS(&str, "i:%d", *s); ++s; }
		else if (type == 'S') { ajFmtPrintAppS(&str, "i:%u", *(uint16_t*)s); s += 2; }
		else if (type == 's') { ajFmtPrintAppS(&str, "i:%d", *(int16_t*)s); s += 2; }
		else if (type == 'I') { ajFmtPrintAppS(&str, "i:%u", *(ajuint*)s); s += 4; }
		else if (type == 'i') { ajFmtPrintAppS(&str, "i:%d", *(ajint*)s); s += 4; }
		else if (type == 'f') { ajFmtPrintAppS(&str, "f:%g", *(float*)s); s += 4; }
		else if (type == 'd') { ajFmtPrintAppS(&str, "d:%lg", *(double*)s); s += 8; }
		else if (type == 'Z' || type == 'H') { ajFmtPrintAppS(&str, "%c:", type); while (*s) ajStrAppendK(&str, *s++); ++s; }
	}

        ret = ajCharNewS(str);
        ajStrDel(&str);

	return ret;
}

char *bam_format1(const bam_header_t *header, const bam1_t *b)
{
	return bam_format1_core(header, b, BAM_OFDEC);
}

void bam_view1(const bam_header_t *header, const bam1_t *b)
{
	char *s = bam_format1(header, b);
	printf("%s\n", s);
	AJFREE(s);
}

// FIXME: we should also check the LB tag associated with each alignment
const char *bam_get_library(bam_header_t *h, const bam1_t *b)
{
	const uint8_t *rg;
	if (h->dict == 0) h->dict = sam_header_parse2(h->text);
	if (h->rg2lib == 0) h->rg2lib = sam_header2tbl(h->dict, "RG", "ID", "LB");
	rg = bam_aux_get(b, "RG");
	return (rg == 0)? 0 : ajTableFetch(h->rg2lib, (const char*)(rg + 1));
}



/* from bgzf.c in samtools */

typedef struct {
	int size;
	int padding;
	unsigned char *block;
	ajlong end_offset;
} cache_t;


#if defined(_WIN32) || defined(_MSC_VER)
#define ftello(fp) ftell(fp)
#define fseeko(fp, offset, whence) fseek(fp, offset, whence)
#endif

typedef char bgzf_byte_t;

static const int DEFAULT_BLOCK_SIZE = 64 * 1024;
static const int MAX_BLOCK_SIZE = 64 * 1024;

static const int BLOCK_HEADER_LENGTH = 18;
static const int BLOCK_FOOTER_LENGTH = 8;

static const int GZIP_ID1 = 31;
static const int GZIP_ID2 = 139;
static const int CM_DEFLATE = 8;
static const int FLG_FEXTRA = 4;
static const int OS_UNKNOWN = 255;
static const int BGZF_ID1 = 66; // 'B'
static const int BGZF_ID2 = 67; // 'C'
static const int BGZF_LEN = 2;
static const int BGZF_XLEN = 6; // BGZF_LEN+4

static const int GZIP_WINDOW_BITS = -15; // no zlib header
static const int Z_DEFAULT_MEM_LEVEL = 8;

inline void packInt16(unsigned char* buffer, ajushort value);
inline int unpackInt16(const unsigned char* buffer);
inline void packInt32(unsigned char* buffer, ajuint value);

inline void packInt16(unsigned char* buffer, ajushort value)
{
    buffer[0] = value;
    buffer[1] = value >> 8;
}
static inline
int
bgzf_min(int x, int y);

inline
int
unpackInt16(const unsigned char* buffer)
{
    return (buffer[0] | (buffer[1] << 8));
}

inline
void
packInt32(unsigned char* buffer, ajuint value)
{
    buffer[0] = value;
    buffer[1] = value >> 8;
    buffer[2] = value >> 16;
    buffer[3] = value >> 24;
}

static inline
int
bgzf_min(int x, int y)
{
    return (x < y) ? x : y;
}

static
void
report_error(BGZF* fp, const char* message) {
    fp->error = message;
}

static BGZF *bgzf_read_init(void)
{
	BGZF *fp;
	fp = calloc(1, sizeof(BGZF));
    fp->uncompressed_block_size = MAX_BLOCK_SIZE;
    fp->uncompressed_block = malloc(MAX_BLOCK_SIZE);
    fp->compressed_block_size = MAX_BLOCK_SIZE;
    fp->compressed_block = malloc(MAX_BLOCK_SIZE);
	fp->cache_size = 0;
	fp->cache = ajTableNewLen(512);
	return fp;
}

static
BGZF*
open_read(int fd)
{
    FILE* file = fdopen(fd, "r");
    BGZF* fp;
	if (file == 0) return 0;
	fp = bgzf_read_init();
    fp->file_descriptor = fd;
    fp->open_mode = 'r';
    fp->file = file;
    return fp;
}

static
BGZF*
open_write(int fd, bool is_uncompressed)
{
    FILE* file = fdopen(fd, "w");
    BGZF* fp;
	if (file == 0) return 0;
	fp = malloc(sizeof(BGZF));
    fp->file_descriptor = fd;
    fp->open_mode = 'w';
    fp->owned_file = 0; fp->is_uncompressed = is_uncompressed;
    fp->file = file;
    fp->uncompressed_block_size = DEFAULT_BLOCK_SIZE;
    fp->uncompressed_block = NULL;
    fp->compressed_block_size = MAX_BLOCK_SIZE;
    fp->compressed_block = malloc(MAX_BLOCK_SIZE);
    fp->block_address = 0;
    fp->block_offset = 0;
    fp->block_length = 0;
    fp->error = NULL;
    return fp;
}

BGZF*
bgzf_open(const char* __restrict path, const char* __restrict mode)
{
    BGZF* fp = NULL;
    if (mode[0] == 'r' || mode[0] == 'R') { /* The reading mode is preferred. */
		int fd, oflag = O_RDONLY;
#ifdef _WIN32
		oflag |= O_BINARY;
#endif
		fd = open(path, oflag);
		if (fd == -1) return 0;
        fp = open_read(fd);
    } else if (mode[0] == 'w' || mode[0] == 'W') {
		int fd, oflag = O_WRONLY | O_CREAT | O_TRUNC;
#ifdef _WIN32
		oflag |= O_BINARY;
#endif
		fd = open(path, oflag, 0666);
		if (fd == -1) return 0;
        fp = open_write(fd, strstr(mode, "u")? 1 : 0);
    }
    if (fp != NULL) {
        fp->owned_file = 1;
    }
    return fp;
}

BGZF*
bgzf_fdopen(int fd, const char * __restrict mode)
{
	if (fd == -1) return 0;
    if (mode[0] == 'r' || mode[0] == 'R') {
        return open_read(fd);
    } else if (mode[0] == 'w' || mode[0] == 'W') {
        return open_write(fd, strstr(mode, "u")? 1 : 0);
    } else {
        return NULL;
    }
}

static
int
deflate_block(BGZF* fp, int block_length)
{
    // Deflate the block in fp->uncompressed_block into fp->compressed_block.
    // Also adds an extra field that stores the compressed block length.

    bgzf_byte_t* buffer = fp->compressed_block;
    int buffer_size = fp->compressed_block_size;
    int input_length;
    int compressed_length;
    int compress_level;
    int status;
    z_stream zs;
    ajuint crc;
    int remaining;

    // Init gzip header
    buffer[0] = GZIP_ID1;
    buffer[1] = GZIP_ID2;
    buffer[2] = CM_DEFLATE;
    buffer[3] = FLG_FEXTRA;
    buffer[4] = 0; // mtime
    buffer[5] = 0;
    buffer[6] = 0;
    buffer[7] = 0;
    buffer[8] = 0;
    buffer[9] = OS_UNKNOWN;
    buffer[10] = BGZF_XLEN;
    buffer[11] = 0;
    buffer[12] = BGZF_ID1;
    buffer[13] = BGZF_ID2;
    buffer[14] = BGZF_LEN;
    buffer[15] = 0;
    buffer[16] = 0; // placeholder for block length
    buffer[17] = 0;

    // loop to retry for blocks that do not compress enough
    input_length = block_length;
    compressed_length = 0;
    while (1) {
        compress_level = fp->is_uncompressed? 0 : Z_DEFAULT_COMPRESSION;
        zs.zalloc = NULL;
        zs.zfree = NULL;
        zs.next_in = fp->uncompressed_block;
        zs.avail_in = input_length;
        zs.next_out = (void*)&buffer[BLOCK_HEADER_LENGTH];
        zs.avail_out = buffer_size - BLOCK_HEADER_LENGTH - BLOCK_FOOTER_LENGTH;

        status = deflateInit2(&zs, compress_level, Z_DEFLATED,
                                  GZIP_WINDOW_BITS, Z_DEFAULT_MEM_LEVEL, Z_DEFAULT_STRATEGY);
        if (status != Z_OK) {
            report_error(fp, "deflate init failed");
            return -1;
        }
        status = deflate(&zs, Z_FINISH);
        if (status != Z_STREAM_END) {
            deflateEnd(&zs);
            if (status == Z_OK) {
                // Not enough space in buffer.
                // Can happen in the rare case the input doesn't compress enough.
                // Reduce the amount of input until it fits.
                input_length -= 1024;
                if (input_length <= 0) {
                    // should never happen
                    report_error(fp, "input reduction failed");
                    return -1;
                }
                continue;
            }
            report_error(fp, "deflate failed");
            return -1;
        }
        status = deflateEnd(&zs);
        if (status != Z_OK) {
            report_error(fp, "deflate end failed");
            return -1;
        }
        compressed_length = zs.total_out;
        compressed_length += BLOCK_HEADER_LENGTH + BLOCK_FOOTER_LENGTH;
        if (compressed_length > MAX_BLOCK_SIZE) {
            // should never happen
            report_error(fp, "deflate overflow");
            return -1;
        }
        break;
    }

    packInt16((unsigned char*)&buffer[16], compressed_length-1);
    crc = crc32(0L, NULL, 0L);
    crc = crc32(crc, fp->uncompressed_block, input_length);
    packInt32((unsigned char*)&buffer[compressed_length-8], crc);
    packInt32((unsigned char*)&buffer[compressed_length-4], input_length);

    remaining = block_length - input_length;
    if (remaining > 0) {
        if (remaining > input_length) {
            // should never happen (check so we can use memcpy)
            report_error(fp, "remainder too large");
            return -1;
        }
        memcpy((unsigned char*)fp->uncompressed_block,
               (unsigned char*)fp->uncompressed_block + input_length,
               remaining);
    }
    fp->block_offset = remaining;
    return compressed_length;
}

static
int
inflate_block(BGZF* fp, int block_length)
{
    // Inflate the block in fp->compressed_block into fp->uncompressed_block
    int status;
    z_stream zs;
    zs.zalloc = NULL;
    zs.zfree = NULL;
    zs.next_in = (unsigned char*)fp->compressed_block + 18;
    zs.avail_in = block_length - 16;
    zs.next_out = fp->uncompressed_block;
    zs.avail_out = fp->uncompressed_block_size;

    status = inflateInit2(&zs, GZIP_WINDOW_BITS);
    if (status != Z_OK) {
        report_error(fp, "inflate init failed");
        return -1;
    }
    status = inflate(&zs, Z_FINISH);
    if (status != Z_STREAM_END) {
        inflateEnd(&zs);
        report_error(fp, "inflate failed");
        return -1;
    }
    status = inflateEnd(&zs);
    if (status != Z_OK) {
        report_error(fp, "inflate failed");
        return -1;
    }
    return zs.total_out;
}

static
int
check_header(const bgzf_byte_t* header)
{
    return (header[0] == GZIP_ID1 &&
            header[1] == (bgzf_byte_t) GZIP_ID2 &&
            header[2] == Z_DEFLATED &&
            (header[3] & FLG_FEXTRA) != 0 &&
            unpackInt16((const unsigned char*)&header[10]) == BGZF_XLEN &&
            header[12] == BGZF_ID1 &&
            header[13] == BGZF_ID2 &&
            unpackInt16((const unsigned char*)&header[14]) == BGZF_LEN);
}

static void free_cache(BGZF *fp)
{
    void **keyarray = NULL;
    void **valarray = NULL;
    ajint i;

    if (fp->open_mode != 'r') return;

    if(!fp->cache) return;

    ajTableToarrayKeysValues(fp->cache, &keyarray, &valarray);

    for(i = 0; keyarray[i]; i++)
    {
	AJFREE(keyarray[i]);		/* the key */
	AJFREE(valarray[i]);		/* the key */
    }

    AJFREE(keyarray);
    AJFREE(valarray);
    ajTableFree(&fp->cache);
}

static int load_block_from_cache(BGZF *fp, ajlong block_address)
{
	cache_t *p;
	p = ajTableFetch(fp->cache, &block_address);
	if (!p) return 0;
	if (fp->block_length != 0) fp->block_offset = 0;
	fp->block_address = block_address;
	fp->block_length = p->size;
	memcpy(fp->uncompressed_block, p->block, MAX_BLOCK_SIZE);
	fseeko(fp->file, p->end_offset, SEEK_SET);
	return p->size;
}

/* add a block to the cache
** remove an old block if it exceeds maximum size
*/

static void cache_block(BGZF *fp, int size)
{
	cache_t *p;
	cache_t *oldp;

	if (MAX_BLOCK_SIZE >= fp->cache_size) return;
	if ((ajint) ajTableGetLength(fp->cache) * MAX_BLOCK_SIZE > fp->cache_size) {
		/* A better way would be to remove the oldest block in the
		 * cache, but here we remove a random one for simplicity. This
		 * should not have a big impact on performance. */

/* some way to remove a value - or half the values - from the table */
            
        }
	AJNEW0(p);
	p->size = fp->block_length;
	p->end_offset = fp->block_address + size;
	p->block = malloc(MAX_BLOCK_SIZE);
	memcpy(p->block, fp->uncompressed_block, MAX_BLOCK_SIZE);
        oldp = ajTablePut(fp->cache, &fp->block_address, p);
        if(oldp)
        {
            AJFREE(oldp->block);
            AJFREE(oldp);
        }

        return;
}

static int read_block(BGZF* fp)
{
    bgzf_byte_t header[BLOCK_HEADER_LENGTH];
    int size = 0;
    ajlong block_address;
    int count;
    int block_length;
    bgzf_byte_t* compressed_block;
    int remaining;
    block_address = ftello(fp->file);
	if (load_block_from_cache(fp, block_address)) return 0;
    count = fread(header, 1, sizeof(header), fp->file);
    if (count == 0) {
        fp->block_length = 0;
        return 0;
    }
	size = count;
        if ((unsigned int) count != sizeof(header)) {
        report_error(fp, "read failed");
        return -1;
    }
    if (!check_header(header)) {
        report_error(fp, "invalid block header");
        return -1;
    }
    block_length = unpackInt16((unsigned char*)&header[16]) + 1;
    compressed_block = (bgzf_byte_t*) fp->compressed_block;
    memcpy(compressed_block, header, BLOCK_HEADER_LENGTH);
    remaining = block_length - BLOCK_HEADER_LENGTH;
    count = fread(&compressed_block[BLOCK_HEADER_LENGTH], 1, remaining, fp->file);
    if (count != remaining) {
        report_error(fp, "read failed");
        return -1;
    }
	size += count;
    count = inflate_block(fp, block_length);
    if (count < 0) {
        return -1;
    }
    if (fp->block_length != 0) {
        // Do not reset offset if this read follows a seek.
        fp->block_offset = 0;
    }
    fp->block_address = block_address;
    fp->block_length = count;
	cache_block(fp, size);
    return 0;
}

int bgzf_read(BGZF* fp, void* data, int length)
{
    int bytes_read;
    bgzf_byte_t* output;
    int available;
    int copy_length;
    bgzf_byte_t* buffer;

    if (length <= 0) {
        return 0;
    }
    if (fp->open_mode != 'r') {
        report_error(fp, "file not open for reading");
        return -1;
    }

    bytes_read = 0;
    output = data;
    while (bytes_read < length) {
        available = fp->block_length - fp->block_offset;
        if (available <= 0) {
            if (read_block(fp) != 0) {
                return -1;
            }
            available = fp->block_length - fp->block_offset;
            if (available <= 0) {
                break;
            }
        }
        copy_length = bgzf_min(length-bytes_read, available);
        buffer = fp->uncompressed_block;
        memcpy(output, buffer + fp->block_offset, copy_length);
        fp->block_offset += copy_length;
        output += copy_length;
        bytes_read += copy_length;
    }
    if (fp->block_offset == fp->block_length) {
        fp->block_address = ftello(fp->file);
        fp->block_offset = 0;
        fp->block_length = 0;
    }
    return bytes_read;
}

static
int
flush_block(BGZF* fp)
{
    int count;
    int block_length;
    while (fp->block_offset > 0) {
        block_length = deflate_block(fp, fp->block_offset);
        if (block_length < 0) {
            return -1;
        }
        count = fwrite(fp->compressed_block, 1, block_length, fp->file);
        if (count != block_length) {
            report_error(fp, "write failed");
            return -1;
        }
        fp->block_address += block_length;
    }
    return 0;
}

int
bgzf_write(BGZF* fp, const void* data, int length)
{
    const bgzf_byte_t* input;
    int block_length;
    int bytes_written;
    int copy_length;
    bgzf_byte_t* buffer;

    if (fp->open_mode != 'w') {
        report_error(fp, "file not open for writing");
        return -1;
    }

    if (fp->uncompressed_block == NULL) {
        fp->uncompressed_block = malloc(fp->uncompressed_block_size);
    }

    input = data;
    block_length = fp->uncompressed_block_size;
    bytes_written = 0;
    while (bytes_written < length) {
        copy_length = bgzf_min(block_length - fp->block_offset, length - bytes_written);
        buffer = fp->uncompressed_block;
        memcpy(buffer + fp->block_offset, input, copy_length);
        fp->block_offset += copy_length;
        input += copy_length;
        bytes_written += copy_length;
        if (fp->block_offset == block_length) {
            if (flush_block(fp) != 0) {
                break;
            }
        }
    }
    return bytes_written;
}

int
bgzf_close(BGZF* fp)
{
    if (fp->open_mode == 'w') {
        if (flush_block(fp) != 0) {
            return -1;
        }
		{ // add an empty block
			int count, block_length = deflate_block(fp, 0);
			count = fwrite(fp->compressed_block, 1, block_length, fp->file);
		}
        if (fflush(fp->file) != 0) {
            report_error(fp, "flush failed");
            return -1;
        }
    }
    if (fp->owned_file) {
        if (fclose(fp->file) != 0) {
            return -1;
        }
    }
    free(fp->uncompressed_block);
    free(fp->compressed_block);
	free_cache(fp);
    free(fp);
    return 0;
}

ajlong
bgzf_tell(BGZF* fp)
{
    return ((fp->block_address << 16) | (fp->block_offset & 0xFFFF));
}

void bgzf_set_cache_size(BGZF *fp, int cache_size)
{
	if (fp) fp->cache_size = cache_size;
}

int bgzf_check_EOF(BGZF *fp)
{
	static unsigned char magic[28] = "\037\213\010\4\0\0\0\0\0\377\6\0\102\103\2\0\033\0\3\0\0\0\0\0\0\0\0\0";
	unsigned char buf[28];
	off_t offset;
	offset = ftello(fp->file);
	if (fseeko(fp->file, -28, SEEK_END) != 0) return -1;
	fread(buf, 1, 28, fp->file);
	fseeko(fp->file, offset, SEEK_SET);
	return (memcmp(magic, buf, 28) == 0)? 1 : 0;
}

ajlong
bgzf_seek(BGZF* fp, ajlong pos, int where)
{
    int block_offset;
    ajlong block_address;

    if (fp->open_mode != 'r') {
        report_error(fp, "file not open for read");
        return -1;
    }
    if (where != SEEK_SET) {
        report_error(fp, "unimplemented seek option");
        return -1;
    }
    block_offset = pos & 0xFFFF;
    block_address = (pos >> 16) & 0xFFFFFFFFFFFFLL;
    if (fseeko(fp->file, block_address, SEEK_SET) != 0) {
        report_error(fp, "seek failed");
        return -1;
    }
    fp->block_length = 0;  // indicates current block is not loaded
    fp->block_address = block_address;
    fp->block_offset = block_offset;
    return 0;
}

void bam_destroy_header_hash(bam_header_t *header)
{
	if (header->hash)
            ajTableFree(&header->hash);
}

void sam_header_free(AjPList *header)
{
    HeaderLine *hline = NULL;

    while (ajListGetLength(*header))
    {
        ajListPop(*header, (void*)hline);
        sam_header_line_free(hline);
    }
    ajListFree(header);
}

static void sam_header_line_free(HeaderLine *hline)
{
    AjPList tags = hline->tags;
    HeaderTag *htag = NULL;

    while (ajListGetLength(tags))
    {
        ajListPop(tags, (void*)htag);
        AJFREE(htag->value);
        AJFREE(htag);
    }

    ajListFree(&tags);
    AJFREE(hline);
}

AjPList sam_header_parse2(const char *headerText)
{
    AjPList hlines = NULL;
    HeaderLine *hline;
    const char *text;
    char *buf=NULL;
    size_t nbuf = 0;

    if ( !headerText )
		return 0;

    text = headerText;
    while ( (text=nextline(&buf, &nbuf, text)) )
    {
        hline = sam_header_line_parse(buf);
        if ( hline && sam_header_line_validate(hline) )
            ajListPushAppend(hlines, hline);
        else
        {
            if (hline) sam_header_line_free(hline);
            sam_header_free(&hlines);
            if ( buf ) AJFREE(buf);
            return NULL;
        }
    }
    if ( buf ) free(buf);

    return hlines;
}

AjPTable sam_header2tbl(const AjPList dict, const char type[2], const char key_tag[2], const char value_tag[2])
{
    AjPTable tbl = NULL;
    HeaderLine *hline = NULL;
    AjIList k;
    HeaderTag *key, *value;

    tbl = ajTableNewLen(512);

    if (dict == 0) return tbl; // return an empty (not null) hash table

    k = ajListIterNewread(dict);
    while (!ajListIterDone(k))
    {
        hline = ajListIterGet(k);
        if ( hline->type[0]!=type[0] || hline->type[1]!=type[1] ) 
            continue;
        
        key   = header_line_has_tag(hline,key_tag);
        value = header_line_has_tag(hline,value_tag); 
        if ( !key || !value )
            continue;
        
        if(ajTablePut(tbl, key->value, value->value))
            ajWarn("[sam_header_lookup_table] The key %s not unique.\n",
                   key->value);

    }

    ajListIterDel(&k);

    return tbl;
}

/* array of char* strings matching type from the header dictionary */

char **sam_header2list(const AjPList dict, char type[2],
                       char key_tag[2], int *_n)
{
    int max, n;
    char **ret;
    AjIList iter = NULL;
    HeaderLine *hline;
    HeaderTag *key;

    ret = 0; *_n = max = n = 0;

    iter = ajListIterNewread(dict);

    while (!ajListIterDone(iter))
    {
        hline = ajListIterGet(iter);
        if ( hline->type[0]!=type[0] || hline->type[1]!=type[1] ) 
            continue;
        
        key   = header_line_has_tag(hline,key_tag);
        if ( !key )
            continue;

		if (n == max) {
			max = max? max<<1 : 4;
			ret = realloc(ret, max * sizeof(void*));
		}
		ret[n++] = key->value;

    }
    *_n = n;

    return ret;
}

// Mimics the behaviour of getline, except it returns pointer to the next chunk of the text
//  or NULL if everything has been read. The lineptr should be freed by the caller. The
//  newline character is stripped.
static const char *nextline(char **lineptr, size_t *n, const char *text)
{
    ajuint len;
    const char *to = text;

    if ( !*to ) return NULL;

    while ( *to && *to!='\n' && *to!='\r' ) to++;
    len = to - text + 1;

    if ( *to )
    {
        // Advance the pointer for the next call
        if ( *to=='\n' ) to++;
        else if ( *to=='\r' && *(to+1)=='\n' ) to+=2;
    }
    if ( !len )
        return to;

    if ( !*lineptr ) 
    {
        *lineptr = malloc(len);
        *n = len;
    }
    else if ( *n<len ) 
    {
        *lineptr = realloc(*lineptr, len);
        *n = len;
    }
    if ( !*lineptr )
        ajDie("[bam nextline] Insufficient memory!\n");

    memcpy(*lineptr,text,len);
    (*lineptr)[len-1] = 0;

    return to;
}

static HeaderLine *sam_header_line_parse(const char *headerLine)
{
    HeaderLine *hline;
    HeaderTag *tag;
    const char *from, *to;
    int itype;

    from = headerLine;

    if ( *from != '@' ) {
        ajWarn("[sam_header_line_parse] expected '@', got [%s]\n", headerLine);
        return 0;
    }
    to = ++from;

    while (*to && *to!='\t') to++;
    if ( to-from != 2 ) {
        ajWarn("[sam_header_line_parse] expected '@XY', got [%s]\n", headerLine);
        return 0;
    }
    
    hline = malloc(sizeof(HeaderLine));
    hline->type[0] = from[0];
    hline->type[1] = from[1];
    hline->tags = NULL;

    itype = tag_exists(hline->type, types);
    
    from = to;
    while (*to && *to=='\t') to++;
    if ( to-from != 1 ) {
        ajWarn("[sam_header_line_parse] multiple tabs on line [%s] (%d)\n",
               headerLine,(int)(to-from));
        return 0;
    }
    from = to;
    while (*from)
    {
        while (*to && *to!='\t') to++;

        if ( !required_tags[itype] && !optional_tags[itype] )
            tag = new_tag("  ",from,to-1);
        else
            tag = new_tag(from,from+3,to-1);

        if ( header_line_has_tag(hline,tag->key) ) 
            ajWarn("The tag '%c%c' present (at least) twice on line [%s]\n",
                   tag->key[0],tag->key[1], headerLine);
        ajListPushAppend(hline->tags, tag);

        from = to;
        while (*to && *to=='\t') to++;
        if ( *to && to-from != 1 ) {
            ajWarn("[sam_header_line_parse] multiple tabs on line [%s] (%d)\n",
                   headerLine,(int)(to-from));
            return 0;
        }

        from = to;
    }
    return hline;
}


// Must be of an existing type, all tags must be recognised and all required tags must be present
static int sam_header_line_validate(HeaderLine *hline)
{
    AjPList tags = NULL;
    HeaderTag *tag = NULL;
    int itype, itag;
    AjIList iter = NULL;
    
    // Is the type correct?
    itype = tag_exists(hline->type, types);
    if ( itype==-1 ) 
    {
        ajWarn("The type [%c%c] not recognised.\n",
               hline->type[0],hline->type[1]);
        return 0;
    }

    // Has all required tags?
    itag = 0;
    while ( required_tags[itype] && required_tags[itype][itag] )
    {
        if ( !header_line_has_tag(hline,required_tags[itype][itag]) )
        {
            ajWarn("The tag [%c%c] required for [%c%c] not present.\n",
                   required_tags[itype][itag][0],required_tags[itype][itag][1],
                   hline->type[0],hline->type[1]);
            return 0;
        }
        itag++;
    }

    // Are all tags recognised?
    tags = hline->tags;
    iter = ajListIterNewread(tags);
    while (!ajListIterDone(iter))
    {
        tag = ajListIterGet(iter);
        if ( !tag_exists(tag->key,required_tags[itype]) && !tag_exists(tag->key,optional_tags[itype]) )
        {
            ajWarn("Unknown tag [%c%c] for [%c%c].\n",
                   tag->key[0],tag->key[1], hline->type[0],hline->type[1]);
            return 0;
        }
    }

    return 1;
}


static HeaderTag *header_line_has_tag(HeaderLine *hline, const char *key)
{
    AjPList tags = hline->tags;
    HeaderTag *tag;
    AjIList iter = NULL;

    iter = ajListIterNewread(tags);
    while (!ajListIterDone(iter))
    {
        tag = ajListIterGet(iter);
        if ( tag->key[0]==key[0] && tag->key[1]==key[1] ) return tag;
    }
    return NULL;
}
// Look for a tag "XY" in a predefined const char *[] array.
static int tag_exists(const char *tag, const char **tags)
{
    int itag=0;
    if ( !tags ) return -1;
    while ( tags[itag] )
    {
        if ( tags[itag][0]==tag[0] && tags[itag][1]==tag[1] ) return itag; 
        itag++;
    }
    return -1;
}



// name points to "XY", value_from points to the first character of the value string and
//  value_to points to the last character of the value string.
static HeaderTag *new_tag(const char *name, const char *value_from, const char *value_to)
{
    HeaderTag *tag = malloc(sizeof(HeaderTag));
    int len = value_to-value_from+1;

    tag->key[0] = name[0];
    tag->key[1] = name[1];
    tag->value = malloc(len+1);
    memcpy(tag->value,value_from,len+1);
    tag->value[len] = 0;

    return tag;
}

#define __skip_tag(s) do { \
        int type = toupper(*(s));                                       \
        ++(s);                                                          \
        if (type == 'C' || type == 'A') ++(s);                          \
        else if (type == 'S') (s) += 2;                                 \
        else if (type == 'I' || type == 'F') (s) += 4;                  \
        else if (type == 'D') (s) += 8;                                 \
        else if (type == 'Z' || type == 'H') { while (*(s)) ++(s); ++(s); } \
    } while (0)

unsigned char *bam_aux_get(const bam1_t *b, const char tag[2])
{
	uint8_t *s;
	int y = tag[0]<<8 | tag[1];
	s = bam1_aux(b);
	while (s < b->data + b->data_len) {
		int x = (int)s[0]<<8 | s[1];
		s += 2;
		if (x == y) return s;
		__skip_tag(s);
	}
	return 0;
}
