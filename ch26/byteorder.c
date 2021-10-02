/*
 * Chapter 26
 * Encryption
 * by Mike Wright
 *
 * Functions for converting between native and big-endian
 * byte orders.   Before calling an encryption function,
 * your blocks should be converted to native byte order.
 * If the data originated as a byte stream, you should
 * assume the stream is a stream of big-endian blocks
 * and requires conversion to the native byte order
 * before encryption/decryption.   Likewise, for output
 * plaintext or ciphertext, if it is being represented as
 * a byte stream (a file perhaps), you should convert the
 * blocks from the native byte order to big-endian.
 *
 * The only reason you need to go through all this
 * trouble is to make your output compatible with
 * implementations on other platforms.   It does not
 * affect security at all.   If you don't care about
 * compatibility (in other words, if you are stupid),
 * you don't need to perform this step at all.
 * Remember that you will most likely have to use the
 * proper byte order to verify your implementation
 * against test vectors anyway, so you may as well use
 * it.   If you know your code will always only be
 * compiled on big-endian machines [sarcasm & laughing
 * is appropriate], you don't need this step.
 *
 * If you are using your own encryption algorithm
 * and want to impose your own byte order on the
 * rest of the world, you will surely earn the distain
 * of all who would otherwise use your algorithm.
 * Big-endian is the accepted "normal" byte order
 * and is assumed for anything not hardware-specific.
 */

#include <stddef.h>

/*
 * Change these types to match your platform
 */
typedef unsigned char byte_t;/* 8-bit integer */
typedef unsigned short uint16_t;/* 16-bit integer */
typedef unsigned long uint32_t;/* 32-bit integer */
typedef unsigned long long uint64_t;/* 64-bit integer */

/*
 * Translate the message blocks into the native byte order
 * from big-endian byte order
 */
void native_byte_order16(
    uint16_t* msg,
    size_t num_blocks)
{
    byte_t* b_ptr;

    while(num_blocks--)
    {
        b_ptr = (byte_t*)(msg + num_blocks);
        msg[num_blocks] =
            (uint16_t)b_ptr[0] << 8 |
            (uint16_t)b_ptr[1];
    }
}

void native_byte_order32(
    uint32_t* msg,
    size_t num_blocks)
{
    byte_t* b_ptr;

    while(num_blocks--)
    {
        b_ptr = (byte_t*)(msg + num_blocks);
        msg[num_blocks] =
            (uint32_t)b_ptr[0] << 24 |
            (uint32_t)b_ptr[1] << 16 |
            (uint32_t)b_ptr[2] << 8 |
            (uint32_t)b_ptr[3];
    }
}

void native_byte_order64(
    uint64_t* msg,
    size_t num_blocks)
{
    byte_t* b_ptr;

    while(num_blocks--)
    {
        b_ptr = (byte_t*)(msg + num_blocks);
        msg[num_blocks] =
            (uint64_t)b_ptr[0] << 56 |
            (uint64_t)b_ptr[1] << 48 |
            (uint64_t)b_ptr[2] << 40 |
            (uint64_t)b_ptr[3] << 32 |
            (uint64_t)b_ptr[4] << 24 |
            (uint64_t)b_ptr[5] << 16 |
            (uint64_t)b_ptr[6] << 8 |
            (uint64_t)b_ptr[7];
    }
}

/*
 * Translates the message blocks from the native byte order
 * to big-endian byte order
 */
void big_endian16(
    uint16_t* msg,
    size_t num_blocks)
{
    byte_t* b_ptr;
    uint16_t temp;

    while(num_blocks--)
    {
        b_ptr = (byte_t*)(msg + num_blocks);
        temp = msg[num_blocks];
        b_ptr[0] = temp >> 8;
        b_ptr[1] = temp;
    }
}

void big_endian32(
    uint32_t* msg,
    size_t num_blocks)
{
    byte_t* b_ptr;
    uint32_t temp;

    while(num_blocks--)
    {
        b_ptr = (byte_t*)(msg + num_blocks);
        temp = msg[num_blocks];
        b_ptr[0] = temp >> 24;
        b_ptr[1] = temp >> 16;
        b_ptr[2] = temp >> 8;
        b_ptr[3] = temp;
    }
}

void big_endian64(
    uint64_t* msg,
    size_t num_blocks)
{
    byte_t* b_ptr;
    uint64_t temp;

    while(num_blocks--)
    {
        b_ptr = (byte_t*)(msg + num_blocks);
        temp = msg[num_blocks];
        b_ptr[0] = temp >> 56;
        b_ptr[1] = temp >> 48;
        b_ptr[2] = temp >> 40;
        b_ptr[3] = temp >> 32;
        b_ptr[4] = temp >> 24;
        b_ptr[5] = temp >> 16;
        b_ptr[6] = temp >> 8;
        b_ptr[7] = temp;
    }
}
