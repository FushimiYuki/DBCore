#include "postgres.h"

#include "fmgr.h"
#include "libpq/pqformat.h" /* needed for send/recv functions */

PG_MODULE_MAGIC;

typedef struct IntSet {
    int length;
    int size;
    int lens;
    int set[FLEXIBLE_ARRAY_MEMBER];
} IntSet;

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

#define MAX_LEN 12
#define false 0
#define true 1
int get_max(int a, int b) { return a > b ? a : b; }

void quick_sort(int *arr, int left, int right) {
    if (left >= right) {
        return;
    }
    int l = left;
    int r = right;
    int pilot = arr[l];
    while (l < r) {
        while (l < r && arr[r] >= pilot) {
            r--;
        }
        arr[l] = arr[r];
        while (l < r && arr[l] < pilot) {
            l++;
        }
        arr[r] = arr[l];
    }
    arr[l] = pilot;
    quick_sort(arr, left, l - 1);
    quick_sort(arr, l + 1, right);
}
// 从字符串中解析出合法数字来 如果不合法返回<0的值 if -1 then '12 12'这一种情况
// if -2 then '  ' 合法返回合法数字
int get_valid_num(char *str) {
    char c;
    int i = 0;
    int num = 0;
    int flag = 0;
    while ((c = str[i]) != '\0') {
        if (i - 1 >= 0 && str[i - 1] == ' ' && c != ' ' && num > 0) {
            return -1;
        }
        if (c != ' ') {
            flag = 1;
            num = num * 10 + (c - '0');
        }
        i++;
    }
    return flag == 1 ? num : -2;
}
// 获得字符串数组中可能的数字多少
int *get_possible_size(char *str) {
    int n = strlen(str);
    int size = 1;
    // 记录可能的最长的数字
    int pre = 0;
    int max = 0;
    for (int i = 0; i < n; i++) {
        if (str[i] == ',' || str[i] == '{' || str[i] == '}') {
            max = get_max(max, i - pre);
            pre = i;
            size++;
        }
    }
    int *info = (int *)palloc(sizeof(int) * 3);
    info[0] = size;
    info[1] = max;
    return info;
}
// 转化整形为字符串数组
char *my_itoa(int num, char *buffer, int base) {
    int curr = 0;
 
    if (num == 0) {
        // Base case
        buffer[curr++] = '0';
        buffer[curr] = '\0';
        return buffer;
    }
 
    int num_digits = 0;
 
    if (num < 0) {
        if (base == 10) {
            num_digits++;
            buffer[curr] = '-';
            curr++;
            // Make it positive and finally add the minus sign
            num *= -1;
        } else
            // Unsupported base. Return NULL
            return NULL;
    }
 
    num_digits += (int)floor(log10(num) / log10(base)) + 1;
 
    // Go through the digits one by one
    // from left to right
    while (curr < num_digits) {
        // Get the base value. For example, 10^2 = 1000, for the third digit
        int base_val = (int)pow(base, num_digits - 1 - curr);
 
        // Get the numerical value
        int num_val = num / base_val;
 
        char value = num_val + '0';
        buffer[curr] = value;
 
        curr++;
        num -= base_val * num_val;
    }
    buffer[curr] = '\0';
    return buffer;
}
// 封装取值逻辑
char *get_str_from_int_set(int *set, int size, int length) {
    char *result = (char *)palloc(sizeof(char) * (length + 2));
    result[0] = '{';
    int j = 1;
    char c;
    for (int i = 0; i < size; i++) {
        char str[MAX_LEN] = {'\0'};
        my_itoa(set[i], str, 10);
        int si = 0;
        while ((c = str[si]) != '\0') {
            result[j++] = str[si++];
        }
        result[j++] = ',';
    }
    if (size == 0) {
        j = 2;
    }
    result[j - 1] = '}';
    result[j] = '\0';
    return result;
}
// 输入字符串数组获得 IntSet 数据
IntSet *get_int_set_from_str(char *str) {
    // 开始结束符号的序号标记
    int left = 0;
    int right = 0;
    int sindex = 0;
    int strn = strlen(str);
    int *info = get_possible_size(str);
    int pSize = info[0];
    int maxStrLens = info[1];
    // 记录扫描过程中的临时储存序列
    char *s = (char *)palloc(sizeof(char) * maxStrLens);
    memset(s, ' ', maxStrLens);
    // 记录结果
    IntSet *intset;
    intset = (IntSet *)palloc(VARHDRSZ + (pSize + 3) * sizeof(int));
    SET_VARSIZE(intset, VARHDRSZ + (pSize + 3) * sizeof(int));
    int size = 0;
    int *set = intset->set;
    int setIndex = 0;
    int index = 0;
    while (str[index] != '{' && index < strn) {
        index++;
    }
    // 1. 记录开始符号序号
    left = index;
    // 检查是否有开始符号
    if (index > strn - 2) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("invalid input syntax for type %s: \"%s\"",
                               "intset", str)));
    }
 
    // 2. 扫描原字符串序列
    while (++index < strn) {
        switch (str[index]) {
        case '}': {
            // 记录结束符号序号
            right = index;
        }
        case ',': {
            s[sindex++] = '\0';
            int num = get_valid_num(s);
            if (num == -2 && size == 0 && str[index] == '}') {
                // 如果是全空格的情况
                break;
            }
            if (num < 0) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("invalid input syntax for type %s: \"%s\"",
                                "intset --- ", str)));
            }
            // if (contain(set, size, num) == false) {
                set[setIndex++] = num;
                size++;
            // }
            memset(s, ' ', maxStrLens);
            sindex = 0;
            break;
        }
        default: {
            if ((str[index] < '0' || str[index] > '9') && str[index] != ' ') {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("invalid input syntax for type %s: \"%s\" %d",
                                "intset ---- default", str, index)));
            }
            s[sindex++] = str[index];
        }
        }
        // 判断
        if (str[index] == '}') {
            index++;
            while (index < strn && str[index] == ' ') {
                index++;
            }
            if (index < strn) {
                ereport(ERROR,
                        (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                         errmsg("invalid input syntax for type %s: \"%s\"",
                                "intset --- end", str)));
            }
            break;
        }
    }
 
    // 3. 检查是否有结束符号
    if (left >= right) {
        ereport(ERROR, (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
                        errmsg("invalid input syntax for type %s: \"%s\"",
                               "intset --- end", str)));
    }
    intset->size = size;
    intset->lens = right - left;
    return intset;
}
PG_FUNCTION_INFO_V1(intset_in);
// 输入函数
Datum intset_in(PG_FUNCTION_ARGS) {
    // 输入字符串
    char *str = PG_GETARG_CSTRING(0);
    IntSet *intset = get_int_set_from_str(str);
    quick_sort(intset->set, 0, intset->size - 1);
    PG_RETURN_POINTER(intset);
}
 
PG_FUNCTION_INFO_V1(intset_out);
// 输出函数-
Datum intset_out(PG_FUNCTION_ARGS) {
    IntSet *intset = (IntSet *)PG_GETARG_POINTER(0);
    char *result =
        get_str_from_int_set(intset->set, intset->size, intset->lens);
    PG_RETURN_CSTRING(pstrdup(result));
}