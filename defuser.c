#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;

    uint32_t first = (uint32_t)challenge;
    uint32_t high = (uint32_t)(challenge >> 32);

    uint32_t a = (first * 3u + (uint32_t)(challenge >> 16)) ^ UINT32_C(0xdb50);
    uint32_t b = ((first ^ UINT32_C(0x3b5e)) + high * 4u);
    uint32_t result = (b + UINT32_C(0xb08d)) ^ a;

    values[0] = result;
    count = 1;

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    
    values[0] = (uint32_t)(((challenge >> 39) & 31u) + 28u);

    for (uint32_t i = 1; i < 6; ++i) {
        uint32_t bits = (uint32_t)((challenge >> (5 * i + 4)) & 31u);
        values[i] = (bits ^ (i + 5u)) + 3u * values[i-1];
    }

    count = 6;

    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;

    uint32_t index = (uint32_t)(challenge & 7u);
    uint32_t x = (uint32_t)(challenge >> 3)
           ^ (uint32_t)(challenge >> 32);

    uint32_t result;

    switch (index) {
        case 0:
            result = (x ^ UINT32_C(0x6536)) + (x >> 2);
            break;

        case 1:
            result = x * 5u + UINT32_C(0x1809c);
            break;

        case 2:
            result = x * 5u + UINT32_C(0x9dee);
            break;

        case 3:
            result = (x * 2u) ^ UINT32_C(0x1434);
            break;

        case 4:
            result = (x ^ UINT32_C(0xc19b)) + (x >> 1);
            break;

        case 5:
            result = x * 3u + UINT32_C(0x4e22);
            break;

        case 6:
            result = x * 5u + UINT32_C(0x804d);
            break;

        default:  /* index == 7 */
            result = (x * 2u) ^ UINT32_C(0xa832);
            break;
    }

    values[0] = index;
    values[1] = result;
    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    
    uint32_t x = (uint32_t)(challenge & 0x7fu);
    uint32_t key = (uint32_t)(challenge >> 7)
             ^ (uint32_t)(challenge >> 32);

    uint32_t mids[8];
    uint32_t dirs[8];
    uint32_t depth = 0;
    uint32_t low = 0;
    uint32_t high = 127;
    uint32_t mid;

    while (1) {
        mid = low + ((high - low) >> 1);

        if (x == mid) {
            break;
        }

        mids[depth] = mid;

        if (x > mid) {
            dirs[depth] = 1;
            low = mid + 1;
        } else {
            dirs[depth] = 0;
            high = mid - 1;
        }

        ++depth;
    }

    uint32_t result = (mid + 0x6cu) ^ key;

    while (depth > 0) {
        --depth;
        result = (5u * result) ^ (mids[depth] + key);

        if (dirs[depth]) {
            result ^= 0x10u;
        } else {
            result ^= 0x15u;
        }
    }

    values[0] = result;
    count = 1;

    return write_answer(out, cap, values, count);
}
    

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;

    static const uint32_t node_value[12] = {
    0xf2fc, 0xc97c, 0x9ccc, 0xe860,
    0x46eb, 0xa597, 0x79d0, 0x72d0,
    0x62bf, 0x35ff, 0x3788, 0x72c2
    };

    static const uint32_t node_id[12] = {
    0x0827, 0x0b4a, 0x0438, 0x070b,
    0x0613, 0x033d, 0x0013, 0x0212,
    0x010d, 0x0a28, 0x055a, 0x0960
    };

    static const int next_node[12] = {
        1, -1, 0, 8,
        9, 2, 11, 4,
        7, 6, 3, 5
    };

    static const int head_node[8] = { 
        10, 3, 8, 7, 4, 9, 6, 11
    };

    uint32_t index = (uint32_t)(challenge & 7u);
    uint32_t steps = (uint32_t)((challenge >> 3) & 3u);

    int node = head_node[index];

    uint32_t x = (uint32_t)(challenge >> 6)
               ^ (uint32_t)(challenge >> 32);

    uint32_t acc = (x ^ UINT32_C(0x5cf2));
    acc += node_value[node];
    acc *= 5u;
    acc ^= node_id[node];

    for (uint32_t i = 0; i < steps; ++i) {
        node = next_node[node];

        acc += node_value[node];
        acc *= 5u;
        acc ^= node_id[node];
    }

    values[0] = acc;
    values[1] = node_id[node];
    count = 2;
    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    
    static const uint32_t op_map[8] = {
    4, 4, 2, 0, 1, 3, 5, 2
    };

    uint32_t x = (uint32_t)(challenge >> 6)
               ^ (uint32_t)(challenge >> 32);

    uint32_t op1 = op_map[(uint32_t)(challenge & 7u)];
    uint32_t first;

    switch (op1) {
        case 0:
            first = ((x << 7) | (x >> 25)) ^ UINT32_C(0x73ff);
        break;

        case 1:
            first = ((x >> 2) ^ x) + UINT32_C(0x92fe);
            break;

        case 2:
            first = (x + UINT32_C(0x096b)) ^ (x >> 1);
            break;

        case 3:
            first = ((x << 5) + UINT32_C(0x336d)) ^ (x * 7u);
            break;

        case 4:
            first = (x ^ UINT32_C(0x4f09)) + x * 4u;
            break;

        default:  /* op1 == 5 */
            first = (x >> 2) | (x << 30);
            break;
    }

    uint32_t y = first ^ UINT32_C(0xf670);
    uint32_t op2 = op_map[(uint32_t)((challenge >> 3) & 7u)];
    uint32_t result;

    switch (op2) {
        case 0:
            result = ((y << 7) | (y >> 25)) ^ UINT32_C(0x73ff);
            break;

        case 1:
            result = ((y >> 2) ^ y) + UINT32_C(0x92fe);
            break;

        case 2:
            result = (y + UINT32_C(0x096b)) ^ (y >> 1);
            break;

        case 3:
            result = ((y << 5) + UINT32_C(0x336d)) ^ (y * 7u);
            break;

        case 4:
            result = (y ^ UINT32_C(0x4f09)) + y * 4u;
            break;

        default:  /* op2 == 5 */
            result = (y >> 2) | (y << 30);
            break;
    }

    values[0] = result + UINT32_C(0x46a7);
    count = 1;
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;

    uint32_t a = (uint32_t)(context & 3u);

    uint32_t challenge_mix =
        (uint32_t)challenge ^ (uint32_t)(challenge >> 32);

    uint32_t context_mix =
        (uint32_t)context ^ (uint32_t)(context >> 32);

    uint32_t arg = challenge_mix ^ context_mix;

    uint32_t mixed =
        ((arg >> 1) ^ UINT32_C(0x3b94))
        + (arg ^ UINT32_C(0x478c));

    uint32_t r9 =
        (a * UINT32_C(0x478c))
        ^ (uint32_t)(context >> 16);

    uint32_t b = mixed ^ r9;

    uint32_t c =
        b ^ (context_mix + a + UINT32_C(0x3b94)
            + (challenge_mix >> 1));

    int n = snprintf(out, cap, "%032uOPEN %u %u %u",
                     0u, a, b, c);

    if (n < 0 || (size_t)n >= cap) {
        return 1;
    }
    return 0;
}
    

