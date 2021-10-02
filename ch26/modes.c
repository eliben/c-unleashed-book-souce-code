/*
 * Chapter 26
 * Encryption
 * by Mike Wright
 *
 * The encryption operation modes
 *
 * The weak Electronic Code Book (ECB) mode should be
 * used only for debugging, verification, etc.   Any
 * production program should use one of the other modes.
 * Any of the other modes provide good security, and
 * your choice should be dictated by the particular
 * opportunities for optimization your program may be
 * able to take advantage of.   If they all seem equally
 * optimal, I suggest the Cipher Feedback (CFB) mode
 * with equal BLOCKSIZE and ELEMENTSIZE as a general-
 * purpose default choice (the CFB_encryption() and
 * CFB_decryption() functions).
 *
 * The encrypt() and decrypt() functions must be
 * defined elsewhere.   They should implement some
 * secure encryption algorithm.   Only the ECB and
 * CBC modes require both functions.   If you use
 * only the CFB or OFB modes, you needn't implement
 * the decrypt() function.
 *
 * Pass in plaintext, len, key, and origin as input to
 * encryption functions, and ciphertext as output.
 * Decryption is similar, but you pass ciphertext in
 * and get plaintext out.   The same len, key, and
 * origin values must be passed into the decryption
 * function as were used with the encryption function.
 *
 * Plaintext and ciphertext should already be converted
 * into blocks in the native byte order.   On input to
 * these functions, you should convert arbitrary byte
 * streams as if they were big-endian (ABCD), and
 * produce blocks in the native byte order.   Likewise,
 * after receiving data from these functions, you should
 * convert from the native byte order to big-endian if
 * you are changing the blocks into byte streams.   No
 * conversion is necessary if your data will always be
 * used in block form (uncommon; usually the data is
 * an arbitrary sequence of bytes).
 *
 * If you plan on compressing your data (highly
 * recommeded), do it before calling one of these
 * encryption functions, and decompress it after
 * calling the decryption function.   Compression
 * increases security and increases efficiency.   Such
 * a win-win is rare; use it!
 *
 * 'len' is the length of the plaintext and ciphertext
 * (both the same length) in blocks.   That is, len
 * is equal to the number of blocks of plaintext and
 * also equal to the number of blocks of ciphertext.
 *
 * 'key' is a secret value, of a type and size dictated
 * by your implementation.   Regardless of the cipher
 * chosen, the key should be handled very carefully and
 * cleared (set to a constant like 0) when it is no
 * longer needed.   The key should be generated using
 * truly random data (not rand()!!!).
 *
 * 'origin' is the initialization vector (IV), which
 * acts as salt on the ciphertext.   It must be unique
 * every time you call an encryption function, even if
 * it is the same data being encrypted.   This value
 * can be kept secret, but it is just fine to keep it
 * out in the clear; it really won't help anyone crack
 * your message.   The same origin must be used to
 * decrypt the message, so you will probably want to
 * store your origin block with the ciphertext.   The
 * origin does not need to be a random value, only a
 * unique value.   Constants are very bad, but a counting
 * index is fine if you can guarantee that it will always
 * be unique for each message.   Timestamps, random
 * numbers, pseudorandom numbers, etc. are all OK so long
 * as the uniqueness requirement is met.   Remember, the
 * same origin should never be used, not even if it is
 * the same key and the same data.
 *
 * Although the origin is a required salt element,
 * feel free to add additional salt, perhaps by XORing
 * it into the message or the key.   More salt is less
 * efficient because it must be stored to enable later
 * decryption, but it does add more security.   But
 * more salt adds less security than a larger or more
 * frequently changed key.
 */

#include <stddef.h>

/* Block size in bits, dictated by implementation */
#define BLOCKSIZE   32

/*
 * Element size in bits, only for CFB mode if different than BLOCKSIZE
 * Can be any value from 1 to BLOCKSIZE
 * Must equal the number of bits in element_t, defined below
 *
 * Only used by CFBdif_...() functions
 */
#define ELEMENTSIZE BLOCKSIZE

/*
 * Cryptographic primitive types
 * Change these to suit your platform and implementation
 */
typedef unsigned long block_t;
typedef block_t element_t;/* Only for CFBdif_...() functions */
typedef block_t key_t;


/*
 * Prototypes for the functions implementing the
 * particular cipher chosen for your program.
 * They are not defined here; you have to do that
 * yourself.
 *
 * These functions operate on only a single block
 *
 * NOTE: No need for decrypt() if you only use
 * CFB or OFB modes
 */

/* Returns one block of ciphertext */
block_t encrypt(
  block_t plaintext,
  key_t key
);

/* Returns one block of plaintext */
block_t decrypt(
  block_t ciphertext,
  key_t key
);


/* The Electronic Code Book (ECB) Mode */
void ECB_encryption(
  block_t const plaintext[],
  block_t ciphertext[],
  size_t len,
  key_t key)
{
  int i;
  for(i = 0; i < len; i++)
  {
    ciphertext[i] = encrypt(plaintext[i], key);
  }
}

void ECB_decryption(
  block_t const ciphertext[],
  block_t plaintext[],
  size_t len,
  key_t key)
{
  int i;
  for(i = 0; i < len; i++)
  {
    plaintext[i] = decrypt(ciphertext[i], key);
  }
}


/* The Cipher Block Chaining (CBC) Mode */
void CBC_encryption(
  block_t const plaintext[],
  block_t ciphertext[],
  size_t len,
  key_t key,
  block_t origin)
{
  int i;
  ciphertext[0] = encrypt(plaintext[0] ^ origin, key);
  for(i = 1; i < len; i++)
  {
    ciphertext[i] =
      encrypt(plaintext[i] ^ ciphertext[i - 1], key);
  }
}

void CBC_decryption(
  block_t const ciphertext[],
  block_t plaintext[],
  size_t len,
  key_t key,
  block_t origin)
{
  int i;
  plaintext[0] = decrypt(ciphertext[0], key) ^ origin;
  for(i = 1; i < len; i++)
  {
    plaintext[i] =
      decrypt(ciphertext[i], key) ^ ciphertext[i - 1];
  }
}


/* Output Feedback (OFB) Mode with ELEMENTSIZE == BLOCKSIZE */
void OFB_encryption(
  block_t const plaintext[],
  block_t ciphertext[],
  size_t len,
  key_t key,
  block_t origin)
{
  int i;
  block_t temp;

  temp = origin;

  for(i = 0; i < len; i++)
  {
    temp = encrypt(temp, key);
    ciphertext[i] = plaintext[i] ^ temp;
  }
}

void OFB_decryption(
  block_t const ciphertext[],
  block_t plaintext[],
  size_t len,
  key_t key,
  block_t origin)
{
  int i;
  block_t temp;

  temp = origin;

  for(i = 0; i < len; i++)
  {
    temp = encrypt(temp, key);
    plaintext[i] = ciphertext[i] ^ temp;
  }
}


/* Cipher Feedback (CFB) Mode when ELEMENTSIZE == BLOCKSIZE */
void CFB_encryption(
  block_t const plaintext[],
  block_t ciphertext[],
  size_t len,
  key_t key,
  block_t origin)
{
  int i;

  ciphertext[0] = plaintext[0] ^ encrypt(origin, key);

  for(i = 1; i < len; i++)
  {
    ciphertext[i] =
      plaintext[i] ^ encrypt(ciphertext[i - 1], key);
  }
}

void CFB_decryption(
  block_t const ciphertext[],
  block_t plaintext[],
  size_t len,
  key_t key,
  block_t origin)
{
  int i;

  plaintext[0] = ciphertext[0] ^ encrypt(origin, key);

  for(i = 1; i < len; i++)
  {
    plaintext[i] =
      ciphertext[i] ^ encrypt(ciphertext[i - 1], key);
  }
}


/* Cipher Feedback (CFB) Mode when BLOCKSIZE != ELEMENTSIZE */
void CFBdif_encryption(
  element_t const plaintext[],
  element_t ciphertext[],
  size_t len,
  key_t key,
  block_t origin)
{
  int i;
  block_t temp;

  temp = origin;
  ciphertext[0] =
    plaintext[0] ^
    encrypt(temp, key) >> BLOCKSIZE - ELEMENTSIZE &
      (1 << ELEMENTSIZE) - 1;

  for(i = 1; i < len; i++)
  {
    temp = temp << ELEMENTSIZE | ciphertext[i - 1];
    ciphertext[i] =
      plaintext[i] ^
      encrypt(temp, key) >> BLOCKSIZE - ELEMENTSIZE &
        (1 << ELEMENTSIZE) - 1;
  }
}

void CFBdif_decryption(
  element_t const ciphertext[],
  element_t plaintext[],
  size_t len,
  key_t key,
  block_t origin)
{
  int i;
  block_t temp;

  temp = origin;
  plaintext[0] =
    ciphertext[0] ^
    encrypt(temp, key) >> BLOCKSIZE - ELEMENTSIZE &
      (1 << ELEMENTSIZE) - 1;

  for(i = 1; i < len; i++)
  {
    temp = temp << ELEMENTSIZE | ciphertext[i - 1];
    plaintext[i] =
      ciphertext[i] ^
      encrypt(temp, key) >> BLOCKSIZE - ELEMENTSIZE &
        (1 << ELEMENTSIZE) - 1;
  }
}
