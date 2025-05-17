/**
 * @file
 * @brief Dummy main.c to test compilation and linkage of ECU library.
 * 
 * @author Ian Ress
 * @version 0.1
 * @date 2025-05-17
 * @copyright Copyright (c) 2025
 */

/*------------------------------------------------------------*/
/*--------------------------- MAIN ---------------------------*/
/*------------------------------------------------------------*/

int main(void)
{
    return 0;
}

/*------------------------------------------------------------*/
/*---------------- ASSERT HANDLER DEFINITION -----------------*/
/*------------------------------------------------------------*/

void ecu_assert_handler(const char *file, int line)
{
    (void)file;
    (void)line;
    while(1)
    {

    }
}
