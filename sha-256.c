#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define ROTR(x, y) ((((uint32_t)(x)) >> ((uint32_t)(y))) | (((uint32_t)(x)) << (32 - ((uint32_t)(y)))))
#define SHR(x, y) ((((uint32_t)(x)) >> ((uint32_t)(y))))

#define CH(x, y, z) ((((uint32_t)(x)) & ((uint32_t)(y))) ^ (~((uint32_t)(x)) & ((uint32_t)(z))))
#define MAJ(x, y, z) ((((uint32_t)(x)) & ((uint32_t)(y))) ^ (((uint32_t)(x)) & ((uint32_t)(z))) ^ (((uint32_t)(y)) & ((uint32_t)(z))))
#define SIG0(x) (ROTR(((uint32_t)(x)), 2) ^ ROTR(((uint32_t)(x)), 13) ^ ROTR(((uint32_t)(x)), 22))
#define SIG1(x) (ROTR(((uint32_t)(x)), 6) ^ ROTR(((uint32_t)(x)), 11) ^ ROTR(((uint32_t)(x)), 25))
#define GAMA0(x) (ROTR(((uint32_t)(x)), 7) ^ ROTR(((uint32_t)(x)), 18) ^ SHR(((uint32_t)(x)), 3))
#define GAMA1(x) (ROTR(((uint32_t)(x)), 17) ^ ROTR(((uint32_t)(x)), 19) ^ SHR(((uint32_t)(x)), 10))

typedef struct s_sha_256
{
	uint32_t state[8];
}	t_sha_256;

typedef struct s_block
{
	uint32_t	arr[64];
}	t_block;

enum char_to_num{A, B, C, D, E, F, G, H};

const uint32_t keys[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

char	*malloc_padding(char *str, size_t *len_addr, int64_t len)
{
	int		i;
	char	*new_str;
	int		new_len;

	i = 1;
	while (((len * 8) + i) % 512 != 448)
		i++;
	i += 64;
	new_len = len + (i / 8);
	new_str = calloc(new_len, sizeof(char));
	memcpy(new_str, str, len);
	*len_addr = new_len;
	return (new_str);
}

char	*insert_padding(char *str, size_t *len_addr)
{
	int64_t	len;
	size_t	i = 1;
	char	*new_str;
	int		isLittleEndian = (*(char *)&i == 1);

	len = strlen(str);
	new_str = malloc_padding(str, len_addr, len);
	new_str[len] = 0b10000000;
	i = len * 8 + 8;
	while(i % 512 != 448)
		i++;
	for (int k = *len_addr - 1; k >= *len_addr - 8; k++)
	{
		if (isLittleEndian)
			new_str[k] = *(((char *) &len) + 7 - (k % 8));
		else
			new_str[k] = *(((char *) &len) + (k % 8));
	}
	return (new_str);
}

t_block	*get_blocks(char *str, size_t str_size, size_t *size_addr)
{
	t_block	*blocks = NULL;
	size_t	blocks_size = 0;

	blocks_size = str_size / 64;
	blocks = malloc(blocks_size * (unsigned int)sizeof(t_block));
	for (size_t i = 0; i < blocks_size; i++)
	{
		for (int k = 0; k < 16; k++)
			blocks[i].arr[k] = *(((uint32_t *)(str + (i * 64))) + k);
		for (int k = 16; k < 64; k++)
			blocks[i].arr[k] = GAMA1(blocks[i].arr[k - 2]) + GAMA0(blocks[i].arr[k - 15]) + blocks[i].arr[k - 16];
	}
	*size_addr = blocks_size;
	return (blocks);
}

void	init_state(t_sha_256 *state)
{
	state->state[0] = 0x6a09e667;
	state->state[1] = 0xbb67ae85;
	state->state[2] = 0x3c6ef372;
	state->state[3] = 0xa54ff53a;
	state->state[4] = 0x510e527f;
	state->state[5] = 0x9b05688c;
	state->state[6] = 0x1f83d9ab;
	state->state[7] = 0x5be0cd19;
}

t_sha_256	sha256(char *str)
{
	t_block		*blocks = NULL;
	t_sha_256	result;
	char		*new_str = NULL;
	size_t		blocks_size = 0, new_str_size = 0;

	init_state(&result);
	new_str = insert_padding(str, &new_str_size);
	blocks = get_blocks(new_str, new_str_size, &blocks_size);
	free(new_str);
	for (size_t t = 0; t < blocks_size; t++)
	{
		uint32_t	vars[8], t1, t2;
		memcpy(vars, result.state, 8 * sizeof(uint32_t));
		for (int i = 0; i < 64; i++)
		{
			t1 = vars[H] + SIG1(vars[E]) + CH(vars[E], vars[F], vars[G]) + keys[i] + blocks[t].arr[i];
			t2 = SIG0(vars[A]) + MAJ(vars[A], vars[B], vars[C]);
			vars[H] = vars[G];
			vars[G] = vars[F];
			vars[F] = vars[E];
			vars[E] = vars[D] + t1;
			vars[D] = vars[C];
			vars[C] = vars[B];
			vars[A] = t1 + t2;
		}
		for (int i = 0; i < 8; i++)
			result.state[i] += vars[i];
	}
	free(blocks);
	return (result);
}


int main(int ac, char **av)
{
	t_sha_256	result;
	char		*buffer = NULL;
	long 		length;
	FILE		*f;

	if (ac < 2)
	{
		fprintf(stderr, "Usuage:\n%s <filename>\n", av[0]);
		return (1);
	}
	f = fopen (av[0], "rb");
	if (!f)
	{
		fprintf(stderr, "file could not be opened!\n");
		return (1);
	}
	fseek (f, 0, SEEK_END);
	length = ftell (f);
	fseek (f, 0, SEEK_SET);
	buffer = malloc (length + 1);
	if (!buffer)
	{
		fprintf(stderr, "file could not be read!\n");
		return (1);
	}
	fread (buffer, 1, length, f);
	fclose (f);
	buffer[length] = '\0';
	result = sha256(buffer);
	free(buffer);
	for (int i = 0; i < 8; i++)
		printf("%08x", result.state[i]);
	printf("\n");
}

