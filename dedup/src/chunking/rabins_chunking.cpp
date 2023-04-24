/**
 * @file rabins_chunking.cpp
 * @author WASL
 * @brief Implementations for raibn chunking technique
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "rabins_chunking.hpp"

#include <errno.h>

#include <cassert>
#include <fstream>

Rabins_Chunking::Rabins_Chunking() { init(); }

Rabins_Chunking::Rabins_Chunking(const Config &config) {
    init();
    min_block_size = config.get_rabinc_min_block_size();
    avg_block_size = config.get_rabinc_avg_block_size();
    max_block_size = config.get_rabinc_max_block_size();

    window_size = config.get_rabinc_window_size();
}

void Rabins_Chunking::reset_stream() {
    error = 0;
    inbuf_data_size = 0;
    block_size = 0;
    block_streampos = 0;
    block_addr = inbuf;
    bzero((unsigned char *)inbuf, inbuf_size * sizeof(unsigned char));
}

void Rabins_Chunking::init() {
    min_block_size = DEFAULT_RABINC_MIN_BLOCK_SIZE;
    avg_block_size = DEFAULT_RABINC_AVG_BLOCK_SIZE;
    max_block_size = DEFAULT_RABINC_MAX_BLOCK_SIZE;

    inbuf_size = DEFAULT_RABINC_MAX_BLOCK_SIZE * 10;
    fingerprint_mask = (1 << (fls32(avg_block_size) - 1)) - 1;
    inbuf = new unsigned char [inbuf_size];
    r_hash = new Rabins_Hashing();
    technique_name = "Rabins Chunking";
}

size_t Rabins_Chunking::rp_stream_read(unsigned char *dst, size_t size) {
    stream.read((char*)dst, size);
    size_t count = stream.gcount();
    error = 0;
    if (count == 0) {
        error = EOF;
    }
    return count;
}

int Rabins_Chunking::rp_block_next() {
    block_streampos += block_size;
    block_addr += block_size;
    block_size = 0;

    /*
     * Skip early part of each block -- there appears to be no reason
     * to checksum the first min_block_size-N bytes, because the
     * effect of those early bytes gets flushed out pretty quickly.
     * Setting N to 256 seems to work; not sure if that's the "right"
     * number, but we'll use that for now.  This one optimization
     * alone provides a 30% speedup in benchmark.py though, with no
     * detected change in block boundary locations or fingerprints in
     * any of the existing tests.  - stevegt
     *
     * @moinakg found similar results, and also seems to think 256 is
     * right: https://moinakg.wordpress.com/tag/rolling-hash/
     *
     */
    size_t skip = min_block_size - 256;
    size_t data_remaining = inbuf_data_size - (block_addr - inbuf);
    if ((data_remaining > min_block_size + 1) && (min_block_size > 512)) {
        block_size += skip;
    }

    while (true) {
        if (block_addr + block_size == inbuf + inbuf_size) {
            /* end of input buffer: there's a partial block at the end
             * of the buffer; move it to the beginning of the buffer
             * so we can append more from input stream
             */
            memmove(inbuf, block_addr, block_size);
            block_addr = inbuf;
            inbuf_data_size = block_size;
        }

        if (block_addr + block_size == inbuf + inbuf_data_size) {
            /* no more valid data in input buffer */
            int count = 0;
            if (!error) {
                /* use rp_stream_read to refill buffer */
                int size = inbuf_size - inbuf_data_size;
                assert(size > 0);
                count = rp_stream_read(inbuf + inbuf_data_size, size);
                if (!count) {
                    assert(error);
                }
                inbuf_data_size += count;
            }
            if (error && (count == 0)) {
                /* we're either carrying an error from earlier, or the
                 * rp_stream_read above just threw one
                 */
                if (block_size == 0) {
                    /* we're done. caller shouldn't call us again */
                    return error;
                } else {
                    /* give final block to caller; caller should call
                     * us again to get e.g. eof error
                     */
                    return 0;
                }
            }
        }

        /* feed the next byte into rabinpoly algo */
        r_hash->slide8(block_addr[block_size]);
        block_size++;

        /*
         *
         * We compare the low-order fingerprint bits (LOFB) to
         * something other than zero in order to avoid generating
         * short blocks when scanning long strings of zeroes.
         * Mechiel Lukkien (mjl), while working on the Plan9 gsoc,
         * seemed to think that avg_block_size - 1 was a good value.
         *
         * http://gsoc.cat-v.org/people/mjl/blog/2007/08/06/1_Rabin_fingerprints/
         *
         * ...and since we're already using avg_block_size - 1 to set
         * the fingerprint mask itself, then simply comparing LOFB to
         * the mask itself will do the right thing.
         *
         */
        if ((block_size == max_block_size) ||
            ((block_size >= min_block_size) &&
             ((r_hash->fingerprint & fingerprint_mask) == fingerprint_mask))) {
            /* full block or fingerprint boundary */
            return 0;
        }
    }
}

std::vector<File_Chunk> Rabins_Chunking::chunk_file(std::string file_path) {
    stream.open(file_path, std::ios::binary);

    // prepare the stream for the new file
    reset_stream();
    // reset the hash function
    r_hash->init(window_size);

    std::vector<File_Chunk> file_chunks;
    while (true) {
        int rc = rp_block_next();
        if (rc == 0) {
            File_Chunk new_chunk(block_size);
            memccpy(new_chunk.get_data(), block_addr, 0, block_size);
            file_chunks.push_back(new_chunk);
        }
        if (rc) {
            assert(rc == EOF);
            break;
        }
    }
    stream.close();
    for (File_Chunk ch : file_chunks) {
        ch.print();
    }
    return file_chunks;
}

void Rabins_Chunking::chunk_stream(std::vector<File_Chunk> &result,
                                   std::istream &stream) {
    return;

    // FILE *stream = fopen(file_path.c_str(), "rb");
    // if (!stream) {
    //     error = errno;
    // }
    // // prepare the stream for the new file
    // reset_stream();
    // // reset the hash function
    // r_hash->init(window_size);

    // while (true) {
    //     int rc = rp_block_next();
    //     if (rc == 0) {
    //         File_Chunk new_chunk(block_size);
    //         memccpy(new_chunk.get_data(), block_addr, 0, block_size);
    //         file_chunks.push_back(new_chunk);
    //     }
    //     if (rc) {
    //         assert(rc == EOF);
    //         break;
    //     }
    // }

    // return file_chunks;
}
