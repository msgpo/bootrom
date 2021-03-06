/**
 * Copyright (c) 2015 Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __COMMON_INCLUDE_TFTF_H
#define __COMMON_INCLUDE_TFTF_H

#include <stdint.h>
#include <stdbool.h>
#include "chipcfg.h"

#define TFTF_HEADER_SIZE_MIN            512
#define TFTF_HEADER_SIZE_MAX            32768
#define TFTF_HEADER_SIZE_DEFAULT        TFTF_HEADER_SIZE_MIN


#if MAX_TFTF_HEADER_SIZE_SUPPORTED < TFTF_HEADER_SIZE_MIN || \
    MAX_TFTF_HEADER_SIZE_SUPPORTED > TFTF_HEADER_SIZE_MAX
#error "Invalid MAX_TFTF_HEADER_SIZE_SUPPORTED"
#endif

#define TFTF_NUM_RESERVED                 4

/**
 * @brief TFTF Sentinel value "TFTF"
 *
 * Note: string must be in reverse order so that it looks OK on a little-
 * endian dump.
 */
#define TFTF_SENTINEL_SIZE                4
static const char tftf_sentinel[] = "TFTF";
/* Compile-time test to verify consistency of _SIZE and _VALUE */
typedef char ___tftf_sentinel_test[(TFTF_SENTINEL_SIZE ==
                                    sizeof(tftf_sentinel) - 1) ?
                                   1 : 0];

#define TFTF_TIMESTAMP_SIZE               16
#define TFTF_FW_PKG_NAME_SIZE             48


/* Section types */
#define TFTF_SECTION_END                  0xFE
#define TFTF_SECTION_RAW_CODE             1
#define TFTF_SECTION_RAW_DATA             2
    #define DATA_ADDRESS_TO_BE_IGNORED    0xFFFFFFFF
#define TFTF_SECTION_COMPRESSED_CODE      3
#define TFTF_SECTION_COMPRESSED_DATA      4
#define TFTF_SECTION_MANIFEST             5
#define TFTF_SECTION_SIGNATURE            0x80
#define TFTF_SECTION_CERTIFICATE          0x81

typedef struct {
    unsigned int section_type : 8;   /* One of the TFTF_SECTION_xxx above */
    unsigned int section_class : 24;
    uint32_t section_id;
    uint32_t section_length;
    uint32_t section_load_address;
    uint32_t section_expanded_length;
} __attribute__ ((packed)) tftf_section_descriptor;

#define TFTF_SECTION_SIZE sizeof(tftf_section_descriptor)
/* Compile-time test hack to verify that the element descriptor is 20 bytes */
typedef char ___tftf_section_test[(TFTF_SECTION_SIZE == 20) ? 1 : -1];

static inline bool is_section_hashed(tftf_section_descriptor *section) {
    return !(section->section_type & (1 << 7));
}

typedef union {
    struct __attribute__ ((packed)) {
        char sentinel_value[TFTF_SENTINEL_SIZE];
        uint32_t header_size;
        char build_timestamp[TFTF_TIMESTAMP_SIZE];
        char firmware_package_name[TFTF_FW_PKG_NAME_SIZE];
        uint32_t package_type;  /* Must match FFFF element_type */
        uint32_t start_location;
        uint32_t unipro_mid;    /* Match to MIPI DME 0x5003, or zero */
        uint32_t unipro_pid;    /* Match to MIPI DME 0x5004, or zero */
        uint32_t ara_vid;
        uint32_t ara_pid;
        uint32_t reserved[TFTF_NUM_RESERVED];
        tftf_section_descriptor sections[];
    };
    unsigned char buffer[MAX_TFTF_HEADER_SIZE_SUPPORTED];
} tftf_header;

/**
 * @brief Macro to calculate the address of the start of the TFTF payload.
 */
#define SECTION_PAYLOAD_START(tftf_hdr) \
    (((uint8_t *)(tftf_hdr)) + tftf_hdr->header_size)


/**
 * @brief Macro to calculate the last address in a section.
 */
#define SECTION_END_ADDRESS(section_ptr) \
    ((section_ptr)->section_load_address + \
     (section_ptr)->section_expanded_length - 1)


/**
 * @brief Macro to calculate the number of sections in a TFTF header
 */
#define CALC_MAX_TFTF_SECTIONS(header_size) \
		(((header_size) - offsetof(tftf_header, sections)) / \
		 sizeof(tftf_section_descriptor))


static inline bool is_section_out_of_range(tftf_header *header,
                                           tftf_section_descriptor *section) {
    return ((unsigned char *)section >= header->buffer +
                                        header->header_size -
                                        sizeof(tftf_section_descriptor));
}

/**
 * Compile-time test hack to verify that the header is
 * MAX_TFTF_HEADER_SIZE_SUPPORTED bytes
 */
typedef char ___tftf_header_test[(sizeof(tftf_header) ==
                                 MAX_TFTF_HEADER_SIZE_SUPPORTED) ?
                                 1 : -1];

#define TFTF_SIGNATURE_KEY_NAME_SIZE    96
#define TFTF_SIGNATURE_SIZE             256

typedef struct {
    uint32_t length;            /* total size of tftf_signature structure */
    uint32_t type;              /* Some ALGORITHM_TYPE_xxx from crypto.h */
    char key_name[TFTF_SIGNATURE_KEY_NAME_SIZE];
    unsigned char signature[TFTF_SIGNATURE_SIZE];
} __attribute__ ((packed)) tftf_signature;

#endif /* __COMMON_INCLUDE_TFTF_H */
