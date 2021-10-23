#include <stdio.h>
#include <math.h>
#define str_size 310
#define int_size 9
#define uint_max 4294967296

struct uint1024_t{
    unsigned long long int arr[32];
    int ub;
    int len;
};

void init (struct uint1024_t *x){
    for (int i = 0; i < 32; i++)
        x->arr[i] = 0;
    x->ub = 0;
}

void find_len(struct uint1024_t *x){
    int len = 1;
    for (int i = 31; i >= 0; i--)
        if (x->arr[i] != 0){
            len = i + 1;
            break;
        }
    x->len = len;
}

struct uint1024_t from_uint(unsigned long long int x){
    struct uint1024_t res;
    res.ub = 0;
    for (int i = 1; i < 32; i++)
        res.arr[i] = 0;
    res.arr[0] = x;
    find_len(&res);
    return res;
}

struct uint1024_t add_op(struct uint1024_t x, struct uint1024_t y){
    struct uint1024_t res;
    res.ub = 0;
    unsigned long long int mod = 0;
    for (int i = 31; i >= 0; i--){
        unsigned long long int sum = x.arr[i] + y.arr[i] + mod;
        res.arr[i] = sum % uint_max;
        mod = sum / uint_max;
    }
    find_len(&res);
    if (mod == 0)
        return res;
    else{
        res.ub = 1;
        return res;
    }
}

struct uint1024_t subtr_op(struct uint1024_t x, struct uint1024_t y){
    struct uint1024_t res;
    res.ub = 0;
    int debt = 0;
    for (int i = 31; i >= 0; i--) {
        if (x.arr[i] >= y.arr[i] + debt){
            res.arr[i] = x.arr[i] - y.arr[i] - debt;
            debt = 0;
        } else{
            res.arr[i] = x.arr[i] + uint_max - y.arr[i] - debt;
            debt = 1;
        }
    }
    find_len(&res);
    if (debt == 0)
        return res;
    else {
        res.ub = 1;
        return res;
    }
}

struct uint1024_t mult_op(struct uint1024_t x, struct uint1024_t y){
    struct uint1024_t res = from_uint(0);
    res.ub = 0;
    struct uint1024_t delt = from_uint(1);
    struct uint1024_t i = from_uint(1);
    while ((subtr_op(y, i).ub == 0) && (x.ub == 0)){
        res = add_op(res, x);
        i = add_op(i, delt);
    }
    find_len(&res);
    return res;
}

void printf_value(struct uint1024_t x){
    if (x.ub == 0) {
        for (int i = x.len - 1; i >= 0; i--){
            int j = 8;
            while (i != x.len - 1 && pow(10, j--) - x.arr[i] > 0 && j >= 0)
                printf("0");
            printf("%i", x.arr[i]);
        }
    }
    else
        printf("undefined behavior");
    printf ("\n");
}

void scanf_value(struct uint1024_t *x){
    char temp_str[str_size];
    int i, digits = 0;
    init(x);
    for (i = 0; i < str_size; i++)
        temp_str[i] = '\0';
    i--;
    scanf("%309s", temp_str);
    while (temp_str[i] == '\0')
        i--;

    while (i >= 0) {
        x->arr[digits / int_size] += (temp_str[i] - '0') * pow(10, digits % int_size);
        --i;
        ++digits;
    }
    x->len = (digits - 1) / 9 + 1;
}

int main(){
    struct uint1024_t n1, n2, sum, dif, multi;
    unsigned long long int n;
    struct uint1024_t *n_s = &n1;
    scanf_value(&n1);
    printf_value(n1);
    scanf ("%llu", &n);
    n2 = from_uint(n);
    printf_value(n2);
    multi = mult_op(n1, n2);
    dif = subtr_op(n1, n2);
    sum = add_op(n1, n2);
    printf_value(sum);
    printf_value(multi);
    printf_value(dif);
    return 0;
}