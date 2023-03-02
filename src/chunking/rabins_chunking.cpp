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
#include <fstream>
#include <errno.h>

bool Rabins_Chunking::set_min_block_size(uint64_t _min_block_size)
{
    /**
     * @brief Sets min chunk size.
     * @param _min_chunk_size: New min chunk size
     * @return: True if success, False otherwise
     */
    Rabins_Chunking::min_block_size = _min_block_size;

    if (min_block_size == _min_block_size)
        return true;
    else
        return false;
}
uint64_t Rabins_Chunking::get_min_block_size()
{
    /**
     * @brief Returns min block size
     * @return: uint64_t with window size value
     */
    return Rabins_Chunking::min_block_size;
}

bool Rabins_Chunking::set_avg_block_size(uint64_t _avg_block_size)
{
    /**
     * @brief Sets min chunk size.
     * @param _avg_chunk_size: New min chunk size
     * @return: True if success, False otherwise
     */
    Rabins_Chunking::min_block_size = _avg_block_size;

    if (avg_block_size == _avg_block_size)
        return true;
    else
        return false;
}

uint64_t Rabins_Chunking::get_avg_block_size()
{
    /**
     * @brief Returns avg block size
     * @return: uint64_t with avg block size value
     */
    return Rabins_Chunking::avg_block_size;
}

bool Rabins_Chunking::set_max_block_size(uint64_t _max_block_size)
{
    /**
     * @brief Sets min chunk size.
     * @param _max_chunk_size: New min chunk size
     * @return: True if success, False otherwise
     */
    Rabins_Chunking::max_block_size = _max_block_size;

    if (max_block_size == _max_block_size)
        return true;
    else
        return false;
}

uint64_t Rabins_Chunking::get_max_block_size()
{
    /**
     * @brief Returns max block size
     * @return: uint64_t with max block size value
     */
    return Rabins_Chunking::max_block_size;
}

bool Rabins_Chunking::set_buffer_size(uint64_t _buffer_size)
{
    /**
     * @brief Sets internal buffer size.
     * @param _buffer_size: New buffer size
     * @return: True if success, False otherwise
     */
    Rabins_Chunking::inbuf_size = _buffer_size;

    if (inbuf_size == _buffer_size)
        return true;
    else
        return false;
}

uint64_t Rabins_Chunking::get_buffer_size()
{
    /**
     * @brief Returns buffer size
     * @return: uint64_t with buffer size value
     */
    return Rabins_Chunking::inbuf_size;
}

bool Rabins_Chunking::init_stream()
{
    inbuf_data_size = 0;
    block_size = 0;
    block_streampos = 0;
    block_addr = inbuf;
}
size_t Rabins_Chunking::rp_stream_read(unsigned char *dst, size_t size)
{
    size_t count = fread(dst, 1, size, stream);
    error = 0;
    if (count == 0)
    {
        if (ferror(stream))
        {
            error = errno;
        }
        else if (feof(stream))
        {
            error = EOF;
        }
    }
    return count;
}

#define CUR_ADDR block_addr + block_size
#define INBUF_END inbuf + inbuf_size
int Rabins_Chunking::rp_block_next()
{

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
    if ((data_remaining > min_block_size + 1) &&
        (min_block_size > 512))
    {
        block_size += skip;
    }

    while (true)
    {

        if (CUR_ADDR == INBUF_END)
        {
            /* end of input buffer: there's a partial block at the end
             * of the buffer; move it to the beginning of the buffer
             * so we can append more from input stream
             */
            memmove(inbuf, block_addr, block_size);
            block_addr = inbuf;
            inbuf_data_size = block_size;
        }

        if (CUR_ADDR == inbuf + inbuf_data_size)
        {
            /* no more valid data in input buffer */
            int count = 0;
            if (!error)
            {
                /* use rp_stream_read to refill buffer */
                int size = inbuf_size - inbuf_data_size;
                assert(size > 0);
                count = rp_stream_read(inbuf + inbuf_data_size, size);
                if (!count)
                {
                    assert(error);
                }
                inbuf_data_size += count;
            }
            if (error && (count == 0))
            {
                /* we're either carrying an error from earlier, or the
                 * rp_stream_read above just threw one
                 */
                if (block_size == 0)
                {
                    /* we're done. caller shouldn't call us again */
                    return error;
                }
                else
                {
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
             ((r_hash->fingerprint & fingerprint_mask) == fingerprint_mask)))
        {
            /* full block or fingerprint boundary */
            return 0;
        }
    }
}

std::vector<File_Chunk> Rabins_Chunking::chunk_file(std::string file_path)
{

    FILE *stream = fopen(file_path.c_str(), "rb");
    if (!stream)
    {
        error = errno;
    }
    init_stream();
    r_hash->init();
    std::vector<File_Chunk> file_chunks;

    while (true)
    {
        int rc = rp_block_next();
        printf("%d", r_hash->fingerprint);
        char *chunk_data = new char[block_size];
        memccpy(chunk_data, block_addr, 0, block_size);
        if (rc == 0)
        {
            File_Chunk new_chunk(chunk_data, block_size);
        }
        if (rc)
        {
            assert(rc == EOF);
            break;
        }
    }

    return file_chunks;
}
