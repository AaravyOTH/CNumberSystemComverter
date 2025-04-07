#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include <float.h>

#define ROUNDUP 0
#define ROUNDDOWN 1
#define ROUNDTOEVEN 2

unsigned long binary_to_unsigned_decimal(const char *input_string);
long binary_to_signed_magnitude_decimal(const char *input_string);
long binary_to_ones_complement_decimal(const char *input_string);
long binary_to_twos_complement_decimal(const char *input_string);
double ieee754_to_decimal(char *input, int exp, int mantissa);
char *unsigned_decimal_to_binary(unsigned long input, unsigned int numbits);
char *signed_decimal_to_signed_magnitude(long input, unsigned int numbits);
char *signed_decimal_to_ones_complement(long input, unsigned int numbits);
char *signed_decimal_to_twos_complement(long input, unsigned int numbits);
char *decimal_to_ieee754_binary(double input, int exp_bits, int mantissa_bits, int rounding_mode);

int main(void) {
    char choice[10];
    char numberStr[256];
    int inputSystem, outputSystem;
    unsigned int numbits = 0; // used for output conversion when needed
    double dvalue = 0.0;
    long value = 0;
    
    while (1) {
        // Display input system menu.
        printf("Select input system (or 'q'/'quit' to exit):\n");
        printf("  1. Unsigned binary (binary digits only)\n");
        printf("  2. Signed magnitude binary (binary digits only)\n");
        printf("  3. One's complement binary (binary digits only)\n");
        printf("  4. Two's complement binary (binary digits only)\n");
        printf("  5. IEEE754 binary (exactly 32 bits required)\n");
        printf("  6. Decimal (base 10)\n");
        printf("Enter input system (1-6) or 'q'/'quit': ");
        if (!fgets(choice, sizeof(choice), stdin))
            break;
        choice[strcspn(choice, "\n")] = '\0';
        if (strcasecmp(choice, "q") == 0 || strcasecmp(choice, "quit") == 0) {
            break;
        }
        inputSystem = atoi(choice);
        if (inputSystem < 1 || inputSystem > 6) {
            printf("Invalid input system. Please enter a number from 1 to 6.\n\n");
            continue;
        }
        
        // Display output system menu.
        printf("\nSelect output system:\n");
        printf("  1. Unsigned binary\n");
        printf("  2. Signed magnitude binary\n");
        printf("  3. One's complement binary\n");
        printf("  4. Two's complement binary\n");
        printf("  5. IEEE754 binary (32 bits output)\n");
        printf("Enter output system (1-5): ");
        if (!fgets(choice, sizeof(choice), stdin))
            break;
        choice[strcspn(choice, "\n")] = '\0';
        outputSystem = atoi(choice);
        if (outputSystem < 1 || outputSystem > 5) {
            printf("Invalid output system. Please enter a number from 1 to 5.\n\n");
            continue;
        }
        
        // Prompt for the number to convert.
        if (inputSystem >= 1 && inputSystem <= 5) {
            if (inputSystem == 5)
                printf("\nEnter the IEEE754 number to convert (exactly 32 binary digits): ");
            else
                printf("\nEnter the number to convert (binary digits only): ");
        } else if (inputSystem == 6) {
            printf("\nEnter the decimal number to convert: ");
        }
        if (!fgets(numberStr, sizeof(numberStr), stdin))
            break;
        numberStr[strcspn(numberStr, "\n")] = '\0';
        
        // Process input based on chosen system.
        if (inputSystem >= 1 && inputSystem <= 5) {
            // Validate that the number consists only of 0's and 1's.
            bool valid = true;
            for (int i = 0; numberStr[i] != '\0'; i++) {
                if (numberStr[i] != '0' && numberStr[i] != '1') {
                    valid = false;
                    break;
                }
            }
            if (!valid || strlen(numberStr) == 0) {
                printf("Invalid number input. Please enter only binary digits (0 and 1).\n\n");
                continue;
            }
            // For IEEE754 input, require exactly 32 bits.
            if (inputSystem == 5 && strlen(numberStr) != 32) {
                printf("Invalid IEEE754 input. Must be exactly 32 bits (1 sign + 8 exponent + 23 mantissa).\n\n");
                continue;
            }
            // For binary-based inputs, use the length as the number of bits.
            numbits = strlen(numberStr);
            
            // Convert the input string to an intermediate numeric value.
            switch (inputSystem) {
                case 1:
                    value = binary_to_unsigned_decimal(numberStr);
                    break;
                case 2:
                    value = binary_to_signed_magnitude_decimal(numberStr);
                    break;
                case 3:
                    value = binary_to_ones_complement_decimal(numberStr);
                    break;
                case 4:
                    value = binary_to_twos_complement_decimal(numberStr);
                    break;
                case 5:
                    dvalue = ieee754_to_decimal(numberStr, 8, 23);
                    if (isnan(dvalue)) {
                        printf("Invalid IEEE754 number.\n\n");
                        continue;
                    }
                    break;
                default:
                    printf("Unknown input system.\n\n");
                    continue;
            }
        } else if (inputSystem == 6) {
            // For decimal input, convert using strtod.
            dvalue = strtod(numberStr, NULL);
            // For conversions that require integer representation, we warn if there is a fractional part.
            if (dvalue != (double)((long)dvalue) && outputSystem != 5) {
                printf("Warning: Fractional part will be truncated for integer conversions.\n");
            }
            // If the output system is not IEEE754, ask for the desired bit width.
            if (outputSystem != 5) {
                printf("Enter the desired number of bits for the output representation: ");
                if (!fgets(choice, sizeof(choice), stdin))
                    break;
                choice[strcspn(choice, "\n")] = '\0';
                numbits = (unsigned int)atoi(choice);
                if (numbits < 1) {
                    printf("Invalid number of bits.\n\n");
                    continue;
                }
            }
            // Set the integer value (truncating if necessary) for conversion functions that expect a long.
            value = (long)dvalue;
        }
        
        // Convert the intermediate value to the desired output representation.
        char *result = NULL;
        switch (outputSystem) {
            case 1:
                // Unsigned binary conversion (if negative, report error).
                if ((inputSystem == 5 || inputSystem == 6) ? (dvalue < 0) : (value < 0)) {
                    printf("Cannot represent a negative number in an unsigned binary system.\n\n");
                    continue;
                }
                if (inputSystem == 5)
                    result = unsigned_decimal_to_binary((unsigned long)dvalue, numbits);
                else if (inputSystem == 6)
                    result = unsigned_decimal_to_binary((unsigned long)value, numbits);
                else
                    result = unsigned_decimal_to_binary((unsigned long)value, numbits);
                break;
            case 2:
                if (inputSystem == 5)
                    result = signed_decimal_to_signed_magnitude((long)dvalue, numbits);
                else if (inputSystem == 6)
                    result = signed_decimal_to_signed_magnitude(value, numbits);
                else
                    result = signed_decimal_to_signed_magnitude(value, numbits);
                break;
            case 3:
                if (inputSystem == 5)
                    result = signed_decimal_to_ones_complement((long)dvalue, numbits);
                else if (inputSystem == 6)
                    result = signed_decimal_to_ones_complement(value, numbits);
                else
                    result = signed_decimal_to_ones_complement(value, numbits);
                break;
            case 4:
                if (inputSystem == 5)
                    result = signed_decimal_to_twos_complement((long)dvalue, numbits);
                else if (inputSystem == 6)
                    result = signed_decimal_to_twos_complement(value, numbits);
                else
                    result = signed_decimal_to_twos_complement(value, numbits);
                break;
            case 5:
                // For IEEE754, we use fixed parameters (8 exponent bits and 23 mantissa bits).
                if (inputSystem == 5)
                    result = decimal_to_ieee754_binary(dvalue, 8, 23, ROUNDTOEVEN);
                else if (inputSystem == 6)
                    result = decimal_to_ieee754_binary(dvalue, 8, 23, ROUNDTOEVEN);
                else {
                    double tmp = (double)value;
                    result = decimal_to_ieee754_binary(tmp, 8, 23, ROUNDTOEVEN);
                }
                break;
            default:
                printf("Unknown output system.\n\n");
                continue;
        }
        
        if (result == NULL) {
            printf("Conversion failed due to an error (memory allocation or invalid input).\n\n");
        } else {
            printf("Converted result: %s\n\n", result);
            free(result);
        }
    }
    
    return 0;
}

/**
 * Part 1.1 
 * @brief Converts a binary string into its unsigned decimal value.
 * @param input_string The binary string to convert.
 * @return The unsigned decimal value.
 */
unsigned long binary_to_unsigned_decimal(const char *input_string)
{
    int numOfBits = strlen(input_string);
    unsigned long sum = 0;
    for (int i = 0; i < numOfBits; i++){
        if (input_string[i] == '1'){
            sum += 1UL << (numOfBits - 1 - i);
        }
    }
    return sum;
} 

/**
 * Part 3.2
 * @brief Converts a binary string into its decimal value using signed magnitude representation.
 * @param input_string The binary string to convert.
 * @return The signed decimal value.
 */
long binary_to_signed_magnitude_decimal(const char *input_string)
{
    bool negative = false;
    if (input_string[0] == '1'){
        negative = true;
    }

    int len = strlen(input_string + 1);
    char *mut_str = malloc(len + 1);  // +1 for the null terminator
    if (!mut_str) {
        return 0;
    }
    strcpy(mut_str, input_string + 1);

    long result = 0;
    for (int i = 0; i < len; i++){
        if (mut_str[i] == '1'){
            result += 1L << (len - 1 - i);
        }
    }
    if (negative){
        result = -result;
    }
    free(mut_str);
    return result;
}

/**
 * Part 3.3
 * @brief Converts a binary string into its decimal value using one's complement.
 * @param input_string The binary string to convert.
 * @return The signed decimal value.
 */
long binary_to_ones_complement_decimal(const char *input_string)
{
    char *mut_str = strdup(input_string);
    int len = strlen(mut_str); 
    if (input_string[0] == '1'){
        // Invert every bit.
        for (int i = 0; i < len; i++){
            mut_str[i] = (mut_str[i] == '1') ? '0' : '1';
        }
    }
  
    unsigned long result = 0;
    for (int i = 1; i < len; i++){
        if (mut_str[i] == '1'){
            result += 1UL << (len - 1 - i);
        }
    }
    free(mut_str);
    return (input_string[0] == '1') ? -((long)result) : (long)result;
}

/**
 * Part 1.4
 * @brief Converts a binary string into its decimal value using two's complement.
 * @param input_string The binary string to convert.
 * @return The signed decimal value.
 */
long binary_to_twos_complement_decimal(const char *input_string)
{
    int len = strlen(input_string);
    unsigned long unsigned_num = binary_to_unsigned_decimal(input_string);

    // Use an unsigned constant for shifting to correctly compute 2^len.
    if (input_string[0] == '0'){
        return (long)unsigned_num;
    } else {
        return (long)(unsigned_num - (1UL << len));
    }
}

/**
 * Part 2.1
 * @brief Converts an unsigned decimal value into its unsigned binary representation.
 * @param input The decimal value to convert.
 * @param numbits The number of bits for the representation.
 * @return The binary string (allocated on the heap).
 */
char *unsigned_decimal_to_binary(unsigned long input, unsigned int numbits)
{
    char *str = malloc(numbits + 1);
    if (!str){
        return NULL;
    }
    str[numbits] = '\0';

    unsigned long remainder = input;
    unsigned long power;
    for (unsigned int i = 0; i < numbits; i++){
        power = 1UL << (numbits - 1 - i);
        if (power <= remainder){
            str[i] = '1';
            remainder -= power;
        } else {
            str[i] = '0';
        }
    }
    return str;
}

/**
 * Part 2.1
 * @brief Converts a signed decimal value into its binary representation using signed magnitude.
 * @param input The decimal value to convert.
 * @param numbits The number of bits for the representation.
 * @return The binary string (allocated on the heap).
 */
char *signed_decimal_to_signed_magnitude(long input, unsigned int numbits)
{
    if (numbits < 1){
        return NULL;
    }
    char *binary = malloc(numbits + 1);
    if (!binary){
        return NULL;
    }
    binary[numbits] = '\0';

    unsigned long magnitude;
    if (input < 0){
        binary[0] = '1';
        magnitude = (unsigned long)(-input);
    } else {
        binary[0] = '0';
        magnitude = (unsigned long)input;
    }
    for (unsigned int i = 1; i < numbits; i++){
        unsigned int shift = numbits - 1 - i;
        unsigned long power = 1UL << shift;
        if (power <= magnitude) {
            binary[i] = '1';
            magnitude -= power;
        } else {
            binary[i] = '0';
        }
    }
    return binary;
}

/**
 * Part 2.1
 * @brief Converts a signed decimal value into its one's complement binary representation.
 * @param input The decimal value to convert.
 * @param numbits The number of bits for the representation.
 * @return The binary string (allocated on the heap).
 */
char *signed_decimal_to_ones_complement(long input, unsigned int numbits)
{
    char *binary;
    if (input >= 0) {
        binary = unsigned_decimal_to_binary((unsigned long)input, numbits);
    } else {
        binary = unsigned_decimal_to_binary((unsigned long)(-input), numbits);
        if (!binary) return NULL;
        // Flip each bit.
        for (unsigned int i = 0; i < numbits; i++){
            binary[i] = (binary[i] == '0') ? '1' : '0';
        }
    }
    return binary;
}

/**
 * Part 2.1
 * @brief Converts a signed decimal value into its two's complement binary representation.
 * @param input The decimal value to convert.
 * @param numbits The number of bits for the representation.
 * @return The binary string (allocated on the heap).
 */
char *signed_decimal_to_twos_complement(long input, unsigned int numbits)
{
    if (input >= 0) {
        return unsigned_decimal_to_binary((unsigned long)input, numbits);
    } else {
        // Two's complement: add 2^n to the negative number.
        unsigned long rep = (1UL << numbits) + input;
        return unsigned_decimal_to_binary(rep, numbits);
    }
}

/**
 * Part 3.1
 * @brief Converts an IEEE754 binary string into its decimal value.
 * @param input The IEEE754 binary string.
 * @param exp The number of exponent bits.
 * @param mantissa The number of mantissa bits.
 * @return The decimal value.
 */
double ieee754_to_decimal(char *input, int exp, int mantissa)
{
    int total_bits = 1 + exp + mantissa;
    if ((int)strlen(input) != total_bits) {
        return NAN;
    }
    int sign = (input[0] == '1') ? -1 : 1;
    
    // Convert exponent bits.
    unsigned int exponent = 0;
    for (int i = 1; i <= exp; i++){
        exponent = (exponent << 1) | (input[i] - '0');
    }
    int bias = (1 << (exp - 1)) - 1;
    
    // Convert mantissa bits to fraction.
    double fraction = 0.0;
    for (int i = 0; i < mantissa; i++){
        if (input[1 + exp + i] == '1') {
            fraction += 1.0 / (1 << (i + 1));
        }
    }
    
    // Special cases: All exponent bits 1.
    if (exponent == ((1U << exp) - 1)) {
        if (fraction != 0.0) {
            return NAN;
        } else {
            return sign * INFINITY;
        }
    }
    
    double significand;
    int E;
    if (exponent == 0) {
        significand = fraction;
        E = 1 - bias;
    } else {
        significand = 1.0 + fraction;
        E = exponent - bias;
    }
    
    return sign * ldexp(significand, E);
}

/**
 * Part 3.2
 * @brief Converts a decimal value into its IEEE754 binary representation.
 * @param input The decimal number.
 * @param exp_bits The number of exponent bits.
 * @param mantissa_bits The number of mantissa bits.
 * @param rounding_mode Rounding mode (ROUNDUP, ROUNDDOWN, or ROUNDTOEVEN).
 * @return The IEEE754 binary string (allocated on the heap).
 */
char *decimal_to_ieee754_binary(double input, int exp_bits, int mantissa_bits, int rounding_mode)
{
    int total_bits = 1 + exp_bits + mantissa_bits;
    char *result = malloc(total_bits + 1);
    if (!result) return NULL;
    result[total_bits] = '\0';
    
    int sign_bit = (input < 0) ? 1 : 0;
    result[0] = sign_bit ? '1' : '0';
    
    if (isnan(input)) {
        for (int i = 1; i <= exp_bits; i++) result[i] = '1';
        result[1 + exp_bits] = '1'; // Set a nonzero mantissa.
        for (int i = 1 + exp_bits + 1; i < total_bits; i++) result[i] = '0';
        return result;
    }
    if (isinf(input)) {
        for (int i = 1; i <= exp_bits; i++) result[i] = '1';
        for (int i = 1 + exp_bits; i < total_bits; i++) result[i] = '0';
        return result;
    }
    if (input == 0.0) {
        for (int i = 1; i < total_bits; i++) result[i] = '0';
        return result;
    }
    
    double abs_val = fabs(input);
    int bias = (1 << (exp_bits - 1)) - 1;
    int extra = 3;  // Number of extra bits (guard, round, sticky)
    int total_fraction_bits = mantissa_bits + extra;
    double eps = 0.0;  // Using 0 to avoid unintended rounding shifts
    
    // Denormalized numbers: if the absolute value is too small to be normalized.
    if (abs_val < pow(2, 1 - bias)) {
        for (int i = 1; i <= exp_bits; i++) {
            result[i] = '0';
        }
        double fraction_denorm = abs_val / pow(2, 1 - bias);
        double scaled = fraction_denorm * (double)(1UL << total_fraction_bits) + eps;
        unsigned long full = (unsigned long)floor(scaled);
        
        unsigned long mantissa_candidate = full >> extra;
        unsigned long extra_bits_val = full & ((1UL << extra) - 1);
        
        int round = 0;
        if (rounding_mode == ROUNDUP) {
            if (extra_bits_val > 0)
                round = 1;
        } else if (rounding_mode == ROUNDDOWN) {
            round = 0;
        } else if (rounding_mode == ROUNDTOEVEN) {
            int guard = (extra_bits_val >> (extra - 1)) & 1;
            int round_bit = (extra > 1) ? ((extra_bits_val >> (extra - 2)) & 1) : 0;
            int sticky = (extra > 2) ? ((extra_bits_val & ((1UL << (extra - 2)) - 1)) ? 1 : 0) : 0;
            if (guard == 0)
                round = 0;
            else {
                if (round_bit == 1 || sticky == 1)
                    round = 1;
                else { // exactly halfway
                    if (mantissa_candidate & 1UL)
                        round = 1;
                    else
                        round = 0;
                }
            }
        }
        if (round)
            mantissa_candidate++;
        // In denormalized numbers, we simply truncate to mantissa_bits.
        for (int i = 0; i < mantissa_bits; i++) {
            int bit = (mantissa_candidate >> (mantissa_bits - 1 - i)) & 1;
            result[1 + exp_bits + i] = bit ? '1' : '0';
        }
        return result;
    }
    
    // Normalized numbers.
    int E = 0;
    double normalized = abs_val;
    if (normalized >= 2.0) {
        while (normalized >= 2.0) {
            normalized /= 2.0;
            E++;
        }
    } else if (normalized < 1.0) {
        while (normalized < 1.0) {
            normalized *= 2.0;
            E--;
        }
    }
    
    int exp_field = E + bias;
    // Check for exponent overflow (set to infinity).
    if (exp_field >= (1 << exp_bits) - 1) {
        for (int i = 1; i <= exp_bits; i++) result[i] = '1';
        for (int i = 1 + exp_bits; i < total_bits; i++) result[i] = '0';
        return result;
    }
    
    double fraction = normalized - 1.0;
    double X = fraction * (double)(1UL << total_fraction_bits) + eps;
    unsigned long full = (unsigned long)floor(X);
    unsigned long mantissa_candidate = full >> extra;
    unsigned long extra_bits_val = full & ((1UL << extra) - 1);
    
    int round = 0;
    if (rounding_mode == ROUNDUP) {
        if (extra_bits_val > 0)
            round = 1;
    } else if (rounding_mode == ROUNDDOWN) {
        round = 0;
    } else if (rounding_mode == ROUNDTOEVEN) {
        int guard = (extra_bits_val >> (extra - 1)) & 1;
        int round_bit = (extra > 1) ? ((extra_bits_val >> (extra - 2)) & 1) : 0;
        int sticky = (extra > 2) ? ((extra_bits_val & ((1UL << (extra - 2)) - 1)) ? 1 : 0) : 0;
        if (guard == 0)
            round = 0;
        else {
            if (round_bit == 1 || sticky == 1)
                round = 1;
            else { // exactly halfway
                if (mantissa_candidate & 1UL)
                    round = 1;
                else
                    round = 0;
            }
        }
    }
    if (round) {
        mantissa_candidate++;
        if (mantissa_candidate >= (1UL << mantissa_bits)) {
            // Rounding overflow: reset mantissa and increment exponent.
            mantissa_candidate = 0;
            exp_field++;
            if (exp_field >= (1 << exp_bits) - 1) {
                // Exponent overflow: represent infinity.
                for (int i = 1; i <= exp_bits; i++) result[i] = '1';
                for (int i = 1 + exp_bits; i < total_bits; i++) result[i] = '0';
                return result;
            }
        }
    }
    
    // Convert exponent field to binary.
    for (int i = 0; i < exp_bits; i++) {
        int bit = (exp_field >> (exp_bits - 1 - i)) & 1;
        result[1 + i] = bit ? '1' : '0';
    }
    
    // Convert the mantissa to binary.
    for (int i = 0; i < mantissa_bits; i++) {
        int bit = (mantissa_candidate >> (mantissa_bits - 1 - i)) & 1;
        result[1 + exp_bits + i] = bit ? '1' : '0';
    }
    
    return result;
}

