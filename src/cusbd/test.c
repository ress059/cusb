#include <uchar.h>
#include <stdio.h>
#include <stddef.h>

static size_t get_size(const char16_t *str)
{
    size_t count = 0;

    if (str)
    {
        size_t i = 0;

        while (str[i])
        {
            count++;
            i++;
        }
    }

    return count;
}

int main(void)
{
    size_t count = get_size(u"hello world");
    printf("size = %d\n", (int)count);
    return 0;
}
